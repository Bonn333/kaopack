#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pak_t8fm.h"
#include "pak_tate.h"

char *program_name;
int version_num[3] = {0, 0, 1};

void usage(void)
{
    fprintf(stderr, "usage: %s [options] path\n", program_name);
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t -l \t list entries in archive \n");
    /*fprintf(stderr, "\t -u \t unpack archive \n");*/
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
    fseek(fd, 0, SEEK_CUR);
    fread(pak_magic, sizeof(pak_magic), 1, fd);
    if (strncmp(pak_magic, PAK_TATE_MAGIC, sizeof(pak_magic)) == 0) return 2;

    return -1;
}


void tate_list_all(FILE* fd)
{
    struct pak_tate_item_entry *entry;
    while ((entry = pak_tate_readdir(fd)))
    {
        printf(entry->name);
    }
}


int main(int argc, char *argv[])
{
    char opt;
    FILE *pak_file;
    char *pak_file_path;
    int pak_ver;
    char prog_mode = '\0';

    if (argv[0]) program_name = argv[0];

    while ((opt = getopt(argc, argv, "hl:v")) != -1) {
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
                pak_file_path = strdup(optarg);
            }
            break;
        /*case 'u':
            if (prog_mode)
            {
                usage();
                exit(EXIT_SUCCESS);
            }
            else
            {
                prog_mode = 'u';
                pak_file_path = strdup(optarg);
            }
            break;
        case 'p':
            if (prog_mode)
            {
                usage();
                exit(EXIT_SUCCESS);
            }
            else
            {
                prog_mode = 'p';
                pak_file_path = strdup(optarg);
            }
            break;*/
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
    pak_file = fopen(pak_file_path, "rb");
    if (!pak_file)
    {
        fprintf(stderr, "Cannot open file \"%s\"\n", pak_file_path);
        exit(EXIT_FAILURE);
    }

    pak_ver = pak_identify(pak_file);

    switch (prog_mode)
    {
    case 'l':
        if (pak_ver == 2) tate_list_all(pak_file);
        break;
    case 'u':
        break;
    case 'p':
        break;
    default:
        usage();
        exit(EXIT_FAILURE);
        break;
    }

    return EXIT_SUCCESS;
}
