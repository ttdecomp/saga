#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif

    i32 EdFileOpen(char *filepath, NUFILEMODE mode);
    i32 EdFileClose();
    void EdFileSetMedia(i32 media);
    void EdFileSetPakFile(void *pak);
    void EdFileSetReadWrongEndianess(i32 value);

    void EdFileRead(void *buf, i32 len);
    char EdFileReadChar();
    unsigned char EdFileReadUnsignedChar();
    f32 EdFileReadFloat();
    i32 EdFileReadInt();
    u32 EdFileReadUnsignedInt();
    i16 EdFileReadShort();
    u16 EdFileReadUnsignedShort();
    void EdFileReadNuVec(NUVEC *out);

    void EdFileWrite(void *data, i32 len);

    void EdFileSwapEndianess16(void *data);
    void EdFileSwapEndianess32(void *data);

#ifdef __cplusplus
}
#endif
