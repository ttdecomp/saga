#include "nu2api.saga/nufile/nufile.h"

int32_t DontDoFileUpperCaseHack = 0;

void NuFileUpCase(void *param_1, char *path) {
    char local_5;

    if (param_1 == (void *)0x0) {
        local_5 = '\\';
    } else {
        local_5 = *(char *)((int)param_1 + 0x10);
    }

    if (DontDoFileUpperCaseHack == 0) {
        for (; *path != '\0'; path = path + 1) {
            char cVar1 = *path;
            if (cVar1 < '{') {
                if (cVar1 < 'a') {
                    if ((cVar1 == '/') || (cVar1 == '\\')) {
                        *path = local_5;
                    }
                } else {
                    *path = *path + -0x20;
                }
            }
        }
    } else {
        for (; *path != '\0'; path = path + 1) {
            if ((*path == '\\') || (*path == '/')) {
                *path = local_5;
            }
        }
    }
}
