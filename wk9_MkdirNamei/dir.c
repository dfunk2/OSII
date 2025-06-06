#include "block.h"
#include "ctest.h"
#include "inode.h"
#include "dir.h"
#include "pack.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BLOCK_SIZE 4096
#define ENTRY_SIZE 32
#define ROOT_INODE_NUM 0

void mkfs(void){
    //ialloc inode
    struct inode *inum = ialloc(); //should be 0

    //alloc a data block to hold hardcoded entries
    int block_num = alloc();

    //intialize to hold correct metadata
    inum->flags = 2; //file is directory
    inum->size = 64; //two 32 byte records for one directory
    inum->block_ptr[0] = block_num;

    //write the directory entries to an in-memory block
    unsigned char buffer_block[BLOCK_SIZE] = {0};

    //first directory entry
    write_u16(buffer_block + 0, inum->inode_num);
    char fndot[] = ".";
    strcpy((char*)buffer_block + 2, fndot);

    //second directory entry
    write_u16(buffer_block + 32, inum->inode_num);
    char fndotdot[] = "..";
    strcpy((char*) buffer_block + 34, fndotdot);

    //write the block out to disk
    bwrite(block_num, buffer_block);

    //free up incore inode
    iput(inum);
}

struct directory *directory_open(int inode_num){
    //get inode for file
    struct inode *dir_inode = iget(inode_num);
    if(dir_inode == NULL){
        return NULL;
    }
    //make space for new struct directory
    struct directory *new_dir = malloc(sizeof(struct directory));
    if(new_dir == NULL){
        printf("memory allocation for new struct directory failed");
        return NULL;
    }

    //initialize struct dir data
    new_dir->inode = dir_inode;
    new_dir->offset = 0;

    //return pointer to the struct
    return new_dir;
}

//Directory get function
int directory_get(struct directory *dir, struct directory_entry *ent) {
    if (dir->offset >= dir->inode->size) return -1;

    int data_block_index = dir->offset / BLOCK_SIZE;
    int data_block_num = dir->inode->block_ptr[data_block_index];
    unsigned char block[BLOCK_SIZE];

    bread(data_block_num, block);

    int offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char *)(block + offset_in_block + 2));

    dir->offset += ENTRY_SIZE;
    return 0;
}

struct inode *namei(char *path){

    if (strcmp(path, "/") == 0) {
        return iget(ROOT_INODE_NUM);
    }

    char name[64];
    strncpy(name, path, sizeof(name));
    char *token = strtok(name, "/");
    

    //start from root inode 
    struct inode *curr = iget(ROOT_INODE_NUM);

    while(token != NULL){
        struct directory *dir = directory_open(curr->inode_num);
        if(!dir) {
            iput(curr); 
            return NULL;
        }
    
        struct directory_entry ent;
  
        while(directory_get(dir, &ent) == 0){
            if(strcmp(ent.name, token) == 0){
                struct inode *next = iget(ent.inode_num);
                if(!next){
                    directory_close(dir);
                    iput(curr);
                    return NULL;
                }
                iput(curr);
                curr = next;
                break;
            } 
        }
        directory_close(dir);
        token = strtok(NULL, "/"); 
    }
    return curr;

    //old
    // getting a single component assuming one level
    //get_basename(path,name);

    // //Assuming only /name structure
    // if (path[0] != '/') return NULL;

    // char target[16];
    // get_basename(path, target);
}

//helper function, returns every component of path except last one
char *get_dirname(const char *path, char *dirname) {
    strcpy(dirname, path);

    char *p = strrchr(dirname, '/');

    if (p == NULL) { //path isnt a directory name
        strcpy(dirname, "."); 
        return dirname; //return current directory
    }

    if (p == dirname)  // Last slash is the root /
        *(p+1) = '\0';  

    else
        *p = '\0';  // Last slash is not the root /, chop off last component

    return dirname;
}

//helper function, finds the new directory name from path
char *get_basename(const char *path, char *basename) {
    if (strcmp(path, "/") == 0) {
        strcpy(basename, path);
        return basename;
    }

    const char *p = strrchr(path, '/');

    if (p == NULL)
        p = path;   // No slash in name, start at beginning
    else
        p++;        // Start just after slash

    strcpy(basename, p);

    return basename;
}


int directory_make(char *path){
    if (path[0] != '/') return -1;

    char dirname[64], basename[16];
    get_dirname(path, dirname);
    get_basename(path, basename);

    struct inode *parent = namei(dirname);
    if (!parent) return -1;

    struct inode *child = ialloc();
    if (!child){
        iput(parent);
        return -1;
    }

    int block_num = alloc();
    if(block_num == -1){
        iput(parent);
        iput(child);
        return -1;
    }

    unsigned char block[BLOCK_SIZE] = {0};
    write_u16(block + 0, child->inode_num);
    strcpy((char *)(block +2), ".");
    write_u16(block +32, parent->inode_num);
    strcpy((char *)(block + 34), "..");
    bwrite(block_num, block);

    child ->flags = 2;
    child->size = 64;
    child->block_ptr[0] = block_num;
    iput(child);

    //Adding entry to parent
    int offset = parent->size;
    int data_block= parent->block_ptr[0];
    bread(data_block, block);
    write_u16(block + offset, child->inode_num);
    strcpy((char *)(block + offset +2), basename);
    bwrite(data_block, block);

    parent->size += 32;
    
    iput(parent);

    return 0;

}


void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}

