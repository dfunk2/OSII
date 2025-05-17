#include "inode.h"
#include "ctest.h"
#include "free.h"
#include "block.h"
#include "pack.h"
#include <stdio.h>

#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
//static struct inode incore holds in-core inodes and is dynamically loaded and evicted as files are closed or opened
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
    //didnt find a free inode
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
//allocate a free inode in the inode map, set and return pointer to incore inode
struct inode *ialloc(void){
    unsigned char *inode_map = bread(1, buffer_block);
    //locate a free inode
    int free_inode = find_free(inode_map);
    if (free_inode == -1){
        return NULL;
    }
    //see if existing, read from disk and load to memory
    struct inode *iget_inode = iget(free_inode);
    if(iget_inode == NULL){
        return NULL;
    }

    //mark as used
    set_free(inode_map, free_inode, 1);

    //initalize the inode 
    iget_inode->size = 0;
    iget_inode->owner_id = 0;
    iget_inode->permissions = 0;
    iget_inode->flags = 0;
    for(int i = 0; i < INODE_PTR_COUNT; i++){
        iget_inode->block_ptr[i] = 0;
    }

    iget_inode->inode_num = free_inode;

    //save inode to disk
    bwrite(1, inode_map);

    return iget_inode;
}

//stores the inode data pointed to by in on disk
void write_inode(struct inode *in){
    //map inode number to a block and offset
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = in->inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    //read the data from disk into a block and pack the new inode data
    bread(block_num, buffer_block);

    write_u32(buffer_block + block_offset_bytes + 0, in->size);
    write_u16(buffer_block + block_offset_bytes + 4, in->owner_id);
    write_u8(buffer_block + block_offset_bytes + 6, in->permissions);
    write_u8(buffer_block + block_offset_bytes + 7, in->flags);
    write_u8(buffer_block + block_offset_bytes + 8, in->link_count);
    for(int i = 0; i < INODE_PTR_COUNT; i++){
        write_u16(buffer_block + block_offset_bytes + ((i * 2) + 9), in->block_ptr[i]);
    }

    //write the updated block back out to disk
    bwrite(block_num, buffer_block);

}

//take a pointer to an empty struct inode and put data into it
void read_inode(struct inode *in, int inode_num){
    //map inode number to a block and offset 
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    //read data from disk into a block and unpack it
    bread(block_num, buffer_block);

    unsigned int File_size = read_u32(buffer_block + block_offset_bytes + 0);
    unsigned int oID = read_u16(buffer_block + block_offset_bytes + 4);
    unsigned int perm = read_u8(buffer_block + block_offset_bytes + 6);
    unsigned int flags = read_u8(buffer_block + block_offset_bytes + 7);
    unsigned int LinkCount = read_u8(buffer_block + block_offset_bytes + 8);
    
    //store the results in struct inode
    in->size = File_size;
    in->owner_id = oID;
    in->permissions = perm;
    in->flags = flags;
    in->link_count = LinkCount;

    for(int i = 0; i < INODE_PTR_COUNT; i++){
        unsigned int BP = read_u16(buffer_block + block_offset_bytes + ((i * 2) + 9));
        in->block_ptr[i] = BP;
    }
    in->ref_count = 0;
    in->inode_num = inode_num;
}

//return a pointer to an incore inode for a given inode number or NULL on failure
//iget, gets data from incore memory, if not, load it from disk
struct inode *iget(int inode_num){
    
    //inode already in-core 
    struct inode *in = incore_find(inode_num);
    if(in != 0){
        in->ref_count += 1;
        return in;
    } 
    //find a free in-core inode 
    struct inode *free = incore_find_free();
    //none found
    if (free == NULL){
        return NULL;
    }
    //read from disk load data into memory (struct inode)
    read_inode(free, inode_num);
    free->ref_count++;
    free->inode_num = inode_num;
    return free;
}

//opposite of iget(). Free the inode if no one is using it
void iput(struct inode *in){
    if (in->ref_count == 0){
        return;
    }else {
        in->ref_count--;
    }
    write_inode(in);

}
