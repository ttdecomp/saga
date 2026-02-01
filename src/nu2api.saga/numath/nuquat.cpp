#include "nu2api.saga/numath/nuquat.h"

#include "nu2api.saga/numath/nufloat.h"
#include "nu2api.saga/numath/nutrig.h"

void NuQuatFromEulerXYZ(NUQUAT *out, NUANG psi, NUANG theta, NUANG phi) {
    float sin_psi_over_2;
    float cos_psi_over_2;
    float sin_theta_over_2;
    float cos_theta_over_2;
    float sin_phi_over_2;
    float cos_phi_over_2;
    float sin_phi_times_sin_theta;
    float cos_phi_times_cos_theta;
    float sin_phi_times_cos_theta;
    float cos_phi_times_sin_theta;

    psi /= 2;
    theta /= 2;
    phi /= 2;

    cos_psi_over_2 = NU_COS_LUT(psi);
    sin_psi_over_2 = NU_SIN_LUT(psi);

    sin_theta_over_2 = NU_SIN_LUT(theta);
    cos_theta_over_2 = NU_COS_LUT(theta);

    cos_phi_over_2 = NU_COS_LUT(phi);
    sin_phi_over_2 = NU_SIN_LUT(phi);

    cos_phi_times_cos_theta = cos_phi_over_2 * cos_theta_over_2;
    sin_phi_times_sin_theta = sin_phi_over_2 * sin_theta_over_2;

    out->w = cos_phi_times_cos_theta * cos_psi_over_2 + sin_phi_times_sin_theta * sin_psi_over_2;
    out->x = cos_phi_times_cos_theta * sin_psi_over_2 - sin_phi_times_sin_theta * cos_psi_over_2;

    cos_phi_times_sin_theta = cos_phi_over_2 * sin_theta_over_2;
    sin_phi_times_cos_theta = sin_phi_over_2 * cos_theta_over_2;

    out->y = cos_phi_times_sin_theta * cos_psi_over_2 + sin_phi_times_cos_theta * sin_psi_over_2;
    out->z = sin_phi_times_cos_theta * cos_psi_over_2 - cos_phi_times_sin_theta * sin_psi_over_2;
}

void NuQuatToMtx(NUQUAT *quat, NUMTX *out) {
    float w_sq;
    float x_sq;
    float y_sq;
    float z_sq;
    float xy_times_2;
    float xz_times_2;
    float xw_times_2;
    float yz_times_2;
    float yw_times_2;
    float zw_times_2;

    w_sq = quat->w * quat->w;
    x_sq = quat->x * quat->x;
    y_sq = quat->y * quat->y;
    z_sq = quat->z * quat->z;

    xy_times_2 = (quat->x * quat->y) + (quat->x * quat->y);
    xz_times_2 = (quat->x * quat->z) + (quat->x * quat->z);
    xw_times_2 = (quat->x * quat->w) + (quat->x * quat->w);
    yz_times_2 = (quat->y * quat->z) + (quat->y * quat->z);
    yw_times_2 = (quat->y * quat->w) + (quat->y * quat->w);
    zw_times_2 = (quat->z * quat->w) + (quat->z * quat->w);

    out->m00 = w_sq + x_sq - y_sq - z_sq;
    out->m10 = xy_times_2 - zw_times_2;
    out->m20 = xz_times_2 + yw_times_2;
    out->m30 = 0.0f;

    out->m01 = xy_times_2 + zw_times_2;
    out->m11 = w_sq - x_sq + y_sq - z_sq;
    out->m21 = yz_times_2 - xw_times_2;
    out->m31 = 0.0f;

    out->m02 = xz_times_2 - yw_times_2;
    out->m12 = yz_times_2 + xw_times_2;
    out->m22 = w_sq - x_sq - y_sq + z_sq;
    out->m32 = 0.0f;

    out->m03 = 0.0f;
    out->m13 = 0.0f;
    out->m23 = 0.0f;
    out->m33 = 1.0f;
}

void NuQuatInv(NUQUAT *out, NUQUAT *quat) {
    float recip_sq;

    recip_sq = 1.0f / (quat->w * quat->w + quat->x * quat->x + quat->y * quat->y + quat->z * quat->z);

    out->w = quat->w * recip_sq;
    out->x = -quat->x * recip_sq;
    out->y = -quat->y * recip_sq;
    out->z = -quat->z * recip_sq;
}

void NuQuatLerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, float t) {
    out->x = (to->x - from->x) * t + from->x;
    out->y = (to->y - from->y) * t + from->y;
    out->z = (to->z - from->z) * t + from->z;
    out->w = (to->w - from->w) * t + from->w;
}

void NuQuatSlerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, float t) {
    float _unused;
    NUQUAT to_prime;
    float scale;
    float omega;
    float sin_omega;
    float from_factor;
    float to_factor;

    // Even though this same number is used later in the function, the
    // assignment here has nothing to do with it.
    _unused = 1e-05f;

    scale = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;

    if (scale < 0.0f) {
        scale = -scale;

        to_prime.x = -to->x;
        to_prime.y = -to->y;
        to_prime.z = -to->z;
        to_prime.w = -to->w;
    } else {
        to_prime.x = to->x;
        to_prime.y = to->y;
        to_prime.z = to->z;
        to_prime.w = to->w;
    }

    if (1.0f - scale > 1e-05f) {
        omega = NuACos(scale);

        sin_omega = NU_SIN_LUT((int)omega);
        from_factor = NuFdiv(NU_SIN_LUT((int)((1.0f - t) * omega)), sin_omega);
        to_factor = NuFdiv(NU_SIN_LUT((int)(t * omega)), sin_omega);
    } else {
        from_factor = 1.0f - t;
        to_factor = t;
    }

    out->x = from->x * from_factor + to_prime.x * to_factor;
    out->y = from->y * from_factor + to_prime.y * to_factor;
    out->z = from->z * from_factor + to_prime.z * to_factor;
    out->w = from->w * from_factor + to_prime.w * to_factor;
}

void NuQuatCubicInt(NUQUAT *out, NUQUAT *m, NUQUAT *a, NUQUAT *b, NUQUAT *c, float t) {
    NuQuatHarmonize(m, a);
    NuQuatHarmonize(a, b);
    NuQuatHarmonize(b, c);

    out->x = NuCubicInterpolation(m->x, a->x, b->x, c->x, t);
    out->y = NuCubicInterpolation(m->y, a->y, b->y, c->y, t);
    out->z = NuCubicInterpolation(m->z, a->z, b->z, c->z, t);
    out->w = NuCubicInterpolation(m->w, a->w, b->w, c->w, t);
}

void NuQuatHermiteInt(NUQUAT *out, NUQUAT *m, NUQUAT *q0, NUQUAT *q1, NUQUAT *q2, float t) {
    NuQuatHarmonize(m, q0);
    NuQuatHarmonize(q0, q1);
    NuQuatHarmonize(q1, q2);

    out->x = NuHermiteInterpolation(m->x, q0->x, q1->x, q2->x, t);
    out->y = NuHermiteInterpolation(m->y, q0->y, q1->y, q2->y, t);
    out->z = NuHermiteInterpolation(m->z, q0->z, q1->z, q2->z, t);
    out->w = NuHermiteInterpolation(m->w, q0->w, q1->w, q2->w, t);
}

void NuQuatHarmonize(NUQUAT *a, NUQUAT *b) {
    float dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;

    if (dot < 0.0f) {
        b->x = -b->x;
        b->y = -b->y;
        b->z = -b->z;
        b->w = -b->w;
    }
}

float NuCubicInterpolation(float m, float y0, float y1, float y2, float t) {
    float a;
    float b;
    float c;
    float d;
    float t_cb;
    float t_sq;

    a = -m / 6.0f + y0 / 2.0f + -y1 / 2.0f + y2 / 6.0f;
    b = m / 2.0f - y0 + y1 / 2.0f;
    c = -m / 3.0f + -y0 / 2.0f + y1 + -y2 / 6.0f;
    d = y0;

    t_sq = t * t;
    t_cb = t * t_sq;

    return a * t_cb + b * t_sq + c * t + d;
}

float NuHermiteInterpolation(float m, float y0, float y1, float y2, float t) {
    float m0;
    float m1;
    float t_sq;
    float t_cb;
    float h0;
    float h1;
    float h2;
    float h3;

    m0 = (y1 - m) / 2.0f;
    m1 = (y2 - y0) / 2.0f;

    t_sq = t * t;
    t_cb = t * t_sq;

    h0 = (t_cb + t_cb - t_sq * 3.0f) + 1.0f;
    h1 = t_cb - (t_sq + t_sq) + t;
    h2 = t_cb * -2.0f + t_sq * 3.0f;
    h3 = t_cb - t_sq;

    return h0 * y0 + h1 * m0 + h2 * y1 + h3 * m1;
}
