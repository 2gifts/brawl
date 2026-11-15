#pragma once

#include <types.h>

class soCollisionLog;
class soDamage;
struct soStatusData;
class soModuleAccesser;

// HYPOTHESIS: common interface of the per-fighter "Transactor" singletons (ftRobotTransactor, ftIkeTransactor, ...).
// The default bodies live in sora_melee; the fighters only override the hooks they need. Slots whose purpose is not
// known yet are named after their vtable index.
class ftTransactor {
public:
    virtual ~ftTransactor() { }
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual void processCollision(soModuleAccesser* moduleAccesser);
    virtual void processFixPosition(soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(int situationKind, int prevSituationKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void unk8(soModuleAccesser* moduleAccesser);
    virtual bool unk9(soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck(soModuleAccesser* moduleAccesser);
    virtual bool activeArticle(class soArticle* article, soModuleAccesser* moduleAccesser);
    virtual bool shootArticle(class soArticle* article, soModuleAccesser* moduleAccesser);
    virtual void unk14(soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* log, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser);
};
