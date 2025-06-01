#include <stdio.h>
#include "dir.h"
#include "inode.h"

void ls(const char *path) {
    struct inode *inode = namei((char *)path);
    if (!inode){
        printf("No such directory: %s\n", path);
        return;
    }

    struct directory *dir = directory_open(inode->inode_num);
    if (!dir){
        printf("Failed to open directory at path %s\n", path);
        iput(inode);
        return;

    }

    struct directory_entry ent;
    while (directory_get(dir, &ent) != -1)
        printf("%d %s\n", ent.inode_num, ent.name);

    directory_close(dir);
    iput(inode);

    // struct directory *dir;
    // struct directory_entry ent;

    // dir = directory_open(0);
    // if (!dir) {
    //     printf("Failed to open root directory.\n");
    //     return;
    // }

    // while (directory_get(dir, &ent) != -1)
    //     printf("%d %s\n", ent.inode_num, ent.name);

    // directory_close(dir);
}
