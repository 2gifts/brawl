#pragma once

#include <snd/snd_3d_generator.h>
#include <st_gw/gr_gw.h>
#include <yk/yakumono.h>

// A keeper of the lion scene (one on each side): it appears when the scene starts, picks one of three places at random
// every few frames and hits fighters that are near (a hit area follows its hit node).
class grGWLionKeeper : public grGW {
protected:
    Vec3f* m_posWork;           // 0x174 the places of the keeper (from the stage)
    u8* m_sceneWork;            // 0x178 the scene that plays (from the stage)
    u8 m_sceneNo;               // 0x17C the scene that the ground belongs to
    u8 m_posIndex;              // 0x17D the place of the keeper (0 - 2)
    u8 m_type;                  // 0x17E the side (4 = left, 5 = right)
    u8 m_played;                // 0x17F the animation of the appearing is over
    u8 m_yakumonoMade;          // 0x180
    u8 m_attackOn;              // 0x181
    char _182[2];
    ykData* m_work;             // 0x184
    snd3DGenerator m_snd;       // 0x188

public:
    static grGWLionKeeper* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    grGWLionKeeper(const char* taskName);
    virtual ~grGWLionKeeper();
    virtual void update(float deltaFrame);
    virtual void onInflict(soCollisionLog* log, u32 unk, float damage);
    virtual void updateCallBack(float deltaFrame);
    virtual void setSceneIDWork(u8* scene);
    virtual void setSceneID(u8 scene);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setPosWork(Vec3f* pos);
    virtual void setType(u8 type);
};
static_assert(sizeof(grGWLionKeeper) == 0x190, "Class is wrong size!");
