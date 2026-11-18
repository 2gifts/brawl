#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_hide_lib.h>
#include <types.h>

// gmAddFigure (the map name); src/sora/gm/gm_common.cpp calls it by this stand-in name.
extern "C" void fn_80052704(int id, int num);
// gmAddSeal (the map name); src/sora/gm/gm_common.cpp calls it by this stand-in name.
extern "C" void fn_80052AC0(int id, int num);
// gmSetBGMIDUseEnable (the map name); src/sora/ad/ad_keep_manager.cpp calls it by this stand-in name.
extern "C" void fn_8004FB48(u32 id);

extern const struct {
    struct {
        u16 unk0[5];
    } unk0;
    u16 unkA[3];
} lbl_80406E98;

void gmActionOpenClearGet(int condition, int kind, int reward) {
    switch (kind) {
    case 5: {
        u32* flags = (u32*)g_GameGlobal->getHideEtcAppFlag();
        int bit = (reward - 1) * 2;
        flags[bit / 32] |= 1 << (bit % 32);
        break;
    }
    case 3: {
        u32* flags = (u32*)g_GameGlobal->getHideEtcAppFlag();
        int bit = (reward + 2) * 2;
        flags[bit / 32] |= 1 << (bit % 32);
        break;
    }
    case 1:
        fn_80052704((u16)reward, 1);
        gmSetFlagInfoFigure((u16)reward, 1);
        break;
    case 0: {
        __typeof__(lbl_80406E98.unk0) seals = lbl_80406E98.unk0;
        int seal = seals.unk0[reward - 1];
        fn_80052AC0(seal, 1);
        gmSetFlagInfoSeal(seal, 1);
        break;
    }
    case 2:
        fn_8004FB48(reward);
        break;
    case 4:
        gmSetStageUseEnable((srStageKind)reward);
        break;
    }
}

bool gmCheckTargetBreakHideElement() {
    if (!(g_GameGlobal->getCharaAppFlag()[0] & 0x10000000)) {
        for (int level = 0; level < 5; ++level) {
            u32 count = 0;
            for (int character = 0; character < 35; ++character) {
                if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unkCC[level].unk0_30 == 1) {
                    ++count;
                    if (count >= 30) break;
                }
            }
            if (count >= 30) {
                g_GameGlobal->getCharaAppFlag()[0] |= 0x8000000;
                return true;
            }
        }
    }
    if (!(g_GameGlobal->getCharaAppFlag()[0] & 0x10000)) {
        u32 cleared = 0;
        for (int level = 0; level < 5; ++level) {
            cleared = 0;
            for (int character = 0; character < 35; ++character) {
                if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unkCC[level].unk0_30 == 1) {
                    cleared = 1;
                    break;
                }
            }
            if (((g_GameGlobal->m_record->unk34FC[level].unk0 >> 30) & 1) == 1) cleared = 1;
            if (!cleared) break;
        }
        if (cleared == 1) {
            g_GameGlobal->getCharaAppFlag()[0] |= 0x8000;
            return true;
        }
    }
    return false;
}

bool checkAllstar(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 27:
    case 24:
        int count = 0;
        for (int character = 0; character < 35; ++character) {
            if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unk9C_7) ++count;
        }
        switch (condition) {
        case 27:
            if (count >= 35) satisfied = true;
            break;
        case 24:
            if (count >= 10) satisfied = true;
            break;
        }
        break;
    }
    return satisfied;
}

bool checkBossBattle(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 127:
    case 101:
    case 61:
        int count = 0;
        for (int character = 0; character < 35; ++character) {
            if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unk9C_4) ++count;
        }
        switch (condition) {
        case 127:
            if (count >= 35) satisfied = true;
            break;
        case 101:
            if (count >= 10) satisfied = true;
            break;
        case 61:
            if (count >= 20) satisfied = true;
            break;
        }
        break;
    }
    return satisfied;
}

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

extern const struct {
    struct {
        u16 unk0[49];
    } unk0;
    u16 unk62;
} lbl_80406FE8;

extern const struct {
    u16 unk0[10];
} lbl_8040704C;

