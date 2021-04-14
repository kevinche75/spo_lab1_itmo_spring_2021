//
// Created by kevinche on 12.04.2021.
//

#ifndef SPO_LAB1_NTFS_UTILS_H
#define SPO_LAB1_NTFS_UTILS_H

#endif //SPO_LAB1_NTFS_UTILS_H

char *ls(struct ntfs_sb_info *fs, char *path);
int count_nodes(char *path);
char *cd(struct ntfs_sb_info *fs, char *path);