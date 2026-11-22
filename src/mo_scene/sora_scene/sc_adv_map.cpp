#include <ad/ad_keep_manager.h>
#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <mu/mu_adv_difficulty.h>
#include <mu/mu_object.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <revolution/OS/OSError.h>
#include <types.h>

// The map select menu task of sora_adv_menu_sel_map.rel as the scene reads it.
// HYPOTHESIS: the names; only the decided item and the selected index are read.
struct scAdvMapSelTask : public gfTask {
    // gfTask is 0x40 bytes
    int m_decide;     // 0x40 what the player decided
    int m_unk44;
    int m_selected;   // 0x48 the selected map/stage
    u8 m_unk4C[0x5A - 0x4C];
    u8 m_memoryLayout; // 0x5A
    u8 m_unk5B;
    int m_pad;        // 0x5C the pad of the player
    u8 m_unk60[0x1BC - 0x60];
    int m_step;       // 0x1BC
    u8 m_unk1C0[8];
    MuObject* m_map;  // 0x1C8
    u8 m_unk1CC[0x4A4 - 0x1CC];
    int m_inputStep;  // 0x4A4
    u8 m_unk4A8[0x4C8 - 0x4A8];
    u8 m_isMapPoint;  // 0x4C8
};

// The menu decides an item that leaves the map (the map task goes back to its wait step).
// HYPOTHESIS: the names; the original is an inline member of the map select task.
static inline void scAdvMapResetSelMap(scAdvMapSelTask* task) {
    int decide = task->m_decide;
    bool reset;
    if (decide == 2 || decide == 3 || decide == 4 || decide == 8 || decide == 9 || (u32)(decide - 5) <= 1) {
        reset = true;
    } else {
        reset = false;
    }
    if (reset) {
        task->m_decide = 0;
        task->m_step = 10;
    }
}

// The scene parameter the map scene hands its result to (the difficulty at 0x10, the memory layout at 0x11).
// HYPOTHESIS: the names.
struct scAdvMapResult {
    u8 m_unk0[0x10];
    u8 m_difficulty;   // 0x10 0xFF: not chosen yet
    u8 m_memoryLayout; // 0x11
};

// One stage record of the adventure save data (0x14 bytes).
// HYPOTHESIS: the names.
struct scAdvMapStageRecord {
    int m_unk0;
    int m_kind;        // 0x4
    int m_difficulty;  // 0x8 the last difficulty (-1: none)
    u8 m_unkC[8];
};

// The stage select (map) scene of the adventure mode (module sora_scene).
// HYPOTHESIS: the member names; process and the select steps besides selMapProc are not written yet.
class scAdvMap : public gfScene {
public:
    scAdvMapSelTask* m_selMap;  // 0x08
    muAdvDifficultyTask* m_difficulty; // 0x0C
    gfTask* m_task10;           // 0x10
    gfTask* m_task14;           // 0x14
    gfTask* m_task18;           // 0x18
    int m_step;                 // 0x1C
    int m_selected;             // 0x20 the selected stage + 1
    int m_unk24;                // 0x24
    u8 m_unk28[0x24];
    scAdvMapResult* m_result;   // 0x4C

    scAdvMap(const char* name)
        : gfScene(name), m_selMap(NULL), m_difficulty(NULL), m_task10(NULL), m_task14(NULL), m_task18(NULL), m_step(1),
          m_selected(0), m_unk24(0), m_result(NULL) { }
    static scAdvMap* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    int selMapProc();
    int selDiffProc();
    int selCharProc(u32 mask, int kind);
    int sel1PNameProc();
    int sel2PNameProc();
    int comerProc();
};
static_assert(sizeof(scAdvMap) == 0x50, "Class is wrong size!");

scAdvMap* scAdvMap::create() {
    return new (Heaps::GlobalMode) scAdvMap("scAdvMap");
}

void scAdvMap::setModule() {
    setLoadModule("sora_adv_menu_sel_map.rel", false);
    setLoadModule("sora_adv_menu_name.rel", false);
    setLoadModule("sora_adv_menu_difficulty.rel", false);
}

int scAdvMap::selMapProc() {
    scAdvMapSelTask* task = m_selMap;
    int next = m_step;
    switch (task->m_decide) {
    case 2:
        m_selected = task->m_selected + 1;
        next = 2;
        break;
    case 3:
        next = 7;
        break;
    case 4:
        next = 8;
        break;
    case 5:
        m_selected = task->m_selected + 1;
        next = 4;
        break;
    case 6:
        m_selected = task->m_selected + 1;
        next = 6;
        break;
    case 8:
        m_selected = task->m_selected + 1;
        next = 9;
        break;
    case 9:
        next = 10;
        break;
    case 7:
        m_selected = task->m_selected + 1;
        next = 11;
        break;
    case 10:
        m_selected = task->m_selected + 1;
        next = 12;
        break;
    }
    return next;
}

