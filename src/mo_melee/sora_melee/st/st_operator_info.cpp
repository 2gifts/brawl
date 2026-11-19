#include <ft/ft_manager.h>
#include <gm/gm_global_mode_melee.h>
#include <if/if_mngr.h>
#include <snd/snd_system.h>
#include <st/operator/st_operator_info.h>
#include <types.h>

extern "C" {
int fn_80110310(int* trigger, int* padNo);
void fn_800E5174(IfPlayer* player, int mode);
extern u8 lbl_27_bss_59D8;
}

// HYPOTHESIS: IfMngr maps a player index to its IfPlayer slot through the bytes at 0x68 (unk68 in the local IfMngr
// header).

// HYPOTHESIS: two (value, flag) pairs filled by the static initializer (same shape as other sora_melee units).
struct stOperatorInfoPair {
    u32 m_0;
    u32 m_4;
    stOperatorInfoPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static stOperatorInfoPair g_stOperatorInfoPairA(0xff, 0);
static stOperatorInfoPair g_stOperatorInfoPairB(0xff, 1);

int stOperatorInfo::getPauseTriggerAndPadNo() {
    int trigger;
    int padNo;
    fn_80110310(&trigger, &padNo);
    return trigger;
}

void stOperatorInfo::setLocalPlayerPadMask() {
    if (m_c0 & 0x80) {
        m_cc = 0xff;
        return;
    }
    m_cc = 0;
    u8 mode = m_condition->m_meleeInitData.m_gameMode;
    if (mode != Game_Mode_Melee && mode != Game_Mode_Tournament) {
        for (int i = 0; i < 4; i++) {
            gmPlayerInitData* data = &m_condition->m_playersInitData[i];
            if (static_cast<u8>(data->_0x3C[1]) != 0) {
                m_cc |= 1 << (static_cast<u8>(data->_0x3C[1]) - 1);
                m_ec[static_cast<u8>(data->_0x3C[1])] = i;
            }
        }
    }
    if (m_cc == 0) {
        m_cc = 0xff;
    }
}

void stOperatorInfo::setLocalPlayerPadMaskWifi() {
    if (m_c0 & 0x80) {
        m_c8 = 0xff;
        m_cc = 0xff;
        return;
    }
    m_c8 = 0;
    m_cc = 0;
    for (int i = 0; i < 4; i++) {
        gmPlayerInitData* data = &m_condition->m_playersInitData[i];
        if (static_cast<u8>(data->_0x3C[1]) != 0) {
            m_c8 |= 1 << i;
            m_cc |= 1 << (static_cast<u8>(data->_0x3C[1]) - 1);
            m_ec[static_cast<u8>(data->_0x3C[1])] = i;
        }
    }
    if (m_cc == 0) {
        m_cc = 0xff;
    }
    if (m_c8 == 0) {
        m_c8 = 0xff;
    }
}

void stOperatorInfo::lockPlayer() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_loaderPlayers[i] && m_loaderPlayers[i]->isEntity()) {
            m_loaderPlayers[i]->lockEntity();
        }
    }
}

void stOperatorInfo::unlockPlayer() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_loaderPlayers[i] && m_loaderPlayers[i]->isEntity()) {
            m_loaderPlayers[i]->unlockEntity();
        }
    }
}

void stOperatorInfo::setPlayerCursorClear(u8 index) {
    IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
    if (player->m_headCursorMode) {
        fn_800E5174(player, 0);
    }
    m_playersCursorStatus[index] = 0;
    m_playersNameCursorStatus[index] = 0;
}

void stOperatorInfo::setPlayerCursorStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the float table store.
    int index = idx;
    if (status == 0 && status != m_playersCursorStatus[index] && m_playersNameCursorStatus[index] == 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (player->m_headCursorMode) {
            fn_800E5174(player, 0);
        }
        m_98[index] = 0.0f;
    }
    m_playersCursorStatus[index] = status;
}

