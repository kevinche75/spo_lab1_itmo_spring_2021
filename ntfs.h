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

typedef struct _BIOS_PARAMETER_BLOCK {
/*0x0b*/USHORT bytes_per_sector;    /* Размер сектора, в байтах */
/*0x0d*/UCHAR sectors_per_cluster;    /* Секторов в кластере */
/*0x0e*/USHORT reserved_sectors;        /* должен быть ноль */
/*0x10*/UCHAR fats;            /* должен быть ноль */
/*0x11*/USHORT root_entries;        /* должен быть ноль */
/*0x13*/USHORT sectors;            /* должен быть ноль */
/*0x15*/UCHAR media_type;        /* тип носителя, 0xf8 = hard disk */
/*0x16*/USHORT sectors_per_fat;        /* должен быть ноль */
/*0x18*/USHORT sectors_per_track;    /* не используется */
/*0x1a*/USHORT heads;            /* не используется */
/*0x1c*/ULONG hidden_sectors;        /* не используется */
/*0x20*/ULONG large_sectors;        /* должен быть ноль */
/* sizeof() = 25 (0x19) bytes */
} BIOS_PARAMETER_BLOCK, *PBIOS_PARAMETER_BLOCK;

typedef struct _NTFS_BOOT_SECTOR {
/*0x00*/UCHAR jump[3];            /* переход на загрузочный код */
/*0x03*/ULARGE_INTEGER oem_id;    /* сигнатура "NTFS    ". */
/*0x0b*/BIOS_PARAMETER_BLOCK bpb;
/*0x24*/UCHAR physical_drive;        /* не используется */
/*0x25*/UCHAR current_head;        /* не используется */
/*0x26*/UCHAR extended_boot_signature; /* не используется */
/*0x27*/UCHAR reserved2;            /* не используется */
/*0x28*/ULARGE_INTEGER number_of_sectors;    /* Количество секторов на томе. */
/*0x30*/ULARGE_INTEGER mft_lcn;    /* Стартовый кластер MFT. */
/*0x38*/ULARGE_INTEGER mftmirr_lcn;/* Стартовый кластер копии MFT */
/*0x40*/CHAR clusters_per_mft_record;    /* Размер MFT записи в кластерах. */
/*0x41*/UCHAR reserved0[3];        /* зарезервировано */
/*0x44*/CHAR clusters_per_index_record;/* Размер индексной записи в кластерах. */
/*0x45*/UCHAR reserved1[3];        /* зарезервировано */
/*0x48*/ULARGE_INTEGER volume_serial_number;    /* уникальный серийный номер тома */
/*0x50*/ULONG checksum;            /* не используется */
/*0x54*/UCHAR bootstrap[426];        /* загрузочный-код */
/*0x1fe*/USHORT end_of_sector_marker;    /* конец загрузочного сектора, сигнатура 0xaa55 */
/* sizeof() = 512 (0x200) bytes */
} NTFS_BOOT_SECTOR, *PNTFS_BOOT_SECTOR;

NTFS_BOOT_SECTOR *openFileSystem(char *name);

typedef enum {
    MFT_RECORD_NOT_USED = 0, //запись не используется
    MFT_RECORD_IN_USE = 1, //запись используется
    MFT_RECORD_IS_DIRECTORY = 2 //запись описывает каталог
} MFT_RECORD_FLAGS;

typedef struct _MFT_RECORD {
/*0x00*/    ULONG signature; //сигнатура 'FILE'
/*0x04*/    USHORT usa_offs;
/*0x06*/    USHORT usa_count;
/*0x08*/    ULARGE_INTEGER lsn;
/*0x10*/    USHORT sequence_number;
/*0x12*/    USHORT link_count;
/*0x14*/    USHORT attrs_offset;
/*0x16*/    USHORT flags;//флаги, см. MFT_RECORD_FLAGS
/*0x18*/    ULONG bytes_in_use;
/*0x1C*/    ULONG bytes_allocated;
/*0x20*/    ULARGE_INTEGER base_mft_record; //адрес базовой MFT-записи
/*0x28*/    USHORT next_attr_instance;
/*0x2A*/    USHORT reserved;
/*0x2C*/    ULONG mft_record_number;
//size - 48 b
} MFT_RECORD, *PMFT_RECORD;

//struct MFT_REF
//{
//    unsigned int64_t index : 48; //индекс элемента в таблице
//    unsigned int64_t ordinal : 16; //порядковый номер
//};

