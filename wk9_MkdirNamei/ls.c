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

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: ls <path>\n");
        return 1;
    }

    if (image_open("test.txt", 0) == -1){
        printf("Failed to open filesystem image.\n");
        return 1;
    }

    ls(argv[1]);
    image_close();
    return 0;
}
