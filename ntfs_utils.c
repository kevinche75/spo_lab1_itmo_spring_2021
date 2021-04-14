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

int find_node_by_name(struct ntfs_sb_info *fs, char *path, struct ntfs_inode **start_node, struct ntfs_find_info **result){
    struct ntfs_inode *result_node = malloc(sizeof (struct ntfs_inode));
    struct ntfs_inode *head;
    memcpy(result_node, *start_node, sizeof (struct ntfs_inode));
    result_node->filename = NULL;
    struct ntfs_inode *start_result_node = result_node;
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
            free_inode(&start_result_node);
            return -1;
        }
        err = ntfs_readdir(fs, &result_node);
        if (err == -1) {
            free_inode(&start_result_node);
            return -1;
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
            free_inode(&start_result_node);
            return -1;
        }
        result_node = result_node->next_inode;
        result_node->next_inode = NULL;
        if (count==1) {
            *result = malloc(sizeof (struct ntfs_find_info));
            (*result)->start = start_result_node;
            (*result)->result = result_node;
            return 0;
        } else count--;
        sub_dir = strtok(NULL, sep);
    }
    return -1;
}

char *ls(struct ntfs_sb_info *fs, char *path){
    bool wo_path = false;
    bool parent_pars = false;
    struct ntfs_find_info *find_result;
    int err_f = 0;
    if (path == NULL || strcmp(path, ".")==0) {
        find_result = malloc(sizeof (struct ntfs_find_info));
        find_result->result =  fs->cur_node;
        wo_path = true;
        goto parse;
    }
    if (strcmp(path, "..")==0){
        find_result = malloc(sizeof (struct ntfs_find_info));
        find_result->result = fs->cur_node->parent;
        wo_path = true;
        parent_pars = true;
        goto parse;
    }
    if (path[0] == '/'){
        err_f = find_node_by_name(fs, path, &fs->root_node, &find_result);
    } else {
        err_f = find_node_by_name(fs, path, &fs->cur_node, &find_result);
    }
    parse:
    if (err_f != -1){
        int err = ntfs_readdir(fs, &(find_result->result));
        if (err == -1) return NULL;
        struct ntfs_inode *tmp = find_result->result->next_inode;
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
            free_inode(&(find_result->result->next_inode));
            find_result->result->next_inode = NULL;
        } else {
            free_inode(&(find_result->start));
        }
        if (parent_pars){
            fs->cur_node->parent->next_inode = fs->cur_node;
        }
        find_result->result = NULL;
        find_result->start = NULL;
        return output;
    }
    return NULL;
}

char *cd(struct ntfs_sb_info *fs, char *path){
    char *output = malloc(27);
    output[0] = '\0';
    char *message;
    if (strcmp(path, ".")==0){
        return output;
    }
    if (strcmp(path, "..")==0){
        if (fs->cur_node->mft_no == FILE_root) return output;
        struct ntfs_inode *tmp = fs->cur_node->parent;
        free_inode(&(fs->cur_node));
        fs->cur_node = tmp;
        fs->cur_node->next_inode = NULL;
        return output;
    }
    struct ntfs_find_info *result;
    int err = 0;
    if (path[0] == '/'){
        err = find_node_by_name(fs, path, &(fs->root_node), &result);
        if (err == -1) goto no_f;
        if (result->result->type & MFT_RECORD_IS_DIRECTORY){
            fs->root_node->next_inode = result->start->next_inode;
            result->start->next_inode->parent = fs->root_node;
            fs->cur_node = result->result;
            result->start->next_inode = NULL;
            free_inode(&(result->start));
            free(result);
            return output;
        } else goto is_f;
    } else {
        err = find_node_by_name(fs, path, &(fs->cur_node), &result);
        if (err == -1) goto no_f;
        if (result->result->type & MFT_RECORD_IS_DIRECTORY){
            fs->cur_node->next_inode = result->start->next_inode;
            result->start->next_inode->parent = fs->cur_node;
            fs->cur_node = result->result;
            result->start->next_inode = NULL;
            free_inode(&(result->start));
            free(result);
            return output;
        } else goto is_f;
    }
    no_f:
        message = "No such file or directory\n";
        sprintf(output, "%s", message);
        return output;
    is_f:
        message = "Not a directory\n";
        sprintf(output, "%s", message);
        return output;
}