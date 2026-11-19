#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <sr/sr_common.h>
#include <types.h>

// The rule menu of the online match (sora_menu_qm.rel). Only what the scene reads is declared.
class muQueueMeleeRuleTask : public gfTask {
public:
    u8 m_unk40[8];
    int m_result;                       // 0x48 how the menu was left (1: on to the entry, 2: back)
    u8 m_unk4C[0x3D8 - 0x4C];

    muQueueMeleeRuleTask(bool resume);
};

// One seat of the tournament of the game data (the part the scene writes).
struct scQMSeat {
    u8 m_kind;                          // 0x00 0: a player's
    u8 m_unk01[0x1F];
    int m_order;                        // 0x20 the place the seat plays in
    u8 m_unk24[4];
};
static_assert(sizeof(scQMSeat) == 0x28, "Class is wrong size!");

struct scQMTournament {
    u8 m_count;                         // 0x00 how many seats are used
    u8 m_unk01[7];
    scQMSeat m_seats[16];               // 0x08
};

// The scene of the rules of the online match (module sora_scene): runs the rule menu of sora_menu_qm.rel; when it is left
// to go on, the places of the seats of the tournament are drawn.
class scQMSetting : public gfScene {
public:
    muQueueMeleeRuleTask* m_task;       // 0x08

    scQMSetting(const char* name) : gfScene(name) { }
    static scQMSetting* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scQMSetting) == 0xC, "Class is wrong size!");

scQMSetting* scQMSetting::create() {
    return new (Heaps::GlobalMode) scQMSetting("scQMSetting");
}

void scQMSetting::start() {
    if (gfSceneManager::getInstance()->m_memoryLayout == 0) {
        g_GameGlobal->initTournamentData();
        muQueueMeleeRuleTask* task = new (Heaps::MenuInstance) muQueueMeleeRuleTask(true);
        task->init();
        m_task = task;
    } else {
        muQueueMeleeRuleTask* task = new (Heaps::MenuInstance) muQueueMeleeRuleTask(false);
        task->init();
        m_task = task;
    }
}

s32 scQMSetting::process() {
    switch (m_task->m_result) {
    case 1: {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 1;
        mgr->processStep = 2;
        scQMTournament* data = reinterpret_cast<scQMTournament*>(g_GameGlobal->m_tournamentData);
        int count = data->m_count;
        int order[16];
        int i;
        for (i = 0; i < 16; i++) {
            data->m_seats[i].m_order = -1;
            order[i] = -1;
        }
        for (i = 0; i < count; i++) {
            int r;
            do {
                r = randi(count);
            } while (order[r] != -1);
            order[r] = i;
        }
        int* next = order;
        scQMSeat* seat = data->m_seats;
        for (i = 0; i < 16; i++) {
            if (seat->m_kind == 0) {
                seat->m_order = *next++;
            }
            seat++;
        }
        break;
    }
    case 2: {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
        break;
    }
    }
    return 0;
}

s32 scQMSetting::exit() {
    m_task->exit();
    m_task = NULL;
    return 0;
}

void scQMSetting::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_tour.rel", false);
    setLoadModule("sora_menu_qm.rel", false);
}
