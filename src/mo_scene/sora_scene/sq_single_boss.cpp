#include <ad/ad_keep_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <revolution/OS/OSTime.h>
#include <sq/sq_single_boss.h>
#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: muMenu::exchangeGmCharacterKindToMuCharKind is called with one argument; the mangled name has three.
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);

sqSingleBoss* sqSingleBoss::create() {
    return new (Heaps::GlobalMode) sqSingleBoss("sqSingleBoss");
}

void sqSingleBoss::setScore(int chara, int flag, u32 score) {
    u8 kind = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(chara);
    if (kind >= 0x23) {
        return;
    }
    gmGlobalRecord::CharaNumData* data = g_GameGlobal->getGlobalRecordCharaNumDatap(kind);
    if (data->unk9C_15 == 0) {
        if (flag != 0) {
            data->unk9C_15 = 1;
            data->unkA8 = score;
        } else if (data->unkA8 < score) {
            data->unkA8 = score;
        }
    } else if (flag != 0 && data->unkA8 > score) {
        data->unkA8 = score;
    }
    if (flag != 0) {
        int level = unk11 + 1;
        if (data->unk9C_4 < level) {
            data->unk9C_4 = level;
        }
        checkClearGet();
    }
}

void sqSingleBoss::setScore2Play(int chara1, int p2, int chara2, int p4, int flag, u32 score) {
    gmGlobalRecord* rec = g_GameGlobal->m_record;
    u8 kind1 = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(chara1);
    u8 kind2 = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(chara2);
    int updated = 0;
    if (rec->unk34D4_15 == 0) {
        if (flag != 0) {
            rec->unk34D4_15 = 1;
            updated = 1;
        } else if (rec->unk34DC < score) {
            updated = 1;
        }
    } else if (flag != 0 && rec->unk34DC > score) {
        updated = 1;
    }
    if (flag != 0) {
        int level = unk11 + 1;
        if (rec->unk34D4_7 < level) {
            rec->unk34D4_7 = level;
        }
        checkClearGet();
    }
    if (updated != 0) {
        rec->unk34DC = score;
        rec->unk3528 = kind1;
        rec->unk352A = kind2;
        rec->unk3529 = p2;
        rec->unk352B = p4;
        rec->unk34D4_0 = 1;
    }
}

void sqSingleBoss::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xFF) {
        unk8 = 0x10;
    } else {
        g_GameGlobal->m_advData->m_saveRequest = 0;
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        g_GameGlobal->m_selCharData->unk2E_b7 = 1;
        u32 pad = gmGetMenuDecisionPad();
        if (g_GameGlobal->m_advData != 0) {
            g_GameGlobal->m_advData->m_padInfo[0].m_flag.m_isSet = false;
            g_GameGlobal->m_advData->m_padInfo[0].m_padNo = pad;
        }
        if (g_GameGlobal->m_advData != 0) {
            g_GameGlobal->m_advData->m_padInfo[1].m_flag.m_isSet = false;
            g_GameGlobal->m_advData->m_padInfo[1].m_padNo = -1;
        }
        srandi(OSGetTick());
    }
}

void sqSingleBoss::exit() {
    g_GameGlobal->m_advData->m_flags.m_unk0 = false;
    adKeepManager::getInstance()->useBossBattleEnd();
}

void sqSingleBoss::checkClearGet() {
    int ids[5] = {36, 90, 23, 103, 0};
    for (int i = 0; i < 5; ++i) {
        int id = ids[i];
        if (!isClearGetterOpen(id)) {
            bool open = false;
            switch (id) {
            case 36:
                if (unk11 == 0) open = true;
                break;
            case 90:
                if (unk11 == 1) open = true;
                break;
            case 23:
                if (unk11 == 2) open = true;
                break;
            case 103:
                if (unk11 == 3) open = true;
                break;
            case 0:
                if (unk11 == 4) open = true;
                break;
            }
            if (open) setClearGetterOpen(id);
        }
    }
}

void sqSingleBoss::makeStageTable() {
    u8 candidates[9];
    for (int i = 0; i < 9; ++i) {
        u8* next = candidates;
        int count = 0;
        for (int candidate = 0; candidate < 9; ++candidate) {
            bool used = false;
            for (int j = 0; j < i; ++j) {
                if (candidate == unk12[j]) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                *next++ = candidate;
                ++count;
            }
        }
        unk12[i] = candidates[randi(count * 1000) % count];
    }
    for (int i = 0; i < 9; ++i) {
        if (unk12[i] > 3) ++unk12[i];
        if (unk12[i] == 3 && randi(99) % 2 != 0) ++unk12[i];
    }
    unk12[9] = 10;
}

