#include "block.h"
#include "ctest.h"
#include "inode.h"
#include "dir.h"
#include "pack.h"
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 4096

void root_directory(void){
    //ialloc inode
    struct inode *inum = ialloc(); //should be 0

    //alloc a data block to hold hardcoded entries
    int block_num = alloc();

    //intialize to hold correct metadata
    inum->flags = 2;
    inum->size = 64; //two 32 byte records for one directory
    inum->block_ptr[0] = block_num;

    //write the directory entries to an in-memory block
    
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
