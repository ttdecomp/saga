#include "nu2api/nu3d/nuspline.h"

#include "nu2api/nucore/nustring.h"

// Finds the spline named `name` in `scene`, or NULL if there is no such
// spline.
NUGSPLINE *NuSplineFind(NUGSCN *scene, char *name) {
    if (scene == NULL) {
        return NULL;
    }

    NUGSPLINE *spline = scene->splines;

    if (scene->numsplines <= 0) {
        return NULL;
    }

    for (int i = 0; i < scene->numsplines; i++, spline++) {
        if (NuStrICmp(name, spline->name) == 0) {
            return spline;
        }
    }

    return NULL;
}
