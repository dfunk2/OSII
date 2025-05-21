#ifndef DIR_H
#define DIR_H

struct directory {
    struct inode *inode;
    unsigned int offset;
};

void root_directory(void);
struct directory *directory_open(int inode_num);
void directory_close(struct directory *d);

#endif