#include <ip/ip_padconfig.h>
#include <ip/ip_pad_status.h>
#include <gf/gf_pad_status.h>
#include <gm/gm_global.h>
#include <mt/mt_vector.h>
#include <sr/sr_common.h>
#include <math.h>

static ipPadConfig* g_ipPadConfig;

namespace {
    enum PadButton {
        Attack = 1, Special = 2, Jump = 4, Guard = 8, StockShare = 0x10,
        Smash = 0x20, DirLeft = 0x40, DirUp = 0x80, DirRight = 0xC0,
        CStick = 0x100, AppealHi = 0x400, AppealSide = 0x800,
        AppealLw = 0x1000, AppealLeft = 0x2000, AppealRight = 0x4000,
        FlickJump = 0x8000
    };
    // Profile action 4 means attack + shield (grab), rather than bit 4.
    inline u16 mappedButton(u8 action) {
        u16 mask = 0;
        if (action == 4) mask |= Attack | Guard;
        else mask |= 1u << action;
        return mask;
    }

    // A direction released in any of the preceding three samples can qualify
    // a fresh attack/special press for full-strength directional input.
    inline bool hasRecentRelease(int history) {
        bool released = false;
        for (int i = 1; i < 4; ++i) {
            if (((history >> i) & 1) == 0) released = true;
        }
        return released;
    }

    inline void copyConfig(ipGcButtonConfig& dst, const ipGcButtonConfig& src) {
        dst.l = src.l;
        dst.r = src.r;
        dst.z = src.z;
        dst.up = src.up;
        dst.side = src.side;
        dst.down = src.down;
        dst.a = src.a;
        dst.b = src.b;
        dst.substick = src.substick;
        dst.y = src.y;
        dst.x = src.x;
        dst.flags = src.flags;
    }
    inline void copyConfig(ipWiiButtonConfig& dst, const ipWiiButtonConfig& src) {
        dst.core.one = src.core.one;
        dst.core.two = src.core.two;
        dst.core.oneTwo = src.core.oneTwo;
        dst.core.a = src.core.a;
        dst.core.b = src.core.b;
        dst.core.ab = src.core.ab;
        dst.core.up = src.core.up;
        dst.core.extra = src.core.extra;
        dst.nunchuk.c = src.nunchuk.c;
        dst.nunchuk.z = src.nunchuk.z;
        dst.nunchuk.cz = src.nunchuk.cz;
        dst.nunchuk.a = src.nunchuk.a;
        dst.nunchuk.b = src.nunchuk.b;
        dst.nunchuk.ab = src.nunchuk.ab;
        dst.nunchuk.extra = src.nunchuk.extra;
        dst.nunchuk.motion = src.nunchuk.motion;
        dst.nunchuk.up = src.nunchuk.up;
        dst.nunchuk.side = src.nunchuk.side;
        dst.nunchuk.down = src.nunchuk.down;
        dst.nunchuk.flags = src.nunchuk.flags;
        dst.classic.l = src.classic.l;
        dst.classic.r = src.classic.r;
        dst.classic.zl = src.classic.zl;
        dst.classic.zr = src.classic.zr;
        dst.classic.up = src.classic.up;
        dst.classic.side = src.classic.side;
        dst.classic.down = src.classic.down;
        dst.classic.a = src.classic.a;
        dst.classic.b = src.classic.b;
        dst.classic.substick = src.classic.substick;
        dst.classic.x = src.classic.x;
        dst.classic.y = src.classic.y;
        dst.classic.flags = src.classic.flags;
    }
}

ipPadConfig* ipPadConfig::getInstance() {
    if (!g_ipPadConfig) g_ipPadConfig = new (Heaps::SystemFW) ipPadConfig;
    return g_ipPadConfig;
}