typedef enum {
    NTFS_AT_UNUSED = 0x00,
    NTFS_AT_STANDARD_INFORMATION = 0x10,
    NTFS_AT_ATTR_LIST = 0x20,
    NTFS_AT_FILENAME = 0x30,
    NTFS_AT_OBJ_ID = 0x40,
    NTFS_AT_SECURITY_DESCP = 0x50,
    NTFS_AT_VOL_NAME = 0x60,
    NTFS_AT_VOL_INFO = 0x70,
    NTFS_AT_DATA = 0x80,
    NTFS_AT_INDEX_ROOT = 0x90,
    NTFS_AT_INDEX_ALLOCATION = 0xA0,
    NTFS_AT_BITMAP = 0xB0,
    NTFS_AT_REPARSE_POINT = 0xC0,
    NTFS_AT_EA_INFO = 0xD0,
    NTFS_AT_EA = 0xE0,
    NTFS_AT_PROPERTY_SET = 0xF0,
    NTFS_AT_LOGGED_UTIL_STREAM = 0x100,
    NTFS_AT_FIRST_USER_DEFINED_ATTR = 0x1000,
    NTFS_AT_END = 0xFFFFFFFF,
} ATTR_TYPES;

typedef enum {
    NTFS_FILE_ATTR_READONLY                     = 0x00000001,
    NTFS_FILE_ATTR_HIDDEN                       = 0x00000002,
    NTFS_FILE_ATTR_SYSTEM                       = 0x00000004,
    NTFS_FILE_ATTR_DIRECTORY                    = 0x00000010,
    NTFS_FILE_ATTR_ARCHIVE                      = 0x00000020,
    NTFS_FILE_ATTR_DEVICE                       = 0x00000040,
    NTFS_FILE_ATTR_NORMAL                       = 0x00000080,
    NTFS_FILE_ATTR_TEMPORARY                    = 0x00000100,
    NTFS_FILE_ATTR_SPARSE_FILE                  = 0x00000200,
    NTFS_FILE_ATTR_REPARSE_POINT                = 0x00000400,
    NTFS_FILE_ATTR_COMPRESSED                   = 0x00000800,
    NTFS_FILE_ATTR_OFFLINE                      = 0x00001000,
    NTFS_FILE_ATTR_NOT_CONTENT_INDEXED          = 0x00002000,
    NTFS_FILE_ATTR_ENCRYPTED                    = 0x00004000,
    NTFS_FILE_ATTR_VALID_FLAGS                  = 0x00007FB7,
    NTFS_FILE_ATTR_VALID_SET_FLAGS              = 0x000031A7,
    NTFS_FILE_ATTR_DUP_FILE_NAME_INDEX_PRESENT  = 0x10000000,
    NTFS_FILE_ATTR_DUP_VIEW_INDEX_PRESENT       = 0x20000000,
} ATTR_FLAGS;

typedef struct _ATTR_RECORD {
/*0x00*/    ATTR_TYPES type; //тип атрибута
/*0x04*/    USHORT length; //длина заголовка; используется для перехода к //следующему   атрибуту
/*0x06*/    USHORT Reserved;
/*0x08*/    UCHAR non_resident; //1 если атрибут нерезидентный, 0 - резидентный
/*0x09*/    UCHAR name_length; //длина имени атрибута, в символах
/*0x0A*/    USHORT name_offset; //смещение имени атрибута, относительно заголовка атрибута
/*0x0C*/    USHORT flags; //флаги, перечислены в ATTR_FLAGS
/*0x0E*/    USHORT instance;

    union {
        //Резидентный атрибут
        struct {
/*0x10*/    ULONG value_length; //размер, в байтах, тела атрибута
/*0x14*/    USHORT value_offset; //байтовое смещение тела, относительно заголовка
            //атрибута
/*0x16*/    UCHAR resident_flags; //флаги, перечислены в RESIDENT_ATTR_FLAGS
/*0x17*/    UCHAR reserved;
        } r;
        //Нерезидентный атрибут
        struct {
/*0x10*/    ULARGE_INTEGER lowest_vcn;
/*0x18*/    ULARGE_INTEGER highest_vcn;
/*0x20*/    USHORT mapping_pairs_offset;//смещение списка отрезков
/*0x22*/    UCHAR compression_unit;
/*0x23*/    UCHAR reserved1[5];
/*0x28*/    ULARGE_INTEGER allocated_size; //размер дискового пространства,
            //которое было выделено под тело
            //атрибута
/*0x30*/    ULARGE_INTEGER data_size; //реальный размер атрибута
/*0x38*/    ULARGE_INTEGER initialized_size;
        } nr;
    } u;
} ATTR_RECORD, *PATTR_RECORD;