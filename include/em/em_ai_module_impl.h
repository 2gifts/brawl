#pragma once

// SHADOW of BrawlHeaders em/em_ai_module_impl.h (-I include comes first): the class is 0x64 bytes (embedded in Enemy at
// 0x1108, the next member starts at 0x116C) and has the constructor the Enemy constructor calls (fn_41_28B30).

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/collision/so_collision_attack_event_presenter.h>
#include <types.h>

class soModuleAccesser;
struct emAIEntry;

struct emAIInfo {
    u8 unk0[0x4];
    s32 unk4;
};
// TODO size assertion

class emAIModuleImpl : public soAnimCmdEventObserver, public soCollisionAttackEventObserver {
    u8 unk18[0x64 - 0x18];

public:
    // HYPOTHESIS: aiData is the enemy's AI parameter block (a soValueAccesser constant); 6 and 7 are passed by every enemy.
    emAIModuleImpl(soModuleAccesser* acc, emAIEntry* entries, void* aiData, int entryNum, int p5, int p6);
    virtual ~emAIModuleImpl();

    emAIInfo* getCurrentAIInfo();
};
static_assert(sizeof(emAIModuleImpl) == 0x64, "Class is wrong size!");
