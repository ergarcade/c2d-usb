#include <hidapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define C2_MAX_SERIAL_NUMBER_LEN 9
#define C2_VENDOR_ID 0x17a4
#define C2_PRODUCT_ID 0x0003

typedef struct _monitor {
    char serial_number[C2_MAX_SERIAL_NUMBER_LEN+1];
    hid_device *dev;
    struct _monitor *next;
} monitor;

monitor *monitors = NULL;

static monitor* monitor_registered(const char *serial_number) {
    return NULL;
}

static monitor* register_monitor(const wchar_t *sernum) {
    hid_device *dev;
    char serial_number[C2_MAX_SERIAL_NUMBER_LEN+1] = { 0 };
    monitor *m;

    swscanf(sernum, L"%s", serial_number);

    dev = hid_open(C2_VENDOR_ID, C2_PRODUCT_ID, sernum);
    if (!dev) {
        fprintf(stderr, "failed to open device %s\n", serial_number);
        return NULL;
    }

    m = (monitor*)calloc(1, sizeof(monitor));
    if (!m) {
        fprintf(stderr, "couldn't alloc monitor\n");
        return NULL;
    }

    memcpy(m->serial_number, serial_number, C2_MAX_SERIAL_NUMBER_LEN);
    m->dev = dev;
    m->next = monitors;

    monitors = m;

    return m;
}

void release_monitors(void) {
    monitor *m = monitors;

    while (m) {
        m = m->next;
        fprintf(stderr, "releasing %s\n", monitors->serial_number);
        free(monitors);
        monitors = m;
    }
}

void find_monitors(void) {
    struct hid_device_info *devs, *cur;
    char serial_number[C2_MAX_SERIAL_NUMBER_LEN+1] = { 0 };

    devs = hid_enumerate(C2_VENDOR_ID, C2_PRODUCT_ID);
    for (cur = devs; cur; cur = cur->next) {
        swscanf(cur->serial_number, L"%s", serial_number);
        fprintf(stderr, "found %s\n", serial_number);

        if (monitor_registered(serial_number)) {
            continue;
        }

        if (!register_monitor(cur->serial_number)) {
            fprintf(stderr, "failed to register monitor %s\n", serial_number);
            continue;
        }

        fprintf(stderr, "monitor %s registered\n", serial_number);
    }
    if (devs) {
        hid_free_enumeration(devs);
    }
}

void query_monitors(void) {
}

int main(void) {
    if (hid_init()) {
        fprintf(stderr, "hid_init() failed\n");
        return 1;
    }

/*    while (;;) {*/
        find_monitors();
        query_monitors();
/*    } */

    release_monitors();


    return 0;
}
