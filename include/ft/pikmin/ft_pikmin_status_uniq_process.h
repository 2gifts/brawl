#pragma once

#include <ft/ft_status_uniq_process_catch_pull_impl.h>
#include <ft/ft_status_uniq_process_smash_attack.h>
#include <so/status/so_status_module_impl.h>

class soModuleAccesser;
class StageObject;
class wnPikminPikminTroopsManager;

// The status processes of Olimar. Eleven of them are members of the fighter (0x200D8..0x20130, built with the pointer to the
// troops manager of the fighter), Throw and SmashAttack are global objects of their own units. The fighter registers them with the
// status module in its constructor; the hold statuses (see ftPikmin::updateHoldPikmin) call their updateHoldPikmin.
// HYPOTHESIS: the class names are the ones of the module map; the status numbers are named where the units use them.

// The common catch wait process (ft_status_uniq_process_catch_wait.cpp in sora_melee) as its derived classes see it.
class ftStatusUniqProcessCatchWait : public ftStatusUniqProcessCatchPull {
public:
    ftStatusUniqProcessCatchWait() { }
    virtual ~ftStatusUniqProcessCatchWait() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

class ftPikminStatusUniqProcessThrow : public soStatusUniqProcess {
public:
    virtual ~ftPikminStatusUniqProcessThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    static ftPikminStatusUniqProcessThrow* getInstance();
};
extern ftPikminStatusUniqProcessThrow g_ftPikminStatusUniqProcessThrow;
inline ftPikminStatusUniqProcessThrow* ftPikminStatusUniqProcessThrow::getInstance() { return &g_ftPikminStatusUniqProcessThrow; }

class ftPikminStatusUniqProcessSmashAttack : public soStatusUniqProcess {
public:
    virtual ~ftPikminStatusUniqProcessSmashAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    static ftPikminStatusUniqProcessSmashAttack* getInstance();
};
extern ftPikminStatusUniqProcessSmashAttack g_ftPikminStatusUniqProcessSmashAttack;
inline ftPikminStatusUniqProcessSmashAttack* ftPikminStatusUniqProcessSmashAttack::getInstance() { return &g_ftPikminStatusUniqProcessSmashAttack; }

class ftPikminStatusUniqProcessSpecialS : public soStatusUniqProcess {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessSpecialS(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void updateHoldPikmin(soModuleAccesser* moduleAccesser);
};

class ftPikminStatusUniqProcessAttackAir : public soStatusUniqProcess {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackAir(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackAir() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void updateHoldPikmin(soModuleAccesser* moduleAccesser);
    void detachAttackAirPikmin(soModuleAccesser* moduleAccesser);
};

class ftPikminStatusUniqProcessAttackS4Start : public soStatusUniqProcess {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackS4Start(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackS4Start() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

class ftPikminStatusUniqProcessAttackS4Hold : public ftStatusUniqProcessSmashAttack {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackS4Hold(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackS4Hold() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

class ftPikminStatusUniqProcessAttackS4 : public ftStatusUniqProcessSmashAttack {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackS4(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackS4() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void updateHoldPikmin(soModuleAccesser* moduleAccesser);
};

class ftPikminStatusUniqProcessAttackHi4Start : public soStatusUniqProcess {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackHi4Start(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackHi4Start() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

class ftPikminStatusUniqProcessAttackHi4Hold : public ftStatusUniqProcessSmashAttack {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackHi4Hold(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackHi4Hold() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

class ftPikminStatusUniqProcessAttackHi4 : public ftStatusUniqProcessSmashAttack {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessAttackHi4(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessAttackHi4() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void updateHoldPikmin(soModuleAccesser* moduleAccesser);
};

class ftPikminStatusUniqProcessSpecialHi : public soStatusUniqProcess {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessSpecialHi(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void updateHoldPikmin(soModuleAccesser* moduleAccesser);
};

class ftPikminStatusUniqProcessCatch : public soStatusUniqProcess {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessCatch(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessCatch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    void updateHoldPikmin(soModuleAccesser* moduleAccesser);
};

class ftPikminStatusUniqProcessCatchWait : public ftStatusUniqProcessCatchWait {
    wnPikminPikminTroopsManager* m_troops;
public:
    ftPikminStatusUniqProcessCatchWait(wnPikminPikminTroopsManager* troops) : m_troops(troops) { }
    virtual ~ftPikminStatusUniqProcessCatchWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};