#include <string.h>
// TODO: clean up, give variables real names
namespace jj_2A038 {

struct Mode {
    u8 pad0[8];
    u8 gameMode : 6;
    u8 low : 2;
    u8 pad9[0x11];
    u16 stage;
    u8 pad1c[4];
    u32 value;
    u8 pad24[0xc];
    u32 w30;
    u32 w34;
    u8 pad38[0x61];
    u8 b99;
    u8 pad9a[7];
    s8 a1;
    u8 pada2[0x53];
    u8 bF5;
    u8 padf6[7];
    s8 fd;
    u8 padfe[0x53];
    u8 b151;
    u8 pad152[0x5b];
    u8 b1ad;
};
struct Adv {
    u8 pad0[0x5f8];
    u16 stage;
    u8 pad5fa[9];
    u8 mode;
    u8 b604;
    u8 b605;
    u8 b606_7 : 1;
    u8 b606_6 : 1;
    u8 b606_5 : 1;
    u8 rest606 : 5;
    u8 b607;
    u8 pad608[0x14];
    u32 value;
    u32 a620;
    u32 a624;
    u32 a628;
    u32 a62c;
    u8 a630[0x20];
    u8 pad650[0x42c8];
    u32 w4918;
    u32 w491c;
    u8 pad4920[4];
    u8 b4924;
};
struct Other {
    u8 pad0[0xb9];
    u8 b0;
    u8 pad1[0x5b];
    u8 b1;
    u8 pad2[0x5b];
    u8 b2;
    u8 pad3[0x5b];
    u8 b3;
};
struct Global {
    u8 pad0[8];
    Mode* mode;
    u8 padc[4];
    Other* other;
    u8 pad14[0x1c];
    Adv* adv;
};
struct Scene {
    u8 pad0[0x12];
    u8 list[10];
    int count;
    u32 value;
};
extern Global* g_GameGlobal;
extern "C" void* getInstance__13adKeepManagerFv();
extern "C" void useBossBattleStart__13adKeepManagerFii(void*, int, int);
extern "C" void setGlobalFlag__13adKeepManagerFUlb(void*, int, int);
extern "C" void gmGetItemSwitchData_NormalStage__Fi(u16);

extern "C" void fn_1_2A038(Scene* self) {
    Mode* mode = g_GameGlobal->mode;
    Adv* adv = g_GameGlobal->adv;
    mode->gameMode = 6;
    mode->stage = 0x3d;
    adv->stage = 0x3d;
    adv->b604 = 2;
    adv->a62c = 0;
    adv->a628 = 0;
    adv->mode = 6;
    adv->b606_6 = 0;
    if (adv->b606_7) {
        adv->b606_5 = 0;
    } else {
        adv->b606_5 = 1;
    }
    adv->b607 |= 0x30;
    mode->value = self->value;
    adv->value = self->value;
    adv->b4924 |= 4;
    gmGetItemSwitchData_NormalStage__Fi(mode->stage);
    mode->w34 = 0;
    mode->w30 = 0;
    adv->w491c = 0;
    adv->w4918 = 0;
    memset(adv->a630, 0, 0x20);
    Other* o = g_GameGlobal->other;
    mode->b99 = o->b0;
    mode->bF5 = o->b1;
    mode->b151 = o->b2;
    mode->b1ad = o->b3;
    int id;
    int count = self->count;
    u32 value = self->list[count];
    id = value * 2 + 0x100;
    if (value > 3) id = (u8)value * 2 + 0xfe;
    useBossBattleStart__13adKeepManagerFii(getInstance__13adKeepManagerFv(), count, id);
    if (adv->b606_7) setGlobalFlag__13adKeepManagerFUlb(getInstance__13adKeepManagerFv(), 0x11d, 1);
    if (self->count < 10) {
        switch (self->list[self->count]) {
        default: adv->a620 = 0x5c000100; break;
        case 1: adv->a620 = 0x5c010100; break;
        case 2: adv->a620 = 0x5c020100; break;
        case 3: adv->a620 = 0x5c030100; break;
        case 4: adv->a620 = 0x5c040100; break;
        case 5: adv->a620 = 0x5c050100; break;
        case 6: adv->a620 = 0x5c060100; break;
        case 7: adv->a620 = 0x5c070100; break;
        case 8: adv->a620 = 0x5a010100; break;
        case 9: adv->a620 = 0x5a020100; break;
        case 10: adv->a620 = 0x5c080100; break;
        }
    }
    u32 c = self->list[self->count];
    if (c == 7 || c == 9) {
        mode->a1 = -1;
        mode->fd = -1;
    } else {
        mode->a1 = 1;
        mode->fd = 1;
    }
    adv->a624 = adv->a620;
}
}

