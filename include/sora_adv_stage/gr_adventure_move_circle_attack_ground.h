#pragma once

#include <gr/gr_gimmick.h>
#include <gr/gr_hit_circle_subject.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>

struct grAdventureMoveGroundData;

class grAdventureMoveCircleAttackGround : public grGimmick,
                                         public grHitCircleSubject,
                                         public grAdventureNotifyGimmickTriggerStatus {
public:
    grAdventureMoveCircleAttackGround(const char* taskName)
        : grGimmick(taskName), unk164(NULL), unk168(NULL) { }
    virtual ~grAdventureMoveCircleAttackGround();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void setInitializeFlag();
    virtual void notifyHitCircle(void* circle);
    virtual void notifyEventOnTrigger(int flag);
    virtual void notifyEventOffTrigger(int flag);

    static grAdventureMoveCircleAttackGround* create(int mdlIndex, const char* taskName);

    union {
        struct {
            u8 unk0[0x1E0];
            stTriggerData unk1E0;
            stTriggerData unk1E4;
            u8 unk1E8[0x60];
            u8 unk248;
            u8 unk249[3];
            float unk24C;
        }* unk164;
        // HYPOTHESIS: the record begins with the moving-ground motion path and effect data.
        grAdventureMoveGroundData* unk164Path;
    };
    void* unk168;
};
static_assert(sizeof(grAdventureMoveCircleAttackGround) == 0x16C, "Class is wrong size!");
