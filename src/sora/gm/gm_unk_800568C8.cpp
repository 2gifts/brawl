#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_hide_lib.h>
#include <types.h>

namespace {
void gmCheckClearGetCondition();
void gmCheckInfoCondition();
void gmCheckRareUP();
}

extern const struct {
    struct {
        u32 unk0;
        int unk4;
    } unk0[14];
} lbl_80406E28;

int gmCheckChallengerToVsMelee() {
    u32* flags = g_GameGlobal->getCharaAppFlag();
    u32 count = gmGetOfflineVsMeleeTotalCount();
    __typeof__(lbl_80406E28) entries = lbl_80406E28;
    for (int i = 13; i >= 0; --i) {
        if (count >= entries.unk0[i].unk0) {
            int kind = entries.unk0[i].unk4;
            int bit = kind * 3 + 1;
            if (!(flags[bit / 32] & (1 << (bit % 32)))) {
                bit = kind * 3;
                flags[bit / 32] |= 1 << (bit % 32);
                return kind;
            }
        }
    }
    return -1;
}

void gmCheckHideCondition() {
    gmCheckSoftList();
    gmCheckClearGetCondition();
    gmCheckInfoCondition();
    gmCheckRareUP();
}
