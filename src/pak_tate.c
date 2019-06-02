#include <stdio.h>
#include <string.h>

#include "pak_tate.h"

struct pak_tate_item_entry *pak_tate_readdir(FILE *pak)
{
    static struct pak_tate_item_entry item;
    int ret;
    do {
        ret = fread(&item, sizeof(item), 1, pak);
        if (!ret) return 0;
    } while (strncmp(item.magic, PAK_TATE_ITEM_MAGIC, sizeof(item.magic)));

    return &item;
}
