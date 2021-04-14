#include "available_devices.h"
#include <stdio.h>
#include "ntfs.h"
#include <string.h>
#include <malloc.h>
#include "ntfs_utils.h"

int shell(char *filename){
    struct ntfs_sb_info *fs  = ntfs_init(filename);
    if (fs == NULL) return -1;
    printf("NTFS filesystem detected.\n");
    int exitFlag = 0;
    char *inputString = malloc(1024);
    while (!exitFlag) {
        printf("> ");
        fgets(inputString, 1024, stdin);
        char *command = strtok(inputString, " \n");
        if (command == NULL) {
            continue;
        }
        char *path = strtok(NULL, " \n");
        char *outPath = strtok(NULL, " \n");
        if (strcmp(command, "exit") == 0) {
            exitFlag = 1;
        } else if (strcmp(command, "help") == 0) {
            printf("cd [directory] - change working directory\n");
            printf("pwd - print working directory full name\n");
            printf("cp - [directory] [target directory] - copy dir or file from mounted device\n");
            printf("ls - show working directory elements\n");
            printf("exit - terminate program\n");
            printf("help - print help\n");
        } else if (strcmp(command, "ls") == 0) {
            char *output = ls(fs, path);
            if (output == NULL) continue;
            printf("%s", output);
            free(output);
        } else if (strcmp(command, "pwd") == 0) {
//            char *output = pwd(fileSystem);
//            printf("%s", output);
//            free(output);
        } else if (strcmp(command, "cd") == 0) {
            if (path == NULL){
                printf("cd command require path argument");
                continue;
            }
//            char *output = cd(fileSystem, path);
//            printf("%s", output);
//            free(output);
        } else if (strcmp(command, "cp") == 0) {
//            char *output = cp(fileSystem, path, outPath);
//            printf("%s", output);
//            free(output);
        } else {
            printf("Wrong command. Enter 'help' to get help.\n");
        }
    }
    free_fs(&fs);
    return 0;
}

int main(int argc, char *argv[]) {

    if (argc >= 2 && strcmp(argv[1], "help") == 0) {
        printf("To run program in list mode use 'list' key. To run it in shell mode use 'shell' key and specify drive or deviceDescriptor.");
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "list") == 0) {
        print_available_devices(1);
        return 0;
    }
    if (argc >= 3 && strcmp(argv[1], "shell") == 0) {
        print_available_devices(0);
        shell(argv[2]);
        return 0;
    }
    printf("Incorrect command line arguments. Run with 'help' argument to get help");
    return 0;
}