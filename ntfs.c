//
// Created by kevinche on 20.03.2021.
//

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "ntfs.h"
#include <string.h>
#include <stdio.h>

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

    struct ntfs_mft_record *mft_rec;

    mft_rec = ntfs_mft_record_lookup_3_1(sbi, FILE_Volume);

    return  sbi;
}

static struct ntfs_mft_record *ntfs_mft_record_lookup_3_1(struct ntfs_sb_info *fs,
                                                          uint32_t file)
{
    const uint64_t mft_record_size = fs->mft_record_size;
    const block_t mft_blk = fs->mft_blk;
    uint64_t offset;
    const uint32_t mft_record_shift = ilog2(mft_record_size);
    const uint32_t clust_byte_shift = fs->clust_byte_shift;
    uint64_t lcn;
    struct ntfs_mft_record *mft_rec = malloc(mft_record_size);
    lcn = fs->mft_lcn + (file << mft_record_shift >> clust_byte_shift);
    offset = ((file << mft_record_shift) % fs->clust_size) + (lcn << fs->clust_byte_shift);
    while (mft_rec->magic != NTFS_MAGIC_FILE || mft_rec->mft_record_no != file){
        pread(fs->fd, mft_rec, sizeof (struct ntfs_mft_record), (long)offset);
        offset += fs->mft_record_size;
    }
    return mft_rec;            /* found MFT record */
//    for (;;) {
//        right_blk = cur_blk + fs->mft_blk;
//        err = ntfs_read(fs, buf, mft_record_size, mft_record_size, &right_blk,
//                        &offset, &next_offset, &lcn);
//        if (err) {
//            printf("Error while reading from cache.\n");
//            break;
//        }
//
//        ntfs_fixups_writeback(fs, (struct ntfs_record *)buf);
//
//        mrec = (struct ntfs_mft_record *)buf;
//        /* Check if the NTFS 3.1 MFT record number matches */
//        if (mrec->magic == NTFS_MAGIC_FILE && mrec->mft_record_no == file) {
//            if (blk)
//                *blk = cur_blk;     /* update record starting block */
//
//            return mrec;            /* found MFT record */
//        }
//
//        if (next_offset >= BLOCK_SIZE(fs)) {
//            /* try the next FS block */
//            offset = 0;
//            cur_blk = right_blk - NTFS_SB(fs)->mft_blk + 1;
//        } else {
//            /* there's still content to fetch in the current block */
//            cur_blk = right_blk - NTFS_SB(fs)->mft_blk;
//            offset = next_offset;   /* update FS block offset */
//        }
//    }
//
//    free(buf);

//    return NULL;
}