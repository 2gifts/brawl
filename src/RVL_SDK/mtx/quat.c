// quat: quaternion helpers (matrix conversion and slerp), plain C on top of libm doubles.
#include "revolution/mtx.h"

extern double sqrt(double);
extern double acos(double);
extern double sin(double);

// MATCH-ONLY: 1.0 goes into the constant pool ahead of 0.0, retail order
void quat_pool_one(void) {
    (1.0f);
}

void C_QUATMtx(Quaternion* quat, const Mtx mtx) {
    f32 root, trace;
    f32 inv;
    u32 dmax, dnext, dlast;
    u32 next[3] = {1, 2, 0};
    f32 temp[3];

    trace = mtx[0][0] + mtx[1][1] + mtx[2][2];

    if (trace > 0.0f) {
        root = (f32)sqrt(1.0f + trace);
        quat->w = root * 0.5f;

        inv = 0.5f / root;
        quat->x = inv * (mtx[2][1] - mtx[1][2]);
        quat->y = inv * (mtx[0][2] - mtx[2][0]);
        quat->z = inv * (mtx[1][0] - mtx[0][1]);
    } else {
        dmax = 0;

        if (mtx[1][1] > mtx[dmax][dmax]) {
            dmax = 1;
        }

        if (mtx[2][2] > mtx[dmax][dmax]) {
            dmax = 2;
        }

        dnext = next[dmax];
        dlast = next[dnext];

        root = (f32)sqrt(mtx[dmax][dmax] - (mtx[dnext][dnext] + mtx[dlast][dlast]) +
                     1.0f);
        temp[dmax] = 0.5f * root;

        if (0.0f != root) {
            root = 0.5f / root;
        }

        quat->w = root * (mtx[dlast][dnext] - mtx[dnext][dlast]);
        temp[dnext] = root * (mtx[dmax][dnext] + mtx[dnext][dmax]);
        temp[dlast] = root * (mtx[dmax][dlast] + mtx[dlast][dmax]);

        quat->x = temp[0];
        quat->y = temp[1];
        quat->z = temp[2];
    }
}

void C_QUATSlerp(const Quaternion* a, const Quaternion* b, Quaternion* out,
                 f32 t) {
    f32 dot;
    f32 coeffa, coeffb;
    f32 theta, sintheta;

    dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
    coeffb = 1.0f;

    if (dot < 0) {
        dot = -dot;
        coeffb = -coeffb;
    }

    if (dot <= 1.0f - 0.00001f) {
        theta = (f32)acos(dot);
        sintheta = (f32)sin(theta);

        coeffa = (f32)sin((1.0f - t) * theta) / sintheta;
        coeffb *= (f32)sin(t * theta) / sintheta;
    } else {
        coeffa = 1.0f - t;
        coeffb *= t;
    }

    out->x = coeffa * a->x + coeffb * b->x;
    out->y = coeffa * a->y + coeffb * b->y;
    out->z = coeffa * a->z + coeffb * b->z;
    out->w = coeffa * a->w + coeffb * b->w;
}