void stOperatorInfo::setPlayerNameCursorStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the float table store.
    int index = idx;
    if (status == 0 && status != m_playersNameCursorStatus[index] && m_playersCursorStatus[index] == 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (player->m_headCursorMode) {
            fn_800E5174(player, 0);
        }
        m_98[index] = 0.0f;
    }
    m_playersNameCursorStatus[index] = status;
}

void stOperatorInfo::setPlayerLoupeStatus(u8 index, u8 status) {
    if (status == 0 && status != m_playersLoupeStatus[index]) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (player->m_isLoupeVisible) {
            player->setLoupeVisible(false);
        }
        player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (player->unk2B0) {
            player->unk2B0 = 0;
        }
    }
    m_playersLoupeStatus[index] = status;
}

void stOperatorInfo::clearPlayerCursor() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        m_playersCursorStatus[i] = 0;
        m_playersNameCursorStatus[i] = 0;
        m_72[i] = 0;
        m_7c[i] = 0;
        m_98[i] = 0.0f;
        m_playersLoupeStatus[i] = 0;
    }
    if (g_IfMngr && g_ftManager) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (g_ftManager->getEntryId(i) != -1) {
                u8 playerId = m_condition->m_playersInitData[i].m_playerId;
                IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[playerId])];
                if (player->m_isLoupeVisible) {
                    player->setLoupeVisible(false);
                }
                player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[playerId])];
                if (player->unk2B0) {
                    player->unk2B0 = 0;
                }
            }
        }
    }
    if (m_114 >= 0) {
        g_sndSystem->stopSE(m_114, 0);
        m_114 = -1;
    }
}

void stOperatorInfo::checkOnamaeForcePut() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        gmPlayerInitData* data = &m_condition->m_playersInitData[i];
        if (data->m_state != 3) {
            bool forcePut = false;
            u8 playerId = data->m_playerId;
            if (data->m_0x1C_5 && !data->m_isSpycloak) {
                forcePut = true;
            }
            m_72[playerId] = forcePut;
        }
    }
}

void stOperatorInfo::startEventCursor() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!m_condition->m_playersInitData[i].m_isSpycloak) {
            if (m_condition->m_playersInitData[i].m_state == 0) {
                m_7c[m_condition->m_playersInitData[i].m_playerId] = 180;
            }
            if (m_condition->m_playersInitData[i].m_state == 1) {
                m_7c[m_condition->m_playersInitData[i].m_playerId] = 300;
            }
        }
    }
}

void stOperatorInfo::processEnd() {
    if (m_119) {
        updatePlayerCursor(m_11a);
        m_119 = false;
    }
}

bool stOperatorInfo::isError() {
    return false;
}

u32 stOperatorInfo::getErrorLevel() {
    return 0;
}

u32 stOperatorInfo::getEndType() {
    return 1;
}

bool stOperatorInfo::isEnableOperation() {
    return true;
}

void stOperatorInfo::stopOperatorRequest() { }

void stOperatorInfo::startOperator() { }

void stOperatorInfo::processBegin() { }

stOperatorInfo::~stOperatorInfo() { }

