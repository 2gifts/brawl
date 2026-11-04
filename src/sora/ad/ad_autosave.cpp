#include <types.h>
#include <ad/ad_autosave.h>
#include <gf/gf_sys_recorder2.h>
#include <gm/gm_global.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
adAutoSave* lbl_805A0468[2] __attribute__((aligned(4))) = { 0 };

adAutoSave::~adAutoSave() {
    lbl_805A0468[0] = 0;
}

void adAutoSave::processDefault() {
    switch (unk40) {
    case 0: {
        if (!gfSysRecorder2::isEnable(0)) {
            unk40 = 6;
            break;
        }
        unk4C = gfSysRecorder2::createInstance(Heaps::Tmp);
        if (!unk4C) {
            unk40 = 6;
            break;
        }
        void* buffer = unk4C->getBuffer(0);
        if (!buffer) {
            unk40 = 6;
            break;
        }
        g_GameGlobal->getSaveData(buffer);
        if (!unk4C->saveSystem()) {
            unk40 = 6;
            break;
        }
        unk40 = 1;
        unk44 = 0;
        break;
    }
    case 1:
        ++unk44;
        if (!unk4C->m_busy) {
            unk40 = 2;
        }
        break;
    case 2:
        if (!unk4C->m_result) {
            if (unk48) {
                unk40 = 6;
            } else {
                unk40 = 3;
            }
        } else {
            unk40 = 6;
        }
        break;
    case 3: {
        if (!gfSysRecorder2::isEnable(1)) {
            unk40 = 6;
            break;
        }
        void* buffer = unk4C->getBuffer(1);
        if (!buffer) {
            unk40 = 6;
            break;
        }
        g_GameGlobal->getAdventureSaveData(buffer);
        if (!unk4C->saveAdventure(0)) {
            unk40 = 6;
            break;
        }
        unk40 = 4;
        unk44 = 0;
        break;
    }
    case 4:
        ++unk44;
        if (!unk4C->m_busy) {
            unk40 = 5;
        }
        break;
    case 5:
        unk40 = 6;
        break;
    case 6:
        if (unk4C) {
            gfSysRecorder2::dropInstance();
            unk4C = 0;
        }
        exit();
        lbl_805A0468[0] = 0;
        break;
    }
}

