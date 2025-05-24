#include "block.h"
#include "ctest.h"
#include "inode.h"
#include "dir.h"
#include "pack.h"
#include <string.h>
#include <stdlib.h>

#define BLOCK_SIZE 4096
#define ENTRY_SIZE 32

void root_directory(void){
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

void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}

