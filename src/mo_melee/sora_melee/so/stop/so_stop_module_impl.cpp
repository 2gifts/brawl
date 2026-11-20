// TODO: notifyEventAnimCmd (the stop anim cmds) is not decompiled yet.
#include <so/stop/so_stop_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_world.h>
#include <so/event/so_event_system.h>
#include <so/link/so_link_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <types.h>

// The stop state as the module's methods see it. The hit stop (0x10) counts down the hit-stop frames, 0x14 holds the
// fixed hit-stop frames and 0x18 the frames of an "other" stop; the object stays stopped while any of the hit, other,
// link or special stops is active and the world is not paused, and its link children and status module follow.
// HYPOTHESIS: names. TODO: clean up: the header declares 0x14 as a char and leaves 0x18 in a spacer, so the methods work
// through this view.
struct soStopModuleImplState {
    u8 _0[0x10];
    s32 m_hitStopFrame;      // 0x10
    s32 m_hitStopFrameFix;   // 0x14
    s32 m_otherStopFrame;    // 0x18
    u8 m_isStop : 1;         // 0x1C
    u8 m_isHit : 1;
    u8 m_isDamage : 1;
    u8 m_isOtherStop : 1;
    u8 m_isLinkStop : 1;
    u8 m_isSpecialStop : 1;
    u8 m_1c_b1 : 1;
    u8 m_1c_b0 : 1;
    soModuleAccesser* m_moduleAccesser; // 0x20

    // Starts or ends the stop to match the active stop kinds.
    void updateStop() {
        u32 isHit = m_isHit;
        if (!isHit && !m_isOtherStop && !m_isLinkStop && !m_isSpecialStop && !g_soWorld->m_12) {
            if (m_isStop == true) {
                bool isDamage = m_isDamage;
                m_isStop = false;
                m_isHit = false;
                m_hitStopFrame = 0;
                m_hitStopFrameFix = 0;
                m_isLinkStop = false;
                m_isDamage = false;
                m_moduleAccesser->getLinkModule().leaveLinkStop();
                m_moduleAccesser->getStatusModule().leaveStop(m_moduleAccesser, isHit != 0, isDamage);
            }
        } else if (!m_isStop) {
            m_isStop = true;
            m_moduleAccesser->getLinkModule().toLinkStop();
        }
    }
};

static inline soStopModuleImplState* state(soStopModuleImpl* module) {
    return (soStopModuleImplState*)module;
}

soStopModuleImpl::soStopModuleImpl(soModuleAccesser* acc) :
    soAnimCmdEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), (s8)-1) {
    state(this)->m_hitStopFrame = 0;
    state(this)->m_hitStopFrameFix = 0;
    state(this)->m_otherStopFrame = 0;
    m_isStop = false;
    m_isHit = false;
    m_isDamage = false;
    unk1C_b4 = false;
    unk1C_b3 = false;
    m_isSpecialStop = false;
    m_moduleAccesser = acc;
}

soStopModule::~soStopModule() { }

soStopModuleImpl::~soStopModuleImpl() { }

void soStopModuleImpl::activate() {
    // MATCH-ONLY: the original passes nothing meaningful as end's argument (the register is left as it is).
    s32 unused;
    end(unused);
    m_isSpecialStop = false;
}

void soStopModuleImpl::end(s32) {
    state(this)->m_hitStopFrame = 0;
    state(this)->m_hitStopFrameFix = 0;
    state(this)->m_otherStopFrame = 0;
    m_isStop = false;
    m_isHit = false;
    m_isDamage = false;
    unk1C_b4 = false;
    unk1C_b3 = false;
}

void soStopModuleImpl::setHitStopFrame(u32 duration, int allowSDI) {
    if ((int)duration > 0) {
        soStopModuleImplState* s = state(this);
        s->m_isHit = true;
        s->m_hitStopFrame = duration;
        s->m_isDamage = allowSDI;
        if (s->m_hitStopFrameFix > (int)duration) {
            s->m_hitStopFrame = s->m_hitStopFrameFix;
        }
        s->m_hitStopFrameFix = 0;
        if (!s->m_isStop) {
            s->m_isStop = true;
            m_moduleAccesser->getLinkModule().toLinkStop();
        }
    }
}

void soStopModuleImpl::setHitStopFrameFix(s32 frame) {
    state(this)->m_hitStopFrameFix = frame;
}

void soStopModuleImpl::cancelHitStop() {
    soStopModuleImplState* s = state(this);
    s->m_hitStopFrame = 0;
    s->m_hitStopFrameFix = 0;
    s->m_isHit = false;
    s->updateStop();
}

void soStopModuleImpl::setOtherStop(u32 duration) {
    soStopModuleImplState* s = state(this);
    if ((int)duration > 0 && (int)duration > s->m_otherStopFrame) {
        s->m_isOtherStop = true;
        s->m_otherStopFrame = duration;
        if (s->m_hitStopFrameFix > (int)duration) {
            s->m_otherStopFrame = s->m_hitStopFrameFix;
        }
        s->m_hitStopFrameFix = 0;
        if (!s->m_isStop) {
            s->m_isStop = true;
            m_moduleAccesser->getLinkModule().toLinkStop();
        }
    }
}

void soStopModuleImpl::cancelOtherStop() {
    soStopModuleImplState* s = state(this);
    s->m_otherStopFrame = 0;
    s->m_isOtherStop = false;
    s->updateStop();
}

void soStopModuleImpl::setLinkStop(u32 duration) {
    soStopModuleImplState* s = state(this);
    if (duration == 1) {
        if (s->m_isStop == true) {
            return;
        }
    } else if (s->m_isLinkStop == true) {
        s->m_isOtherStop = false;
        s->m_isHit = false;
    }
    s->m_isLinkStop = duration;
    s->updateStop();
}

void soStopModuleImpl::setSpecialStop(u32 duration) {
    soStopModuleImplState* s = state(this);
    s->m_isSpecialStop = duration;
    s->updateStop();
}

void soStopModuleImpl::update() {
    soStopModuleImplState* s = state(this);
    if (!g_soWorld->m_12) {
        if (s->m_otherStopFrame != 0 && --s->m_otherStopFrame == 0) {
            s->m_isOtherStop = false;
        }
        if (s->m_hitStopFrame != 0 && --s->m_hitStopFrame == 0) {
            s->m_isHit = false;
        }
    }
    if (s->m_isLinkStop == true && !m_moduleAccesser->getLinkModule().isValidLinkStop()) {
        setLinkStop(0);
        return;
    }
    s->updateStop();
}

bool soStopModuleImpl::isObserv(char group) {
    return group == 0x1C;
}

s32 soStopModuleImpl::getHitStopFrame() {
    return 0;
}

int soStopModuleImpl::calcHitStopFrame(float power, float mul, float add, float scale, float maxFrame) {
    if (power > 0.0f) {
        power = add + power * mul;
        power *= scale;
        if (power > maxFrame) {
            power = maxFrame;
        }
        return (float)floor(power);
    }
    return 0;
}