bool checkSimple(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 5:
    case 120:
        int count = 0;
        for (int character = 0; character < 35; ++character) {
            if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unk9C_b10) {
                ++count;
            }
        }
        switch (condition) {
        case 5:
            if (count >= 35) satisfied = true;
            break;
        case 120:
            if (count >= 20) satisfied = true;
            break;
        }
        break;
    }
    return satisfied;
}

bool checkMeleeEtc(int condition) {
    bool satisfied = false;
    u32 kills = *g_GameGlobal->getGlobalRecordKillTotalCountDatap();
    u32 hours = *g_GameGlobal->getGlobalRecordVsMeleeTimeTotalDatap() / 60 / 60;
    u32 coins = 0;
    for (int character = 0; character < 35; ++character) {
        coins += g_GameGlobal->getGlobalRecordCharaNumDatap(character)->m_diagram.m_getCoin;
    }
    switch (condition) {
    case 26:
        if (kills >= 1000) satisfied = true;
        break;
    case 2:
        if (kills >= 2000) satisfied = true;
        break;
    case 122:
        if (kills >= 500) satisfied = true;
        break;
    case 79:
        if (hours >= 30) satisfied = true;
        break;
    case 115:
        if (hours >= 50) satisfied = true;
        break;
    case 39:
        if (hours >= 100) satisfied = true;
        break;
    case 81:
        if (g_GameGlobal->getGlobalRecordCharaNumDatap(12)->m_diagram.m_champNum >= 5) satisfied = true;
        break;
    case 72:
        if (*g_GameGlobal->getGlobalRecordVsCoinMeleeCountDatap() >= 10) satisfied = true;
        break;
    case 119:
        if (coins >= 1000) satisfied = true;
        break;
    case 91:
        if (coins >= 5000) satisfied = true;
        break;
    case 80:
        break;
    }
    return satisfied;
}

bool checkTraining(int condition) {
    bool satisfied = false;
    u16 maximum = 0;
    u16 total = 0;
    for (int character = 0; character < 35; ++character) {
        u16 count = g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unkC8;
        if (count > maximum) maximum = count;
        total += count;
    }
    switch (condition) {
    case 108:
        if (maximum >= 10) satisfied = true;
        break;
    case 33:
        if (total >= 400) satisfied = true;
        break;
    }
    return satisfied;
}

bool checkInfo_advFigure(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 58: {
        __typeof__(lbl_80406FE8.unk0) figures = lbl_80406FE8.unk0;
        satisfied = true;
        for (int index = 0; index < 49; ++index) {
            if (!gmCheckExistFigure(figures.unk0[index])) {
                satisfied = false;
                break;
            }
        }
        break;
    }
    case 59: {
        __typeof__(lbl_8040704C) figures = lbl_8040704C;
        satisfied = true;
        for (int index = 0; index < 10; ++index) {
            if (!gmCheckExistFigure(figures.unk0[index])) {
                satisfied = false;
                break;
            }
        }
        break;
    }
    }
    return satisfied;
}

// TODO: clean up, give variables real names
extern "C" void fn_800569F8(void) {}

// TODO: clean up, give variables real names
namespace jj_80056DB0 {
extern "C" unsigned int* fn_8004E5F8(void*);
inline bool testBit(unsigned int* bits, int idx) {
    int n = idx * 2;
    return bits[n / 32] & (1 << (n % 32));
}
inline void setBit(unsigned int* bits, int idx) {
    int n = idx * 2;
    bits[n / 32] |= (1 << (n % 32));
}
extern "C" void fn_80056DB0(int arg0) {
    int idx;
    switch (arg0) {
    case 0: idx = 30; break;
    case 1: idx = 31; break;
    case 2: idx = 32; break;
    }
    if (!testBit(fn_8004E5F8(g_GameGlobal), idx)) {
        setBit(fn_8004E5F8(g_GameGlobal), idx);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_80059F94 {
typedef unsigned int u32;

extern "C" void* fn_8004E5B8(char*);
extern "C" char* g_GameGlobal;

extern "C" int fn_80059F94(int arg0) {
    int ret = 0;
    switch (arg0) {
    case 0x7C:
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x10000) {
            ret = 1;
        }
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x40000) {
            ret = 1;
        }
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x100000) {
            ret = 1;
        }
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x400000) {
            ret = 1;
        }
        break;
    case 0x7D:
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x40000) {
            ret = 1;
        }
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x100000) {
            ret = 1;
        }
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x400000) {
            ret = 1;
        }
        break;
    case 0x7E:
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x100000) {
            ret = 1;
        }
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x400000) {
            ret = 1;
        }
        break;
    case 0x7F:
        if (*(u32*)fn_8004E5B8(g_GameGlobal) & 0x400000) {
            ret = 1;
        }
        break;
    }
    return ret;
}
}

