#include <cstdio>
#include <gf/gf_boot_task.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gm/gm_sel_char_data.h>
#include <sc/sc_fig_get_demo.h>
#include <sq/sq_single_allstar.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_relocate.h>

// gmAddFigure (the map name); gm_common.cpp defines it by this stand-in name.
extern "C" void fn_80052704(u16 id, int num);

// HYPOTHESIS: All-Star stage table pointers (1P and 2P)
extern void* lbl_1_bss_1998[2][19];
// HYPOTHESIS: All-Star preload buffer address
extern u8* lbl_1_bss_1A30;
// HYPOTHESIS: All-Star preload buffer size and clear character kinds
extern u32 lbl_1_bss_1A34[3];
// HYPOTHESIS: cached scene manager pointer
extern gfSceneManager* lbl_1_bss_1A40;

// HYPOTHESIS: character kind to All-Star trophy ID mapping entry
struct AllstarFigureEntry {
    s32 charKind;
    s32 figId;
};

// HYPOTHESIS: All-Star trophy ID table
struct AllstarFigureTable {
    AllstarFigureEntry entries[37];
};

extern const AllstarFigureTable lbl_1_rodata_C28;

namespace {

void setupData(u8* buf, u32 size);

}

sqSingleAllstar* sqSingleAllstar::create() {
    sqSingleAllstar* seq = new (Heaps::GlobalMode) sqSingleAllstar("sqSingleAllstar");
    if (lbl_1_bss_1A30 == nullptr) {
        gfBootTask::PreLoadDataBuffer buf = gfBootTask::getPreLoadDataBuffer(0);
        lbl_1_bss_1A30 = buf.addr;
        lbl_1_bss_1A34[0] = buf.size;
        setupData(lbl_1_bss_1A30, lbl_1_bss_1A34[0]);
    }
    return seq;
}

void sqSingleAllstar::start() {
    unk2C = gfSceneManager::getInstance()->unk31C;
    if (lbl_1_bss_1A40 == nullptr) {
        lbl_1_bss_1A40 = gfSceneManager::getInstance();
    }
    if (unk2C == 0xFF) {
        unk8 = 0x17;
        unk2C = 0;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        gmInitGlobalModeMelee(g_GameGlobal->m_modeMelee);
        g_GameGlobal->m_selCharData->unk2E_b7 = 1;
    }
}

namespace {

void setupData(u8* buf, u32 size) {
    utRelocate relocate(buf, size);
    for (int i = 0; i < 19; i++) {
        char name[64];
        std::sprintf(name, "allstarStage%dTbl", i + 1);
        lbl_1_bss_1998[0][i] = relocate.getPublicAddress(name);
    }
    for (int i = 0; i < 19; i++) {
        char name[64];
        std::sprintf(name, "allstarStage%dTbl2p", i + 1);
        lbl_1_bss_1998[1][i] = relocate.getPublicAddress(name);
    }
}

__declspec(force_export) void setAllstarGetFigure(int charKind1, int charKind2) {
    scFigureGetDemo* scene = static_cast<scFigureGetDemo*>(lbl_1_bss_1A40->searchScene("scFigureGetDemo"));
    AllstarFigureTable table = lbl_1_rodata_C28;
    s32 figId = 0;
    for (int i = 0; i < 2; i++) {
        int kind = (i == 0) ? charKind2 : charKind1;
        if (kind != -1) {
            switch (kind) {
            case 0x11:
            case 0x12:
                kind = 0x10;
                break;
            case 0x1D:
            case 0x1E:
            case 0x1F:
            case 0x20:
            case 0x21:
            case 0x22:
                kind = 0x1D;
                break;
            }
            for (int j = 0; j < 37; j++) {
                if (kind == table.entries[j].charKind) {
                    figId = table.entries[j].figId;
                    break;
                }
            }
            fn_80052704(figId, 1);
        }
    }
    scene->unkC = figId;
    for (int i = 0; i < 37; i++) {
        if (gmCheckExistFigure(table.entries[i].figId) == true) {
            s32 id = table.entries[i].figId;
            if (id != figId) {
                scene->unk10.pushBack(id);
            }
        }
    }
}

}
