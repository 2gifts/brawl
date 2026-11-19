#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// One seat of the tournament of the game data.
// HYPOTHESIS: the names follow how the scene fills the seats.
struct scQMEntrySeat {
    u8 m_kind;                          // 0x00 0: a player's
    u8 m_unk01;
    char m_name[0x15];                  // 0x02 the name that is shown
    u8 m_tag;                           // 0x17 the number of the saved name (0x78: none)
    u8 m_number;                        // 0x18 the number the name was given
    u8 m_unk19;
    u8 m_character;                     // 0x1A the fighter of the seat
    u8 m_unk1B;
    u8 m_costume;                       // 0x1C
    u8 m_unk1D;
    u16 m_unk1E;
    int m_order;                        // 0x20 the place of the seat (in the list of the selection of the fighters)
    u8 m_unk24[4];
};
static_assert(sizeof(scQMEntrySeat) == 0x28, "Class is wrong size!");

struct scQMEntryTournament {
    u8 m_count;                         // 0x00 how many seats are used
    u8 m_playing;                       // 0x01 how many play
    u8 m_unk02;
    u8 m_limit;                         // 0x03
    u8 m_unk04[4];
    scQMEntrySeat m_seats[16];          // 0x08
};

// What the scene hands to the entry menu for one seat: the name that is shown, the number of the tag and the color.
struct scQMEntrySlot {
    int m_tag;                          // 0x00 the number of the saved name
    char m_name[0x18];                  // 0x04 the name that is shown
    int m_order;                        // 0x1C
    int m_character;                    // 0x20 the fighter of the seat
    u8 m_costume;                       // 0x24
    u8 m_number;                        // 0x25
    u8 m_unk26[2];
};
static_assert(sizeof(scQMEntrySlot) == 0x28, "Class is wrong size!");

// What the selection of the fighters keeps of one player (the result record of the game data).
struct scQMEntryResult {
    u8 m_unk00[0x32];
    u8 m_rank;                          // 0x32 the place the player had
    u8 m_nextRank;                      // 0x33 the place for the next round
    u8 m_unk34[0x2AC - 0x34];
};

// The entry menu of the online match (sora_menu_qm.rel). Only what the scene needs is declared.
class muQueueMeleeEntryTask : public gfTask {
public:
    u8 m_unk40[8];
    int m_result;                       // 0x48 how the menu was left (1: on, 2: back)
    u8 m_unk4C[0x13A4 - 0x4C];

    muQueueMeleeEntryTask(const void* slots, const int* order, int count, int playing);
};

// The scene of the entry of the online match (module sora_scene): runs the entry menu of sora_menu_qm.rel.
class scQMEntry : public gfScene {
public:
    muQueueMeleeEntryTask* m_task;      // 0x08

    scQMEntry(const char* name) : gfScene(name) { }
    static scQMEntry* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scQMEntry) == 0xC, "Class is wrong size!");

scQMEntry* scQMEntry::create() {
    return new (Heaps::GlobalMode) scQMEntry("scQMEntry");
}

// Fills the seats for the entry menu from the tournament of the game data and starts the menu.
// HYPOTHESIS: the code is written from the disassembly and is not matched yet (the order of the seats is computed with
// loops that the compiler unrolls in a way that is not reproduced).
void scQMEntry::start() {
    scQMEntryTournament* data = reinterpret_cast<scQMEntryTournament*>(g_GameGlobal->m_tournamentData);
    int layout = gfSceneManager::getInstance()->m_memoryLayout;
    scQMEntrySlot slots[16];
    int order[4];
    if (data->m_count > 16 || data->m_count == 0) {
        data->m_count = 16;
        data->m_playing = 7;
    }
    for (u32 i = 0; i < 16; i++) {
        scQMEntrySeat* seat = &data->m_seats[i];
        slots[i].m_order = seat->m_order;
        slots[i].m_number = seat->m_number;
        if (layout == 0) {
            seat->m_character = 0x28;
            seat->m_number = i + 1;
            slots[i].m_number = i + 1;
        }
        int player = seat->m_order;
        if (layout != 0 && player < data->m_playing && player >= 0) {
            u8* sel = reinterpret_cast<u8*>(g_GameGlobal->m_selCharData);
            seat->m_character = sel[player * 4 + 0xA];
            seat->m_costume = sel[player * 0x5C + 0xBD];
            u8 tag = sel[player * 0x5C + 0xD0];
            if (seat->m_tag != tag && tag != 0x78) {
                seat->m_tag = tag;
                g_GameGlobal->getName(tag, seat->m_name);
            }
        }
        slots[i].m_tag = seat->m_tag;
        sprintf(slots[i].m_name, "%s", seat->m_name);
        slots[i].m_character = seat->m_character;
        slots[i].m_costume = seat->m_costume;
    }
    int playing = data->m_playing;
    switch (layout) {
    case 0:
        order[0] = order[1] = order[2] = order[3] = 0;
        break;
    case 1: {
        int split = data->m_count - playing;
        if (static_cast<u32>(split) >= data->m_limit) {
            split = data->m_limit;
        }
        scQMEntryResult* results = reinterpret_cast<scQMEntryResult*>(g_GameGlobal->m_resultInfo);
        for (u8 i = 0; i < playing; i++) {
            results[i].m_rank = results[i].m_nextRank;
        }
        int i;
        if (data->m_unk02 == 0) {
            split = playing - split;
            for (i = 0; i < playing; i++) {
                order[i] = split > results[i].m_rank ? 1 : 2;
            }
        } else {
            for (i = 0; i < playing; i++) {
                order[i] = static_cast<u32>(split) > results[i].m_rank ? 2 : 1;
            }
        }
        for (; i < 4; i++) {
            order[i] = 0;
        }
        break;
    }
    case 2:
    case 3:
    case 4:
    case 5:
        order[0] = order[1] = order[2] = order[3] = 1;
        order[layout - 2] = 3;
        break;
    }
    muQueueMeleeEntryTask* task =
        new (Heaps::MenuInstance) muQueueMeleeEntryTask(slots, order, data->m_count, data->m_playing);
    task->init();
    m_task = task;
}

s32 scQMEntry::process() {
    switch (m_task->m_result) {
    case 1: {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 1;
        mgr->processStep = 2;
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

s32 scQMEntry::exit() {
    m_task->exit();
    m_task = NULL;
    return 0;
}

void scQMEntry::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_tour.rel", false);
    setLoadModule("sora_menu_qm.rel", false);
}
