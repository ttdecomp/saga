#include "nu2api_nusound_types.h"

void SetSoundFadeDist(WORLDINFO_s *, OPTIONSSAVE_s *) {
}

void edanimSoundPlace(i32, nuvec_s *) {
}

void edanimSoundCreate(nuvec_s *) {
}

void GameSetSoundVolume(OPTIONSSAVE_s *) {
}

NuSoundListener *NuSoundSystem::GetNearestRealListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &) {
    return NULL;
}

NuSoundListener *NuSoundSystem::GetNearestFocusListener(NuEList<NuSoundListener, DefaultElist> const &, VuVec const &,
                                                        float &) {
    return NULL;
}
