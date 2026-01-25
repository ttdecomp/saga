#ifndef NU2API_SAGA_NUCORE_NUVIDEO_H
#define NU2API_SAGA_NUCORE_NUVIDEO_H

enum NUVIDEO_SWAPMODE {
    NUVIDEO_SWAPMODE_INVALID = 0,
    NUVIDEO_SWAPMODE_ASYNC = 1,
    NUVIDEO_SWAPMODE_FIELDSYNC = 2,
    NUVIDEO_SWAPMODE_FRAMESYNC = 3,
    NUVIDEO_SWAPMODE_MIN2FIELD = 4,
    NUVIDEO_SWAPMODE_ROLLING = 5,
    NUVIDEO_SWAPMODE_ROLLING2FRAMES = 6,
    NUVIDEO_SWAPMODE_CNT = 7
};

typedef enum NUVIDEO_SWAPMODE NUVIDEO_SWAPMODE;

enum {
	NUVIDEOMODE_UNDEFINED = 0,
	NUVIDEOMODE_UNUSED1 = 1,
	NUVIDEOMODE_NTSC = 2,
	NUVIDEOMODE_PAL = 3,
	NUVIDEOMODE_PALFF = 4,
	NUVIDEOMODE_NTSCFF = 5,
	NUVIDEOMODE_NTSC_J = 6
};

#ifdef __cplusplus
int NuVideoGetAspectPS(void);
void NuVideoSetBrightnessPS();
void NuVideoSetSwapModePS(NUVIDEO_SWAPMODE video_swap_mode);

extern "C" {
#endif

int NuVideoGetMode(void);
void NuVideoSetMode(int mode);
int NuVideoGetAspect(void);
NUVIDEO_SWAPMODE NuVideoGetSwapMode(void);
void NuVideoRollingFrameRateReset(void);
void NuVideoSetBrightness(float brightness);
void NuVideoSetResolution(int width, int height);
NUVIDEO_SWAPMODE NuVideoSetSwapMode(NUVIDEO_SWAPMODE video_swap_mode);

#ifdef __cplusplus
}
#endif

#endif // NU2API_SAGA_NUCORE_NUPAD_H