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

    //return root directory inode
    struct inode *inode;
    if(strcmp(path, "/") == 0) {
        return inode = iget(ROOT_INODE_NUM);
    } else {
        return NULL;
    }

    struct directory *rootdir = directory_open(ROOT_INODE_NUM);
    
    //parse other paths 
    const char *token = strtok(path, "/");
    while(rootdir != NULL){
        if(rootdir == token){
            char* p = strcpy(path, token);
            //link path to inode number 
            //return result from iget()
        }else {
            return NULL;
        }
    } 
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
    struct inode *FIncoreInode = namei(path);
    struct inode *new_inode = ialloc();
    int new_block = alloc();

    //create a new block-sized array for the new directory data block and initialize it
    unsigned int buffer_block[BLOCK_SIZE] = {0};

    write_u16(buffer_block+ 0, new_inode->inode_num);
    char currdir[] = ".";
    strcpy((char*)buffer_block + 2, currdir);

    write_u16(buffer_block + 32, new_inode->inode_num);
    char parentdir[] = "..";
    strcpy((char*)buffer_block + 34, parentdir);

    //intialize incore inode with proper metadata
    new_inode->flags = 2;
    new_inode->size = 64;
    new_inode->block_ptr[0] = new_block;

    bwrite(new_block, buffer_block);

}


void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}

