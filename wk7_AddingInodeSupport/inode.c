#include "inode.h"
#include "ctest.h"
#include "free.h"
#include "block.h"

#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};
unsigned char buffer_block[BLOCK_SIZE];

//find first free in-core inode in the incore array
struct inode *incore_find_free(void){
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++){
        //point to address of current inode
        struct inode *free = &incore[i];
        if (free->ref_count == 0){
            return free;
        }
    }
    return 0;
}

//find in-core inode record in the incore array by inode number 
struct inode *incore_find(unsigned int inode_num){
    for(int i = 0; i< MAX_SYS_OPEN_FILES; i++){
        struct inode *curr = &incore[i];
        if(curr->inode_num == inode_num && curr->ref_count != 0){
            return curr;
        }
    }
    //searched whole array didnt find inode num
    return 0;
}

//set ref_count to all in-core inodes to 0
void incore_free_all(void){
    for(int i = 0; i < MAX_SYS_OPEN_FILES; i++){
         struct inode *set = &incore[i];
         set->ref_count = 0;
    }

}

int ialloc(void){
    //allocate a previously-free inode in the inode map
    //get inode map, pass 1, thats the inode map block num
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

//take a pointer to an empty struct inode and put data into it
void read_inode(struct inode *in, int inode_num){
    

}
