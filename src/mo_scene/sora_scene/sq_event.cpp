#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_event.h>
#include <sr/sr_common.h>
#include <types.h>
#include <gf/gf_boot_task.h>

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
