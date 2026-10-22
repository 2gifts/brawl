#pragma once

#include <StaticAssert.h>
#include <gf/gf_3d_scene.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_restex.h>
#include <revolution/GX.h>
#include <types.h>

namespace nw4r { namespace g3d { class ScnObj; } }
class cmSubject;

// HYPOTHESIS: the sora map gives the class and method names only; parameter types come from the callers
// (gf_3d_scene, if_player, soModelModuleImpl), field names from the getters and setters.
class gfLoupe {
public:
    struct ObjInfo {
        gfSceneRoot::LayerType m_layer;
        nw4r::g3d::ScnObj* m_scnObj;
    };

    struct LoupeInfo {
        bool m_enable;
        u32 m_scnObjNum;
        ObjInfo m_objInfo[8];
        cmSubject* m_subject;
        void* m_image;
        float m_bgScrollU;
        float m_bgScrollV;
        Vec3f m_center;
        float m_radius;

        LoupeInfo() { }
    };

    static void initialize();
    static void setup();
    static void grabLoupeBGColor();
    static GXColor getBGColor(u8 index);
    static GXTexObj* getBGTexture();
    static u32 getScnObjNum(u32 index);
    static ObjInfo* getObjInfoArray(u32 index);
    static Vec3f* getLoupeCenter(u32 index);
    static float getLoupeRadius(u32 index);
    static void* getImage(u32 index);
    static void setBGTexture(nw4r::g3d::ResTex tex);
    static void setEnable(u8 index, bool enable);
    static bool isEnable(u32 index);
    static void setLoupeTextureSize(u32 width, u32 height);
    static void setTextureRef(u8 index, void* image);
    static void addPlayerLoupeScnObj(u32 index, gfSceneRoot::LayerType layer, nw4r::g3d::ScnObj* scnObj);
    static void removePlayerLoupeScnObj(u32 index, nw4r::g3d::ScnObj* scnObj);
    static void setPlayerLoupeInfo(u32 index, Vec3f* center, float radius);
};
static_assert(sizeof(gfLoupe::LoupeInfo) == 0x68, "Class is wrong size!");
