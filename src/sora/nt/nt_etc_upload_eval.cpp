#include <gm/gm_global.h>
#include <nt/nt_etc_global.h>
#include <nt/nt_etc_upload_eval.h>
#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
nteUpdateEvaluator* lbl_805A0460[2] = { 0 };

nteUpdateEvaluator* nteUpdateEvaluator::createInstance() {
    if (lbl_805A0460[0] == NULL) {
        lbl_805A0460[0] = new (Heaps::System) nteUpdateEvaluator;
    }
    return lbl_805A0460[0];
}

void nteUpdateEvaluator::dropInstance() {
    if (lbl_805A0460[0] != NULL) {
        delete lbl_805A0460[0];
        lbl_805A0460[0] = NULL;
    }
}

void nteUpdateEvaluator::reset() {
    unk0 = -1;
    unk4 = 0;
    unk8 = -1;
    unkC = 0;
    unk10 = 0;
    for (int player = 0; player < 4; ++player) {
        unk14[player] = -1;
        unk24[player] = 0;
    }
}

void nteUpdateEvaluator::DamagePostProcess() {
    u8* setting = nteGlobal::getUploadSetting();
    for (int player = 0; player < 4; ++player) {
        if (unk14[player] != -1 && unk24[player] >= setting[0x47]) {
            unk10 += static_cast<s8>(setting[0x46]);
        }
    }
}

void nteUpdateEvaluator::notifyEventBeat(int playerNo1, int playerNo2) {
    u32 frame = g_GameGlobal->getGameFrame();
    if (unk8 == -1) {
        unk8 = frame;
    } else {
        u8* setting = nteGlobal::getUploadSetting();
        u32 elapsed = frame - unk8;
        if (elapsed <= 600) {
            unkC += static_cast<s8>(setting[0x39]);
        } else if (elapsed <= 900) {
            unkC += static_cast<s8>(setting[0x3A]);
        } else if (elapsed <= 1200) {
            unkC += static_cast<s8>(setting[0x3B]);
        } else if (elapsed <= 1500) {
            unkC += static_cast<s8>(setting[0x3C]);
        } else if (elapsed <= 1800) {
            unkC += static_cast<s8>(setting[0x3D]);
        } else if (elapsed <= 2100) {
            unkC += static_cast<s8>(setting[0x3E]);
        } else if (elapsed <= 2400) {
            unkC += static_cast<s8>(setting[0x3F]);
        } else {
            unkC += static_cast<s8>(setting[0x40]);
        }
        unk8 = frame;
    }
}
