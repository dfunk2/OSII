#ifndef DIR_H
#define DIR_H

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

void root_directory(void);
struct directory *directory_open(int inode_num);
void directory_close(struct directory *d);

#endif