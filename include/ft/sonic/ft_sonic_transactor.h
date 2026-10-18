#pragma once

#include <types.h>

class soModuleAccesser;
class soCollisionLog;

// Sonic's per-fighter helper singleton (the code is in this REL; only the slots the fighter calls are named).
// HYPOTHESIS: the slot names; the layout follows ftTransactor (include/ft/ft_transactor.h), slot 10 takes the collision log.
class ftSonicTransactor {
public:
    ftSonicTransactor();
    virtual ~ftSonicTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual void processCollision(soModuleAccesser* moduleAccesser); // slot 4
    virtual void processFixPosition(soModuleAccesser* moduleAccesser);
    virtual void unk6(soModuleAccesser* moduleAccesser);
    virtual void unk7(soModuleAccesser* moduleAccesser);
    virtual void unk8(soModuleAccesser* moduleAccesser);
    virtual bool unk9(soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser); // slot 10
    // Local declaration of the REL singleton-holder entry point.
    static ftSonicTransactor* getInstance();
};

// Views of two modules whose headers do not declare the slots Sonic's event handlers call (HYPOTHESIS: names).
// MATCH-ONLY: placeholder virtuals put the called slot at its vtable offset (slot k at 8 + 4k).
class ftSonicGlowView {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void setGlow(int kind, int frames); // slot 18, vtable offset 0x50
};

class ftSonicVisibilityView {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void notifyEventChangeMotion(int motionKind, int prevMotionKind, void* motionData, soModuleAccesser* moduleAccesser); // slot 4, offset 0x18
};