void ipPadConfig::reset() {
    ipButtonConfig defaults;
    buttonConfigInit(&defaults);
    for (u32 i = 0; i < 4; ++i) copyConfig(m_gc[i], defaults.gc);
    for (u32 i = 0; i < 4; ++i) copyConfig(m_wii[i], defaults.wii);
    for (u32 i = 0; i < 4; ++i) m_playerPads[i] = i;
    for (u32 i = 0; i < 8; ++i) {
        ipCorePadState& s = m_coreState[i];
        s.attackHistory = 0;
        s.specialHistory = 0;
        s.downHold = 0;
        s.leftHistory = 0;
        s.rightHistory = 0;
        s.upHistory = 0;
        s.downHistory = 0;
        s.stickX = 0;
        s.stickY = 0;
        s.downFrames = 0;
    }
    for (u32 i = 0; i < 8; ++i) {
        ipSubstickState& s = m_substickState[i];
        s.history = 0;
        s.motionHistory = 0;
        s.unk2 = 0;
        s.smashFrames = 0;
        s.smashX = 0;
        s.smashY = 0;
    }
    for (u32 i = 0; i < 8; ++i) m_padPlayers[i] = -1;
}

// C-stick edge history is retained separately from the main stick. Attack and
// special act on a fresh tilt, then become jump while held; smash must return
// almost to center before it can fire again. A fresh attack/special tilt needs
// the full eight-bit history to equal one, not just its newest two samples.
void ipPadConfig::setupPadSubstick(ipPadStatus* out, const gfPadStatus* in, int action, ipSubstickState* state) {
    int x = (s8)in->m_subStickX;
    int y = (s8)in->m_subStickY;
    // MATCH-ONLY: retain the native pre-threshold float initialization;
    // the raw integer axes remain the output for a fresh special tilt.
    Vec2f stick(x, y);
    int lengthSquared;
    int motionHistory;
    int history;
    motionHistory = (u8)(state->motionHistory << 1);
    history = (u8)(state->history << 1);
    lengthSquared = x * x + y * y;
    if (lengthSquared > 2500) {
        history |= 1;
        bool overrideStick = false;
        bool edge = false;
        out->m_button |= CStick;
        if (history == 1) edge = true;
        Vec2f direction(x, y);
        direction.normalize();
        direction.m_x *= 82.0f;
        direction.m_y *= 82.0f;
        int normalizedX = direction.m_x;
        int normalizedY = direction.m_y;
        switch (action) {
        case 0:
            if (edge) {
                out->m_button |= Attack;
                overrideStick = true;
                x = 0.32f * direction.m_x;
                y = 0.32f * direction.m_y;
                break;
            }
            // A held attack C-stick follows the same path as special.
        case 1:
            if (edge) {
                out->m_button |= Special;
                overrideStick = true;
                break;
            }
        case 2: out->m_button |= Jump; break;
        case 3: out->m_button |= Guard; break;
        case 4:
            if (edge) out->m_button |= Attack | Guard;
            break;
        case 5:
            motionHistory |= 1;
            ++state->smashFrames;
            if (state->smashFrames > 100) state->smashFrames = 100;
            if (state->smashFrames < 2) {
                out->m_button |= Attack | Smash;
                state->smashX = normalizedX;
                state->smashY = normalizedY;
                out->m_stickX = normalizedX;
                out->m_stickY = normalizedY;
            }
            break;
        }
        if (overrideStick) {
            out->m_stickX = x;
            out->m_stickY = y;
        }
    }
    state->history = history;
    state->motionHistory = motionHistory;
    if ((u32)action == 5 && lengthSquared < 20) state->smashFrames = 0;
}

