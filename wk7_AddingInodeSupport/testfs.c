#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "ctest.h"
#include "free.h"
#include "inode.h"
#include "pack.h"

#ifdef CTEST_ENABLE


void test_image_open(void){
    int fd = image_open("test.txt", 1);
    //int fd = setup();
    CTEST_ASSERT(fd >= 0, "image_open ran successfully");
    CTEST_ASSERT(image_fd == fd, "result of test fd is the same as global fd");
}

void test_image_close(void){
    int result = image_close();
    CTEST_ASSERT(result == 0, "image_close closed successfully");
}

void test_free(void){
    setup();
    unsigned char block[4096];
    set_free(block, 0, 1); // bit 0 to used
    int fz = find_free(block);
    CTEST_ASSERT(fz == 1, "bit 0 is taken, bit 1 is free");
    set_free(block, 1, 1); //bit 1 to used
    fz = find_free(block);
    CTEST_ASSERT(fz == 2, "bit 1 is taken, bit 2 is free");
}

void test_inode(void){

    int result = ialloc(); //assign an inode
    printf("Allocated inode: %d\n", result);
    CTEST_ASSERT(result >= 0, "ialloc should return an inode index >= 0");
}

void test_block(void){

    int result = alloc(); //assign a data block
    printf("allocated data block: %d\n", result);
    CTEST_ASSERT(result >= 0, "alloc should return an inode index >= 0");

}

void setup(void){
    image_open("test.txt", 1);
}

void teardown(void){
    image_close();
}

//data read in and offset you want to extract a value from

void ReadBinaryValues(void){
    unsigned char block[4096];
    int block_num = 3;
    int offset = 128;
    bread(block_num, block);
    unsigned int v = read_u32(block + offset);
    printf("The value is %u\n", v);


}

int main(void){
    //call image_open and image_close
    CTEST_VERBOSE(1);
    test_image_open();
    test_image_close();
    test_free();
    test_inode();
    test_block();
    ReadBinaryValues();
    CTEST_RESULTS();
    CTEST_EXIT();
}

#else 
int main(void){
    printf("Running normally!\n");
}
#endif