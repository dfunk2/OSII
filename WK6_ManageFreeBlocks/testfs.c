#include <stdio.h>
#include "image.h"
#include "block.h"
#include "ctest.h"
#include "free.h"

#ifdef CTEST_ENABLE

void test_image_open(void){
    int fd = image_open("test.txt", 1);
    CTEST_ASSERT(fd >= 0, "image_open ran successfully");
    CTEST_ASSERT(image_fd == fd, "result of test fd is the same as global fd");
}

void test_image_close(void){
    int result = image_close();
    CTEST_ASSERT(result == 0, "image_close closed successfully");
}

void test_free(void){
    unsigned char block[4096] = {0};
    set_free(block, 1, 1); //set bit 1 
    int fz = find_free(block);
    CTEST_ASSERT(fz == 0, "only bit 1 is set, find free should return 0, pass");
    set_free(block, 0, 1);
    fz = find_free(block);
    CTEST_ASSERT(fz == 1, "bit 0 now set to 1, free bit should be 1, fail");
}



int main(void){
    //call image_open and image_close
    CTEST_VERBOSE(1);
    test_image_open();
    test_image_close();
    test_free();
    CTEST_RESULTS();
    CTEST_EXIT();
}

#else 
int main(void){
    printf("Running normally!\n");
}
#endif