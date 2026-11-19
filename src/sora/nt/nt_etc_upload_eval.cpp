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

// TODO: clean up, give variables real names
namespace jj_80158638 {
extern "C" {
extern int lbl_805A0460;

int getInstance__18nteUpdateEvaluatorFv(void) {
    return lbl_805A0460;
}
}
}

// TODO: clean up, give variables real names
namespace jj_80158738 {
typedef unsigned int u32;
typedef signed char s8;
typedef unsigned char u8;

struct Data {
    u32 unk0;
    int unk4;
    char pad[8];
    int unk10;
    u32 ids[4];
    u32 values[4];
};

extern void* g_GameGlobal;
extern "C" float getGameFrame__10GameGlobalCFv(void* self);
extern "C" void* getUploadSetting__9nteGlobalFv();

extern "C" void notifyEventOnDamage__18nteUpdateEvaluatorFiUl(Data* arg0, int arg1, int arg2) {
    u32 frame = (u32)getGameFrame__10GameGlobalCFv(g_GameGlobal);
    if ((u32)(arg0->unk0 + 0x10000) == 0xFFFF) {
        arg0->unk0 = frame;
        return;
    }
    char* p = (char*)getUploadSetting__9nteGlobalFv();
    u32 d = frame - arg0->unk0;
    if (d <= 0x3C) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x30];
    } else if (d <= 0x78) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x31];
    } else if (d <= 0xB4) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x32];
    } else if (d <= 0xF0) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x33];
    } else if (d <= 0x12C) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x34];
    } else if (d <= 0x1E0) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x35];
    } else if (d <= 0x258) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x36];
    } else if (d <= 0x2D0) {
        arg0->unk4 = arg0->unk4 + (s8)p[0x37];
    } else {
        arg0->unk4 = arg0->unk4 + (s8)p[0x38];
    }
    arg0->unk0 = frame;
    if (arg1 >= 0 && arg1 < 4) {
        if ((u32)(arg0->ids[arg1] + 0x10000) == 0xFFFF) {
            arg0->ids[arg1] = frame;
        } else if (frame / 600 != arg0->ids[arg1] / 600) {
            if (arg0->values[arg1] >= (u8)p[0x47]) {
                arg0->unk10 = arg0->unk10 + (s8)p[0x46];
            }
            arg0->ids[arg1] = frame;
            arg0->values[arg1] = 0;
        }
        arg0->values[arg1] = arg0->values[arg1] + arg2;
    }
}
}

