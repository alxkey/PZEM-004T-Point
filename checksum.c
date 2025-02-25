#include "checksum.h"

uint8_t checksum8(char *data, int length) {

    uint8_t check = 0;
    int i;
    for (i = 0; i < length; i++) {
        check += data[i];
    }
    check = ~check;
    check = check + 1;
    return check;
}