namespace jj_80056E68 {
extern "C" u32* fn_8004E580(void*);
extern "C" u32* fn_8004E634(void*);
extern "C" u8* fn_8004E7B8(void*, int);
extern "C" u32 fn_8005297C();
extern void* g_GameGlobal;
extern "C" void fn_80056E68() {
    if (!(*fn_8004E580(g_GameGlobal) & 0x80000000) && *fn_8004E634(g_GameGlobal) / 60 / 60 >= 10) {
        *fn_8004E580(g_GameGlobal) |= 0x40000000;
    } else if (!(*fn_8004E580(g_GameGlobal) & 0x400000) && fn_8005297C() >= 250) {
        *fn_8004E580(g_GameGlobal) |= 0x200000;
    } else if (!(*fn_8004E580(g_GameGlobal) & 0x80000) && (int)*fn_8004E7B8(g_GameGlobal, 0x22) >= 15) {
        *fn_8004E580(g_GameGlobal) |= 0x40000;
    } else if (!(*fn_8004E580(g_GameGlobal) & 2)) {
        u8* p = *(u8**)((u8*)g_GameGlobal + 0x18);
        u32 total = 0;
        if (!p[0x25]) total = *(u32*)(p + 0x254);
        if (!p[0x2d1]) total += *(u32*)(p + 0x500);
        if (!p[0x57d]) total += *(u32*)(p + 0x7ac);
        if (!p[0x829]) total += *(u32*)(p + 0xa58);
        if (total >= 10) *fn_8004E580(g_GameGlobal) |= 1;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800596EC {

struct Pair { s32 a; s32 b; };
struct Blob { Pair p[10]; };
extern const Blob lbl_80406F98;
struct Table { Blob b; s32 pad; };

inline s32 testRange(s32 r9, s32 r4) {
    s32 result = 1;
    for (s32 i = r9; i <= r4; i++) {
        s32 k = i - 1;
        s32 sh = (k * 2) % 32;
        void* base = g_GameGlobal->m_record;
        u32 word = *(u32*)((u8*)base + ((k * 2) / 32) * 4 + 0x15AC);
        if (!((1 << sh) & word)) {
            result = 0;
            break;
        }
    }
    return result;
}

extern "C" s32 fn_800596EC(s32 arg0) {
    Table t;
    s32 r9 = 0;
    s32 r4 = 0;
    t.b = lbl_80406F98;
    switch (arg0) {
    case 0x30: r9 = t.b.p[0].a; r4 = t.b.p[0].b; break;
    case 0x33: r9 = t.b.p[1].a; r4 = t.b.p[1].b; break;
    case 0x35: r9 = t.b.p[2].a; r4 = t.b.p[2].b; break;
    case 0x37: r9 = t.b.p[3].a; r4 = t.b.p[3].b; break;
    case 0x32: r9 = t.b.p[4].a; r4 = t.b.p[4].b; break;
    case 0x36: r9 = t.b.p[5].a; r4 = t.b.p[5].b; break;
    case 0x38: r9 = t.b.p[6].a; r4 = t.b.p[6].b; break;
    case 0x34: r9 = t.b.p[7].a; r4 = t.b.p[7].b; break;
    case 0x39: r9 = t.b.p[8].a; r4 = t.b.p[8].b; break;
    case 0x2F: r9 = t.b.p[9].a; r4 = t.b.p[9].b; break;
    case 0x6B: r9 = 1; r4 = 0x1A5; break;
    default: r4 = 0; r9 = 0; break;
    }
    if (r9 == 0) return 0;
    return testRange(r9, r4);
}
}

namespace jj_80058320 {
extern "C" int fn_800557F8(int arg0, int arg1, int arg2);

extern "C" int fn_80058320(int arg0) {
    int ret = 0;
    switch (arg0) {
    case 0x72:
        ret = fn_800557F8(1, 1, 0);
        break;
    case 0x2F:
        ret = fn_800557F8(1, 3, 0);
        break;
    case 0xE:
        ret = fn_800557F8(1, 0xD, 0);
        break;
    case 0x6D:
        ret = fn_800557F8(1, 6, 0);
        break;
    case 0x55:
        ret = fn_800557F8(1, 0x10, 0);
        break;
    case 0x50:
        ret = fn_800557F8(1, 0x14, 0);
        break;
    case 0x47:
        ret = fn_800557F8(1, 0x18, 2);
        break;
    case 0x28:
        ret = fn_800557F8(1, 0x1A, 0);
        break;
    case 0x56:
        ret = fn_800557F8(1, 0x1F, 0);
        break;
    case 0x76:
        ret = fn_800557F8(1, 0x20, 0);
        break;
    case 0x4C:
        ret = fn_800557F8(1, 0x24, 0);
        break;
    case 0x2B:
        ret = fn_800557F8(1, 0x2B, 2);
        break;
    case 0x45:
        ret = fn_800557F8(1, 0x27, 0);
        break;
    case 0x5E:
        ret = fn_800557F8(1, 0x2F, 2);
        break;
    }
    return ret;
}
}

namespace jj_80059890 {
extern "C" {
extern void* g_GameGlobal;
void* fn_8004E5D8(void*, void*);
int fn_80055888(int, int);
}
extern "C" s32 fn_80059890(s32 arg0) {
    s32 fourth;
    s32 third;
    s32 second;
    s32 first;
    s32 last;
    s32 result = 0;
    switch (arg0) {
    case 0x4B:
        for (first = 0; first < 0x23; first++) {
            u16 v = *(u16*)((char*)fn_8004E5D8(g_GameGlobal, (void*)first) + 0x9c);
            if ((v >> 8) & 7) { result = 1; break; }
        }
        break;
    case 0x4C:
        result = 1;
        for (second = 0; second < 0x23; second++) {
            u16 v = *(u16*)((char*)fn_8004E5D8(g_GameGlobal, (void*)second) + 0x9c);
            if (!((v >> 8) & 7)) { result = 0; break; }
        }
        break;
    case 0x59:
        for (third = 0; third < 0x23; third++) {
            u16 v = *(u16*)((char*)fn_8004E5D8(g_GameGlobal, (void*)third) + 0x9c);
            if (((u32)(v >> 8) & 7) >= 5) { result = 1; break; }
        }
        break;
    case 0x4F:
        result = 1;
        for (fourth = 0; fourth < 0x23; fourth++) {
            u16 v = *(u16*)((char*)fn_8004E5D8(g_GameGlobal, (void*)fourth) + 0x9c);
            if (!((v >> 5) & 7)) { result = 0; break; }
        }
        break;
    case 0x51:
        result = 1;
        for (fourth = 0; fourth < 0x23; fourth++) {
            u16 v = *(u16*)((char*)fn_8004E5D8(g_GameGlobal, (void*)fourth) + 0x9c);
            if (!((v >> 2) & 7)) { result = 0; break; }
        }
        break;
    case 0x54: case 0x55: case 0x56: case 0x57: case 0x58:
        result = 1;
        for (last = 0; last < 0x23; last++) {
            u32 v = *(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)last) + 0xcc + (arg0 - 0x54) * 4);
            if (((v >> 30) & 1) != 1) { result = 0; break; }
        }
        break;
    case 0x5E:
        if (fn_80055888(1, 0) >= 20) result = 1;
        break;
    case 0x5F:
        if (fn_80055888(1, 0) >= 41) result = 1;
        break;
    case 0x60:
        if (fn_80055888(2, 0) >= 10) result = 1;
        break;
    case 0x61:
        if (fn_80055888(2, 0) >= 21) result = 1;
        break;
    }
    return result;
}
}

namespace jj_80058F64 {
extern "C" u8* fn_8004E5D8(void*, int);
extern "C" int fn_80058F64(int arg0) {
    int off;
    int result = 0;
    int index = 0;
    switch (arg0) {
    case 4: case 0x46: case 0x4a: case 0x4d: index = 0; break;
    case 0xb: case 0x12: case 0x16: case 0x2a: index = 1; break;
    case 0x23: case 0x3a: case 0x58: case 0x61: index = 2; break;
    case 8: case 0x38: case 0x43: case 0x6a: index = 3; break;
    case 0x1d: case 0x1f: case 0x63: case 0x68: index = 4; break;
    }
    off = index * 4;
    u32 count = 0;
    u32 minv = -1;
    for (int i = 0; i < 0x23; i++) {
        if (((*(u32*)(fn_8004E5D8(g_GameGlobal, i) + off + 0xcc) >> 30) & 1) == 1) {
            count++;
            u32 value = *(u32*)(fn_8004E5D8(g_GameGlobal, i) + off + 0xcc) & 0x3fffffff;
            if (value < minv) minv = value;
        }
    }
    switch (arg0) {
    case 0x4d: if (count != 0) result = 1; break;
    case 0x46: if (count >= 10) result = 1; break;
    case 0x4a: if (count >= 0x23) result = 1; break;
    case 4: if (minv <= 0x384) result = 1; break;
    case 0x12: if (count != 0) result = 1; break;
    case 0x16: if (count >= 10) result = 1; break;
    case 0x2a: if (count >= 0x23) result = 1; break;
    case 0xb: if (minv <= 0x474) result = 1; break;
    case 0x23: if (count != 0) result = 1; break;
    case 0x3a: if (count >= 10) result = 1; break;
    case 0x61: if (count >= 0x23) result = 1; break;
    case 0x58: if (minv <= 0x4b0) result = 1; break;
    case 0x6a: if (count != 0) result = 1; break;
    case 0x38: if (count >= 10) result = 1; break;
    case 0x43: if (count >= 0x23) result = 1; break;
    case 8: if (minv <= 0x780) result = 1; break;
    case 0x68: if (count != 0) result = 1; break;
    case 0x1d: if (count >= 10) result = 1; break;
    case 0x63: if (count >= 0x23) result = 1; break;
    case 0x1f: if (minv <= 0x708) result = 1; break;
    }
    return result;
}
}

namespace jj_800589C8 {
typedef unsigned short u16;
typedef unsigned int u32;

struct Rec {
    char pad[0x9c];
    u16 b15 : 1;
    u16 b14 : 1;
    u16 b13 : 1;
    u16 b12 : 1;
    u16 b11 : 1;
    u16 rest : 11;
    char pad2[0xb4 - 0x9e];
    u32 unkB4;
    char pad3[0xc0 - 0xb8];
    u32 unkC0;
    u32 unkC4;
};

struct Sub {
    char pad[0x34d4];
    u16 b15 : 1;
    u16 b14 : 1;
    u16 b13 : 1;
    u16 b12 : 1;
    u16 b11 : 1;
    u16 rest : 11;
};

struct GG {
    char pad[0x24];
    Sub* sub;
};

extern "C" Rec* fn_8004E5D8(GG*, int);
extern "C" GG* g_GameGlobal;

extern "C" int fn_800589C8(int arg0) {
    int result = 0;
    u32 count = 0;
    u32 minv = 0x4650;
    for (int i = 0; i < 0x23; i++) {
        if (fn_8004E5D8(g_GameGlobal, i)->b13 == 1) {
            count++;
            u32 v = fn_8004E5D8(g_GameGlobal, i)->unkB4;
            if (v < minv) {
                minv = v;
            }
        }
    }
    Sub* s = g_GameGlobal->sub;
    bool a = s->b13 == 1;
    bool b = s->b11 == 1;
    switch (arg0) {
    case 0x52:
        if (count != 0 || a == true) {
            result = 1;
        }
        break;
    case 0x6:
        if (count >= 0x23) {
            result = 1;
        }
        break;
    case 0x14:
        if (minv <= 0x3138) {
            result = 1;
        }
        break;
    case 0x60:
        if (minv <= 0x3840) {
            result = 1;
        }
        break;
    case 0x54:
        if (b == true) {
            result = 1;
        } else {
            for (int i = 0; i < 0x23; i++) {
                if (fn_8004E5D8(g_GameGlobal, i)->b11 == 1) {
                    result = 1;
                    break;
                }
            }
        }
        break;
    case 0x3F:
        for (int i = 0; i < 0x23; i++) {
            if (fn_8004E5D8(g_GameGlobal, i)->unkC4 >= 10) {
                result = 1;
                break;
            }
        }
        break;
    case 0x44:
        for (int i = 0; i < 0x23; i++) {
            if (fn_8004E5D8(g_GameGlobal, i)->unkC4 >= 5) {
                result = 1;
                break;
            }
        }
        break;
    case 0x74:
        for (int i = 0; i < 0x23; i++) {
            if (fn_8004E5D8(g_GameGlobal, i)->unkC0 >= 100) {
                result = 1;
                break;
            }
        }
        break;
    case 0x79:
        for (int i = 0; i < 0x23; i++) {
            if (fn_8004E5D8(g_GameGlobal, i)->unkC0 >= 50) {
                result = 1;
                break;
            }
        }
        break;
    }
    return result;
}
}

namespace jj_80058544 {
extern "C" {
extern void* g_GameGlobal;
void* fn_8004E5D8(void*, void*);
}
extern "C" s32 fn_80058544(s32 arg0) {
    s32 result = 0;
    switch (arg0) {
    case 0x26:
        result = 1;
        for (s32 i = 0; i < 0x23; i++) {
            if (*(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac) == 0) {
                result = 0;
                break;
            }
        }
        break;
    case 0xd:
        for (s32 i = 0; i < 0x23; i++) {
            if (*(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac) >= 0x2328U) {
                result = 1;
                break;
            }
        }
        break;
    case 0xc:
        for (s32 i = 0; i < 0x23; i++) {
            if (*(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac) >= 0x2ee0U) {
                result = 1;
                break;
            }
        }
        break;
    case 0x69:
        for (s32 i = 0; i < 0x23; i++) {
            if (*(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac) >= 0x3a98U) {
                result = 1;
                break;
            }
        }
        break;
    case 0x57: {
        u32 sum = 0;
        s32 i = 0;
        do {
            sum += *(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac);
            i++;
        } while (i < 0x23);
        if (sum >= 0x493e0U) result = 1;
        break;
    }
    case 0x7b: {
        u32 sum = 0;
        s32 i = 0;
        do {
            sum += *(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac);
            i++;
        } while (i < 0x23);
        if (sum >= 0x5b8d8U) result = 1;
        break;
    }
    case 0x7c: {
        u32 sum = 0;
        s32 i = 0;
        do {
            sum += *(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac);
            i++;
        } while (i < 0x23);
        if (sum >= 0x6ddd0U) result = 1;
        break;
    }
    case 0x4b: {
        u32 sum = 0;
        s32 i = 0;
        do {
            sum += *(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac);
            i++;
        } while (i < 0x23);
        if (sum >= 0x249f0U) result = 1;
        break;
    }
    case 0x5d: {
        u32 sum = 0;
        s32 i = 0;
        do {
            sum += *(u32*)((char*)fn_8004E5D8(g_GameGlobal, (void*)i) + 0xac);
            i++;
        } while (i < 0x23);
        if (sum >= 0x3a980U) result = 1;
        break;
    }
    }
    return result;
}
}

namespace jj_80058C48 {
extern "C" {
struct OriginalIds { int values[10]; };
extern const OriginalIds lbl_80406EA8;
extern void* g_GameGlobal;
void* fn_8004E5C4(void*);
u8* fn_8004E7B8(void*, int);
}
extern "C" s32 fn_80058C48(s32 arg0) {
    s32 result = 0;
    switch (arg0) {
    case 0xA:
        if (*(u8*)((char*)fn_8004E5C4(g_GameGlobal) + 0x32A) >= 15U) result = 1;
        break;
    case 0x6F:
        if (*(u8*)((char*)fn_8004E5C4(g_GameGlobal) + 0x32A) >= 5U) result = 1;
        break;
    case 0x49: {
        OriginalIds ids = lbl_80406EA8;
        int* p = ids.values;
        u32 total = 0;
        int i = 0;
        do {
            total += *fn_8004E7B8(g_GameGlobal, *p);
            if (total >= 10U) {
                result = 1;
                break;
            }
            i++;
            p++;
        } while (i < 10);
        break;
    }
    case 0x13:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x2A) >= 3) result = 1;
        break;
    case 0x15:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x35) >= 10) result = 1;
        break;
    case 0x37:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x19) >= 10) result = 1;
        break;
    case 0x70:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x16) >= 10) result = 1;
        break;
    case 0x1C:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x23) >= 10) result = 1;
        break;
    case 0x6E:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0xE) >= 10) result = 1;
        break;
    case 0x4E:
        if ((int)*fn_8004E7B8(g_GameGlobal, 5) >= 10) result = 1;
        break;
    case 0x32:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0xB) >= 10) result = 1;
        break;
    case 0x11:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0xC) >= 10) result = 1;
        break;
    case 0x31:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x14) >= 10) result = 1;
        break;
    case 0x71:
        if ((int)*fn_8004E7B8(g_GameGlobal, 0x22) >= 15) result = 1;
        break;
    }
    return result;
}
}

