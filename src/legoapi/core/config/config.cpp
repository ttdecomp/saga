#include "decomp.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nuvideo.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern OPTIONSSAVE *Game_OptionsSave;
f32 GameSetSoundVolume(OPTIONSSAVE *options);
f32 GameSetMusicVolume(OPTIONSSAVE *options);

void RestoreOptions() {
    GameSetSoundVolume(Game_OptionsSave);
    GameSetMusicVolume(Game_OptionsSave);

    if (Game_OptionsSave != NULL) {
        NuVideoSetBrightness(static_cast<f32>(Game_OptionsSave->field12_0xc) / 10.0f);
        MechSystems::Get()->input_touch_system.control_mode = SuperOptions.touch_controls == 0 ? 1 : 2;
    }
}
