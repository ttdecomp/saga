#include "nu2api.saga/nuandroid/nuios.h"

int g_isLowEndDevice;

int NuIOS_IsLowEndDevice(void) {
    return g_isLowEndDevice;
}