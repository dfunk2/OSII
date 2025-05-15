#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "ctest.h"

int image_fd;

int image_open(char *filename, int truncate){ 
    //non-zero is true
    int flags = O_CREAT | O_RDWR;
    
    if(truncate != 0){
        flags |= O_TRUNC;

    }
    
    image_fd = open(filename, flags, 0600);
    if (image_fd == -1){
        perror("Error opening file");
        return -1;
    }

    return image_fd;
}

int image_close(void){

    int result = close(image_fd);
    return result;
}