#include "decomp.h"

#include "game/collection.h"

#include "decomp.h"
#include "game/area.h"
#include "game/character.h"
#include "game/store.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"

static COLLECTID *TempCollectID = NULL;

int32_t CollectCount = 0;
COLLECTID *CollectList = NULL;
int32_t COLLECTION_COMPLETIONCOUNT = 0;

int32_t InCollectList_Index(int32_t id, COLLECTID *list, int32_t count) {
    int32_t i;
    int32_t charId;

    if (list == NULL && (list = CollectList, count = CollectCount, CollectList == NULL)) {
        return id;
    }

    if (0 < count) {
        charId = list->id;
        i = 0;
        while (true) {
            if (charId == id) {
                TempCollectID = list;
                return i;
            }
            i = i + 1;
            list = list + 1;
            if (i == count)
                break;
            charId = list->id;
        }
    }

    return -1;
}

void Collection_Configure(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd) {
    byte bVar1;
    ushort uVar2;
    short sVar3;
    COLLECTID *collect;
    int iVar4;
    AREADATA *pAVar5;
    int i;
    char *buf;

    nufpar_s *fp = NuFParCreate(file);
    if (fp != NULL) {
        CollectCount = 0;
        collect = (COLLECTID *)(bufferStart->addr + 3U & 0xfffffffc);
        CollectList = collect;
        bufferStart->void_ptr = collect;

        COLLECTION_COMPLETIONCOUNT = 0;

        while (NuFParGetLine(fp) != 0) {

        LAB_004eb7f3:
            if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "collect") == 0 && NuFParGetWord(fp) != 0) {
                sVar3 = CharIDFromName(fp->word_buf);

                LOG_DEBUG("Collection_Configure: Found collect id %s -> %d", fp->word_buf, sVar3);

                collect->id = sVar3;

                if (sVar3 != -1 && InCollectList_Index((int)sVar3, CollectList, CollectCount) == -1) {
                    collect->type = 0;
                    collect->field2_0x3 = 0xff;
                    collect->can_buy = 0;
                    collect->field3_0x4 = 0;
                    collect->field6_0xa = 0;
                    collect->field5_0x9 = 0;
                    collect->cheat_code[0] = '\0';

                LAB_004eb886:
                    iVar4 = NuFParGetWord(fp);

                    do {
                        if (iVar4 == 0) {
                            bVar1 = collect->type;
                            if (bVar1 == 0) {
                                if (collect->can_buy != 0) {
                                LAB_004eb8ce:
                                    collect->field5_0x9 = 1;
                                    COLLECTION_COMPLETIONCOUNT = COLLECTION_COMPLETIONCOUNT + 1;
                                }
                            } else if (bVar1 != 8 && bVar1 != 7)
                                goto LAB_004eb8ce;

                            CollectCount = CollectCount + 1;
                            iVar4 = NuFParGetLine(fp);
                            collect = collect + 1;

                            if (iVar4 == 0)
                                goto LAB_004eb900;

                            goto LAB_004eb7f3;
                        }

                        iVar4 = NuStrICmp(fp->word_buf, "story");
                        if (iVar4 != 0)
                            goto LAB_004eb920;

                        collect->type = 1;
                        iVar4 = NuFParGetWord(fp);

                    } while (true);
                }
            }
        }

    LAB_004eb900:
        NuFParDestroy(fp);
        if (CollectCount < 1) {
            CollectList = NULL;
            return;
        }
        bufferStart->void_ptr = collect;
    }

    return;

LAB_004eb920:
    iVar4 = NuStrICmp(fp->word_buf, "area_complete");
    if (iVar4 == 0) {
        iVar4 = NuFParGetWord(fp);
        if (iVar4 != 0 && (pAVar5 = Area_FindByName(fp->word_buf, &i), pAVar5 != NULL)) {
            collect->type = 2;
            collect->field2_0x3 = (byte)i;
        }
    } else {
        iVar4 = NuStrICmp(fp->word_buf, "all_episodes_complete");
        if (iVar4 == 0) {
            collect->type = 3;
        } else {
            iVar4 = NuStrICmp(fp->word_buf, "in_pack");
            if (iVar4 == 0) {
                iVar4 = NuFParGetWord(fp);
                if ((iVar4 != 0) && (iVar4 = Store_FindPack(-1, fp->word_buf), iVar4 != -1)) {
                    collect->type = 8;
                    collect->field2_0x3 = (byte)iVar4;
                }
            } else {
                iVar4 = NuStrICmp(fp->word_buf, "100_percent");
                if (iVar4 == 0) {
                    collect->type = 7;
                } else {
                    iVar4 = NuStrICmp(fp->word_buf, "gold_bricks");
                    if (iVar4 == 0) {
                        collect->type = 6;
                        iVar4 = NuFParGetInt(fp);
                        uVar2 = (ushort)(iVar4 >> 31);
                        collect->field6_0xa = ((ushort)iVar4 ^ uVar2) - uVar2;
                    } else {
                        iVar4 = NuStrICmp(fp->word_buf, "all_minikits_complete");
                        if (iVar4 == 0) {
                            collect->type = 4;
                        } else {
                            iVar4 = NuStrICmp(fp->word_buf, "minikit");
                            if (iVar4 == 0) {
                                collect->type = 5;
                            } else {
                                iVar4 = NuStrICmp(fp->word_buf, "buy_in_shop");
                                if (iVar4 == 0) {
                                    collect->can_buy = 1;
                                    iVar4 = NuFParGetInt(fp);
                                    collect->field3_0x4 = iVar4;
                                } else {
                                    iVar4 = NuStrICmp(fp->word_buf, "cheat_code");
                                    if (((iVar4 == 0) && (iVar4 = NuFParGetWord(fp), iVar4 != 0)) &&
                                        (iVar4 = NuStrLen(fp->word_buf), iVar4 == 6)) {
                                        buf = collect->cheat_code;
                                        NuStrCpy(buf, fp->word_buf);
                                        NuStrUpr(buf, buf);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    goto LAB_004eb886;
}
