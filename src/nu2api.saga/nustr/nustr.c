#include "nu2api.saga/nustr/nustr.h"

void NuStrCat(char *dest, const char *src) {
    for (; *dest != '\0'; dest = dest + 1) {
    }

    if (src != NULL) {
        char c;

        do {
            *dest = *src;
            dest = dest + 1;
            c = *src;
            src = src + 1;
        } while (c != '\0');
    }
}