#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "image.h"
#include "ctest.h"
#include "inode.h"
#include "free.h"

#define BLOCK_SIZE 4096

unsigned char *bread(int block_num, unsigned char *block){
    //takes a pointer to a buffer to load with the block data. 
    if(lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) == -1){
        perror("Error seeking in bread()");
        return NULL;
    }
    //It copies it out of the disk image into that buffer i.e block array upto block_size.
    ssize_t bytes_read = read(image_fd, block, BLOCK_SIZE);

    if(bytes_read == -1){
        perror("error reading in bread()");
        return NULL;
    }
    //It also returns a pointer to that same buffer.
    return block;
}

void bwrite(int block_num, unsigned char *block){
    if(lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) == -1){
        perror("Error seeking in bwrite()");
    }
    
    ssize_t bytes_written = write(image_fd, block, BLOCK_SIZE);
    if(bytes_written != BLOCK_SIZE){
        perror("error writing to block");
    }

}

int alloc(void){
    //allocate a previously-free inode in the inode map
    //get inode map, pass 2 because thats the data block num
    unsigned char *data_map = bread(2, buffer_block);
    //locate a free inode
    int free_data = find_free(data_map);
    if (free_data == -1){
        return -1;
    }
    //mark as used
    set_free(data_map, free_data, 1);
    //save inode back out to disk
    bwrite(2, data_map);

    return free_data;

}