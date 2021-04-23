//
// Created by kevinche on 12.04.2021.
//
#pragma once
#ifndef SPO_LAB1_NTFS_UTILS_H
#define SPO_LAB1_NTFS_UTILS_H

#endif //SPO_LAB1_NTFS_UTILS_H

char *ls(struct ntfs_sb_info *fs, char *path);
int count_nodes(char *path);
char *cd(struct ntfs_sb_info *fs, char *path);
char *pwd(struct ntfs_sb_info *fs);
char *cp(struct ntfs_sb_info *fs, char *path, char *out_path);
int print_block_devices();
struct ntfs_sb_info *init_fs(char *filename);
int close_fs(struct ntfs_sb_info *fs);