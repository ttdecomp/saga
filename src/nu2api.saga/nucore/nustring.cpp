#include "nu2api.saga/nucore/nustring.h"

const char *NuStrStripPath(const char *string) {
    const char *cursor;

    do {
        cursor = string;

        while (*cursor != '\\' && *cursor != '/') {
            if (*cursor == '\0') {
                return string;
            }

            cursor++;
        }

        string = cursor + 1;
    } while (*string != '\0');

    return string;
}
