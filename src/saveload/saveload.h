#pragma once

#include <stdint.h>

#include "decomp.h"
#include "nu2api.saga/nucore/common.h"

C_API_START

extern int32_t saveload_status;
extern int32_t saveload_autosave;
extern int32_t saveload_savepresent;

extern int32_t saveload_slotused[6];
extern int32_t saveload_slotcode[6];

void saveloadInit(VARIPTR *buf, VARIPTR buf_end, int32_t, char *prodcode, char *iconname, char *unicodename,
                  int32_t);

int32_t saveloadLoadSlot(int32_t slot, void *buffer, size_t size);

int32_t TriggerExtraDataLoad(void);

void SaveSystemInitialise(int32_t slots, void *makeSaveHash, void *save, int32_t saveSize, int32_t saveCount,
                          void *drawSaveIcon, void *extradata, int32_t extradataSize);

int32_t ChecksumSaveData(void *buffer, int32_t size);

void saveloadASSave(int32_t slot, void *buffer, int32_t size, uint32_t hash);
void saveloadASLoad(int32_t slot, void *buffer, int32_t size);
void saveloadASDelete(int32_t slot);

C_API_END
