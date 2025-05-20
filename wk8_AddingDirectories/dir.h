#ifndef DIR_H
#define DIR_H

struct directory {
    int inode_num; //2 bytes
    char name[16]; //16 bytes
    int reserved; //14 bytes
};

void root_directory(void);

#endif