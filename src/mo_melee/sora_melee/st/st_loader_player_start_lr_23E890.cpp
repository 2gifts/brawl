#include <st/stage.h>
#include <cm/cm_camera_controller.h>
#include <gm/gm_global_mode_melee.h>
#include <math.h>

// The facing a stage loader gives a player at its start point: away from the nearest opponent start point (or by the
// opponent's own facing byte when they are close), else towards the camera centre. Called three times from the
// unmatched stLoaderPlayer code at 0x23B5F0.
// HYPOTHESIS: m_0x9 of the player init data is a start facing (1 / -1); m_gameRule 1 is the stock rule.

static inline int getTeam(gmGlobalModeMelee* melee, int i) {
    gmPlayerInitData& player = melee->m_playersInitData[i];
    int team;
    if (melee->m_meleeInitData.m_corpsKind == 0) {
        if (!melee->m_meleeInitData.m_isTeams) {
            team = i;
        } else {
            team = (u8)player.m_teamNo;
        }
    } else {
        team = (u8)player.m_teamNo;
    }
    return team;
}

static inline float getCameraSide(const float& x) {
    CameraController* cam = CameraController::getInstance();
    if (x <= 0.5f * (cam->m_stageCameraBounds.m_left + cam->m_stageCameraBounds.m_right)) {
        return 1.0f;
    }
    return -1.0f;
}

// TODO: clean up - placeholder fn_ name; the real name (likely a stLoaderPlayer static or member) is unknown.
extern "C" float fn_27_23E890(gmGlobalModeMelee* melee, u32 idx) {
    float xs[4];
    if (g_Stage) {
        for (int i = 0; i < 4; i++) {
            Vec3f pos;
            g_Stage->getFighterStartPos(&pos, melee->m_playersInitData[i].m_startPointIdx);
            xs[i] = pos.m_x;
        }
    } else {
        xs[0] = 0.0f;
        xs[1] = 0.0f;
        xs[2] = 0.0f;
        xs[3] = 0.0f;
    }
    u32 i;
    int found = -1;
    float best = 999999.0f;
    for (i = 0; i < 4; i++) {
        if (i == idx || melee->m_playersInitData[i].m_state == 3 || melee->m_playersInitData[i].m_characterKind == 0x3e) {
            continue;
        }
        if (getTeam(melee, idx) == getTeam(melee, i)) {
            continue;
        }
        if (melee->m_playersInitData[i].m_stockCount == 0 && melee->m_meleeInitData.m_gameRule == 1) {
            continue;
        }
        float d = __fabsf(xs[idx] - xs[i]);
        if (best > d) {
            best = d;
            found = i;
        }
    }
    if (found == -1) {
        return getCameraSide(xs[idx]);
    }
    float diff = xs[found] - xs[idx];
    if (diff < -5.0f) {
        return -1.0f;
    }
    if (diff > 5.0f) {
        return 1.0f;
    }
    s8 lr = melee->m_playersInitData[found].m_0x9;
    if (1.0f == lr) {
        return -1.0f;
    }
    if (-1.0f == lr) {
        return 1.0f;
    }
    return getCameraSide(xs[idx]);
}
