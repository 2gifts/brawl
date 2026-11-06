#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_event.h>
#include <sr/sr_common.h>
#include <types.h>
#include <gf/gf_boot_task.h>
#include <sc/sc_sel_event.h>

// HYPOTHESIS: the layout of an event of the table follows how the fighters are read from it.
struct EventFighterEntry {
    u8 m_kind;
    u8 m_pad01[0x12];
    u8 m_costume[0x25];
};
struct EventEntry {
    u32 m_unk00;
    u8 m_pad04[0x1A];
    u16 m_unk1E;
    u8 m_unk20;
    u8 m_count : 3;
    u8 m_unk21 : 5;
    u8 m_pad22[0x2E];
    EventFighterEntry m_fighters[4];
};

extern s8 lbl_1_data_8000[0x34];
extern s8 lbl_1_data_8034[0x34];
static EventEntry* lbl_1_bss_1A64[0x29];
static EventEntry* lbl_1_bss_1B08[0x15];

static u8* lbl_1_bss_1B5C;
static u32 lbl_1_bss_1B60[2];

namespace {
void setupData(u8* data, u32 size);
}

sqEvent* sqEvent::create() {
    sqEvent* sequence = new (Heaps::GlobalMode) sqEvent("sqEvent");
    if (lbl_1_bss_1B5C == 0) {
        gfBootTask::PreLoadDataBuffer buffer = gfBootTask::getPreLoadDataBuffer(2);
        u8* data = buffer.addr;
        u32 size = buffer.size;
        lbl_1_bss_1B5C = data;
        lbl_1_bss_1B60[0] = size;
        setupData(data, size);
    }
    return sequence;
}


void sqEvent::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xa;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        unk18 = g_gfApplication->unk110[0] != 0;
    }
}

void sqEvent::exit() { }

void sqEvent::checkEventSetting(int eventNo, int level, int playerCount, scEventFighters* fighters) {
    EventEntry* entry;
    if (playerCount == 1) {
        entry = lbl_1_bss_1A64[lbl_1_data_8000[eventNo]];
    } else {
        entry = lbl_1_bss_1B08[lbl_1_data_8034[eventNo]];
    }
    fighters->m_unk00 = entry->m_unk00;
    fighters->m_unk04 = entry->m_unk1E;
    fighters->m_unk06 = entry->m_unk20;
    fighters->m_count = entry->m_count;
    for (int i = 0; i < 4; i++) {
        if (i < entry->m_count) {
            fighters->m_fighters[i].m_kind = entry->m_fighters[i].m_kind;
            fighters->m_fighters[i].m_costume = entry->m_fighters[i].m_costume[level * 14];
        } else {
            fighters->m_fighters[i].m_kind = 0x3E;
            fighters->m_fighters[i].m_costume = 0;
        }
    }
}
