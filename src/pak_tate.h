#define PAK_TATE_MAGIC "TATE"
#define PAK_TATE_ITEM_MAGIC "item"
#define PAK_TATE_BLOCK_SIZE 0x80

#define pak_tate_calc_blocks(B) (B / PAK_TATE_BLOCK_SIZE + (B % PAK_TATE_BLOCK_SIZE != 0))

struct pak_tate_lang
{
    char code[4];
    unsigned int size;
};

struct pak_tate_header
{
    char magic[4];
    unsigned int size;
    unsigned int files;
    unsigned int langs;
    char stream_name[48];
    struct pak_tate_lang lang[8];
};

struct pak_tate_item
{
    char magic[4];
    unsigned int size;
    unsigned int id;
    unsigned int padding;
    char name[112];
};

struct pak_tate_header *pak_tate_get_header(FILE *pak);
struct pak_tate_item *pak_tate_get_item(FILE *pak);
int pak_tate_extract_item(FILE *pak, char *path, void** data);
