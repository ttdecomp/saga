#include "nu2api/nu3d/nudynamiclight.h"
#include "decomp.h"
#include <stdbool.h>
int NuDynamicLightAddRenderScene(NUDYNAMICLIGHTS* a1, int a2, int a3) {
  UNIMPLEMENTED();
  return 1;
}

bool NuDynamicLightIsEnabled(NUDYNAMICLIGHTS* param_1) {
  return param_1->isEnabled != 0;
}