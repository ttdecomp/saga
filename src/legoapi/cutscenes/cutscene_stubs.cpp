#include "legoapi/world/world_shared.h"
#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nugcutscene.h"

#include <string.h>

extern "C" i32 NuGCutLocatorIsVisble(NUGCUTLOCATOR_s *, f32, nuanimtime_s *, f32 *, f32 *);
extern "C" i32 NuGCutLocatorCalcMtx(NUGCUTLOCATOR_s *, f32, NUMTX *, nuanimtime_s *);

extern "C" {
    extern debinftype **debtab;
    extern NUGCUTLOCATORFNENTRY_s *locatorfns;
    extern f32 timeincrement;
    extern i32 processdeb;
    extern f32 glyntestha;

    i32 (*TriggerLocatorVfxFn)(u16, f32 *) = NULL;
    void (*ReleaseLocatorVfxFn)(i32) = NULL;
    void (*UpdateLocatorVfxFn)(i32, f32 *) = NULL;
    void (*NuCutSceneSFXUpdate)(NUGCUTLOCATORSYS_s *, instNUGCUTLOCATOR_s *, NUGCUTLOCATOR_s *, f32, NUMTX *,
                                i32) = NULL;

    void NuAnimData2CalcTime(nuanimdata2_s *, f32, nuanimtime_s *);
    void NuMtxPreTranslate(NUMTX *, NUVEC *);
    void NuMtxMul(NUMTX *, NUMTX *, NUMTX *);

    i32 AddScaledVariableShotDebrisEffect2(i32, NUVEC *, i32, f32, NUMTX *, NUMTX *, f32);
    void AddDebrisEffect(i32 *, i32, f32, f32, f32);
    void DebrisPopulateInstance(i32, f32);
    void DebrisEmitterPos(i32, f32, f32, f32);
    void DebrisOrientationMtx(i32, NUMTX *);
    void DebrisPosOrientationMtx(i32, NUMTX *);
    void AddScaledFiniteShotDebrisEffect(i32 *, i32, NUVEC *, i32, i32, i32, f32);
    void DebrisEmitterOrientationMtx(i32, NUMTX *);
    void DebFreeInstantly(i32 *);
    void NuRndrAxisArrowsMtx(NUMTX *, f32, i32);
}