void ipPadConfig::setupPadStatusGc(ipPadStatus* out, const gfPadStatus* in, const ipGcButtonConfig* config, ipSubstickState* state) {
    if (in->m_buttonsCurrentFrame2.bits & 0x400) out->m_button |= mappedButton(config->x);
    if (in->m_buttonsCurrentFrame2.bits & 0x800) out->m_button |= mappedButton(config->y);
    if (in->m_buttonsCurrentFrame2.bits & 0x100) out->m_button |= mappedButton(config->a);
    if (in->m_buttonsCurrentFrame2.bits & 0x200) out->m_button |= mappedButton(config->b);
    if ((in->m_buttonsCurrentFrame2.bits & 0x100) && (in->m_buttonsCurrentFrame2.bits & 0x200)) out->m_button |= StockShare;
    if ((in->m_buttonsCurrentFrame2.bits & 1) || (in->m_buttonsCurrentFrame2.bits & 2)) out->m_button |= mappedButton(config->side);
    if (in->m_buttonsCurrentFrame2.bits & 8) out->m_button |= mappedButton(config->up);
    if (in->m_buttonsCurrentFrame2.bits & 4) out->m_button |= mappedButton(config->down);
    if (in->m_buttonsCurrentFrame2.bits & 0x40) out->m_button |= mappedButton(config->l);
    if (in->m_buttonsCurrentFrame2.bits & 0x20) out->m_button |= mappedButton(config->r);
    if (in->m_buttonsCurrentFrame2.bits & 0x10) out->m_button |= mappedButton(config->z);
    out->m_stickX = in->m_stickX;
    out->m_stickY = in->m_stickY;
    if (config->side == 11) {
        if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= AppealLeft;
        if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= AppealRight;
        if ((in->m_buttonsCurrentFrame2.bits & 1) || (in->m_buttonsCurrentFrame2.bits & 2)) out->m_button |= AppealSide;
    }
    if (((config->flags >> 7) & 1) != 0) out->m_button |= FlickJump;
    if (in->m_buttonsCurrentFrame2.bits & 0x200) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 0x800) out->m_button |= DirUp;
    if (in->m_buttonsCurrentFrame2.bits & 0x400) out->m_button |= DirRight;
    if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 8) out->m_button |= DirUp;
    if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= DirRight;
    setupPadSubstick(out, in, config->substick, state);
}

// A release in either of the two preceding samples followed by a held sample
// within the recent history identifies a new flick (not a continuously held key).
bool ipPadConfig::check_key_flip(int history) {
    bool flipped;
    bool released;
    int i;
    int start = 8;
    flipped = false;
    released = false;
    if (history > 1) {
        for (i = 1; i < 3; ++i) {
            if (((history >> i) & 1) == 0) { start = i; released = true; break; }
        }
        if (released) {
            for (; start < 11; ++start) {
                if (((history >> start) & 1) == 1) { flipped = true; break; }
            }
        }
    }
    return flipped;
}

void ipPadConfig::setupPadStatusWiiClassic(ipPadStatus* out, const gfPadStatus* in, const ipClassicConfig* config, ipSubstickState* state) {
    if (in->m_buttonsCurrentFrame2.bits & 0x100) out->m_button |= mappedButton(config->a);
    if (in->m_buttonsCurrentFrame2.bits & 0x200) out->m_button |= mappedButton(config->b);
    if ((in->m_buttonsCurrentFrame2.bits & 0x100) && (in->m_buttonsCurrentFrame2.bits & 0x200)) out->m_button |= StockShare;
    if (in->m_buttonsCurrentFrame2.bits & 0x400) out->m_button |= mappedButton(config->x);
    if (in->m_buttonsCurrentFrame2.bits & 0x800) out->m_button |= mappedButton(config->y);
    if (in->m_buttonsCurrentFrame2.bits & 0x40) out->m_button |= mappedButton(config->l);
    if (in->m_buttonsCurrentFrame2.bits & 0x20) out->m_button |= mappedButton(config->r);
    if (in->m_buttonsCurrentFrame2.bits & 0x4000) out->m_button |= mappedButton(config->zl);
    if (in->m_buttonsCurrentFrame2.bits & 0x8000) out->m_button |= mappedButton(config->zr);
    if (in->m_buttonsCurrentFrame2.bits & 8) {
        out->m_button |= mappedButton(config->up);
        out->m_button |= DirUp;
    }
    if ((in->m_buttonsCurrentFrame2.bits & 1) || (in->m_buttonsCurrentFrame2.bits & 2)) out->m_button |= mappedButton(config->side);
    if (in->m_buttonsCurrentFrame2.bits & 4) out->m_button |= mappedButton(config->down);
    bool flickJump = ((config->flags >> 7) & 1) != 0;
    out->m_stickX = in->m_stickX;
    out->m_stickY = in->m_stickY;
    if (flickJump) out->m_button |= FlickJump;
    if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= DirRight;
    if (config->side == 11) {
        if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= AppealLeft;
        if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= AppealRight;
        if ((in->m_buttonsCurrentFrame2.bits & 1) || (in->m_buttonsCurrentFrame2.bits & 2)) out->m_button |= AppealSide;
    }
    if (in->m_buttonsCurrentFrame2.bits & 0x200) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 0x800) out->m_button |= DirUp;
    if (in->m_buttonsCurrentFrame2.bits & 0x400) out->m_button |= DirRight;
    if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 8) out->m_button |= DirUp;
    if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= DirRight;
    setupPadSubstick(out, in, config->substick, state);
}