namespace jj_80059C00 {
extern "C" {
extern void* g_GameGlobal;
unsigned char* fn_8004E790(void*, int);
int fn_80055888(int, int);
unsigned int fn_8004F4A0(int);
int fn_80059C00(int arg) {
    int result = 0;
    unsigned char* a = fn_8004E790(g_GameGlobal, 0);
    unsigned char* b = fn_8004E790(g_GameGlobal, 1);
    switch (arg) {
    case 0x75:
        if (*a < 0x14 && fn_80055888(1, 0) >= 7) {
            result = 1;
            *a = 0x14;
        }
        break;
    case 0x76:
        if (*a >= 0x14 && *a < 0x1c && fn_80055888(1, 0) >= 0x12 &&
            fn_8004F4A0(0xb) == 1U && fn_8004F4A0(0x13) == 1U &&
            fn_8004F4A0(9) == 1U && fn_8004F4A0(0xa) == 1U &&
            fn_8004F4A0(0x24) == 1U && fn_8004F4A0(0x26) == 1U) {
            result = 1;
            *a = 0x1c;
        }
        break;
    case 0x77:
        if (*a >= 0x1c && *a < 0x28 && fn_80055888(1, 0) >= 0x14 &&
            fn_8004F4A0(0x15) == 1U && fn_8004F4A0(0x2a) == 1U &&
            fn_8004F4A0(0x16) == 1U && fn_8004F4A0(0x14) == 1U &&
            fn_8004F4A0(0x2b) == 1U && fn_8004F4A0(0x27) == 1U &&
            fn_8004F4A0(0x28) == 1U && fn_8004F4A0(0x29) == 1U) {
            result = 1;
            *a = 0x28;
        }
        break;
    case 0x78:
        if (*a == 0x28 && fn_80055888(1, 0) >= 0x28) {
            result = 1;
            *a = 0x29;
        }
        break;
    case 0x79:
        if (*b < 0xd && fn_8004F4A0(0xb) == 1U &&
            fn_8004F4A0(0x13) == 1U && fn_8004F4A0(9) == 1U &&
            fn_8004F4A0(0xa) == 1U && fn_8004F4A0(0x24) == 1U &&
            fn_8004F4A0(0x26) == 1U) {
            result = 1;
            *b = 0xd;
        }
        break;
    case 0x7a:
        if (*b >= 0xd && *b < 0x14 && fn_80055888(2, 0) >= 0xa &&
            fn_8004F4A0(0x15) == 1U && fn_8004F4A0(0x2a) == 1U &&
            fn_8004F4A0(0x16) == 1U && fn_8004F4A0(0x14) == 1U &&
            fn_8004F4A0(0x2b) == 1U && fn_8004F4A0(0x27) == 1U &&
            fn_8004F4A0(0x28) == 1U && fn_8004F4A0(0x29) == 1U) {
            result = 1;
            *b = 0x14;
        }
        break;
    case 0x7b:
        if (*b == 0x14 && fn_80055888(2, 0) >= 0x14) {
            result = 1;
            *b = 0x15;
        }
        break;
    }
    return result;
}
}
}

