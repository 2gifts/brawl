#pragma once
#include <types.h>

// Partial down-special record at extendParam[2]. The airborne status consumes
// speedY; the landing-star activator reads the next two floats as spawn offsets.
// HYPOTHESIS: original source member names. All three accesses are scalar floats.
struct ftYoshiSpecialLwParam {
    float speedY;
    float starOffsetX;
    float starOffsetY;
};
