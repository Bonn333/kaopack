#define PAK_T8FM_MAGIC "T8FM"

struct pak_t8fm_footer
{
    unsigned int file_count;
    unsigned int file_table_ptr;
    char magic[4];
};

struct pak_t8fm_entry
{
    char name[80];
    unsigned int offset;
    unsigned int size;
};