// TODO: clean up, give variables real names
namespace jj_2A2CC {

struct Mode {
    u8 pad0[8];
    u8 gameMode : 6;
    u8 low : 2;
    u8 pad9[0x11];
    u16 stage;
    u8 pad1c[4];
    u32 value;
    u8 pad24[0xc];
    u64 items;
    u8 pad38[0x69];
    bool a1;
    u8 pada2[0x5b];
    bool fd;
};
struct Adv {
    u8 pad0[0x5f8];
    u16 stage;
    u8 pad5fa[9];
    u8 mode;
    u8 b604;
    u8 b605;
    u8 b606 : 1;
    u8 rest606 : 7;
    u8 pad607[0x15];
    u32 value;
    u32 a620;
    u32 a624;
    u32 a628;
    u32 a62c;
    u8 a630[0x20];
    u8 pad650[0x42d4];
    u8 unusedFlags : 5;
    u8 flag : 1;
    u8 lowFlags : 2;
};
struct Global {
    u8 pad0[8];
    Mode* mode;
    u8 padc[0x24];
    Adv* adv;
};
struct Scene {
    u8 pad0[0x12];
    u8 list[10];
    int count;
    u32 value;
};
extern Global* g_GameGlobal;
extern "C" void* getInstance__13adKeepManagerFv();
extern "C" void useBossBattleStart__13adKeepManagerFii(void*, int, int);
extern "C" void setGlobalFlag__13adKeepManagerFUlb(void*, int, int);

extern "C" void fn_1_2A2CC(Scene* self) {
    Mode* mode = g_GameGlobal->mode;
    Adv* adv = g_GameGlobal->adv;
    mode->gameMode = 6;
    mode->stage = 0x3d;
    adv->stage = 0x3d;
    adv->b604 = 0;
    adv->a62c = 0;
    adv->a628 = 0;
    adv->mode = 6;
    memset(adv->a630, 0, 0x20);
    int id;
    int count = self->count;
    u32 value = self->list[count];
    id = value * 2 + 0x100;
    if (value > 3) id = (u8)value * 2 + 0xfe;
    useBossBattleStart__13adKeepManagerFii(getInstance__13adKeepManagerFv(), count, id);
    for (id = 0; id < self->count; id++) {
        u8 value = self->list[id];
        if (value > 3) {
            setGlobalFlag__13adKeepManagerFUlb(getInstance__13adKeepManagerFv(), (u8)value * 2 + 0xff, 1);
        } else {
            setGlobalFlag__13adKeepManagerFUlb(getInstance__13adKeepManagerFv(), (u8)value * 2 + 0x101, 1);
        }
    }
    if (adv->b606) setGlobalFlag__13adKeepManagerFUlb(getInstance__13adKeepManagerFv(), 0x11d, 1);
    adv->a620 = 0x5b010100;
    adv->a624 = 0x5b010100;
    mode->value = self->value;
    adv->value = self->value;
    adv->flag = 1;
    mode->a1 = 1;
    mode->fd = 1;
    struct ItemBits {
        u32 unused : 13;
        u32 trophy : 1;
        u32 rest : 18;
        u32 unused2 : 23;
        u32 food : 1;
        u32 rest2 : 8;
    };
    u64 items = 0;
    ((ItemBits*)&items)->food = 1;
    ((ItemBits*)&items)->trophy = 1;
    mode->items = items;
}
}

// TODO: clean up, give variables real names
namespace jj_2AB94 {
extern "C" {
extern int* getInfoAppFlag__10GameGlobalFv(void*);
extern int* getCharaAppFlag__10GameGlobalFv(void*);
extern void* g_GameGlobal;

int fn_1_2AB94(int arg0) {
    int result = 0;

    int* p = getInfoAppFlag__10GameGlobalFv(g_GameGlobal);
    if (!(p[5] & 1)) {
        int* p2 = getInfoAppFlag__10GameGlobalFv(g_GameGlobal);
        p2[5] |= 1;
    }

    p = getInfoAppFlag__10GameGlobalFv(g_GameGlobal);
    if ((p[5] & 0x04000000) == 0 && arg0 >= 4) {
        int* p2 = getInfoAppFlag__10GameGlobalFv(g_GameGlobal);
        p2[5] |= 0x04000000;
    }

    p = getCharaAppFlag__10GameGlobalFv(g_GameGlobal);
    if ((p[1] & 0x100) == 0) {
        char* gg = (char*)g_GameGlobal;
        char* obj1 = *(char**)(gg + 0x30);
        char* obj2 = *(char**)(gg + 0x8);

        if (((*(unsigned char*)(obj1 + 0x606) >> 7) & 1) &&
            (*(unsigned char*)(obj2 + 0xf4) == 7 ||
             *(unsigned char*)(obj2 + 0xf4) == 0x15)) {
            result = 1;
        }

        if (*(unsigned char*)(obj2 + 0x98) == 7 ||
            *(unsigned char*)(obj2 + 0x98) == 0x15) {
            result = 1;
        }

        if (result) {
            int* p2 = getCharaAppFlag__10GameGlobalFv(gg);
            p2[1] |= 0x80;
        }
    }

    return result;
}
}
}

