//
// Created by kevinche on 12.04.2021.
//
#include "ntfs.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "ntfs_utils.h"

int count_nodes(char *path){
    char path_buf[400];
    strcpy(path_buf, path);
    int result = 0;
    char sep[2] = "/";
    char *sub_dir = strtok (path_buf, sep);
    while (sub_dir != NULL){
        result++;
        sub_dir = strtok(NULL, sep);
    }
    return result;
}

struct ntfs_inode *find_node_by_name(struct ntfs_sb_info *fs, char *path, struct ntfs_inode **start_node){
    struct ntfs_inode *result_node = *start_node;
    struct ntfs_inode *head;
    bool found = false;
    char sep[2] = "/";
    char path_buf[400];
    strcpy(path_buf, path);
    int count = count_nodes(path);
    char *sub_dir = strtok (path_buf, sep);
    int err;
    while (sub_dir != NULL){
        found = false;
        if (!(result_node->type & MFT_RECORD_IS_DIRECTORY)) {
            free_inode(&(*start_node)->next_inode);
        }
        err = ntfs_readdir(fs, &result_node);
        if (err == -1) {
            free_inode(&(result_node->next_inode));
            return NULL;
        }
        head = result_node->next_inode;
        while(head != NULL){
            if (strcmp(head->filename, sub_dir)==0){
                struct ntfs_inode *tmp = malloc(sizeof(struct ntfs_inode));
                memcpy(tmp, head, sizeof(struct ntfs_inode));
                tmp->filename = malloc(strlen(head->filename)+1);
                strcpy(tmp->filename, head->filename);
                free_inode(&result_node->next_inode);
                result_node->next_inode = tmp;
                found = true;
                break;
            }
            head = head->next_inode;
        }
        if(!found) {
            free_inode(&result_node->next_inode);
            return NULL;
        }
        result_node = result_node->next_inode;
        if (count==1) return result_node; else count--;
        sub_dir = strtok(NULL, sep);
    }
    return result_node;
}

char *ls(struct ntfs_sb_info *fs, char *path){
    struct ntfs_inode *result_node;
    bool wo_path = false;
    bool parent_pars = false;
    if (path == NULL || strcmp(path, ".")==0) {
        result_node = fs->cur_node;
        wo_path = true;
        goto parse;
    }
    if (strcmp(path, "..")==0){
        result_node = fs->cur_node->parent;
        wo_path = true;
        parent_pars = true;
        goto parse;
    }
    if (path[0] == '/'){
        result_node = find_node_by_name(fs, path, &fs->root_node);
    } else {
        result_node = find_node_by_name(fs, path, &fs->cur_node);
    }
    parse:
    if (result_node != NULL){
        int err = ntfs_readdir(fs, &result_node);
        if (err == -1) return NULL;
        struct ntfs_inode *tmp = result_node->next_inode;
        char result[265];
        char *output = malloc(265*err);
        output[0] = '\0';
        while (tmp != NULL){
            if (tmp->type & MFT_RECORD_IS_DIRECTORY){
                sprintf(result, "DIR \t%s\n", tmp->filename);
            } else {
                sprintf(result, "FILE\t%s\n", tmp->filename);
            }
            strcat(output, result);
            tmp = tmp->next_inode;
        }
        if (wo_path){
            free_inode(&(result_node->next_inode));
            result_node->next_inode = NULL;
        } else {
            result_node->parent->next_inode = NULL;
            free_inode(&result_node);
        }
        if (parent_pars){
            fs->cur_node->parent->next_inode = fs->cur_node;
        }
        return output;
    }
    return NULL;
}

//char *cd(struct ntfs_sb_info *fs, char *path){
//    char *output = malloc(26);
//    output[0] = '\0';
//    if (strcmp(path, ".")==0){
//        return output;
//    }
//    if (strcmp(path, "..")==0){
//
//    }
//}