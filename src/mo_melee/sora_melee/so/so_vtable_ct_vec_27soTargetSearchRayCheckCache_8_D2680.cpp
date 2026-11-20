#pragma force_active off
#include <so/so_array.h>
#include <mt/mt_vector.h>
#include <new>

// HYPOTHESIS: four cached ray points and two flags. Declared here with the Vec3f members that the constructor sets;
// so/templates/so_array_value_soTargetSearchRayCheckCache.h keeps the word layout that the array copies need.
// TODO: clean up: one declaration of the cache.
class soTargetSearchRayCheckCache {
public:
    Vec3f m_pos0;
    Vec3f m_pos1;
    Vec3f m_pos2;
    Vec3f m_pos3;
    u8 unk30;
    u8 unk31;

    soTargetSearchRayCheckCache();
    ~soTargetSearchRayCheckCache();
};

// NONMATCHING: the original's temporaries sit at ascending stack offsets (0x8, 0x14, 0x20, 0x2C).
soTargetSearchRayCheckCache::soTargetSearchRayCheckCache() {
    unk30 = 0;
    m_pos0 = Vec3f(0.0f, 0.0f, 0.0f);
    m_pos1 = Vec3f(0.0f, 0.0f, 0.0f);
    m_pos2 = Vec3f(0.0f, 0.0f, 0.0f);
    m_pos3 = Vec3f(0.0f, 0.0f, 0.0f);
    unk31 = 0;
}

typedef soTargetSearchRayCheckCache VelaElm;
typedef soArrayVector<VelaElm, 8> VelaVec;

// TODO: clean up -- MATCH-ONLY helpers that force the vector members out of line into this unit.
#pragma dont_inline on
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
