#include <stdio.h>
#include "dir.h"

void ls(void) {
    struct directory *dir;
    struct directory_entry ent;

    dir = directory_open(0);
    if (!dir) {
        printf("Failed to open root directory.\n");
        return;
    }

    while (directory_get(dir, &ent) != -1)
        printf("%d %s\n", ent.inode_num, ent.name);

    directory_close(dir);
}
