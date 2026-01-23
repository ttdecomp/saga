#include <string.h>

#include "gameapi.saga/edtools/edfile.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nufile/nufilepak.h"

static int edfile_media;
int edfile_handle = -1;
int edfile_write_flag;

static void *edfile_pakfile;

char edfile_buffer[0x1000];
int edfile_buffer_pointer;
int edfile_lock[2];

int edfile_readwrongendianess;

void EdFileResetBuffers() {
    memset(edfile_buffer, 0, sizeof(edfile_buffer));
    edfile_buffer_pointer = 0;
    edfile_lock[1] = 0;
    edfile_lock[0] = 0;
}

void EdFileFillBuffer() {
    if (edfile_handle != -1) {
        memset(edfile_buffer, 0, sizeof(edfile_buffer));

        if (edfile_media == 1) {
            NuFileRead(edfile_handle, edfile_buffer, 0x1000);
            edfile_buffer_pointer = 0;
        }
    }
}

void EdFileFlushBuffer() {
    if (edfile_handle == -1) {
        return;
    }

    if (edfile_buffer_pointer == 0 || edfile_media != 1) {
        return;
    }

    NuFileWrite(edfile_handle, edfile_buffer, edfile_buffer_pointer);
    edfile_buffer_pointer = 0;
}

int EdFileOpen(char *filepath, NUFILEMODE mode) {
    int item_handle;
    void *item;
    int item_size;

    if (edfile_handle != -1) {
        return 0;
    }

    EdFileResetBuffers();

    edfile_handle = -1;
    edfile_write_flag = 0;

    if (edfile_media != 1) {
        return 0;
    }

    if (mode != NUFILE_READ) {
        if (mode != NUFILE_WRITE) {
            return 0;
        }

        edfile_write_flag = 1;
        edfile_handle = NuFileOpen(filepath, NUFILE_WRITE);

        if (edfile_handle > 0) {
            return 1;
        }

        edfile_handle = -1;

        return 0;
    }

    if (edfile_pakfile != NULL) {
        item_handle = NuFilePakGetItem(edfile_pakfile, filepath);

        if (item_handle != 0) {
            NuFilePakGetItemInfo(edfile_pakfile, item_handle, &item, &item_size);

            edfile_handle = NuMemFileOpen(item, item_size, NUFILE_READ);

            if (edfile_handle > 0) {
                EdFileFillBuffer();

                return 1;
            }

            edfile_handle = -1;
        }
    }

    edfile_handle = NuFileOpen(filepath, NUFILE_READ);

    if (edfile_handle > 0) {
        EdFileFillBuffer();

        return 1;
    }

    edfile_handle = -1;

    return 0;
}

int EdFileClose() {
    edfile_pakfile = NULL;

    if (edfile_handle == -1) {
        return 0;
    }

    if (edfile_write_flag != 0) {
        EdFileFlushBuffer();
    }

    if (edfile_media != 1) {
        return 0;
    }

    NuFileClose(edfile_handle);
    edfile_handle = -1;
    edfile_media = 0;

    return 1;
}

void EdFileSetMedia(int media) {
    edfile_media = media;
}

void EdFileSetPakFile(void *pak) {
    edfile_pakfile = pak;
}

void EdFileSetReadWrongEndianess(int value) {
}

void EdFileRead(void *buf, int len) {
    int to_read;

    while (len > 0) {
        to_read = MIN(0x1000 - edfile_buffer_pointer, len);
        memcpy(buf, edfile_buffer + edfile_buffer_pointer, to_read);

        edfile_buffer_pointer += to_read;
        len -= to_read;
        buf = (void *)((int)buf + to_read);

        if (edfile_buffer_pointer == 0x1000) {
            EdFileFillBuffer();
        }
    }
}

char EdFileReadChar() {
    char data;

    EdFileRead(&data, 1);

    return data;
}

unsigned char EdFileReadUnsignedChar() {
    unsigned char data;

    EdFileRead(&data, 1);

    return data;
}

float EdFileReadFloat() {
    float data;

    EdFileRead(&data, 4);

    if (edfile_readwrongendianess) {
        EdFileSwapEndianess32(&data);
    }

    return data;
}

int EdFileReadInt() {
    int data;

    EdFileRead(&data, 4);

    if (edfile_readwrongendianess) {
        EdFileSwapEndianess32(&data);
    }

    return data;
}

unsigned int EdFileReadUnsignedInt() {
    unsigned int data;

    EdFileRead(&data, 4);

    if (edfile_readwrongendianess) {
        EdFileSwapEndianess32(&data);
    }

    return data;
}

short EdFileReadShort() {
    short data;

    EdFileRead(&data, 2);

    if (edfile_readwrongendianess) {
        EdFileSwapEndianess16(&data);
    }

    return data;
}

unsigned short EdFileReadUnsignedShort() {
    unsigned short data;

    EdFileRead(&data, 2);

    if (edfile_readwrongendianess) {
        EdFileSwapEndianess16(&data);
    }

    return data;
}

void EdFileReadNuVec(NUVEC *out) {
    out->x = EdFileReadFloat();
    out->y = EdFileReadFloat();
    out->z = EdFileReadFloat();
}

void EdFileWrite(void *data, int len) {
    int to_write;

    while (len > 0) {
        to_write = MIN(0x1000 - edfile_buffer_pointer, len);

        memcpy(edfile_buffer + edfile_buffer_pointer, data, to_write);

        edfile_buffer_pointer += to_write;
        len -= to_write;
        data = (void *)((int)data + to_write);

        if (edfile_buffer_pointer == 0x1000) {
            EdFileFlushBuffer();
        }
    }
}

void EdFileSwapEndianess16(void *data) {
    char *bytes;
    char tmp;

    bytes = (char *)data;

    tmp = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = tmp;
}

void EdFileSwapEndianess32(void *data) {
    char *bytes;
    char tmp;

    bytes = (char *)data;

    tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;

    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
}
