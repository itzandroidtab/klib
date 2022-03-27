#include <limits.h>
#include <stdlib.h>

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
    __uint8_t *a = (__uint8_t *) ptr1;
    __uint8_t *b = (__uint8_t *) ptr2;

    while (num--) {
        int r = *(a++) - *(b++);

        if (r != 0) {
            return r;
        }
    }

    return 0;
}

void *memset(void *ptr, int value, size_t num) {
    // According to the standard, the value is interpreted as
    // a unsigned char.

    // for now just use a byte loop
    for (size_t i = 0; i < num; i++) {
        ((__uint8_t*)ptr)[i] = (__uint8_t) value;
    }

    return ptr;
}

void *memcpy(void *destination, const void *source, size_t num) {
    // According to the standard, the value is interpreted as
    // a unsigned char.

    // for now just use a byte loop
    for (size_t i = 0; i < num; i++) {
        ((__uint8_t*)destination)[i] = ((__uint8_t*)source)[i];
    }    

    return destination;
}
