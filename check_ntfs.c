//
// Created by kevinche on 11.03.2021.
//

#include "check_ntfs.h"
#include <stdio.h>
#include <string.h>


int check_ntfs(FILE *fs){
    char oem_id[5];
    fseek(fs, 3, 0);
    fread(oem_id, sizeof(char), 4, fs);
    if (strcmp(oem_id, "NTFS")==0){
        printf("success");
    }
    return 1;
}