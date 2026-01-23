#include "decomp.h"

#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/numath/nuvec.h"

C_API_START

int EdFileOpen(char *filepath, NUFILEMODE mode);
int EdFileClose();
void EdFileSetMedia(int media);
void EdFileSetPakFile(void *pak);
void EdFileSetReadWrongEndianess(int value);

void EdFileRead(void *buf, int len);
char EdFileReadChar();
unsigned char EdFileReadUnsignedChar();
float EdFileReadFloat();
int EdFileReadInt();
unsigned int EdFileReadUnsignedInt();
short EdFileReadShort();
unsigned short EdFileReadUnsignedShort();
void EdFileReadNuVec(NUVEC *out);

void EdFileWrite(void *data, int len);

void EdFileSwapEndianess16(void *data);
void EdFileSwapEndianess32(void *data);

C_API_END
