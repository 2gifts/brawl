#pragma once

// Local BrawlHeaders shadow: the nw4r header in include/nw4r/include declares an empty LightObj. This one gives the
// real layout (flag word + GXLightObj, 0x44 bytes) and the LightSet/LightSetting classes of the nw4r library
// (include/ogws/nw4r/g3d/g3d_light.h, same member layout), which gf_3d_scene.cpp needs. Keep in sync.

#include <revolution/GX.h>
#include <types.h>

namespace nw4r {
namespace g3d {

class LightObj {
public:
    LightObj() : mFlag(0) {}
    ~LightObj() {}

    LightObj& operator=(const LightObj& rOther);

    void Clear();
    void InitLightColor(GXColor color);
    void InitLightPos(f32 x, f32 y, f32 z);
    void InitLightDir(f32 nx, f32 ny, f32 nz);
    void GetLightColor(GXColor* pColor) const;
    void Disable() { mFlag &= ~FLAG_ENABLE_LIGHT; }

private:
    enum { FLAG_ENABLE_LIGHT = (1 << 2) };
    u32 mFlag;       // 0x0
    GXLightObj mObj; // 0x4
};

class LightSetting;

struct LightSetData {
    s8 idxLight[8]; // 0x0
    s8 idxAmbLight; // 0x8
    u8 pad9[3];
};

class LightSet {
public:
    LightSet(LightSetting* pSetting, LightSetData* pData) : mpSetting(pSetting), mpLightSetData(pData) {}
    ~LightSet() {}

    bool IsValid() const { return mpSetting != NULL && mpLightSetData != NULL; }

    LightObj* GetLightObj(u32 lightIdx);
    bool SelectLightObj(u32 lightIdx, int lightObjIdx);
    bool SelectAmbLightObj(int lightObjIdx);

    // HYPOTHESIS: inline accessor; gfSceneRoot::renderPre edits the light indices of a set directly.
    LightSetData* GetData() const { return mpLightSetData; }

private:
    LightSetting* mpSetting;      // 0x0
    LightSetData* mpLightSetData; // 0x4
};

struct AmbLightObj {
    u8 r, g, b, a;
};

class LightSetting {
public:
    u32 GetNumLightObj() const { return mNumLight; }
    u32 GetNumLightSet() const { return mNumLightSet; }
    LightObj* GetLightObjArray() const { return mpLightObjArray; }
    AmbLightObj* GetAmbLightObjArray() const { return mpAmbLightObjArray; }

    // BYTES: with the destructor of the nw4r library's LightSet this member stays out of line, as in the game (one weak
    // copy right behind gfSceneRoot::setAnmScnResToScnRoot3D). The game has no LightSet destructor symbol though, so the
    // weak ~LightSet that follows is an extra function: HYPOTHESIS: its role is played by the weak 64 byte function
    // that the symbols call nw4r::ut::Color::~Color (0x8000C998).
    LightSet GetLightSet(int idx) {
        if (idx < mNumLightSet && idx >= 0) {
            return LightSet(this, &mpLightSetDataArray[idx]);
        }

        return LightSet(this, NULL);
    }

private:
    u16 mNumLight;                     // 0x0
    u16 mNumLightSet;                  // 0x2
    LightObj* mpLightObjArray;         // 0x4
    AmbLightObj* mpAmbLightObjArray;   // 0x8
    LightSetData* mpLightSetDataArray; // 0xC
};

} // namespace g3d
} // namespace nw4r
