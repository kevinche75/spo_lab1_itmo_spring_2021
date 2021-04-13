#include "available_devices.h"
#include <stdio.h>
#include "ntfs.h"
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc >= 3) {
        if (strcmp(argv[1], "shell")==0){
            char *name = argv[2];
            print_available_devices(0);
            ntfs_init(name);
            return 0;
        }
    }
    if (argc >= 2){
        if (strcmp(argv[1], "list")==0){
            print_available_devices(1);
            return 0;
        }
    }
    return 0;
}