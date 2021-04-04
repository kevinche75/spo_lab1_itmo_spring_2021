//
// Created by kevinche on 20.03.2021.
//

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "ntfs.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define clz(x) __builtin_clz(x)

int32_t ilog2(uint32_t x)
{
    return sizeof(uint32_t) * 8 - clz(x) - 1;
}

struct ntfs_bpb *open_file_system(int fd){
    struct ntfs_bpb *bs = malloc(sizeof(struct ntfs_bpb));
    pread(fd, bs, sizeof(struct ntfs_bpb), 0);
    if( strcmp(bs->oem_name, "NTFS    ")==0 )
        return bs;
    free(bs);
    close(fd);
    fprintf(stderr, "ERROR: Can not read filesystem.");
    return NULL;
}

static int ntfs_attr_lookup(struct ntfs_sb_info *fs,
                                                 uint32_t type,
                                                 struct ntfs_mft_record *mft_record,
                                                         struct ntfs_attr_record **attr_record)
{

    if (!mft_record || type == NTFS_AT_END){
        attr_record = NULL;
        return  -1;
    }

    *attr_record = (struct ntfs_attr_record *)((uint8_t *)mft_record + mft_record->attrs_offset);

    void *end = mft_record + fs->mft_record_size - sizeof (struct ntfs_attr_record);

    while((*attr_record)->type != NTFS_AT_END && (*attr_record)->type != type && (void *)(*attr_record) < end){
        *attr_record = (struct ntfs_attr_record *)((uint8_t *)(*attr_record) + (*attr_record)->len);
    }

    if ((*attr_record)->type == NTFS_AT_END || (void *)(*attr_record) >= end) {
        attr_record = NULL;
    }

    return 0;
}

static uint64_t mft_record_lookup(struct ntfs_sb_info *fs, uint32_t file, struct ntfs_mft_record **mft_record)
{
    const uint64_t mft_record_size = fs->mft_record_size;
    uint64_t offset;
    const uint32_t mft_record_shift = ilog2(mft_record_size);
    const uint32_t clust_byte_shift = fs->clust_byte_shift;
    uint64_t lcn;
    lcn = fs->mft_lcn + (file << mft_record_shift >> clust_byte_shift);
    offset = ((file << mft_record_shift) % fs->clust_size) + (lcn << fs->clust_byte_shift);
    while ((*mft_record)->magic != NTFS_MAGIC_FILE || (*mft_record)->mft_record_no != file){
        pread(fs->fd, (*mft_record), mft_record_size, (long)offset);
        offset += fs->mft_record_size;
    }
    if ((*mft_record)->magic != NTFS_MAGIC_FILE) return -1; /* can't find */
    return offset-fs->mft_record_size;            /* found MFT record */
}

static uint8_t ntfs_cvt_filename(char *filename,
                                 const struct ntfs_idx_entry *ie)
{
    const uint16_t *entry_fn;
    uint8_t entry_fn_len;
    unsigned i;

    entry_fn = ie->key.file_name.file_name;
    entry_fn_len = ie->key.file_name.file_name_len;

    for (i = 0; i < entry_fn_len; i++)
        filename[i] = (char)entry_fn[i];

    filename[i] = '\0';

    return entry_fn_len + 1;
}

static inline bool is_filename_printable(const char *s)
{
    return s && (*s != '.' && *s != '$');
}

static int read_clusters2buf(uint8_t **buf, uint64_t *buf_current_size, uint64_t *buf_size, int64_t lcn, uint64_t length, struct ntfs_sb_info * fs){
    length <<= fs->clust_byte_shift;
    uint64_t offset = lcn << fs->clust_byte_shift;
    /* if buf is not enough */
    if (length > (*buf_size - *buf_current_size)){
        *buf = realloc(*buf, *buf_size+fs->block_size);
        if (*buf == NULL){
            return -1;
        } else {
            *buf_size += fs->block_size;
        }
    }
    pread(fs->fd, *buf+*buf_current_size, length, (long)offset);
    *buf_current_size+=length;
    return 0;
}

/* Parse data runs.
 *
 * return 0 on success or -1 on failure.
 */
static int parse_data_run(uint64_t offset, struct ntfs_sb_info *fs,
        struct mapping_chunk **chunk)
{
    /* Pointer to the zero-terminated byte stream */
    uint8_t length_byte;  /* The length byte */
    uint8_t v, l;   /* v is the number of changed low-order VCN bytes;
                     * l is the number of changed low-order LCN bytes
                     */

    uint8_t *run_list = malloc(fs->block_size);
    pread(fs->fd, run_list, fs->block_size, (long)offset);
    uint8_t *run_list_ptr = run_list;

    uint8_t *byte;
    int byte_shift = 8;
    int mask;
    uint8_t val;
    int64_t res;
    int64_t lcn = 0; /*start lcn*/
    int err;

