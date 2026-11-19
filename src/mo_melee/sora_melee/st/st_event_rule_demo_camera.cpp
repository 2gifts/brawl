#include <mt/mt_prng.h>
#include <st/st_event_rule.h>
#include <types.h>

// HYPOTHESIS: an initial value of the rule (a sora_melee data word).
extern "C" u32 lbl_27_data_545A0;

// TODO: chechEventRule (the camera cut sequence).

stEventRuleDemoCamera::stEventRuleDemoCamera() : stEventRule(1) {
    m_started = false;
    unk28 = lbl_27_data_545A0;
    unk2C = 0;
    m_order[0] = 1;
    m_order[1] = 2;
    m_order[2] = 3;
    m_order[3] = 4;
    for (int i = 0; i < 4; i++) {
        int j = randi(3);
        int tmp = m_order[i];
        m_order[i] = m_order[j];
        m_order[j] = tmp;
    }
}

stEventRuleDemoCamera::~stEventRuleDemoCamera() { }

void stEventRuleDemoCamera::notifyEventStartGame() {
    m_started = true;
}

#include <cm/cm_camera_controller.h>
// TODO: clean up, give variables real names
namespace jj_25E1E0 {

struct State {
    char pad0[0x10];
    unsigned char unk10;
    char pad11[3];
    int unk14;
    int unk18;
    int unk1C;
    char pad20[8];
    int unk28;
    int unk2C;
};

extern "C" {
extern int lbl_27_data_545A0[];
extern void* g_GameGlobal;
extern void* g_ftManager;
extern void* g_aiMgr;
void fn_800AAAD8(void* self, int kind);
int getEntryId__9ftManagerFi(void* self, int a);
bool isCpuActive__9ftManagerCFi(void* self, unsigned int id);
int getCpuRank__5aiMgrFi(void* p, int id);
void setCpuRank__5aiMgrFiSc(void* mgr, int obj, int arg);
}

#define CAM_STATE() (*(State**)((char*)CameraController::getInstance() + 0x80))

extern "C" int chechEventRule__21stEventRuleDemoCameraFv(void* p) {
    State* self = (State*)p;
    int* d = lbl_27_data_545A0;
    int v;

    switch (self->unk2C) {
    case 0:
        self->unk28 = d[1];
        self->unk2C = 0x57;
        goto L280;
    L26C:
        self->unk28 = d[2];
        self->unk2C = 0x5B;
        break;
    case 0x57:
    case 0x5B:
    L280:
        if (self->unk10 == 0) {
            goto L26C;
        }
        CameraController::getInstance()->changeInput((CameraController::Kind)0xD);
        {
            char* gg8 = *(char**)((char*)g_GameGlobal + 8);
            unsigned short k = *(unsigned short*)(gg8 + 0x1A);
            if (k == 0x18 || k == 0x25) {
                State* s = CAM_STATE();
                s->unk10 = 1;
            } else {
                State* s = CAM_STATE();
                s->unk10 = 0;
            }
        }
        fn_800AAAD8(CAM_STATE(), 0);
        self->unk14 = 0xF0;
        goto L37C;
    L2F8:
        {
            int id;
            int i;
            for (i = 0; i < 4; i++) {
                id = getEntryId__9ftManagerFi(g_ftManager, i);
                if (id != -1 && isCpuActive__9ftManagerCFi(g_ftManager, id) && getCpuRank__5aiMgrFi(g_aiMgr, i) != 0x46) {
                    setCpuRank__5aiMgrFiSc(g_aiMgr, i, 0x46);
                }
            }
        }
        self->unk14 -= 1;
        self->unk28 = d[3];
        self->unk2C = 0x83;
        break;
    case 0x83:
    L37C:
        if (self->unk14 > 0) {
            goto L2F8;
        }
        fn_800AAAD8(CAM_STATE(), self->unk18);
        self->unk14 = 0x12C;
        goto L440;
    L3A4:
        {
            int id;
            int i;
            for (i = 0; i < 4; i++) {
                id = getEntryId__9ftManagerFi(g_ftManager, i);
                if (id != -1 && isCpuActive__9ftManagerCFi(g_ftManager, id)) {
                    v = (self->unk18 == i + 1) ? 0x64 : 0x14;
                    if (v != getCpuRank__5aiMgrFi(g_aiMgr, i)) {
                        setCpuRank__5aiMgrFiSc(g_aiMgr, i, v);
                    }
                }
            }
        }
        self->unk14 -= 1;
        self->unk28 = d[4];
        self->unk2C = 0x9B;
        break;
    case 0x9B:
    L440:
        if (self->unk14 > 0) {
            goto L3A4;
        }
        fn_800AAAD8(CAM_STATE(), self->unk1C);
        self->unk14 = 0x12C;
        goto L504;
    L468:
        {
            int id;
            int i;
            for (i = 0; i < 4; i++) {
                id = getEntryId__9ftManagerFi(g_ftManager, i);
                if (id != -1 && isCpuActive__9ftManagerCFi(g_ftManager, id)) {
                    v = (self->unk1C == i + 1) ? 0x64 : 0x14;
                    if (v != getCpuRank__5aiMgrFi(g_aiMgr, i)) {
                        setCpuRank__5aiMgrFiSc(g_aiMgr, i, v);
                    }
                }
            }
        }
        self->unk14 -= 1;
        self->unk28 = d[5];
        self->unk2C = 0xB3;
        break;
    case 0xB3:
    L504:
        if (self->unk14 > 0) {
            goto L468;
        }
        fn_800AAAD8(CAM_STATE(), 0);
        self->unk14 = 0xB4;
        goto L5B0;
    L52C:
        {
            int id;
            int i;
            for (i = 0; i < 4; i++) {
                id = getEntryId__9ftManagerFi(g_ftManager, i);
                if (id != -1 && isCpuActive__9ftManagerCFi(g_ftManager, id) && getCpuRank__5aiMgrFi(g_aiMgr, i) != 0x46) {
                    setCpuRank__5aiMgrFiSc(g_aiMgr, i, 0x46);
                }
            }
        }
        self->unk14 -= 1;
        self->unk28 = d[6];
        self->unk2C = 0xCB;
        break;
    case 0xCB:
    L5B0:
        if (self->unk14 > 0) {
            goto L52C;
        }
        self->unk28 = d[7];
        self->unk2C = 0xCE;
        break;
    }
    return 0;
}
}

