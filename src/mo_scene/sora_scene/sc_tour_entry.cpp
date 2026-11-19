#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/mu_tour_entry.h>
#include <mu/mu_tour_table.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <types.h>

// The names that the guests of a new tournament are given (Japanese "Guest 01" ... "Guest 32", UTF-8).
static char* s_guestNames[32] = {
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x91",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x92",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x93",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x94",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x95",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x96",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x97",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x98",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x90" "\xEF\xBC\x99",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x90",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x91",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x92",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x93",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x94",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x95",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x96",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x97",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x98",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x91" "\xEF\xBC\x99",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x90",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x91",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x92",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x93",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x94",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x95",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x96",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x97",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x98",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x92" "\xEF\xBC\x99",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x93" "\xEF\xBC\x90",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x93" "\xEF\xBC\x91",
    "\xE3\x82\xB2\xE3\x82\xB9\xE3\x83\x88" "\xEF\xBC\x93" "\xEF\xBC\x92",
};

// One seat of the tournament of the game data (the part the scene fills).
// HYPOTHESIS: the names follow how the scene fills the seats.
struct scTourEntrySeat {
    u8 m_kind;                          // 0x00 0: a player's
    u8 m_unk01;
    char m_name[0x15];                  // 0x02 the name that is shown
    u8 m_tag;                           // 0x17 the number of the saved name (0x78: none)
    u8 m_unk18[2];
    u8 m_character;                     // 0x1A the fighter of the seat
    u8 m_unk1B;
    u8 m_costume;                       // 0x1C
    u8 m_unk1D;
    u16 m_unk1E;
    int m_order;                        // 0x20 the place of the seat
    u8 m_unk24[4];
};
static_assert(sizeof(scTourEntrySeat) == 0x28, "Class is wrong size!");

struct scTourEntryTournament {
    u8 m_count;                         // 0x00 how many seats are used
    u8 m_playing;                       // 0x01 how many play
    u8 m_unk02;
    u8 m_limit;                         // 0x03
    u8 m_unk04[4];
    scTourEntrySeat m_seats[32];        // 0x08
};

// The scene of the match screen of the tournament (module sora_scene): runs the match screen of sora_menu_tour.rel and keeps
// the table of the tournament.
class scTourEntry : public gfScene {
public:
    muTourEntryTask* m_task;            // 0x08
    muTourTable* m_table;               // 0x0C
    u8 m_unk10[0x5A0 - 0x10];

    scTourEntry();
    static scTourEntry* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scTourEntry) == 0x5A0, "Class is wrong size!");

scTourEntry* scTourEntry::create() {
    return new (Heaps::GlobalMode) scTourEntry();
}

// Starts a new tournament (HYPOTHESIS: of 4 players in 2 rounds) with the 32 guests that are not named yet.
scTourEntry::scTourEntry() : gfScene("scTourEntry") {
    m_table = NULL;
    scTourEntryTournament* data = reinterpret_cast<scTourEntryTournament*>(g_GameGlobal->m_tournamentData);
    data->m_count = 4;
    data->m_playing = 2;
    data->m_unk02 = 0;
    data->m_limit = 3;
    data->m_unk04[0] = 0;
    data->m_unk04[1] = 0;
    data->m_unk04[2] = 0;
    for (int i = 0; i < 32; i++) {
        scTourEntrySeat* seat = &data->m_seats[i];
        seat->m_kind = 0;
        sprintf(seat->m_name, s_guestNames[i]);
        seat->m_tag = 0x78;
        seat->m_character = 0;
        seat->m_costume = 0;
        seat->m_unk1E = 0;
        seat->m_order = i;
    }
}

s32 scTourEntry::exit() {
    m_task->exit();
    m_task = NULL;
    delete m_table;
    m_table = NULL;
    return 0;
}

void scTourEntry::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_tour.rel", false);
}
