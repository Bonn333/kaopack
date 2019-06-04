#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pak_tate.h"

/* Seeks to beginning of file and checks if it's valid PAK container */
/* Returns NULL when it's not a valid PAK file
 * or pointer to header copy for further processing */
struct pak_tate_header *pak_tate_get_header(FILE *pak)
{
    static struct pak_tate_header header;
    int ret;

    rewind(pak);
    ret = fread(&header, sizeof(header), 1, pak);

    if (ret != 1) return NULL;
    if (strncmp(header.magic, PAK_TATE_MAGIC, sizeof(header.magic)) == 0)
        return &header;
    return NULL; /* ? */
}

/* Seeks to next block after header */
/* Returns NULL when cannot fetch next item (EOL?)
 * or pointer to copy of item entry for further processing */
struct pak_tate_item * pak_tate_get_item(FILE *pak)
{
    static struct pak_tate_item item;
    int ret;

    do {
        ret = fread(&item, sizeof(item), 1, pak);
        if (!ret) return NULL;
    } while (strncmp(item.magic, PAK_TATE_ITEM_MAGIC, sizeof(item.magic)));

    return &item;
}

/* Extracts file to memory */
/* Uses dynamic memory, remember to free() extracted data after further processing! */
/* Returns >= 0 when found (file size) or <0 when not found */
int pak_tate_extract_item(FILE *pak, char *path, void** data)
{
    int ret;
    void *data_buf;
    size_t data_size;
    struct pak_tate_item *item;

    /* Find requested file */
    do {
        item = pak_tate_get_item(pak);
    } while (strncmp(item->name, path, sizeof(item->name)) != 0);

    data_size = item->size;
    if (data_size == 0)
    {
        *data = NULL;
        return 0;
    }

    /* Allocate memory for data */
    data_buf = malloc(data_size);
    if (!data_buf) return -1;

    ret = fread(data_buf, data_size, 1, pak);
    if (!ret)
    {
        free(data_buf);
        return 1;
    }

    /* Skip block padding and return extracted data */
    fseek(pak, (PAK_TATE_BLOCK_SIZE * pak_tate_calc_blocks(data_size) - data_size), SEEK_CUR);
    *data = data_buf;
    return data_size;
}

/* Appends file to PAK container */
/* Returns 0 if successfully copied
 * or other number in case of error */
/*int pak_tate_append_item(FILE *pak, pak_tate_item *header, void* data)
{
    size_t data_size_fill;
}*/
