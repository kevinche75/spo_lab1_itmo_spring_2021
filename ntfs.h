//
// Created by kevinche on 17.03.2021.
//

#include <stdint.h>


#ifndef SPO_LAB1_WINDEF_H
#define SPO_LAB1_WINDEF_H

#endif //SPO_LAB1_WINDEF_H

typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef unsigned short WORD;
typedef unsigned short USHORT;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef int64_t LONGLONG;
typedef uint64_t ULARGE_INTEGER;
typedef uint64_t sector_t;
typedef uint64_t block_t;
#define NTFS_MAX_FILE_NAME_LEN 255
#define NTFS_MFT_REF_MASK 0x0000ffffffffffff

struct ntfs_bpb {
    uint8_t jmp_boot[3];
    char oem_name[8];
    uint16_t sector_size; // bytes_per_sector
    uint8_t sec_per_clust;
    uint16_t res_sectors;
    uint8_t zero_0[3];
    uint16_t zero_1;
    uint8_t media;
    uint16_t zero_2;
    uint16_t unused_0;
    uint16_t unused_1;
    uint32_t unused_2;
    uint32_t zero_3;
    uint32_t unused_3;
    uint64_t total_sectors;
    uint64_t mft_lclust;
    uint64_t mft_mirr_lclust;
    int8_t clust_per_mft_record;
    uint8_t unused_4[3];
    uint8_t clust_per_idx_record;
    uint8_t unused_5[3];
    uint64_t vol_serial;
    uint32_t unused_6;
    uint8_t pad[428];       /* padding to a sector boundary (512 bytes) */
} __attribute__((__packed__));

struct ntfs_mft_record {
    uint32_t magic;
    uint16_t usa_ofs;
    uint16_t usa_count;
    uint64_t lsn;
    uint16_t seq_no;
    uint16_t link_count;
    uint16_t attrs_offset;
    uint16_t flags;     /* MFT record flags */
    uint32_t bytes_in_use;
    uint32_t bytes_allocated;
    uint64_t base_mft_record;
    uint16_t next_attr_instance;
    uint16_t reserved;
    uint32_t mft_record_no;
} __attribute__((__packed__));   /* 48 bytes */

struct ntfs_attr_record {
    uint32_t type;      /* Attr. type code */
    uint32_t len;
    uint8_t non_resident;
    uint8_t name_len;
    uint16_t name_offset;
    uint16_t flags;     /* Attr. flags */
    uint16_t instance;
    union {
        struct {    /* Resident attribute */
            uint32_t value_len;
            uint16_t value_offset;
            uint8_t flags;  /* Flags of resident attributes */
            int8_t reserved;
        } __attribute__((__packed__)) resident;
        struct {    /* Non-resident attributes */
            uint64_t lowest_vcn;
            uint64_t highest_vcn;
            uint16_t mapping_pairs_offset;
            uint8_t compression_unit;
            uint8_t reserved[5];
            int64_t allocated_size;
            int64_t data_size; /* Byte size of the attribute value.
                                * Note: it can be larger than
                                * allocated_size if attribute value is
                                * compressed or sparse.
                                */
            int64_t initialized_size;
            int64_t compressed_size;
        } __attribute__((__packed__)) non_resident;
    } __attribute__((__packed__)) data;
} __attribute__((__packed__));

//MFT FLAGS
enum {
    MFT_RECORD_IN_USE       = 0x0001,
    MFT_RECORD_IS_DIRECTORY = 0x0002,
} __attribute__((__packed__));

/* The $MFT metadata file types */
enum ntfs_system_file {
    FILE_MFT            = 0,
    FILE_MFTMirr        = 1,
    FILE_LogFile        = 2,
    FILE_Volume         = 3,
    FILE_AttrDef        = 4,
    FILE_root           = 5,
    FILE_Bitmap         = 6,
    FILE_Boot           = 7,
    FILE_BadClus        = 8,
    FILE_Secure         = 9,
    FILE_UpCase         = 10,
    FILE_Extend         = 11,
    FILE_reserved12     = 12,
    FILE_reserved13     = 13,
    FILE_reserved14     = 14,
    FILE_reserved15     = 15,
    FILE_reserved16     = 16,
};

struct ntfs_sb_info {
    block_t mft_blk;                /* The first MFT record block */
    uint64_t mft_lcn;               /* LCN of the first MFT record */
    unsigned mft_size;              /* The MFT size in sectors */
    uint64_t mft_record_size;       /* MFT record size in bytes */

    uint8_t clust_per_idx_record;   /* Clusters per Index Record */

    unsigned long long clusters;    /* Total number of clusters */

    unsigned clust_shift;           /* Based on sectors */
    unsigned clust_byte_shift;      /* Based on bytes */
    unsigned clust_mask;
    unsigned clust_size;
    unsigned sector_size;
    unsigned sector_shift;
    unsigned block_size;
    unsigned block_shift;

    int fd;
} __attribute__((__packed__));