extern "C" {

    void CheckStreamFileID(void) {
    }

    void ClearLinkedCutSceneMusic(void) {
    }

    void DisplayCutSceneMemory(void) {
    }

    void PauseGameCut(void) {
    }

    void RestoreGameCut(void) {
    }

    void SetForceScenePlayBack(void) {
    }

    void instCutSceneTimeElapsed(void) {
    }

    void instNuGCutSceneAddCamTgt(void) {
    }

    void instNuGCutSceneAddCleanUpItem(void) {
    }

    void instNuGCutSceneCalculateAverageCentre(void) {
    }

    void instNuGCutSceneChain(void) {
    }

    void instNuGCutSceneCharGetStartMtx(void) {
    }

    void instNuGCutSceneCleanUp(void) {
    }

    void instNuGCutSceneCreateCamTgtArray(void) {
    }

    void instNuGCutSceneDestroy(void) {
    }

    void instNuGCutSceneDisable(void) {
    }

    void instNuGCutSceneEnable(void) {
    }

    void instNuGCutSceneEnd(void) {
    }

    void instNuGCutSceneFind(void) {
    }

    void instNuGCutSceneIsFinished(void) {
    }

    void instNuGCutSceneJumpToEnd(void) {
    }

    void instNuGCutSceneJumpToLastFrame(void) {
    }

    void instNuGCutScenePlay(void) {
    }

    void instNuGCutScenePreload(void) {
    }

    void instNuGCutSceneResetCleanUp(void) {
    }

    void instNuGCutSceneRotateY(void) {
    }

    void instNuGCutSceneServiceLoad(void) {
    }

    void instNuGCutSceneSetEndCallback(void) {
    }

    void instNuGCutSceneSetMtx(void) {
    }

    void instNuGCutSceneSetPos(void) {
    }

    void instNuGCutSceneSetRepeat(void) {
    }

    void instNuGCutSceneStop(void) {
    }

    void instNuGCutSceneTimeLeft(void) {
    }

    void instNuGCutSceneTotalTime(void) {
    }

    void instNuGCutSceneTranslate(void) {
    }

    void instNuGCutSceneWaitAtEnd(void) {
    }

    void instNuGCutSoundStream(void) {
    }

    void instNuGCutLocatorUpdate(instNUGCUTSCENE_s *instance, NUGCUTLOCATORSYS_s *system,
                                 instNUGCUTLOCATOR_s *inst_locator, NUGCUTLOCATOR_s *locator, f32 frame,
                                 NUMTX *parent_mtx, i32 paused) {
        NUGCUTLOCATORTYPE_s *type = &system->types[locator->type_index];
        i32 effect_index = static_cast<i32>(static_cast<u16>(type->function_index));
        debinftype *effect =
            debtab != NULL && effect_index >= 0 && effect_index != 0xffff ? debtab[effect_index] : NULL;

        if ((type->flags & 1) != 0) {
            if (paused != 0 || effect == NULL) {
                return;
            }
            nuanimtime_s time;
            f32 scale = 0.0f;
            f32 rate = 0.0f;
            if (locator->animation != NULL) {
                NuAnimData2CalcTime(locator->animation, frame, &time);
            }
            i32 visible = NuGCutLocatorIsVisble(locator, frame, &time, &scale, &rate);
            if (visible != 0 && scale < 0.0f && (locator->flags & 0x10) == 0) {
                scale = static_cast<f32>(effect->frequency) * -scale;
            }
            if (visible != 0 && scale > 0.1f) {
                if ((locator->flags & 0x10) != 0 && inst_locator->effect_handle != 0) {
                    return;
                }
                NUMTX matrix;
                NuGCutLocatorCalcMtx(locator, frame, &matrix, &time);
                if ((locator->flags & 4) != 0) {
                    NuMtxPreTranslate(&matrix, &locator->pivot);
                }
                if (parent_mtx != NULL) {
                    NuMtxMul(&matrix, &matrix, parent_mtx);
                }
                if ((locator->flags & 0x10) == 0) {
                    if ((locator->flags & 0x20) == 0) {
                        AddScaledVariableShotDebrisEffect2(effect_index, reinterpret_cast<NUVEC *>(&matrix.m30),
                                                           static_cast<i32>(scale), timeincrement, &matrix, 0, rate);
                    } else {
                        if (inst_locator->effect_handle == -1) {
                            AddDebrisEffect(&inst_locator->effect_handle, effect_index, matrix.m30, matrix.m31,
                                            matrix.m32);
                            DebrisPopulateInstance(inst_locator->effect_handle, 0.0f);
                        }
                        if (inst_locator->effect_handle >= 0) {
                            if (processdeb == 1) {
                                DebrisEmitterPos(inst_locator->effect_handle, matrix.m30, matrix.m31, matrix.m32);
                            } else if (processdeb == 2) {
                                DebrisOrientationMtx(inst_locator->effect_handle, &matrix);
                            } else {
                                DebrisPosOrientationMtx(inst_locator->effect_handle, &matrix);
                            }
                        }
                        if (glyntestha > 0.0f) {
                            NuRndrAxisArrowsMtx(&matrix, glyntestha, 0);
                        }
                    }
                } else {
                    i32 finite_handle = -1;
                    AddScaledFiniteShotDebrisEffect(&finite_handle, effect_index,
                                                    reinterpret_cast<NUVEC *>(&matrix.m30), 0, 0, 1, rate);
                    DebrisEmitterOrientationMtx(finite_handle, &matrix);
                    inst_locator->effect_handle = 1;
                }
                return;
            }
            if ((locator->flags & 0x20) == 0) {
                inst_locator->effect_handle = 0;
            } else if (inst_locator->effect_handle >= 0) {
                DebFreeInstantly(&inst_locator->effect_handle);
            }
            inst_locator->field_00 = 0;
            return;
        }

        if ((type->flags & 2) != 0) {
            if (effect_index >= 0 && locatorfns != NULL) {
                NUGCUTLOCATORFN fn = locatorfns[effect_index].function;
                if (fn != NULL) {
                    fn(instance, system, inst_locator, locator, frame, parent_mtx, paused);
                    return;
                }
            }
        } else if ((type->flags & 0x10) != 0 && paused == 0) {
            if (TriggerLocatorVfxFn == NULL || ReleaseLocatorVfxFn == NULL || UpdateLocatorVfxFn == NULL) {
                return;
            }
            f32 opacity = 0.0f;
            nuanimtime_s time;
            if (locator->animation != NULL) {
                NuAnimData2CalcTime(locator->animation, frame, &time);
            }
            if (NuGCutLocatorIsVisble(locator, frame, &time, &opacity, NULL) == 0 || opacity <= 0.1f) {
                if (inst_locator->effect_handle >= 0) {
                    ReleaseLocatorVfxFn(inst_locator->effect_handle);
                }
                inst_locator->effect_handle = -1;
            } else {
                NUMTX matrix;
                NuGCutLocatorCalcMtx(locator, frame, &matrix, &time);
                if ((locator->flags & 4) != 0) {
                    NuMtxPreTranslate(&matrix, &locator->pivot);
                }
                if (parent_mtx != NULL) {
                    NuMtxMul(&matrix, &matrix, parent_mtx);
                }
                f32 values[16];
                memcpy(values, &matrix, sizeof(values));
                if (inst_locator->effect_handle < 0) {
                    inst_locator->effect_handle = TriggerLocatorVfxFn(static_cast<u16>(effect_index), values);
                } else {
                    UpdateLocatorVfxFn(inst_locator->effect_handle, values);
                }
            }
        } else if ((type->flags & 4) != 0 && NuCutSceneSFXUpdate != NULL) {
            NuCutSceneSFXUpdate(system, inst_locator, locator, frame, parent_mtx, paused);
        }
    }

} // extern "C"