    /* init buf */
    uint8_t *buf = malloc(fs->block_size);
    uint64_t buf_size = fs->block_size;
    uint64_t buf_current_size = 0;

    do {
        /*parse of the length byte*/
        length_byte = *run_list_ptr;
        v = length_byte & 0x0F;
        l = length_byte >> 4;
        uint64_t length = 0;
        uint8_t count = v;

        byte = (uint8_t *)run_list_ptr + v;
        count = v;

        res = 0LL;
        while (count--) {
            val = *byte--;
            mask = val >> (byte_shift - 1);
            res = (res << byte_shift) | ((val + mask) ^ mask);
        }

        length = res;   /* get length data */

        byte = (uint8_t *)run_list_ptr + v + l;
        count = l;

        mask = 0xFFFFFFFF;
        res = 0LL;
        if (*byte & 0x80)
            res |= (int64_t)mask;   /* sign-extend it */

        /* get vcn */
        while (count--)
            res = (res << byte_shift) | *byte--;

        lcn += res;
        run_list_ptr+= v + l + 1;
        err = read_clusters2buf(&buf, &buf_current_size, &buf_size, lcn, length, fs);
        if (err == -1) {
            free(run_list_ptr);
            free(buf);
            return -1;
        }
    } while (*run_list_ptr);

    free(run_list);
    (*chunk)->buf = buf;
    (*chunk)->length = buf_current_size;
    return 0;
}

static int ntfs_readdir(struct ntfs_sb_info *fs, struct ntfs_inode **inode){
    struct ntfs_mft_record *dir_record = malloc(sizeof (struct ntfs_mft_record));
    uint64_t offset;
    offset = mft_record_lookup(fs, (*inode)->mft_no, &dir_record);
    int err;
    uint8_t filename_len;
    struct ntfs_idx_entry *idx_entry = NULL;
    struct ntfs_inode *current_inode = *inode;
    uint64_t stream;

    if (offset < 0){
        free(dir_record);
        return -1;
    }

    struct ntfs_attr_record *attr_index = malloc(sizeof (struct ntfs_attr_record));
    err = ntfs_attr_lookup(fs, NTFS_AT_INDEX_ROOT, dir_record, &attr_index);
    if (!attr_index || err == -1) {
        free(dir_record);
        free(attr_index);
        return -1;
    }

    char filename[NTFS_MAX_FILE_NAME_LEN + 1];
    struct ntfs_mft_record *dir_entry = malloc(sizeof (struct ntfs_mft_record));

    struct ntfs_idx_root *ir = (struct ntfs_idx_root *)((uint8_t *)attr_index + attr_index->data.resident.value_offset);
    uint8_t *idx_entry_offset = ((uint8_t *)&ir->index + ir->index.entries_offset);
    uint64_t mft_entry_offset;

    do {
        idx_entry = (struct ntfs_idx_entry *) idx_entry_offset;
        idx_entry_offset = ((uint8_t *) idx_entry + idx_entry->len);
        if (idx_entry->key_len > 0) {
            filename_len = ntfs_cvt_filename(filename, idx_entry);
            if (is_filename_printable(filename)) {
                current_inode->next_inode = malloc(sizeof(struct ntfs_inode));
                current_inode = current_inode->next_inode;
                current_inode->parent = *inode;
                memcpy(current_inode->filename, filename, filename_len);
                mft_entry_offset = mft_record_lookup(fs, idx_entry->data.dir.indexed_file & NTFS_MFT_REF_MASK,
                                                     &dir_entry);
                if (mft_entry_offset == -1) {
                    free(dir_record);
                    free(attr_index);
                    free(dir_entry);
                    return -1;
                }
                current_inode->type = dir_entry->flags;
            }
        }
    } while(!(idx_entry->flags & INDEX_ENTRY_END));

    if (!(idx_entry->flags & INDEX_ENTRY_NODE)) {
        free(dir_record);
        free(attr_index);
        free(dir_entry);
        return 0;
    }

    err = ntfs_attr_lookup(fs, NTFS_AT_INDEX_ALLOCATION, dir_record, &attr_index);

    if (!attr_index || err == -1) {
        free(dir_record);
        free(attr_index);
        free(dir_entry);
        return 0;
    }

    if (!attr_index->non_resident) {
        free(dir_record);
        free(attr_index);
        free(dir_entry);
        return -1;
    }

    struct ntfs_idx_allocation *idx_alloc;
    stream = offset + ((uint8_t *)attr_index - (uint8_t *)dir_record) + attr_index->data.non_resident.mapping_pairs_offset;
    free(dir_record);
    struct mapping_chunk *chunk = malloc(sizeof (struct mapping_chunk));
    chunk->current_block = 0;

    parse_data_run(stream, fs, &chunk);

    do {
        if (chunk->length >=0){
            idx_alloc = (struct ntfs_idx_allocation *)(chunk->buf + (chunk->current_block << fs->block_shift));
            if (idx_alloc->magic != NTFS_MAGIC_INDX) {
                printf("Not a valid INDX record.\n");
                free(attr_index);
                free(dir_entry);
                free(chunk->buf);
                free(chunk);
                return -1;
            }
        } else {
            free(attr_index);
            free(dir_entry);
            free(chunk->buf);
            free(chunk);
            return 0;
        }
        idx_entry_offset = ((uint8_t *)&idx_alloc->index + idx_alloc->index.entries_offset);

        do {
            idx_entry = (struct ntfs_idx_entry *)idx_entry_offset;
            idx_entry_offset = ((uint8_t *)idx_entry + idx_entry->len);
            if (idx_entry->key_len > 0){
                filename_len = ntfs_cvt_filename(filename, idx_entry);
                if (is_filename_printable(filename)){
                    current_inode->next_inode = malloc(sizeof (struct ntfs_inode));
                    current_inode = current_inode->next_inode;
                    current_inode->parent = *inode;
                    current_inode->filename = malloc(filename_len);
                    memcpy(current_inode->filename, &filename[0], filename_len);
                    mft_entry_offset = mft_record_lookup(fs, idx_entry->data.dir.indexed_file & NTFS_MFT_REF_MASK, &dir_entry);
                    if (mft_entry_offset == -1) {
                        free(attr_index);
                        free(dir_entry);
                        free(chunk->buf);
                        free(chunk);
                        return -1;
                    }
                    current_inode->type = dir_entry->flags;
                }
            }
        } while (idx_entry_offset < (chunk->buf + chunk->length) && !(idx_entry->flags & INDEX_ENTRY_END));

        chunk->current_block++;
    } while(chunk->current_block < (chunk->length >> fs->block_shift));
    free(attr_index);
    free(dir_entry);
    free(chunk->buf);
    free(chunk);
    return 0;
}