enum {
    NTFS_AT_UNUSED                      = 0x00,
    NTFS_AT_STANDARD_INFORMATION        = 0x10,
    NTFS_AT_ATTR_LIST                   = 0x20,
    NTFS_AT_FILENAME                    = 0x30,
    NTFS_AT_OBJ_ID                      = 0x40,
    NTFS_AT_SECURITY_DESCP              = 0x50,
    NTFS_AT_VOL_NAME                    = 0x60,
    NTFS_AT_VOL_INFO                    = 0x70,
    NTFS_AT_DATA                        = 0x80,
    NTFS_AT_INDEX_ROOT                  = 0x90,
    NTFS_AT_INDEX_ALLOCATION            = 0xA0,
    NTFS_AT_BITMAP                      = 0xB0,
    NTFS_AT_REPARSE_POINT               = 0xC0,
    NTFS_AT_EA_INFO                     = 0xD0,
    NTFS_AT_EA                          = 0xE0,
    NTFS_AT_PROPERTY_SET                = 0xF0,
    NTFS_AT_LOGGED_UTIL_STREAM          = 0x100,
    NTFS_AT_FIRST_USER_DEFINED_ATTR     = 0x1000,
    NTFS_AT_END                         = 0xFFFFFFFF,
};

enum {
    /* Found in $MFT/$DATA */
    NTFS_MAGIC_FILE     = 0x454C4946,   /* MFT entry */
    NTFS_MAGIC_INDX     = 0x58444E49,   /* Index buffer */
    NTFS_MAGIC_HOLE     = 0x454C4F48,

    /* Found in $LogFile/$DATA */
    NTFS_MAGIC_RSTR     = 0x52545352,
    NTFS_MAGIC_RCRD     = 0x44524352,
    /* Found in $LogFile/$DATA (May be found in $MFT/$DATA, also ?) */
    NTFS_MAGIC_CHKDSK   = 0x444B4843,
    /* Found in all ntfs record containing records. */
    NTFS_MAGIC_BAAD     = 0x44414142,
    NTFS_MAGIC_EMPTY    = 0xFFFFFFFF,   /* Record is empty */
};

struct ntfs_inode{
    uint32_t mft_no;
    char* filename;
    uint16_t type;
    struct ntfs_inode *parent; /*parent directory*/
    struct ntfs_inode *next_inode; /*connected list of files and dirs in dir or next inode in connected list*/
};

struct ntfs_filename_attr {
    uint64_t parent_directory;
    int64_t ctime;
    int64_t atime;
    int64_t mtime;
    int64_t rtime;
    uint64_t allocated_size;
    uint64_t data_size;
    uint32_t file_attrs;
    union {
        struct {
            uint16_t packed_ea_size;
            uint16_t reserved;      /* reserved for alignment */
        } __attribute__((__packed__)) ea;
        struct {
            uint32_t reparse_point_tag;
        } __attribute__((__packed__)) rp;
    } __attribute__((__packed__)) type;
    uint8_t file_name_len;
    uint8_t file_name_type;
    uint16_t file_name[0];          /* File name in Unicode */
} __attribute__((__packed__));

//header of $INDEX_ROOT and $INDEX_ALLOCATION
struct ntfs_idx_header {
    uint32_t entries_offset;
    uint32_t index_len;
    uint32_t allocated_size;
    uint8_t flags;              /* Index header flags */
    uint8_t reserved[3];        /* Align to 8-byte boundary */
} __attribute__((__packed__));

//body if $INDEX_ROOT body + header
struct ntfs_idx_root {
    uint32_t type;  /* It is $FILE_NAME for directories, zero for view indexes.
                     * No other values allowed.
                     */
    uint32_t collation_rule;
    uint32_t index_block_size;
    uint8_t clust_per_index_block;
    uint8_t reserved[3];
    struct ntfs_idx_header index;
} __attribute__((__packed__));

//index element in index record
struct ntfs_idx_entry {
    union {
        struct { /* Only valid when INDEX_ENTRY_END is not set */
            uint64_t indexed_file;
        } __attribute__((__packed__)) dir;
        struct { /* Used for views/indexes to find the entry's data */
            uint16_t data_offset;
            uint16_t data_len;
            uint32_t reservedV;
        } __attribute__((__packed__)) vi;
    } __attribute__((__packed__)) data;
    uint16_t len;
    uint16_t key_len;
    uint16_t flags;     /* Index entry flags */
    uint16_t reserved;  /* Align to 8-byte boundary */
    union {
        struct ntfs_filename_attr file_name;
        //SII_INDEX_KEY sii;
        //SDH_INDEX_KEY sdh;
        //GUID object_id;
        //REPARSE_INDEX_KEY reparse;
        //SID sid;
        uint32_t owner_id;
    } __attribute__((__packed__)) key;
} __attribute__((__packed__));

enum {
    INDEX_ENTRY_NODE            = 1,
    INDEX_ENTRY_END             = 2,
    /* force enum bit width to 16-bit */
    INDEX_ENTRY_SPACE_FILTER    = 0xFFFF,
} __attribute__((__packed__));

enum {
    MAP_UNSPEC,
    MAP_START           = 1 << 0,
    MAP_END             = 1 << 1,
    MAP_ALLOCATED       = 1 << 2,
    MAP_UNALLOCATED     = 1 << 3,
    MAP_MASK            = 0x0000000F,
};

struct mapping_chunk {
    uint64_t length;
    uint8_t *buf;
};

struct ntfs_bpb *open_file_system(int fd);
struct ntfs_sb_info *ntfs_init(char *name);
//int32_t ilog2(uint32_t x);
//static uint64_t mft_record_lookup(struct ntfs_sb_info *fs,
//                                  uint32_t file,
//                                  struct ntfs_mft_record *mft_record);
//
//static struct ntfs_attr_record *__ntfs_attr_lookup(struct ntfs_sb_info *fs,
//                                                   uint32_t type,
//                                                   struct ntfs_mft_record *mft_record);
//
//static int ntfs_readdir(struct ntfs_sb_info *fs, struct ntfs_inode *inode);
