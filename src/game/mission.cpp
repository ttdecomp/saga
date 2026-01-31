#include "mission.h"

#include "game/character.h"
#include "game/level.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"

#include <string.h>

MISSIONSYS *Missions_Configure(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, MISSIONSAVE *save) {
    short psVar2;
    short sVar1;
    nufpar_s *fp;
    LEVELDATA *pLVar2;
    int i;
    ushort uVar3;
    MISSIONSYS *dest;
    int charId;
    MISSIONSYS sys;
    MISSIONSAVE local_c;
    short *buffer;

    sys.character_ids[6] = 2475;
    sys.character_ids[7] = 79;

    fp = NuFParCreate(file);
    if (fp != NULL) {
        dest = &sys;

        for (i = 12; i != 0; i--) {
            dest->length = 0;
            dest = (MISSIONSYS *)((int)dest + 4);
        }

        sys.flags = 1;
        buffer = (short *)((int)bufferStart->void_ptr + 3U & 0xfffffffc);
        bufferStart->void_ptr = buffer;
        sys.mission_save = save;
        sys.length = (int)buffer;

        do {
            i = NuFParGetLine(fp);
            if (i == 0)
                break;

            NuFParGetWord(fp);

            i = NuStrICmp(fp->word_buf, "party");
            if (i == 0) {
                while ((sys.character_count < 8 && (i = NuFParGetWord(fp), i != 0))) {
                    charId = CharIDFromName(fp->word_buf);
                    if (charId != -1) {
                        sys.character_ids[sys.character_count] = (short)charId;
                        sys.character_count = sys.character_count + 1;
                    }
                }
            } else {
                i = NuStrICmp(fp->word_buf, "pickups");
                if (i == 0) {
                    i = NuFParGetWord(fp);
                    if (i != 0) {
                        i = NuStrICmp(fp->word_buf, "on");
                        if (i == 0) {
                            sys.flags |= 1;
                        } else {
                            i = NuStrICmp(fp->word_buf, "off");
                            if (i == 0) {
                                sys.flags &= 0xfe;
                            }
                        }
                    }
                } else {
                    i = NuStrICmp(fp->word_buf, "mission");
                    if (i == 0) {
                        *buffer = -1;
                        buffer[1] = -1;
                        buffer[2] = -1;
                        buffer[3] = -1;
                        buffer[4] = -0x7960;
                        buffer[5] = 1;
                        buffer[6] = -0x7960;
                        buffer[7] = 1;
                        buffer[8] = 0xb4;
                        *(byte *)(buffer + 9) = sys.count;
                    LAB_004f0bd0:
                        i = NuFParGetWord(fp);
                        if (i != 0) {
                            while (i = NuStrICmp(fp->word_buf, "find"), i == 0) {
                                i = NuFParGetWord(fp);
                                if (i == 0)
                                    goto LAB_004f0bd0;
                                sVar1 = CharIDFromName(fp->word_buf);
                                *buffer = sVar1;
                                i = NuFParGetWord(fp);
                                if (i == 0)
                                    goto LAB_004f0c1f;
                            }
                            i = NuStrICmp(fp->word_buf, "in_level");
                            if (i == 0) {
                                i = NuFParGetWord(fp);
                                if ((i != 0) && (pLVar2 = Level_FindByName(fp->word_buf, &charId), pLVar2 != NULL)) {
                                    buffer[1] = (short)charId;
                                }
                            } else {
                                i = NuStrICmp(fp->word_buf, "time");
                                if (i == 0) {
                                    i = NuFParGetInt(fp);
                                    uVar3 = 3;
                                    if (2 < (ushort)i) {
                                        uVar3 = (ushort)i;
                                    }
                                    buffer[8] = uVar3;
                                } else {
                                    i = NuStrICmp(fp->word_buf, "bounty");
                                    if (i == 0) {
                                        i = NuFParGetInt(fp);
                                        *(int *)(buffer + 4) = i;
                                        i = NuFParGetInt(fp);
                                        *(int *)(buffer + 6) = i;
                                    } else {
                                        i = NuStrICmp(fp->word_buf, "name_id");
                                        if (i == 0) {
                                            i = NuFParGetInt(fp);
                                            buffer[2] = (short)i;
                                        } else {
                                            i = NuStrICmp(fp->word_buf, "text_id");
                                            if (i == 0) {
                                                i = NuFParGetInt(fp);
                                                buffer[3] = (short)i;
                                            }
                                        }
                                    }
                                }
                            }
                            goto LAB_004f0bd0;
                        }
                    LAB_004f0c1f:
                        if ((*buffer != -1) && (buffer[1] != -1)) {
                            sys.count = sys.count + 1;
                            buffer = buffer + 0xc;
                            bufferStart->void_ptr = (void *)((int)bufferStart->void_ptr + 0x18);
                        }
                    }
                }
            }
        } while (sys.count < 20);

        NuFParDestroy(fp);
        if (sys.count != 0) {
            dest = (MISSIONSYS *)((int)bufferStart->void_ptr + 3U & 0xfffffffc);
            bufferStart->void_ptr = dest;
            memmove(dest, &sys, 0x30);
            bufferStart->void_ptr = (void *)((int)bufferStart->void_ptr + 0x33U & 0xfffffffc);
            return dest;
        }
    }

    return NULL;
}
