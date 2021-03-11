//
// Created by kevinche on 11.03.2021.
//

#include "available_devices.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <blkid/blkid.h>


#define ll long long
#define TB_amount pow(2, 40)
#define GB_amount pow(2, 30)
#define MB_amount pow(2, 20)
#define KB_amount pow(2, 10)

void print_size(ll size) {
    if (size >= TB_amount) {
        printf("%lld TiB\t", (ll) (size / TB_amount));
    } else if (size >= GB_amount) {
        printf("%lld GiB\t", (ll) (size / GB_amount));
    } else if (size >= MB_amount) {
        printf("%lld MiB\t", (ll) (size / MB_amount));
    } else if (size >= KB_amount) {
        printf("%lld KiB\t", (ll) (size / KB_amount));
    } else {
        printf("%lld   B\t", (ll) size);
    }
}


int initialize_cache(blkid_cache *cache) {
    int status_init = blkid_get_cache(cache, NULL);
    if (status_init < 0) {
        fprintf(stderr, "ERROR: Can not get the cache.");
        return 1;
    }
    return 0;
}

int probe_cache(blkid_cache *cache) {
    int status_probe = blkid_probe_all(*cache);
    if (status_probe < 0) {
        fprintf(stderr, "ERROR: Can not probe devices.");
        return 1;
    }
    return 0;
}

void check_and_print(blkid_probe *probe, char *tag) {
    const char *var;
    if (blkid_probe_has_value(*probe, tag)) {
        blkid_probe_lookup_value(*probe, tag, &var, NULL);
        printf("%s=%s\t", tag, var);
    }
}

int iterate_dev(blkid_cache *cache) {
    blkid_dev dev;
    blkid_dev_iterate iterator = blkid_dev_iterate_begin(*cache);

    printf("System partition:\n");
    while (blkid_dev_next(iterator, &dev) == 0) {
        const char *devname = blkid_dev_devname(dev);
        printf("\t%s\t", devname);


        blkid_probe probe = blkid_new_probe_from_filename(devname);
        if (probe == NULL) {
            fprintf(stderr, "Launch util as root to get more information!\n");
        } else {
            blkid_loff_t probeSize = blkid_probe_get_size(probe);
            print_size(probeSize);

            blkid_do_probe(probe);
            printf("\t");
            check_and_print(&probe, "TYPE");
            check_and_print(&probe, "UUID");
            check_and_print(&probe, "LABEL");
            printf("\n");

        }
    }
}

void print_available_devices(){
    blkid_cache cache;
    initialize_cache(&cache);
    probe_cache(&cache);
    iterate_dev(&cache);
}