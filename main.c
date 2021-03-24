#include "available_devices.h"
#include <stdio.h>
#include "ntfs.h"

int main(int argc, char *argv[]) {
    print_available_devices();
    FILE *fs;
    char *name= "/dev/loop8";
    ntfs_init(name);
    return 0;
}