// Nunchuk motion and directional button shortcuts share an edge detector: a
// sustained gesture produces one smash and must release before another can fire.
void ipPadConfig::setupPadStatusWiiFreeStyle(ipPadStatus* out, const gfPadStatus* in, const ipNunchukConfig* config, ipSubstickState* state) {
    bool motion = false;
    Vec2f direction(0.0f, 0.0f);
    if ((in->m_buttonsCurrentFrame2.bits & 0x10000) && (in->m_buttonsCurrentFrame2.bits & 0x20000)) {
        if (config->cz == 11) out->m_button |= AppealSide;
        else out->m_button |= mappedButton(config->cz);
    } else {
        if (in->m_buttonsCurrentFrame2.bits & 0x10000) out->m_button |= mappedButton(config->c);
        if (in->m_buttonsCurrentFrame2.bits & 0x20000) out->m_button |= mappedButton(config->z);
    }
    if ((in->m_buttonsCurrentFrame2.bits & 0x100) && (in->m_buttonsCurrentFrame2.bits & 0x200)) {
        out->m_button |= mappedButton(config->ab);
        out->m_button |= StockShare;
    } else {
        if (in->m_buttonsCurrentFrame2.bits & 0x100) out->m_button |= mappedButton(config->a);
        if (in->m_buttonsCurrentFrame2.bits & 0x200) out->m_button |= mappedButton(config->b);
    }
    if (in->m_buttonsCurrentFrame2.bits & 0x40000) out->m_button |= mappedButton(config->motion);
    if (in->m_buttonsCurrentFrame2.bits & 0x10) out->m_button |= mappedButton(config->extra);
    if (in->m_buttonsCurrentFrame2.bits & 8) {
        if (config->up == 6) { motion = true; direction.m_y = 1.0f; }
        else out->m_button |= mappedButton(config->up);
    }
    if ((in->m_buttonsCurrentFrame2.bits & 1) || (in->m_buttonsCurrentFrame2.bits & 2)) {
        if (config->side == 7) {
            motion = true;
            if (in->m_buttonsCurrentFrame2.bits & 2) direction.m_x = 1.0f;
            else direction.m_x = -1.0f;
        } else out->m_button |= mappedButton(config->side);
    }
    if (in->m_buttonsCurrentFrame2.bits & 4) {
        if (config->down == 8) { motion = true; direction.m_y = -1.0f; }
        else out->m_button |= mappedButton(config->down);
    }
    out->m_stickX = in->m_stickX;
    out->m_stickY = in->m_stickY;
    if (((config->flags >> 7) & 1) != 0) {
        float strength = in->_0x24;
        if (strength > 0.125f) {
            float vertical = in->_0x1c + in->_0x28;
            float horizontal = in->_0x18;
            if (strength > 0.15f && float(__fabs(horizontal)) > 1.05f) {
                motion = true;
                if (horizontal < 0.0f) direction.m_x = -1.0f;
                else direction.m_x = 1.0f;
            }
            if (strength > 0.125f && float(__fabs(vertical)) > 0.95f) {
                motion = true;
                if (vertical < 0.0f) direction.m_y = 1.0f;
                else direction.m_y = -1.0f;
            }
        }
    }
    bool edge = false;
    state->motionHistory <<= 1;
    if (motion) {
        state->motionHistory |= 1;
        if ((state->motionHistory & 3) == 1) edge = true;
    }
    if (edge) {
        out->m_button |= CStick | Smash | Attack;
        out->m_stickX = direction.m_x * 81.0f;
        out->m_stickY = direction.m_y * 81.0f;
    }
    if (((config->flags >> 6) & 1) != 0) out->m_button |= FlickJump;
    if (config->side == 11) {
        if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= AppealLeft;
        if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= AppealRight;
        if ((in->m_buttonsCurrentFrame2.bits & 1) || (in->m_buttonsCurrentFrame2.bits & 2)) out->m_button |= AppealSide;
    }
    if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 8) out->m_button |= DirUp;
    if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= DirRight;
}

