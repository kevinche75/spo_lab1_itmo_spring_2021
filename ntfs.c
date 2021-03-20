//
// Created by kevinche on 20.03.2021.
//

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "ntfs.h"

NTFS_BOOT_SECTOR *openFileSystem(char *name){
    NTFS_BOOT_SECTOR *bs = malloc(sizeof(NTFS_BOOT_SECTOR));
    int fd = open(name, O_RDONLY, 00666);
    pread(fd, bs, sizeof(NTFS_BOOT_SECTOR), 0);
    if( bs->oem_id == 0x202020205346544E )
        return bs;
    free(bs);
    close(fd);
    return NULL;
}