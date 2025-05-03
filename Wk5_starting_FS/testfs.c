#include <stdio.h>
#include "image.h"
#include "block.h"
#include "ctest.h"

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

int main(void){
    //call image_open and image_close
    CTEST_VERBOSE(1);
    test_image_open();
    test_image_close();
    CTEST_RESULTS();
    CTEST_EXIT();
}

#else 
int main(void){
    printf("Running normally!\n");
}
#endif