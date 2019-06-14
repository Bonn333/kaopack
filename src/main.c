#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mkdirp/mkdirp.h"

#include "pak_t8fm.h"
#include "pak_tate.h"

#define TESTPATH_LEN 512

char *program_name;
int version_num[3] = {0, 0, 2};

char * fix_path(char *path)
{
    int i;
    char *copy;
    if ((*path == '\\') || (*path == '/'))
        copy = strdup(path+1);
    else copy = strdup(path);

    for (i = 0; copy[i]; i++)
    {
        if (*(copy+i) == '\\') *(copy+i) = '/';
    }
    return copy;
}

char * path_dirs(char *path)
{
    char *pos = strrchr(path, '/');
    if (pos != NULL) {
        *pos = '\0';
    }

    return pos;
}

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
    rewind(fd);
    fread(pak_magic, sizeof(pak_magic), 1, fd);
    rewind(fd);

    if (strncmp(pak_magic, PAK_TATE_MAGIC, sizeof(pak_magic)) == 0) return 2;

    return -1;
}

void tate_print_header(struct pak_tate_header *header)
{
    int i;

    if (!header) return;

    printf("h,%.48s,%d,%d,%d\n",
           header->stream_name, header->files, header->langs, header->size);
    if (header->langs > 8)
    {
        puts("Invalid count of languages");
    }
    else
    {
        for (i = 0; i < header->langs; i++)
        {
            printf("l,%d,%.4s,%d\n", i, header->lang[i].code, header->lang[i].size);
        }
    }
}

void tate_print_item(struct pak_tate_item *entry)
{
    printf("i,%d,%s,%d\n", entry->id, entry->name, entry->size);
}

void tate_list_all(FILE *fd)
{
    struct pak_tate_item *entry;
    while ((entry = pak_tate_get_item(fd)))
    {
        tate_print_item(entry);
    }

}

void tate_unpack_all(FILE *fd)
{
    int ret;

    struct pak_tate_item *entry;
    void *data;
    long data_size;

    char *path;
    char *slashptr;
    FILE *output;

    rewind(fd);
    while ((entry = pak_tate_get_item(fd)))
    {
        tate_print_item(entry);

        fseek(fd, -PAK_TATE_BLOCK_SIZE, SEEK_CUR);
        data_size = pak_tate_extract_item(fd, entry->name, &data);
        if (data_size < 0) return;

        path = fix_path(entry->name);
        slashptr = path_dirs(path);

        mkdirp(path, 0755);
        *slashptr = '/';
        output = fopen(path, "wb");

        ret = fwrite(data, data_size, 1, output);
        fclose(output);
        free(path);
        free(data);
    }
}


int main(int argc, char *argv[])
{
    int ret;
    char opt;
    FILE *input;
    char *input_path;
    int pak_ver;
    char prog_mode = '\0';
    /* Ale bałagan, funkcje dla danych typów należy przerzucić do dedykowanych im plików! */
    struct pak_tate_header *tate_header = NULL;
    char testpath[TESTPATH_LEN];

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
            tate_header = pak_tate_get_header(input);
            tate_print_header(tate_header);
            tate_list_all(input);
        }
        break;
    case 'p':
        break;
    case 'u':
        if (pak_ver == 2)
        {
            /* Make work directory */
            tate_header = pak_tate_get_header(input);


            #ifdef __linux__
            ret = mkdir(tate_header->stream_name, 0777);
            #else
            ret = mkdir(tate_header->stream_name);
            #endif
            chdir(tate_header->stream_name);
            getcwd(testpath, sizeof(testpath));

            /* Unpack and list everything from container */
            tate_print_header(tate_header);
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
