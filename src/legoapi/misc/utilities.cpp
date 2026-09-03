#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "globals.h"

#include "nu2api/numath/nurand.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern i32 qseed;
extern i32 GAMERAND;
extern "C" void DebrisSetSeed(i32);
extern "C" void NuPartSetSeed(i32);

void HashString(unsigned char *) {
}

void ResetSeeds() {
    qseed = 0x3039;
    GAMERAND = 0x1f3ad27f;
    NuRandSeed(0);
    DebrisSetSeed(0);
    NuRandSetSeed(NULL, 0);
    NuPartSetSeed(0);
}

void makenuvec4(float, float, float, float) {
}

void FindAnglesXY(nuvec_s *, u16 *, u16 *) {
}

void FindAnglesZX(nuvec_s *normal, u16 *x_rotation, u16 *z_rotation) {
    const i32 x_angle = NuAtan2D(normal->z, normal->y);
    if (x_rotation != NULL) {
        *x_rotation = static_cast<u16>(x_angle);
    }
    temp_xrot = static_cast<i16>(x_angle);

    NUVEC rotated;
    const u16 truncated_x_angle = static_cast<u16>(x_angle);
    NuVecRotateX(&rotated, normal, static_cast<NUANG>(-static_cast<i32>(truncated_x_angle)));
    const i32 z_angle = -NuAtan2D(rotated.x, rotated.y);
    if (z_rotation != NULL) {
        *z_rotation = static_cast<u16>(z_angle);
    }
    temp_zrot = static_cast<i16>(z_angle);
}

void getNumDigits(i32) {
}

void LineCrossedXZ(float, float, float, float, float, float, float, float) {
}

void ScaleAndClamp(i32) {
}

void VecRotateAxis(nuvec_s *, u16, nuvec_s *) {
}

void SolveQuadratic(float, float, float, float *, float *) {
}

void XZLinesClosest(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float *, float *) {
}

void LineIntersectXY(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void MakeThrowVector(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float, float) {
}

i32 OnOrInsidePlane(nuvec_s *point, nuvec_s *plane_point, nuvec_s *plane_normal, nuvec_s *corrected_point,
                    float normal_offset, float *distance_out) {
    NUVEC test_point;
    if (normal_offset != 0.0f) {
        NuVecScale(&test_point, plane_normal, normal_offset);
        NuVecAdd(&test_point, &test_point, point);
    } else {
        test_point = *point;
    }

    const f32 distance = (test_point.x - plane_point->x) * plane_normal->x +
                         (test_point.y - plane_point->y) * plane_normal->y +
                         (test_point.z - plane_point->z) * plane_normal->z;
    if (distance_out != NULL) {
        *distance_out = distance;
    }
    if (distance > 0.0f) {
        return 0;
    }

    if (corrected_point != NULL) {
        NuVecScale(corrected_point, plane_normal, 0.5f - distance);
        NuVecAdd(corrected_point, corrected_point, point);
    }
    return 1;
}

void PackCharIntoInt(char, char, char, char) {
}

void TerCrossProduct(nuvec_s *, nuvec_s *) {
}

void DistanceToLineXZ(nuvec_s *, nuvec_s *, nuvec_s *) {
}

i32 MatrixReflection(numtx_s *, i32, float, float, numtx_s *) {
    return 0;
}

void OnOrOutsidePlane(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void PackShortIntoInt(i16, i16) {
}

void RatioAlongLineXZ(nuvec_s *, nuvec_s *, nuvec_s *) {
}

i32 XZLinesIntersect(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float *, float *) {
    return 0;
}

void GetRotationAngles(nuvec_s *, u16 *, u16 *) {
}

void UnpackCharFromInt(i32, char &, char &, char &, char &) {
}

void RatioBetweenPlanes(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void UnpackShortFromInt(i32, i16 &, i16 &) {
}

void AnglesBetweenPoints(nuvec_s *, nuvec_s *, u16 *, u16 *) {
}

void LineIntersectCircle(nuvec_s *, nuvec_s *, nuvec_s *, float) {
}

void LineIntersectSphere(nuvec_s *, nuvec_s *, nuvec_s *, float, float *) {
}

void LineToPlaneDistance(VuVec &, VuVec &, VuVec &) {
}

void LineToPointDistance(VuVec &, VuVec &, VuVec &, VuVec *) {
}

void RatioBetweenEdgesXZ(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void SphereSphereOverlap(nuvec_s *, float, nuvec_s *, float) {
}

void CalcAveragePosAndRad(GIZBUILDIT_s &, VuVec &, float &, bool) {
}

void LineToPlaneIntersecion(VuVec &, VuVec &, VuVec &, VuVec *) {
}

void CalculateInterceptVector(nuvec_s *, nuvec_s *, nuvec_s *, float, nuvec_s *, nuvec_s *) {
}

void LineToSphereIntersection(VuVec &, VuVec &, VuVec &, float, VuVec *, VuVec *) {
}

void MatrixReflectionVU0_AXISY(numtx_s *, float, float, numtx_s *) {
}

void SphereSphereOverlapScaleY(nuvec_s *, float, float, nuvec_s *, float, float) {
}

void IToX(char *, i32) {
}

void XToI(char *) {
}

void IsTok(char const *, char const *) {
}

void CapVec(nuvec_s *, float, nuvec_s *) {
}

void I64ToX(char *, i64) {
}

void XToI64(char *) {
}

i32 RotDiff(u16 current, u16 target) {
    i32 difference = static_cast<u32>(target) - static_cast<u32>(current);
    if (difference > 0x8000) {
        difference -= 0x10000;
    } else if (difference < -0x8000) {
        difference += 0x10000;
    }
    return difference;
}

void memmove(void *, void const *, i32) {
}

void rawClip(VuVec const *, VuVec *, i32, VuVec const &) {
}

void getqseed() {
}

void clampBias(float) {
}

i32 findrange(nugscn_s *scene, i32 first_joint) {
    // This helper's original C++ ABI names nugscn_s, but animation callers
    // pass the hierarchy object whose joint table starts at the same offsets.
    nuhgobj_s *object = reinterpret_cast<nuhgobj_s *>(scene);
    const i32 joint_count = object->joint_count;
    nuhgobjjoint_s *joints = object->joints;
    const i32 parent_index = joints[first_joint].parent_index;
    i32 end_joint = first_joint + 1;
    while (end_joint < joint_count && joints[end_joint].parent_index != parent_index) {
        ++end_joint;
    }
    return end_joint - 1;
}

void makenuvec(float, float, float) {
}

static __used__ i32 MatchExtension(char *, char *, i32) {
    return 0;
}

static __used__ float fxy(unsigned int, unsigned int) {
    return 0;
}

static __used__ unsigned int fxyda(float, float) {
    return 0;
}

static __used__ float fxydaf(float, float) {
    return 0;
}

static __used__ int icomp(const void *, const void *) {
    return 0;
}

static __used__ i32 sort32a(void const *, void const *) {
    return 0;
}
