#pragma once

#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;
class soCollisionAttackModule;
class soCollisionLog;
class wnPeachKinopio;

// Peach's per-fighter helper singleton (the code is in sora_melee): starts the Toad article and takes part in some of the
// events of the fighter.
// HYPOTHESIS: the slot names; the layout follows ftTransactor (include/ft/ft_transactor.h, slot k at 8 + 4k). The slots 6, 8
// and 9 take the arguments of the fighter events of the same name.
class ftPeachTransactor {
public:
    ftPeachTransactor();
    virtual ~ftPeachTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual void unk4(soModuleAccesser* moduleAccesser);
    virtual void processFixPosition(soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(int kind, int prevKind, soModuleAccesser* moduleAccesser); // slot 6
    virtual void unk7(soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser); // slot 8
    virtual bool notifyEventCollisionShieldCheck(soModuleAccesser* moduleAccesser); // slot 9
    // Local declaration of the REL singleton-holder entry point.
    static ftPeachTransactor* getInstance();
    // HYPOTHESIS: runs while the fighter is in status 0x112.
    void updateStatus112(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle1(wnPeachKinopio* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftPeachTransactor) == 4, "Transactor singleton layout is wrong!");
