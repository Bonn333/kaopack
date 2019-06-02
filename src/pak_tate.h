#define PAK_TATE_MAGIC "TATE"
#define PAK_TATE_ITEM_MAGIC "item"

struct pak_tate_header
{
    char magic[4];
    unsigned int size;
    unsigned int file_count;
    unsigned int lang_count;
    char stream_name[48];
};

struct pak_tate_lang_entry
{
    char code[4];
    unsigned int size;
};

struct pak_tate_item_entry
{
    char magic[4];
    unsigned int size;
    unsigned int id;
    unsigned int padding;
    char name[112];
};

struct pak_tate_item_entry *pak_tate_readdir(FILE *pak);