// The sideways remote synthesizes an analog stick from D-pad history. Rapid
// represses reach full tilt, holding builds speed, and release decays toward zero.
void ipPadConfig::setupPadStatusWiiCore(ipPadStatus* out, const gfPadStatus* in, const ipWiiCoreConfig* config, ipCorePadState* state) {
    float slowRate = g_GameGlobal->getSlowRate();
    bool smash = false;
    bool upJump = false;
    if (in->m_buttonsCurrentFrame2.bits & 0x80000) out->m_button |= mappedButton(config->extra);
    if ((in->m_buttonsCurrentFrame2.bits & 0x10000) && (in->m_buttonsCurrentFrame2.bits & 0x20000)) {
        out->m_button |= mappedButton(config->oneTwo);
        if (config->oneTwo == 5) { smash = true; out->m_button |= Attack | Smash; }
    } else {
        if (in->m_buttonsCurrentFrame2.bits & 0x10000) out->m_button |= mappedButton(config->one);
        if (in->m_buttonsCurrentFrame2.bits & 0x20000) out->m_button |= mappedButton(config->two);
    }
    if ((in->m_buttonsCurrentFrame2.bits & 0x200000) && (in->m_buttonsCurrentFrame2.bits & 0x400000)) {
        out->m_button |= mappedButton(config->ab);
        out->m_button |= StockShare;
    } else {
        if (in->m_buttonsCurrentFrame2.bits & 0x200000) out->m_button |= mappedButton(config->a);
        if (in->m_buttonsCurrentFrame2.bits & 0x400000) out->m_button |= mappedButton(config->b);
    }
    if (in->m_buttonsCurrentFrame2.bits & 0x80000) out->m_button |= mappedButton(config->extra);
    if (in->m_buttonsCurrentFrame2.bits & 1) out->m_button |= DirLeft;
    if (in->m_buttonsCurrentFrame2.bits & 2) out->m_button |= DirRight;
    out->m_button |= FlickJump;
    if (in->m_buttonsCurrentFrame2.bits & 8) {
        if (config->up == 2) upJump = true;
        else out->m_button |= mappedButton(config->up);
        out->m_button |= DirUp;
    }
    if (out->m_button & 0x200) {
        bool direction = false;
        if (in->m_buttonsCurrentFrame2.bits & 1) { out->m_button |= AppealSide; direction = true; }
        if (in->m_buttonsCurrentFrame2.bits & 2) { out->m_button |= AppealSide; direction = true; }
        if (in->m_buttonsCurrentFrame2.bits & 8) { out->m_button |= AppealHi; direction = true; }
        if (in->m_buttonsCurrentFrame2.bits & 4) { out->m_button |= AppealLw; direction = true; }
        if (!direction) out->m_button |= AppealHi;
    }
    out->m_stickX = 0;
    out->m_stickY = 0;
    int leftHistory, rightHistory, upHistory, downHistory;
    bool horizontal = false, vertical = false;
    leftHistory = state->leftHistory;
    rightHistory = state->rightHistory;
    upHistory = state->upHistory;
    downHistory = state->downHistory;
    leftHistory <<= 1;
    rightHistory <<= 1;
    upHistory <<= 1;
    downHistory <<= 1;
    if (in->m_buttonsCurrentFrame2.bits & 2) {
        if (state->stickX < 0) state->stickX = 0;
        if (g_GameGlobal->isJustGameFrame()) state->stickX = state->stickX + ((30.0f - slowRate) + 1.0f);
        else ++state->stickX;
        rightHistory |= 1;
        horizontal = true;
    } else if (in->m_buttonsCurrentFrame2.bits & 1) {
        if (state->stickX > 0) state->stickX = 0;
        if (g_GameGlobal->isJustGameFrame()) state->stickX = state->stickX - ((30.0f - slowRate) + 1.0f);
        else --state->stickX;
        leftHistory |= 1;
        horizontal = true;
    }
    if (in->m_buttonsCurrentFrame2.bits & 4) ++state->downFrames;
    if (in->m_buttonsCurrentFrame2.bits & 8) {
        upHistory |= 1;
        if (upJump) vertical = true;
    } else if (in->m_buttonsCurrentFrame2.bits & 4) {
        s16 value = state->stickY;
        downHistory |= 1;
        vertical = true;
        if (value >= 0) value = -22;
        else if (value > -53) value -= 31;
        else if (state->downFrames > slowRate * 8.0f) value -= 2;
        state->stickY = value;
        if (out->m_button & Guard) state->stickY = -81;
    }
    if (!vertical && !(in->m_buttonsCurrentFrame2.bits & 4) && state->downFrames >= 1u && state->downFrames <= slowRate * 8.0f) {
        state->stickY = -81;
        vertical = true;
        state->downHold = 5;
    }
    if (!(in->m_buttonsCurrentFrame2.bits & 4)) state->downFrames = 0;
    if (state->downHold != 0) {
        state->stickY = -81;
        vertical = true;
        --state->downHold;
    }
    state->leftHistory = leftHistory;
    state->rightHistory = rightHistory;
    state->upHistory = upHistory;
    state->downHistory = downHistory;
    bool freshAction = false;
    int held = 0;
    if (out->m_button & Attack) held = 1;
    int attackHistory = (state->attackHistory << 1) | held;
    state->attackHistory = attackHistory;
    if ((attackHistory & 3) == 1) freshAction = true;
    held = 0;
    if (out->m_button & Special) held = 1;
    int specialHistory = (state->specialHistory << 1) | held;
    state->specialHistory = specialHistory;
    if ((specialHistory & 3) == 1) freshAction = true;
    if (horizontal) {
        if ((in->m_buttonsCurrentFrame2.bits & 2) && check_key_flip(rightHistory)) state->stickX = 81;
        if ((in->m_buttonsCurrentFrame2.bits & 1) && check_key_flip(leftHistory)) state->stickX = -81;
    }
    if (vertical) {
        if ((in->m_buttonsCurrentFrame2.bits & 8) && upJump) state->stickY = 81;
        if ((in->m_buttonsCurrentFrame2.bits & 4) && check_key_flip(downHistory)) state->stickY = -81;
    }
    if (!horizontal && state->stickX != 0) {
        if (state->stickX > 0) {
            state->stickX -= 20;
            if (state->stickX < 0) state->stickX = 0;
        } else {
            state->stickX += 20;
            if (state->stickX > 0) state->stickX = 0;
        }
    }
    if (!vertical && state->stickY != 0) {
        if (state->stickY > 0) {
            state->stickY -= 20;
            if (state->stickY < 0) state->stickY = 0;
        } else {
            state->stickY += 40;
            if (state->stickY > 0) state->stickY = 0;
        }
    }
    if (state->stickX > 81) state->stickX = 81;
    if (state->stickX < -81) state->stickX = -81;
    if (state->stickY > 81) state->stickY = 81;
    if (state->stickY < -81) state->stickY = -81;
    out->m_stickX = state->stickX;
    out->m_stickY = state->stickY;
    if (freshAction) {
        if (horizontal) {
            if ((in->m_buttonsCurrentFrame2.bits & 2) && hasRecentRelease(rightHistory)) smash = true;
            if ((in->m_buttonsCurrentFrame2.bits & 1) && hasRecentRelease(leftHistory)) smash = true;
        }
        if ((in->m_buttonsCurrentFrame2.bits & 8) && hasRecentRelease(upHistory)) {
            smash = true;
            state->stickY = 81;
        }
        if ((in->m_buttonsCurrentFrame2.bits & 4) && hasRecentRelease(downHistory)) smash = true;
    }
    if (smash) {
        if (out->m_button & Attack) out->m_button |= Smash;
        if (horizontal) {
            if (state->stickX > 0) out->m_stickX = 81;
            else out->m_stickX = -81;
        }
        if (vertical) {
            if (state->stickY > 0) out->m_stickY = 81;
            else out->m_stickY = -81;
        }
    }
}

