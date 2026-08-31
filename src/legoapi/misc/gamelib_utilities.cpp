#include "legoapi/legoapi_types.h"
#include "globals.h"

struct SoundGroup {
    i16 first_sample;
    i16 sample_count;
    i16 field_0x4;
    i16 field_0x6;
};

SoundGroup g_groups[128];
i16 g_groupBuffer[512];
i32 g_lenGroupBuffer;

DECOMP_ASSERT(sizeof(SoundGroup) == 8, "SoundGroup size");

void GroupBuffer_InGroup(i32, i32) {
}

void GroupBuffer_GetSample(i32, i32) {
}

void GroupBuffer_MakeGroup(i32) {
}

void GroupBuffer_MoveToEnd(i32) {
}

void GroupBuffer_AddToGroup(i32, i32) {
}

void GroupBuffer_RemoveGroup(i32) {
}

i32 GroupBuffer_GetNumInGroup(i32 group_id) {
    return g_groups[group_id].sample_count;
}

void GroupBuffer_RemoveFromGroup(i32, i32) {
}

i32 GroupBuffer_GetSampleByIndex(i32 group_id, i32 sample_index) {
    return g_groupBuffer[g_groups[group_id].first_sample + sample_index];
}

extern "C" {

    void UtilFrameStart(void) {
    }

    void UtilGetFrameStartTime(void) {
    }

    void UtilGetTime(void) {
    }

    void VuQuatSlerpFast(void) {
    }

    void buildBitCountTable(void) {
        BitCountTable[0] = 0;
        for (u32 value = 0; value < 0x100; ++value) {
            if ((value & 2) != 0) {
                ++BitCountTable[value];
            }
            if ((value & 4) != 0) {
                ++BitCountTable[value];
            }
            if ((value & 8) != 0) {
                ++BitCountTable[value];
            }
            if ((value & 0x10) != 0) {
                ++BitCountTable[value];
            }
            if ((value & 0x20) != 0) {
                ++BitCountTable[value];
            }
            if ((value & 0x40) != 0) {
                ++BitCountTable[value];
            }
            if ((value >> 7) != 0) {
                ++BitCountTable[value];
            }
            if (value + 1 < 0x100) {
                BitCountTable[value + 1] = static_cast<u8>((value + 1) & 1);
            }
        }
        isBitCountTable = 1;
    }

} // extern "C"
