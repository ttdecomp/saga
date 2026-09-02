// Pending-transcription stand-ins (original exports without decompiled
// bodies yet). Collected here so each is an explicit, greppable TODO;
// they previously lived as anonymous extern-C stubs that shadowed real
// transcriptions elsewhere.

#include "decomp.h"
#include "nu2api/nu3d/nutexanm.h"

extern "C" void NuShaderManagerDestroy(void) {
}

extern "C" void NuShaderManagerDestroyShaders(void) {
}

extern "C" void NuShaderManagerForceShader(void) {
}

extern "C" void NuShaderManagerGetInstance(void) {
}

extern "C" void NuShaderManagerGetShininessFactor(void) {
}

extern "C" void NuShaderManagerLoadCompiledShaders(void) {
}

extern "C" void NuShaderManagerSetCurrentShader(void) {
}

extern "C" void NuShaderManagerSetElementfv(void) {
}

extern "C" void NuShaderManagerSetElementsfv(void) {
}

extern "C" void NuShaderManagerSetElementsfv_transpose(void) {
}

extern "C" void NuShaderManagerSetShaderSaveFolder(void) {
}

extern "C" void NuShaderManagerSetShininessFactor(void) {
}

extern "C" void NuShaderObjectBaseUpdateWaterTable(void) {
}

extern "C" void NuShaderObjectGLSLAllocateParameter(void) {
}

extern "C" void NuShaderObjectGLSLSetupTextureStates(void) {
}

extern "C" void NuShaderObjectKeyGenerate2(void) {
}

extern "C" void NuShaderObjectKeyGenerate3(void) {
}

extern "C" void NuShaderObjectKeyGenerate4(void) {
}

extern "C" void NuShaderObjectKeySetUberShaderHash(void) {
}

extern "C" void NuShaderObjectLoadFromFile(void) {
}

extern "C" void NuShaderObjectSetElementsfv_transpose(void) {
}

extern "C" void NuShaderObjectUnserialize(void) {
}

extern "C" void NuTexAnimAddList(void) {
}

extern "C" void NuTexAnimCreate(void) {
}

extern "C" void NuTexAnimDestroy(void) {
}

extern "C" void NuTexAnimEnvCreate(void) {
}

extern "C" void NuTexAnimEnvDestroy(void) {
}

extern "C" void NuTexAnimEnvProc(void) {
}

extern "C" void NuTexAnimEnvReset(nutexanimenv_s *env) {
    if (env == NULL) {
        return;
    }

    env->texture = NULL;
    env->program = NULL;
    env->field_cc = NULL;
    env->field_d0 = NULL;
    env->field_d4 = NULL;
    env->field_d8 = NULL;
    env->field_e4 = NULL;
}

extern "C" void NuTexAnimFind(void) {
}

extern "C" void NuTexAnimProgAssembleEnd(void) {
}

extern "C" void NuTexAnimProgCreate(void) {
}

extern "C" void NuTexAnimProgDestroy(void) {
}

extern "C" void NuTexAnimProgFind(void) {
}

extern "C" void NuTexAnimProgRead(void) {
}

extern "C" void NuTexAnimProgReadCFG(void) {
}

extern "C" void NuTexAnimProgReadScript(void) {
}

extern "C" void NuTexAnimProgRelease(void) {
}

extern "C" void NuTexAnimProgSysInit(void) {
}

extern "C" void NuTexAnimProgWrite(void) {
}

extern "C" void NuTexAnimRemoveList(void *) {
}

extern "C" void NuTexAnimRestart(void) {
}

extern "C" void NuTexAnimSetMask(int) {
}

extern "C" void NuTexAnimSetSignals(void) {
}

extern "C" void _Z26DisplayListLinkDynamicMtlsv(void) {
}
