#include "block.h"
#include "image.h"
#include "ctest.h"
#include "inode.h"
#include <stdio.h>

#define BITS_PER_BYTE 8
#define BYTES_PER_BLOCK 4096


void set_free(unsigned char *block, int num, int set){
    //set a specific bit to 0 or 1 
    //int num is a specific number we want to set free in the blocks
    int byte_num = num / BITS_PER_BYTE;
    int bit_num = num % BITS_PER_BYTE;

    //highlight the bit number
    int mask = 1 << bit_num;

    //set = 1, set as used 
    if(set){
        block[byte_num] |= mask;
    } 
    //set = 0, set as free or clear the bit
    //keep all the other bits as they are and clear just bit_num
    else{
        block[byte_num] &= ~(mask);
    }
    
}

//find a 0 bit and return its index (block number that corresponds to this bit)
//computer global index of bit and return it
int find_free(unsigned char *block){
    for(int i = 0; i < BYTES_PER_BLOCK; i++){
        for(int j = 0; j < 8; j++){
            int mask = 1 << j;
            if ((block[i] & mask) == 0){
                int global_index = i * BITS_PER_BYTE + j;
                return global_index;
            }
        }
    }
    return -1;
}