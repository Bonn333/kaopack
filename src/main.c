#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pak_t8fm.h"
#include "pak_tate.h"

char *program_name;
int version_num[3] = {0, 0, 2};

void usage(void)
{
    fprintf(stderr, "usage: %s [options] path\n", program_name);
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t -l \t list entries in archive \n");
    fprintf(stderr, "\t -u \t unpack archive \n");
    /*fprintf(stderr, "\t -p \t pack directory \n");*/
    fprintf(stderr, "\t -v \t print version \n");
    fprintf(stderr, "\t -h \t print this help \n");
}

void version(void)
{
    printf("kaopack %d.%d.%d\n", version_num[0], version_num[1], version_num[2]);
    printf("(C) Rafał Kołucki, 2019\n");
}

int pak_identify(FILE* fd)
{
    char pak_magic[4];

    if (!fd) return 0;

    /* Check if it's Kao 2 PAK */
    fseek(fd, 0, SEEK_SET);
    fread(pak_magic, sizeof(pak_magic), 1, fd);
    fseek(fd, 0, SEEK_SET);

    if (strncmp(pak_magic, PAK_TATE_MAGIC, sizeof(pak_magic)) == 0) return 2;

    return -1;
}

void tate_print_header(FILE* fd)
{
    int i;
    struct pak_tate_header *header;

    header = pak_tate_get_header(fd);

    printf("Stream %.48s contains %d files in %d languages within %d bytes\n",
           header->stream_name, header->files, header->langs, header->size);
    if (header->langs > 8)
    {
        puts("Invalid count of languages");
    }
    else
    {
        puts("Languages: ");
        for (i = 0; i < header->langs; i++)
        {
            printf("\t%d: %.4s size: %d\n", i, header->lang[i].code, header->lang[i].size);
        }
    }
}

void tate_print_items(FILE* fd)
{
    struct pak_tate_item *entry;
/*    fpos_t pos;*/
    puts("Items: ");
/*    fgetpos(fd, &pos);
    printf("0x%x: ", pos);*/
    rewind(fd);
    while ((entry = pak_tate_get_item(fd)))
    {
        printf("\t%d: %s (%d)\n", entry->id, entry->name, entry->size);
        /*fgetpos(fd, &pos);
        printf("0x%x: ", pos);*/
    }
}


void tate_unpack_all(FILE* fd)
{
    struct pak_tate_item *entry;
    void *data;
    long data_size;

    rewind(fd);
    while ((entry = pak_tate_get_item(fd)))
    {
        printf("%d,%s,%d\n", entry->id, entry->name, entry->size);
        fseek(fd, -PAK_TATE_BLOCK_SIZE, SEEK_CUR);
        data_size = pak_tate_extract_item(fd, entry->name, &data);
        if (data_size < 0) return;
        /* TODO: make directories recursively */
        /* TODO: create file itself */
        /* TODO: append data to new file when size > 0 */
        /* TODO: close file */
        free(data);
    }
}


int main(int argc, char *argv[])
{
    char opt;
    FILE *input;
    char *input_path;
    int pak_ver;
    char prog_mode = '\0';

    if (argv[0]) program_name = argv[0];

    while ((opt = getopt(argc, argv, "hl:u:v")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
            break;
        case 'l':
            if (prog_mode)
            {
                usage();
                exit(EXIT_SUCCESS);
            }
            else
            {
                prog_mode = 'l';
                input_path = strdup(optarg);
            }
            break;
        /*case 'p':
            if (prog_mode)
            {
                usage();
                exit(EXIT_SUCCESS);
            }
            else
            {
                prog_mode = 'p';
                input_path = strdup(optarg);
            }
            break;*/
        case 'u':
            if (prog_mode)
            {
                usage();
                exit(EXIT_SUCCESS);
            }
            else
            {
                prog_mode = 'u';
                input_path = strdup(optarg);
            }
            break;
        case 'v':
            version();
            exit(EXIT_SUCCESS);
            break;
        default: /* '?' */
            usage();
            exit(EXIT_FAILURE);
        }
    }

    if ((argc <= 1) && (opt == -1))
    {
        usage();
        exit(EXIT_FAILURE);
    }

    /* Open source file for reading */
    input = fopen(input_path, "rb");
    if (!input)
    {
        fprintf(stderr, "Cannot open file \"%s\"\n", input_path);
        exit(EXIT_FAILURE);
    }

    pak_ver = pak_identify(input);

    switch (prog_mode)
    {
    case 'l':
        if (pak_ver == 2)
        {
            tate_print_header(input);
            tate_print_items(input);
        }
        break;
    case 'p':
        break;
    case 'u':
        if (pak_ver == 2)
        {
            tate_print_header(input);
            tate_unpack_all(input);
        }
        break;
    default:
        usage();
        exit(EXIT_FAILURE);
        break;
    }

    return EXIT_SUCCESS;
}
