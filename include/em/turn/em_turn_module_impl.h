#pragma once

// The turn module of the enemies (mo_enemy/sora_enemy/turn/em_turn_module_impl.cpp): soTurnModuleImpl driven by the
// enemy's anim cmds (group 'l'), turning with one of the enemy's kinetic energies and resetting a model node's rotation
// when the turn ends.
//
// soTurnModuleImpl is a class of sora_melee. The other enemy units only need its size, which ft/builder/ft_dol_types.h
// gives (an opaque 0x38-byte class); the turn unit defines EM_TURN_MODULE_IMPL_FULL for the layout recovered from the
// RTTI, the vtable and the inline accessors this unit has weak copies of.

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/event/so_event_presenter.h>
#include <so/turn/so_turn_event_presenter.h>
#include <types.h>

class acAnimCmd;
class emKineticModuleImpl;
class soModuleAccesser;
class soPostureModule;

#ifdef EM_TURN_MODULE_IMPL_FULL

struct soTurnData {
    const float* angles;
    u32 endFrame;
};

// TODO(shared header): so/turn/so_turn_module.h declares the first six slots of this interface (with update as unk18()
// and isTurn as isTurning()) and lacks the last five; this full copy is used by the turn unit only (which does not
// include that header) until it is completed. Slot order from the emTurnModuleImpl vtable.
class soTurnModule {
public:
    virtual ~soTurnModule();
    virtual void activate();
    virtual void set(const soTurnData* data, bool changeLr, bool isExtern, float lr);
    virtual void end();
    virtual void update(bool active);
    virtual bool isTurn();
    virtual bool isExtern();
    virtual float getRY();
    virtual float getRYLr();
    virtual u32 getEndFrame();
    virtual void setOmitIntermediate(bool omit);
};

// From the RTTI: soTurnModule at 0, soAnimCmdEventObserver at 4, soEventPresenter<soTurnEventObserver> at 0x10.
class soTurnModuleImpl : public soTurnModule, public soAnimCmdEventObserver, public soEventPresenter<soTurnEventObserver> {
public:
    soModuleAccesser* m_moduleAccesser; // 0x1C
    const soTurnData* m_turnData;       // 0x20
    u8 unk24[4];                        // 0x24
    float m_lr;                         // 0x28 (getRYLr: 90 * lr + rotY)
    float m_rotY;                       // 0x2C
    float unk30;                        // 0x30 (cleared with m_rotY when the turn ends)
    // 0: not turning, 1: turning (rotY has not crossed 0 yet), 2: past the half turn. HYPOTHESIS: the meaning of 1 / 2.
    u8 m_state : 3;                     // 0x34
    // (u8 flags, not bool: isExtern converts the bit to bool.)
    u8 m_changeLr : 1;                  // the turn reverses the facing (HYPOTHESIS: set by set's changeLr)
    u8 m_lrReversed : 1;                // the facing was reversed during this turn
    u8 m_isExtern : 1;
    u8 m_omitIntermediate : 1;

    soTurnModuleImpl(soModuleAccesser* acc);
    virtual ~soTurnModuleImpl();
    virtual void activate();
    virtual void set(const soTurnData* data, bool changeLr, bool isExtern, float lr);
    virtual bool isTurn() { return m_state != 0; }
    virtual bool isExtern() { return m_isExtern; }
    virtual float getRY() { return m_rotY; }
    virtual float getRYLr() { return 90.0f * m_lr + m_rotY; }
    virtual u32 getEndFrame() { return m_turnData->endFrame; }
    virtual void setOmitIntermediate(bool omit) { m_omitIntermediate = omit; }
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
static_assert(sizeof(soTurnModuleImpl) == 0x38, "Class is wrong size!");

#else
#include <ft/builder/ft_dol_types.h> // soTurnModuleImpl (opaque)
#endif

class emTurnModuleImpl : public soTurnModuleImpl, public soAnimCmdEventObserver {
public:
    int m_energyId;    // 0x44 the kinetic energy that turns the enemy (-1: none)
    u32 m_rotNodeId;   // 0x48 the node whose rotation Enemy::initRot resets when the turn ends

    emTurnModuleImpl(soModuleAccesser* acc);
    virtual ~emTurnModuleImpl();
#ifdef EM_TURN_MODULE_IMPL_FULL
    virtual void activate();
    virtual void end();
    virtual void update(bool active);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);

    // Starts a turn driven by kinetic energy energyId (anim cmd 'l' 0 / 1).
    void start(int energyId, u32 rotNodeId, bool changeLr, float lr);
    // Reverses the facing and every valid kinetic energy except the first one.
    void updateSubReverseLr(emKineticModuleImpl* kinetic, soPostureModule* posture);
#endif
};
static_assert(sizeof(emTurnModuleImpl) == 0x4C, "Class is wrong size!");
