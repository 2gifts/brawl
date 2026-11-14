#pragma once

#include <StaticAssert.h>
#include <ml/ml_ft_event.h>
#include <types.h>

// Fighter event observer of the Subspace Emissary: replaces mlFtEvent for adventure stages.
class adFtEvent : public mlFtEvent {
public:
    static adFtEvent* create();

    virtual void notifyEventOnDamage(int entryId, u32 hp, soDamage* damage);
    virtual void notifyEventSetDamage(int entryId, float damage, u32 percent, bool, bool);
    virtual void notifyEventRecover(int entryId, int hp);
    virtual void notifyEventOutsideDeadArea(int entryId, soGroundUtil::DeadAreaCheckResult result, bool* out);
    virtual void notifyEventDead(int entryId, int deadCount, Fighter::Dead::Reason deadReason, int respawnFrames);
    virtual void notifyEventBeat(int entryId1, int entryId2);
    virtual void notifyEventSuicide(int entryId);
    virtual void notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind);
    virtual void notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind);
    virtual void notifyEventSetCursor(int entryId, u32 index);
    virtual void notifyEventSetNameCursor(int entryId, u32 index);
    virtual void notifyEventSetLoupe(int entryId, u32 index);
    virtual void notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId);
    virtual ~adFtEvent();
    virtual void destroy();
};
static_assert(sizeof(adFtEvent) == 0x3C, "Class is wrong size!");