struct ntfs_sb_info *ntfs_init(char *name){
    int fd = open(name, O_RDONLY, 00666);
    struct ntfs_bpb ntfs = *open_file_system(fd);
    uint8_t mft_record_shift;
    struct ntfs_sb_info *sbi = malloc(sizeof(struct ntfs_sb_info));

    sbi->sector_shift = ilog2(ntfs.sector_size);
    sbi->sector_size = ntfs.sector_size;

    mft_record_shift = ntfs.clust_per_mft_record < 0 ?
                       -ntfs.clust_per_mft_record :
                       sbi->sector_shift +
                       ilog2(ntfs.sec_per_clust) +
                       ilog2(ntfs.clust_per_mft_record);

    sbi->fd = fd;
    sbi->clust_shift = ilog2(ntfs.sec_per_clust);
    sbi->clust_byte_shift = sbi->clust_shift + sbi->sector_shift;
    sbi->clust_mask = ntfs.sec_per_clust - 1;
    sbi->clust_size = ntfs.sec_per_clust << sbi->sector_shift;
    sbi->mft_record_size = 1 << mft_record_shift;
    sbi->clust_per_idx_record = ntfs.clust_per_idx_record;
    sbi->block_shift = ilog2(ntfs.clust_per_idx_record) + sbi->clust_byte_shift;
    sbi->block_size = 1 << sbi->block_shift;

    sbi->mft_lcn = ntfs.mft_lclust;
    sbi->mft_blk = ntfs.mft_lclust << sbi->clust_shift << sbi->sector_shift >>
                                   sbi->block_shift;

    sbi->clusters = ntfs.total_sectors << sbi->sector_shift >> sbi->clust_shift;

    struct ntfs_mft_record *mft_rec = malloc(sizeof (struct ntfs_mft_record));
    uint64_t record_offset;
    record_offset = mft_record_lookup(sbi, FILE_root, &mft_rec);
    struct ntfs_attr_record *attr = malloc(sizeof (struct ntfs_attr_record));
    ntfs_attr_lookup(sbi, NTFS_AT_FILENAME, mft_rec, &attr);
//    struct ntfs_filename_attr *fn;
//    char filename[NTFS_MAX_FILE_NAME_LEN + 1];
//    const uint16_t *entry_fn;
//    uint8_t entry_fn_len;
//    unsigned i;
//
//    for (int k = 67; k < 100; k++) {
//        record_offset = mft_record_lookup(sbi, k, mft_rec);
//        ntfs_attr_lookup(sbi, NTFS_AT_FILENAME, mft_rec, &attr);
//        fn = (struct ntfs_filename_attr*) ((uint8_t *)attr + attr->data.resident.value_offset);
//        entry_fn = fn->file_name;
//        entry_fn_len = fn->file_name_len;
//        for (i = 0; i < entry_fn_len; i++)
//            filename[i] = (char) entry_fn[i];
//
//        filename[i] = '\0';
//
//    }
    struct ntfs_inode *root_inode = malloc(sizeof (struct ntfs_inode));
    root_inode->mft_no = FILE_root;

    ntfs_readdir(sbi, &root_inode);
    return  sbi;
}