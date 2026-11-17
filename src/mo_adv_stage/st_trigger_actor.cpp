#include <sora_adv_stage/gr_adventure_move4_ground.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>
#include <st/st_trigger.h>
#include <types.h>

class stActTriggerStepJump : public stTriggerCB {
public:
    u32 unk8;
    char _spacerC[8];
    bool unk14;
    char _spacer15[3];
    float unk18;

    virtual ~stActTriggerStepJump();
    virtual void userProc();
};

class stActTriggerScrollLockOn : public stTriggerCB {
public:
    virtual ~stActTriggerScrollLockOn();
};

class stActTriggerForceScrollOn : public stTriggerCB {
public:
    virtual ~stActTriggerForceScrollOn();
};

class stActTriggerCameraDemo : public stTriggerCB {
public:
    virtual ~stActTriggerCameraDemo();
};

class stActTriggerMotionPathMove4Start : public stTriggerCB {
public:
    u32 unk8;
    grAdventureMove4Ground* unkC;
    u8 unk10;

    virtual ~stActTriggerMotionPathMove4Start();
    virtual void userProc();
};

class stActTriggerMotionPathMove4Stop : public stTriggerCB {
public:
    u32 unk8;
    grAdventureMove4Ground* unkC;
    u8 unk10;

    virtual ~stActTriggerMotionPathMove4Stop();
    virtual void userProc();
};

class stActTriggerCreateItem : public stTriggerCB {
public:
    virtual ~stActTriggerCreateItem();
};

class stActTriggerCreateFixedItem : public stTriggerCB {
public:
    virtual ~stActTriggerCreateFixedItem();
};

class stActTriggerQuake : public stTriggerCB {
public:
    virtual ~stActTriggerQuake();
};

class stActTriggerEffect : public stTriggerCB {
public:
    u32 unk8;
    char _spacerC[4];
    bool unk10;
    char _spacer11[3];
    float unk14;

    virtual ~stActTriggerEffect();
    virtual void userProc();
};

class stActTriggerStockReset : public stTriggerCB {
public:
    virtual ~stActTriggerStockReset();
};

void stActTriggerStepJump::userProc() {
    unk14 = true;
    unk18 = 0.0f;
}

void stActTriggerMotionPathMove4Start::userProc() {
    unkC->changePath(unk10);
}

void stActTriggerMotionPathMove4Stop::userProc() {
    unkC->stopPath(unk10);
}

void stActTriggerEffect::userProc() {
    unk10 = true;
    unk14 = 0.0f;
}

stActTriggerGimmickStatus::stActTriggerGimmickStatus(grAdventureNotifyGimmickTriggerStatus* owner, int index, bool isValid, int flag) :
    unkC(1), m_index(index), m_isValid(isValid), m_flag(flag), m_owner(owner) { }

stActTriggerStockReset::~stActTriggerStockReset() { }

stActTriggerEffect::~stActTriggerEffect() { }

stActTriggerQuake::~stActTriggerQuake() { }

stActTriggerCreateFixedItem::~stActTriggerCreateFixedItem() { }

stActTriggerCreateItem::~stActTriggerCreateItem() { }

stActTriggerMotionPathMove4Stop::~stActTriggerMotionPathMove4Stop() { }

stActTriggerMotionPathMove4Start::~stActTriggerMotionPathMove4Start() { }

stActTriggerCameraDemo::~stActTriggerCameraDemo() { }

stActTriggerForceScrollOn::~stActTriggerForceScrollOn() { }

stActTriggerScrollLockOn::~stActTriggerScrollLockOn() { }

stActTriggerStepJump::~stActTriggerStepJump() { }
