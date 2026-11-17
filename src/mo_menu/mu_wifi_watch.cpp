#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// The screen that shows a Wi-Fi battle somebody else plays (module sora_menu_watch). The class is only described by the
// members the decompiled functions touch.
/// Function names come from the sora_menu_watch map.
// HYPOTHESIS: a pair that the menus keep two of in every unit (an id that is 0xFF while it is not used, and a state); the
// pairs are made when the unit starts.
struct muWatchPair {
    int m_id;
    int m_state;

    muWatchPair(int id, int state) : m_id(id), m_state(state) { }
};

static muWatchPair s_pair0(0xFF, 0);
static muWatchPair s_pair1(0xFF, 1);

class nteWatching {
public:
    void cancel();
};

// HYPOTHESIS: the task of the prize window (its state is read at 0x104).
class muWatchSubTask : public gfTask {
public:
    u8 m_unk40[0x104 - 0x40];
    int m_state; // 0x104
};

// HYPOTHESIS: the task of the prize window (its state is read at 0x10C, the flag at 0x3D0 lets it close).
class muWatchPrizeTask : public gfTask {
public:
    u8 m_unk40[0x10C - 0x40];
    int m_state; // 0x10C
    u8 m_unk110[0x3D0 - 0x110];
    u8 m_close; // 0x3D0
};

class muWifiWatchTask : public gfTask {
public:
    // One of the textures of the characters (the ctor clears the first members, the dtor frees the file handle).
    struct CharTex {
        int m_unk0;
        int m_unk4;
        gfFileIOHandle m_file; // 0x8

        CharTex();
        ~CharTex();
    };

    // The data that initStepNextBattle copies over the state of the current battle.
    struct BattleData {
        int m_unk0;
        int m_data[0x33];
    };

    u8 m_unk40[0x48 - 0x40];
    int m_nextStep;                     // 0x48 HYPOTHESIS: the step the main step jumps to
    MuObject* m_obj4C;                  // 0x4C
    u8 m_unk50[0x8C - 0x50];
    MuObject* m_obj8C;                  // 0x8C
    u8 m_unk90[0x108 - 0x90];
    MuObject* m_obj108;                 // 0x108
    u8 m_unk10C[0x18C - 0x10C];
    MuObject* m_obj18C;                 // 0x18C
    u8 m_unk190[0x194 - 0x190];
    nw4r::g3d::ScnMdlExpand* m_group194; // 0x194
    u8 m_unk198[0x1A0 - 0x198];
    nw4r::g3d::ScnMdlExpand* m_group1A0; // 0x1A0
    u8 m_unk1A4[0x4EC - 0x1A4];
    int m_unk4EC;                       // 0x4EC
    int m_unk4F0;                       // 0x4F0
    u8 m_unk4F4[0x504 - 0x4F4];
    int m_stepResult;                   // 0x504 HYPOTHESIS: what the main steps return while they wait
    u8 m_unk508[0x518 - 0x508];
    int m_unk518;                       // 0x518
    u8 m_unk51C[0x534 - 0x51C];
    muWatchPrizeTask* m_prizeTask;      // 0x534
    u8 m_unk538[0x540 - 0x538];
    muWatchSubTask* m_subTask;          // 0x540
    u8 m_unk544[0x548 - 0x544];
    BattleData m_battle;                // 0x548
    BattleData m_nextBattle;            // 0x618
    u8 m_unk6E8[0xD54 - 0x6E8];
    int m_unkD54;                       // 0xD54
    u8 m_unkD58;                        // 0xD58
    u8 m_downloaded;                    // 0xD59
    u8 m_unkD5A[0xD5C - 0xD5A];
    nteWatching* m_watching;            // 0xD5C
    u8 m_unkD60[0xD78 - 0xD60];

    int mainStepNetError();
    void initStepDownloadCancel();
    int mainStepDownloadCancel();
    void initStepNextBattle();
    void initStepPost();
    void fn_4078();
    void initStepDownloadWait();
    int mainStepGetBonus();
    int mainStepPrizeWindow();
};

muWifiWatchTask::CharTex::CharTex() : m_unk4(0) { }

muWifiWatchTask::CharTex::~CharTex() { }

void muWifiWatchTask::initStepPost() { }

void muWifiWatchTask::initStepNextBattle() {
    m_unkD54 = 0x3C;
    m_battle = m_nextBattle;
}

void muWifiWatchTask::initStepDownloadCancel() {
    if (!m_downloaded) {
        m_watching->cancel();
    }
    m_unk4EC = 6;
}

int muWifiWatchTask::mainStepDownloadCancel() {
    if (m_unk4EC != 6) {
        m_nextStep = 2;
        return 0xB;
    }
    return m_stepResult;
}

int muWifiWatchTask::mainStepNetError() {
    if (m_subTask->m_state == 3) {
        m_subTask->exit();
        m_subTask = NULL;
        if (m_unk4F0 != 0) {
            m_nextStep = 3;
        } else {
            m_nextStep = 4;
        }
        return 0xB;
    }
    return m_stepResult;
}

void muWifiWatchTask::fn_4078() {
    m_obj8C->changeAnimN(1, 0x13, true, false);
}

void muWifiWatchTask::initStepDownloadWait() {
    m_obj4C->changeAnimN(1, 1, true, false);
    m_unk518 = 0x1E;
    nw4r::g3d::ScnMdlExpand* group = m_group194;
    MuObject* obj = m_obj18C;
    group->Remove(obj->m_sceneModel);
    group->PushBack(obj->m_sceneModel, "pos00");
}

int muWifiWatchTask::mainStepGetBonus() {
    if (m_obj108->isNodeAnimFinished()) {
        m_group1A0->Remove(m_obj108->m_sceneModel);
        int step;
        if (m_unkD58) {
            if (m_battle.m_unk0 != 0) {
                step = 4;
            } else {
                step = 6;
            }
        } else {
            step = 6;
        }
        return step;
    }
    return m_stepResult;
}

int muWifiWatchTask::mainStepPrizeWindow() {
    if (m_unkD54 > 0) {
        m_unkD54--;
    }
    if (m_prizeTask != NULL) {
        if (m_prizeTask->m_state == 3 && m_unkD54 == 0) {
            m_prizeTask->m_close = 1;
            m_unkD54 = -1;
        }
        if (m_prizeTask->m_state == 5 || m_unkD54 == 0) {
            m_prizeTask->exit();
            m_prizeTask = NULL;
            m_group1A0->Remove(m_obj8C->m_sceneModel);
            return 6;
        }
    }
    return m_stepResult;
}
