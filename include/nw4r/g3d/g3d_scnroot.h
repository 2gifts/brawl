#pragma once

// Local BrawlHeaders shadow: BrawlHeaders has no nw4r::g3d::ScnRoot; declares the root members the game calls.

#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_rescommon.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <revolution/GX.h>
#include <types.h>

struct MEMAllocator;

namespace nw4r {
namespace g3d {
class AnmScn;
class Camera;
// The fog data of a ScnRoot (layout of include/ogws/nw4r/g3d/g3d_fog.h).
struct FogData {
    GXFogType type;         // 0x0
    f32 startz;             // 0x4
    f32 endz;               // 0x8
    f32 nearz;              // 0xC
    f32 farz;               // 0x10
    GXColor color;          // 0x14
    GXBool adjEnable;       // 0x18
    u8 PADDING_0x19;        // 0x19
    u16 adjCenter;          // 0x1A
    GXFogAdjTable adjTable; // 0x1C
};

// HYPOTHESIS: a view of nw4r::g3d::Fog (one pointer to the fog data of the scene); only Init() is known to the game code.
class Fog : public ResCommon<FogData> {
public:
    explicit Fog(FogData* data) : ResCommon(data) {}
    void Init();
    void GetFog(GXFogType* pType, f32* pStartZ, f32* pEndZ, f32* pNearZ, f32* pFarZ, GXColor* pColor);

    void SetZ(f32 startZ, f32 endZ) {
        if (!IsValid()) {
            return;
        }
        FogData& r = ref();
        r.startz = startZ;
        r.endz = endZ;
    }

    void SetFogColor(GXColor color) {
        if (!IsValid()) {
            return;
        }
        ref().color = color;
    }
};

class ScnRoot : public ScnGroup {
public:
    static ScnRoot* Construct(MEMAllocator* allocator, u32* size, u32 maxChildren, u32 maxScnObj, u32 numLightObj,
                              u32 numLightSet);

    Camera GetCamera(int index);
    // The fog of the given index; it is not valid (Fog::IsValid) past the last one. IfCoinShooter's constructor resets them all.
    Fog GetFog(int index);
    void SetCurrentCamera(int index);
    void SetAnmScn(AnmScn* anmScn);
    void UpdateFrame();
    void CalcWorld();
    void CalcMaterial();
    void CalcView();
    void GatherDrawScnObj();
    void ZSort();
    void DrawOpa();
    void DrawXlu();
    void DrawOpaAndXlu();

    // The LightSetting sits at 0x2878 of the ScnRoot (the game's gfSceneRoot copies and walks it).
    LightSetting& GetLightSetting() { return m_lightSetting; }

    char unkE8[0x2878 - 0xE8];
    LightSetting m_lightSetting; // 0x2878
};
} // namespace g3d
} // namespace nw4r
