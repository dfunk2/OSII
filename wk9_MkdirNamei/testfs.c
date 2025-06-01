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
#include "dir.h"

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

/*void test_inode(void){

    int result = ialloc(); //assign an inode
    printf("Allocated inode: %d\n", result);
    CTEST_ASSERT(result >= 0, "ialloc should return an inode index >= 0");
}*/

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

void test_incore_inode(void){
    struct inode * result = incore_find_free();
    CTEST_ASSERT(result != 0, "found a free inode");
    //modify inode 
    result->inode_num = 1;
    result->owner_id = 55;
    result->ref_count = 1; //in use
    struct inode *data = incore_find(1);
    CTEST_ASSERT(data->owner_id == 55, "inode owner id matches");
    CTEST_ASSERT(data == result, "same inode");
}

/*void test_write_read_inode(void){
    int inum = ialloc();

    struct inode test_inode;

    test_inode.inode_num = inum;
    test_inode.size = 5;
    
    write_inode(&test_inode);

    struct inode read_inode_result;
    read_inode(&read_inode_result, inum);

    CTEST_ASSERT(test_inode.size == read_inode_result.size, "value successfully written to disk and updated into in-core inode");

}*/

void test_iget_iput(void){
    struct inode *result = iget(3);
    CTEST_ASSERT(result->ref_count == 1, "sucessfully found or allocated an incore inode");
    iput(result);
    CTEST_ASSERT(result->ref_count == 0, "sucessfully deallocated an incore inode");

}

void test_ialloc(void){
    struct inode *result = ialloc();
    result->size = 10;
    CTEST_ASSERT(result->size == 10, "ialloc ran successfully");

}

void test_mkfs(void){
    mkfs();
    struct inode *result = ialloc();
    CTEST_ASSERT(result->inode_num == 0, "successfully allocated root directory inode number");

}
void test_directory_get(void){
    struct directory *dir = directory_open(0);
    struct directory_entry ent;

    int got = directory_get(dir, &ent);
    CTEST_ASSERT(got == 0, "directory_get returns 0 for first entry");
    CTEST_ASSERT(strcmp(ent.name, ".") == 0 || strcmp(ent.name, "..") == 0, "entry name is valid");

    got = directory_get(dir, &ent);
    CTEST_ASSERT(got == 0, "directory_get returns 0 for second entry");
    CTEST_ASSERT(strcmp(ent.name, ".") == 0 || strcmp(ent.name, "..") == 0, "entry name is valid");

    got = directory_get(dir, &ent);
    CTEST_ASSERT(got == -1, "directory_get returns -1 after entries are read");

    directory_close(dir);
}

void test_namei(void){
    mkfs();
    directory_make("/foo");
    struct inode *root = namei("/");
    CTEST_ASSERT(root != NULL, "found root inode");
    struct inode *foo = namei("/foo");
    CTEST_ASSERT(foo != NULL, "found /foo directory inode");
    struct inode *missing = namei("/bar");
    CTEST_ASSERT(missing == NULL, "missing directory returns NULL");
}
//old test namei
//     char *path = "/";
//     struct inode* incore_inode_rootdir = namei(path);
//     CTEST_ASSERT(incore_inode_rootdir != NULL, "successfully found incore inode for root director");
// }

int main(void){
    //call image_open and image_close
    CTEST_VERBOSE(1);
    test_image_open();
    test_image_close();
    test_free();
    //test_inode();
    test_block();
    test_incore_inode();
    //test_write_read_inode();
    test_iget_iput();
    test_ialloc();
    test_mkdir();
    test_namei();
    CTEST_RESULTS();
    CTEST_EXIT();
    test_directory_get();
}

#else 
int main(void){
    printf("Running normally!\n");
}
#endif