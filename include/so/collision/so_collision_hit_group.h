#pragma once

#include <StaticAssert.h>
#include <so/collision/so_collision_hit_part.h>
#include <so/situation/so_situation_module_impl.h>
#include <types.h>

class soCollisionHitGroup {
public:
    int m_index;
#ifdef YK_STAGE_FULL
    short unk4; // MATCH-ONLY: the stage RELs' array instantiations copy these as a half / a float instead of bytes
#else
    s16 m_partStart; // HYPOTHESIS: first index of the group's parts in the module's part array
#endif
    short m_partSize;
    float m_posX;
    float m_scale;
    SituationKind m_situationKind;
    float m_lr;
#ifdef YK_STAGE_FULL
    float unk18;
#else
    union {
        char _24[4];
        float unk24;
    };
#endif
    int m_whole;
    int m_global;
    int m_xluFrameGlobal;
    int m_invincibleFrameGlobal;
    union {
        struct {
            u32 _ : 29;
            bool m_isSituationODD : 1;
            bool m_isSituationAir : 1;
            bool m_isSituationGround : 1;
        };
        soCollision::SituationMask m_multiSituation;
    };
    union {
        char _48[1];
        u8 unk48;
    };
    bool m_49;
    bool m_50;
    u8 m_51;
    u8 m_globalOffset;
#ifndef YK_STAGE_FULL // (tail padding is not copied there)
    char _53[3];
#endif

    // NOTE: shadows the BrawlHeaders copy to add getCenterPos and the recovered part/global-status operations.
    // HYPOTHESIS: parameter types of the part operations follow soCollisionHitModule's (the map has none).
    void init();
    void resetStatusAll(soArray<soCollisionHitPart>* parts);
    void setStatus(soArray<soCollisionHitPart>* parts, u32 index, int status);
    void setStatusNode(soArray<soCollisionHitPart>* parts, int nodeIndex, int status);
    void setStatusNodeDefault(soArray<soCollisionHitPart>* parts, int nodeIndex, int status);
    void setStatusAll(soArray<soCollisionHitPart>* parts, int status);
    void setWhole(int whole);
    int getWhole();
    void resetStatusGlobal();
    void setXluGlobal();
    void cancelXluGlobal();
    void setXluFrameGlobal(int xluFrameGlobal);
    void setInvincibleGlobal();
    void cancelInvincibleGlobal();
    void setInvincibleFrameGlobal(int invincibleFrameGlobal, bool override);
    void updateStatusGlobal();
    int getGlobal();
    int getPartStatus(soArray<soCollisionHitPart>* parts, u32 index);
    void initMultiSituation();
    void setMultiSituation(SituationKind situation);
    void setOpponentCategory(soArray<soCollisionHitPart>* parts, u32 categoryMask, bool enable);
    void setSelfCategory(soArray<soCollisionHitPart>* parts, soCollision::Category category);
    // HYPOTHESIS: reset arguments follow the stored values and callee registers.
    void reset(soCollision* collision, soArray<soCollisionHitPart>* parts, float posX, float scale, float lr, SituationKind situation);
    // HYPOTHESIS: signatures follow the hit-module interface and register use.
    void set(soArray<soCollisionHitPart>* parts, u32 index, soCollisionHitData* data);
    void set(soArray<soCollisionHitPart>* parts, soSet<soCollisionHitData>* data);
    void setCheckCatch(soArray<soCollisionHitPart>* parts, bool checkCatch);
    Vec3f getCenterPos(soCollision* collision, u16 index);

    soCollisionHitGroup();
    ~soCollisionHitGroup();
};
static_assert(sizeof(soCollisionHitGroup) == 56, "Class is wrong size!");
