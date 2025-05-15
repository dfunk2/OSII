#include "inode.h"
#include "ctest.h"
#include "free.h"
#include "block.h"
#define BLOCK_SIZE 4096
unsigned char buffer_block[BLOCK_SIZE];

int ialloc(void){
    //allocate a previously-free inode in the inode map
    //get inode map, pass 1 because thats the inode block num
    unsigned char *inode_map = bread(1, buffer_block);
    //locate a free inode
    int free_inode = find_free(inode_map);
    if (free_inode == -1){
        return -1;
    }
    //mark as used
    set_free(inode_map, free_inode, 1);
    //save inode back out to disk
    bwrite(1, inode_map);

    return free_inode;

}