// MATCH-ONLY: preserve the original scalar profile-copy schedule.
#pragma scheduling off
void ipPadConfig::setButtonConfig(int player, u32 pad, const ipButtonConfig* config) {
    m_playerPads[player] = pad;
    if (pad < 4) copyConfig(m_gc[pad], config->gc);
    else if (pad < 8) {
        pad -= 4;
        copyConfig(m_wii[pad], config->wii);
    }
}

#pragma scheduling reset

void ipPadConfig::getPadInput(int player, const gfPadStatus* in, ipPadStatus* out) {
    u32 pad = m_playerPads[player];
    if (pad < 8 && in->m_error == 0) {
        out->m_button = 0;
        if (in->m_controllerType == gfPadType::GCC) {
            setupPadStatusGc(out, in, &m_gc[pad], &m_substickState[pad]);
        } else {
            int wireless = pad - 4;
            ipCorePadState* coreState = &m_coreState[pad];
            ipSubstickState* substickState = &m_substickState[pad];
            switch (in->m_controllerType) {
            case gfPadType::WIIMOTE: setupPadStatusWiiCore(out, in, &m_wii[wireless].core, coreState); break;
            case gfPadType::WII_CLASSIC: setupPadStatusWiiClassic(out, in, &m_wii[wireless].classic, substickState); break;
            case gfPadType::NUNCHUK: setupPadStatusWiiFreeStyle(out, in, &m_wii[wireless].nunchuk, substickState); break;
            }
        }
    } else {
        out->m_button = 0;
        out->m_stickY = 0;
        out->m_stickX = 0;
    }
}

s32 ipPadConfig::getPlayerNo(s32 pad) {
    for (int i = 0; i < 4; ++i) if ((u32)pad == m_playerPads[i]) return i;
    return -1;
}

int ipPadConfig::getPlayerNoFast(int pad) { return m_padPlayers[pad]; }

// A later player wins duplicate assignments in the fast reverse table. The
// linear query above deliberately keeps its native first-player behavior.
void ipPadConfig::createPadNo2PlayerNoTable() {
    for (int pad = 0; pad < 8; ++pad) m_padPlayers[pad] = -1;
    for (int player = 0; player < 4; ++player) {
        if (m_playerPads[player] < 8) m_padPlayers[m_playerPads[player]] = player;
    }
}