#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>
#include <it/it_manager.h>
// TODO: clean up, give variables real names
namespace jj_255808 {

extern "C" {
void* __ct__10stOperatorFPCc(void* self, const char* name);
float getFighterCursorForceDispDistance__9ftManagerCFv(ftManager* mgr);
extern u8 lbl_27_bss_59D8;
extern char lbl_27_data_52F90[];
extern const f32 lbl_27_rodata_4140;
extern const f32 lbl_27_rodata_4144;

void* __ct__14stOperatorInfoFPCc(void* self, const char* name) {
    u8* p = (u8*)self;
    __ct__10stOperatorFPCc(self, name);
    *(char**)(p + 0x3C) = lbl_27_data_52F90;
    p[0xBB] = 0;
    p[0xBC] = 1;
    *(s32*)(p + 0xC0) = 0;
    p[0xC4] = 0;
    *(s32*)(p + 0xC8) = 0xFF;
    *(s32*)(p + 0xCC) = 0xFF;
    p[0x10C] = 0;
    *(f32*)(p + 0x110) = lbl_27_rodata_4140;
    *(s32*)(p + 0x114) = -1;
    p[0x119] = 0;
    lbl_27_bss_59D8 = 1;
    *(s32*)(p + 0x48) = 0;
    p[0x64] = 0;
    p[0x6B] = 0;
    p[0x72] = 0;
    *(s32*)(p + 0x7C) = 0;
    *(f32*)(p + 0x98) = lbl_27_rodata_4144;
    p[0xB4] = 0;
    *(s32*)(p + 0x4C) = 0;
    p[0x65] = 0;
    p[0x6C] = 0;
    p[0x73] = 0;
    *(s32*)(p + 0x80) = 0;
    *(f32*)(p + 0x9C) = lbl_27_rodata_4144;
    p[0xB5] = 0;
    *(s32*)(p + 0x50) = 0;
    p[0x66] = 0;
    p[0x6D] = 0;
    p[0x74] = 0;
    *(s32*)(p + 0x84) = 0;
    *(f32*)(p + 0xA0) = lbl_27_rodata_4144;
    p[0xB6] = 0;
    *(s32*)(p + 0x54) = 0;
    p[0x67] = 0;
    p[0x6E] = 0;
    p[0x75] = 0;
    *(s32*)(p + 0x88) = 0;
    *(f32*)(p + 0xA4) = lbl_27_rodata_4144;
    p[0xB7] = 0;
    *(s32*)(p + 0x58) = 0;
    p[0x68] = 0;
    p[0x6F] = 0;
    p[0x76] = 0;
    *(s32*)(p + 0x8C) = 0;
    *(f32*)(p + 0xA8) = lbl_27_rodata_4144;
    p[0xB8] = 0;
    *(s32*)(p + 0x5C) = 0;
    p[0x69] = 0;
    p[0x70] = 0;
    p[0x77] = 0;
    *(s32*)(p + 0x90) = 0;
    *(f32*)(p + 0xAC) = lbl_27_rodata_4144;
    p[0xB9] = 0;
    *(s32*)(p + 0x60) = 0;
    p[0x6A] = 0;
    p[0x71] = 0;
    p[0x78] = 0;
    *(s32*)(p + 0x94) = 0;
    *(f32*)(p + 0xB0) = lbl_27_rodata_4144;
    p[0xBA] = 0;
    *(s32*)(p + 0xEC) = -1;
    *(s32*)(p + 0xF0) = -1;
    *(s32*)(p + 0xF4) = -1;
    *(s32*)(p + 0xF8) = -1;
    *(s32*)(p + 0xFC) = -1;
    *(s32*)(p + 0x100) = -1;
    *(s32*)(p + 0x104) = -1;
    *(s32*)(p + 0x108) = -1;
    if (g_ftManager != 0) {
        *(f32*)(p + 0x110) = getFighterCursorForceDispDistance__9ftManagerCFv(g_ftManager);
    }
    {
        s32 v = 0;
        u32 t = ((u32)(*(u8*)(*(u8**)(p + 0x44) + 8)) >> 2) & 0x3F;
        if (t != 9 && t != 0x12) {
            v = 1;
        }
        p[0x118] = (u8)v;
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_25614C {
extern int g_IfMngr;
extern float lbl_27_rodata_4140[];
extern float lbl_805A02C8;
extern "C" int fn_8009C8C0(void*);
extern "C" void getFighterCursorPos__9ftManagerCFii(void*, void*, int, int);
extern "C" int isDispLoupeArrow__9ftManagerCFii(void*, int, int);
extern "C" float getFighterScreenX__9ftManagerCFii(void*, int, int);
extern "C" void fn_800E5174(void*, int);
extern "C" void setHeadCursorPos__8IfPlayerFPCQ34nw4r4math4VEC3(void*, void*);
extern "C" void setLoupeVisible__8IfPlayerFb(void*, int);
extern "C" void setLoupePos__8IfPlayerFPCQ34nw4r4math4VEC3(void*, void*);
extern "C" void setApealCommentBalloonPos__8IfPlayerFPCQ34nw4r4math4VEC3(void*, void*);
extern "C" void stopSE__9sndSystemFll(void*, int, int);

inline unsigned char* getPane(int player)
{
    unsigned char* hud = (unsigned char*)g_IfMngr;
    return *(unsigned char**)(hud + hud[player + 0x68] * 4 + 0x4C);
}

extern "C" void fn_27_25614C(unsigned char* p, int active)
{
    float* constants = lbl_27_rodata_4140;
    int offset;
    unsigned char* d0;
    float* value;
    int forceMode;
    int soundCount, i, entry, player;
    if (g_IfMngr == 0 || g_ftManager == 0)
        return;
    if (active != 0) {
        active = 0;
        if (fn_8009C8C0(CameraController::getInstance()) != 0 &&
            itManager::getInstance()->getItemNum((itKind)0xA0) == 0)
            active = 1;
    }
    gfCameraManager* camera = gfCameraManager::getManager();
    if (camera != 0) {
        float h = camera->m_cameras[0].unkD0;
        float w = camera->m_cameras[0].unkCC;
        float edge = w * h;
        float value;
        if (p[0x10C])
            value = *(float*)(p + 0x110);
        else
            value = constants[3] + *(float*)(p + 0x110);
        p[0x10C] = value < edge;
    } else {
        p[0x10C] = 0;
    }
    float fZero, fHalfPi, fNegHalfPi, f240, fOne, f120;
    fZero = constants[1];
    fHalfPi = constants[6];
    fNegHalfPi = constants[7];
    f120 = constants[4];
    fOne = constants[3];
    f240 = constants[5];
    soundCount = 0;
    forceMode = (unsigned int)__cntlzw((*(gmGlobalModeMelee**)(p + 0x44))->m_meleeInitData.m_isStamina) >> 5;
    d0 = p;
    unsigned char markerPos[12];
    for (i = 0, offset = 0; i < 7; ++i, offset += 0x5C, d0 += 4) {
        entry = g_ftManager->getEntryId(i);
        if (entry != -1) {
            unsigned char* slot = (unsigned char*)*(void**)(p + 0x44) + offset;
            player = slot[0x9A];
            int condition = 0;
            int enabled = !((gmPlayerInitData*)(slot + 0x98))->m_isSpycloak;
            int first = 0;
            if (enabled && p[player + 0x6B] != 0)
                first = 1;
            if (first) {
                int second = 0;
                if (p[player + 0x72] != 0 ||
                    *(int*)(p + player * 4 + 0x7C) > 0 ||
                    p[0x10C] != 0)
                    second = 1;
                if (second)
                    condition = 1;
            }
            int alternate = enabled && p[player + 0x64] != 0;
            if (active && (alternate || condition)) {
                if (condition) {
                    if (*(int*)(getPane(player) + 0x354) != 1)
                        fn_800E5174(getPane(player), 1);
                    unsigned char tmp[12];
                    getFighterCursorPos__9ftManagerCFii(tmp, g_ftManager, entry, -1);
                    setHeadCursorPos__8IfPlayerFPCQ34nw4r4math4VEC3(getPane(player), tmp);
                    int* timer = (int*)(p + player * 4 + 0x7C);
                    if (*timer > 0)
                        --*timer;
                } else {
                    if (forceMode && *(unsigned int*)(d0 + 0xD0) >= 0x32) {
                        value = (float*)(p + player * 4 + 0x98);
                        int mode = *value < f120 ? 1 : 2;
                        if (mode != *(int*)(getPane(player) + 0x354))
                            fn_800E5174(getPane(player), mode);
                        *value += fOne;
                        if (f240 <= *value)
                            *value -= f240;
                    } else if (*(int*)(getPane(player) + 0x354) != 1) {
                        fn_800E5174(getPane(player), 1);
                    }
                    unsigned char tmp[12];
                    getFighterCursorPos__9ftManagerCFii(tmp, g_ftManager, entry, -1);
                    setHeadCursorPos__8IfPlayerFPCQ34nw4r4math4VEC3(getPane(player), tmp);
                }
            } else {
                if (*(int*)(getPane(player) + 0x354) != 0)
                    fn_800E5174(getPane(player), 0);
                *(float*)(p + player * 4 + 0x98) = fZero;
            }
            if (active && p[0x118] && p[player + 0xB4]) {
                if (getPane(player)[0x350] == 0)
                    setLoupeVisible__8IfPlayerFb(getPane(player), 1);
                Vec3f center = g_ftManager->getFighterCenterPos(entry, -1);
                setLoupePos__8IfPlayerFPCQ34nw4r4math4VEC3(getPane(player), &center);
                if (isDispLoupeArrow__9ftManagerCFii(g_ftManager, entry, -1) != 0) {
                    int mode;
                    unsigned char* hud;
                    int side = lbl_805A02C8 < fNegHalfPi;
                    if (!side)
                        side = fHalfPi < lbl_805A02C8;
                    if (!side)
                        side = (*(int*)((unsigned char*)g_gfSceneRoot + 0x64) >> 24) != 0;
                    if (side) {
                        if (fZero > getFighterScreenX__9ftManagerCFii(g_ftManager, entry, -1))
                            mode = 2;
                        else
                            mode = 1;
                    } else {
                        if (fZero > getFighterScreenX__9ftManagerCFii(g_ftManager, entry, -1))
                            mode = 1;
                        else
                            mode = 2;
                    }
                    hud = (unsigned char*)g_IfMngr;
                    unsigned char* pane = *(unsigned char**)(hud + hud[player + 0x68] * 4 + 0x4C);
                    if (mode != *(int*)(pane + 0x2B0))
                        *(int*)(pane + 0x2B0) = mode;
                    if (((unsigned char*)*(void**)(p + 0x44))[offset + 0x99] == 0)
                        ++soundCount;
                } else {
                    unsigned char* pane = getPane(player);
                    if (*(int*)(pane + 0x2B0) != 0)
                        *(int*)(pane + 0x2B0) = 0;
                }
            } else {
                if (getPane(player)[0x350] != 0)
                    setLoupeVisible__8IfPlayerFb(getPane(player), 0);
                unsigned char* pane = getPane(player);
                if (*(int*)(pane + 0x2B0) != 0)
                    *(int*)(pane + 0x2B0) = 0;
            }
            if (active && p[0xC4] && *(int*)(getPane(player) + 0x554) > 0) {
                getFighterCursorPos__9ftManagerCFii(markerPos, g_ftManager, entry, -1);
                setApealCommentBalloonPos__8IfPlayerFPCQ34nw4r4math4VEC3(getPane(player), markerPos);
            }
        }
    }
    if (soundCount != 0) {
        if (*(int*)(p + 0x114) < 0)
            *(int*)(p + 0x114) =
                g_sndSystem->playSE((SndID)0x11, -1, 0, 0, -1);
    } else if (*(int*)(p + 0x114) >= 0) {
        stopSE__9sndSystemFll(g_sndSystem, *(int*)(p + 0x114), 0);
        *(int*)(p + 0x114) = -1;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_256838 {
typedef unsigned char u8;
struct FlagsB4 { u8 a:2; u8 b:1; u8 c:5; };
struct FlagsB3 { u8 a:5; u8 b:1; u8 c:2; };
struct Obj {
    u8 pad[0x44];
    u8 *data;
};

extern "C" void fn_27_256838(Obj *self) {
    u8 v;
    int off = 0;
    for (int i = 0; i < 7; i++) {
        u8 *p = self->data + off;
        if (p[0x99] != 3) {
            v = 0;
            u8 idx = p[0x9a];
            if (((FlagsB4 *)(p + 0xb4))->b) {
                if (!((FlagsB3 *)(p + 0xb3))->b) {
                    v = 1;
                }
            }
            ((u8 *)self)[0x72 + idx] = v;
        }
        off += 0x5c;
    }
}
}

