#include "available_devices.h"
#include <stdio.h>
#include <check_ntfs.h>

int main(int argc, char *argv[]) {
//    print_available_devices();
    FILE *fs;
    fs = fopen("/dev/loop8","r");
    check_ntfs(fs);
    return 0;
}