s32 scAdvMap::exit() {
    if (m_selMap != NULL) {
        m_selMap->exit();
        m_selMap = NULL;
    }
    if (m_difficulty != NULL) {
        m_difficulty->exit();
        m_difficulty = NULL;
    }
    if (m_task10 != NULL) {
        m_task10->exit();
        m_task10 = NULL;
    }
    if (m_task14 != NULL) {
        m_task14->exit();
        m_task14 = NULL;
    }
    if (m_task18 != NULL) {
        m_task18->exit();
        m_task18 = NULL;
    }
    g_gfApplication->unkD0.startKeepScreen();
    return 0;
}

s32 scAdvMap::process() {
    bool isEnd = false;
    int layout;
    if (adKeepManager::getInstance()->isLoadFinishSeal()) {
        adKeepManager::getInstance()->getSealInfoFromGameGlobal();
    } else {
        return 0;
    }
    switch (m_step) {
    case 0:
        m_step = comerProc();
        break;
    case 1:
        m_step = selMapProc();
        break;
    case 2:
        m_step = selDiffProc();
        break;
    case 3: {
        int kind = 4;
        if (m_selected == 11) {
            kind = 2;
        }
        if (m_selected >= 31) {
            kind = 6;
        }
        m_step = selCharProc(0x3FF, kind);
        break;
    }
    case 4: {
        u32 mask = 0;
        int kind = 1;
        switch (m_selected) {
        case 5:
            mask = 2;
            kind = 2;
            break;
        case 14:
            mask = 8;
            kind = 2;
            break;
        case 18:
            mask = 4;
            kind = 2;
            break;
        case 19:
            mask = 0x40;
            kind = 2;
            break;
        case 27:
            mask = 4;
            kind = 4;
            break;
        }
        m_step = selCharProc(mask, kind);
        break;
    }
    case 5:
        m_step = selCharProc(0, 4);
        break;
    case 6:
        m_step = selCharProc(0, 4);
        break;
    case 7:
        m_step = sel1PNameProc();
        break;
    case 8:
        m_step = sel2PNameProc();
        break;
    case 9:
        break;
    }
    if (m_step == 10) {
        isEnd = true;
        layout = 0;
    }
    if ((u32)(m_step - 12) <= 1 && m_selMap->m_isMapPoint) {
        isEnd = true;
        layout = 1;
        OSReport("MAP POINT ID = %d
", m_selected);
    }
    if (m_step == 11) {
        isEnd = true;
        layout = 2;
    }
    if (m_step == 9) {
        isEnd = true;
        layout = 3;
    }
    if (isEnd) {
        if (m_result != NULL) {
            m_result->m_memoryLayout = m_selMap->m_memoryLayout;
        }
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = layout;
        manager->processStep = 2;
    }
    return 0;
}

// TODO: clean up - 86%: the 30..33 test is a compare chain in the target (ours becomes a range test) and the
// saved registers of this/adv are swapped.
int scAdvMap::selDiffProc() {
    int next = m_step;
    bool isEnd = false;
    gmAdvData* adv = g_GameGlobal->m_advData;
    int index = m_selMap->m_selected;
    scAdvMapStageRecord* records = reinterpret_cast<scAdvMapStageRecord*>(adv);
    if (m_difficulty == NULL) {
        u8 chosen = m_result->m_difficulty;
        int difficulty = records[index].m_difficulty;
        if (chosen == 0xFF) {
            if (difficulty < 0) {
                difficulty = adv->unk5FC;
            } else {
                difficulty++;
            }
        } else {
            difficulty = chosen;
        }
        if (difficulty < 0) {
            difficulty = 0;
        }
        if (difficulty > 4) {
            difficulty = 4;
        }
        muAdvDifficultyTaskParam param = { 0, 0, -1 };
        param.m_difficulty = difficulty;
        param.m_pad0 = m_selMap->m_pad;
        m_difficulty = muAdvDifficultyTask::create(&param);
    }
    switch (m_difficulty->m_result) {
    case 1:
        isEnd = true;
        if (m_result != NULL) {
            m_result->m_difficulty = m_difficulty->m_difficulty;
        }
        switch (records[index].m_kind) {
        case 3:
        case 4:
            next = 3;
            break;
        case 2:
            if (index == 4 || index == 13 || index == 17 || index == 18 || index == 26) {
                next = 4;
            } else if (index == 30 || index == 31 || index == 32 || index == 33) {
                next = 6;
            } else {
                scAdvMapResetSelMap(m_selMap);
                m_selMap->m_inputStep = 3;
                next = 13;
            }
            break;
        default:
            scAdvMapResetSelMap(m_selMap);
            m_selMap->m_inputStep = 3;
            next = 13;
            break;
        }
        break;
    case 2: {
        m_result->m_difficulty = 0xFF;
        scAdvMapResetSelMap(m_selMap);
        MuObject* map = m_selMap->m_map;
        map->changeAnimN("MenAdvMap0001_TopN__0");
        map->setFrameNode(9.0f);
        map->setFrameMatCol(9.0f);
        next = 1;
        isEnd = true;
        break;
    }
    }
    if (isEnd) {
        m_difficulty->exit();
        m_difficulty = NULL;
    }
    return next;
}
