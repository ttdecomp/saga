#include "legoapi/items/base/collection.h"

#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/menus/screens/store.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
struct GIZMOPICKUP_s;
struct PART_s;
struct starfighter_s;

struct APICHARACTERMODELLIST_s;

COLLECTID *TempCollectID = NULL;

i32 CollectCount = 0;
COLLECTID *CollectList = NULL;
i32 COLLECTION_COMPLETIONCOUNT = 0;

i32 InCollectList_Index(i32 id, COLLECTID *list, i32 count) {
    i32 i;
    COLLECTID *p;

    TempCollectID = NULL;

    if (list == NULL) {
        list = CollectList;
        count = CollectCount;
        if (list == NULL)
            return id;
    }

    p = list;
    for (i = 0; i < count; i++, p++) {
        if (p->id == id) {
            TempCollectID = p;
            return i;
        }
    }

    return -1;
}

i32 Collection_Got(i32 id) {
    if (InCollectList_Index(id, NULL, 0) == -1) {
        return 0;
    }

    i32 area = AreaFromMiniKitID(id);
    if (area != -1) {
        if (Game_AreaSave == NULL) {
            return 0;
        }
        return Game_AreaSave[area].minikit_count >= 1 ? 2 : 0;
    }

    if (Game_CharacterSave != NULL && (Game_CharacterSave[id] & 1) == 0) {
        return 0;
    }
    return 1;
}

void Collection_Configure(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd) {
    byte bVar1;
    ushort uVar2;
    i16 sVar3;
    COLLECTID *collect;
    i32 iVar4;
    AREADATA *pAVar5;
    i32 i;
    char *buf;

    nufpar_s *fp = NuFParCreate(file);
    if (fp != NULL) {
        CollectCount = 0;
        collect = (COLLECTID *)ALIGN(bufferStart->addr, 4);
        CollectList = collect;
        bufferStart->void_ptr = collect;

        COLLECTION_COMPLETIONCOUNT = 0;

        while (NuFParGetLine(fp) != 0) {

        LAB_004eb7f3:
            if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "collect") == 0 && NuFParGetWord(fp) != 0) {
                sVar3 = CharIDFromName(fp->word_buf);

                LOG_DEBUG("Collection_Configure: Found collect id %s -> %d", fp->word_buf, sVar3);

                collect->id = sVar3;

                if (sVar3 != -1 && InCollectList_Index((i32)sVar3, CollectList, CollectCount) == -1) {
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

void Collection_Draw(COLLECTION_s *, float, float, float, APICHARACTERMODELLIST_s *, float, i32) {
}

void Collection_GetPos(COLLECTION_s *collection, i32 id, float *x, float *y) {
    COLLECTID *list = collection->list;
    i32 count = collection->count_y;

    for (i32 i = 0; i < count; i++) {
        if (list[i].id == id) {
            float *f = (float *)&list[i].cheat_code[8];
            *x = f[0];
            *y = f[1];
            return;
        }
    }
}

i32 Collection_GetIDList(COLLECTION_s *collection, u32 model_flag_mask, u32 required_model_flags, i16 *ids,
                         i32 *first_id, i32 *second_id, i32 unused) {
    (void)unused;
    if (first_id != NULL) {
        *first_id = -1;
    }
    if (second_id != NULL) {
        *second_id = -1;
    }

    i32 result_count = 0;
    for (i32 index = 0; index < collection->count_y; ++index) {
        const i16 id = collection->list[index].id;
        if ((CDataList[id].model_flags & model_flag_mask) != required_model_flags || Collection_Got(id) == 0) {
            continue;
        }

        if (ids != NULL) {
            ids[result_count] = id;
        }
        ++result_count;

        if (first_id != NULL && *first_id == -1) {
            *first_id = id;
        } else if (first_id != NULL && second_id != NULL && *second_id == -1) {
            *second_id = id;
        }
    }

    if (ids != NULL) {
        ids[result_count] = -1;
    }
    return result_count;
}

void Collection_CreateCustom(char *, i16 *, COLLECTION_s *, u32, u32, u32, i32, i32, variptr_u *, variptr_u *, i32,
                             float) {
}

void Collection_CreateMaster(char *file, i16 *idlist, COLLECTION_s *collection, i32 param4, float param5) {
    collection->list = CollectList;
    collection->count_x = (u16)param4;
    collection->count_y = (u16)CollectCount;
    collection->field_8 = idlist;
    collection->field_c = file;
    collection->field_10 = param5;
}

i32 Collection_GotAnyOfType(i32 type, u32 flags) {
    i32 count;

    if (CollectList == NULL)
        return 0;

    count = CollectCount;
    if (count <= 0)
        return 0;

    for (i32 i = 0; i < count; i++) {
        i32 id = CollectList[i].id;
        if (type == -1) {
            if (flags == 0) {
                if (Collection_Got(id))
                    return 1;
            } else if ((CDataList[id].model_flags & flags) == flags) {
                if (Collection_Got(id))
                    return 1;
            }
        } else {
            if ((signed char)GCDataList[id].field275_0x116 != type)
                continue;
            if (flags == 0) {
                if (Collection_Got(id))
                    return 1;
            } else if ((CDataList[id].model_flags & flags) == flags) {
                if (Collection_Got(id))
                    return 1;
            }
        }
    }

    return 0;
}

static __used__ void Collection_GetSelectingPlayerIDs(i16 *) {
}

void ReleaseEat(GameObject_s *) {
}

void SetCoinType(i32, GIZMOPICKUP_s *) {
}

void PartStop_Coin(PART_s *) {
}

void ShipDropCoins(starfighter_s *) {
}

void AddToCollection(i32) {
}

void AddToGoldBricks() {
}

void Pup_CollectCoin(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

void ResetCoinPacket(COINPACKET_s *) {
}

void UpdateCoinPacket(COINPACKET_s *, i32, i32) {
}

void TotalLevelCoinTally(WORLDINFO_s *, u32 *, u32 *, u32 *, u32 *, u32 *, u32 *, u32 *) {
}

void AddToCompletionPoints(u32) {
}

COLLECTION_s *GetFreePlayCollection(i32 area) {
    const u16 area_flags = ADataList[area].flags;
    if ((area_flags & AREAFLAG_VEHICLE_AREA) == 0) {
        return &CharacterCollection;
    }
    if ((area_flags & AREAFLAG_BONUS_AREA) != 0) {
        return &MiniKitCollection;
    }
    return &VehicleCollection;
}

void ReCalculateCompletionPoints() {
}

void LoseCoins(GameObject_s *, i32) {
}
