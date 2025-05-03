#include <stdio.h>
#include "image.h"
#include "block.h"
#include "ctest.h"

#ifdef CTEST_ENABLE

void test_image_open(void){
    CTEST_ASSERT(image_open("image.txt", 2) == 2, "testing image_open");
}

void test_image_close(void){
    CTEST_ASSERT(image_close() == 0, "testing image_close sucessful");
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