#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "image.h"
#include "ctest.h"

#define BLOCK_SIZE 4096


unsigned char *bread(int block_num, unsigned char *block){
    //starts you in the beginning of the block
    if(lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) == -1){
        perror("Error seeking in bread()");
        return NULL;
    }
    
    ssize_t bytes_read = read(image_fd, block, BLOCK_SIZE);

    if(bytes_read == -1){
        perror("error reading in bread()");
        return NULL;
    }
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