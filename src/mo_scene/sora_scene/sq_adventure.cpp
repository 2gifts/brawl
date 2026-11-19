#include <ad/ad_keep_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_adventure.h>
#include <sr/sr_common.h>
#include <types.h>

sqAdventure* sqAdventure::create() {
    return new (Heaps::GlobalMode) sqAdventure("sqAdventure");
}

void srReportLessRelease(const char* format, ...) {
}

void srReportLessMastar(const char* format, ...) {
}

// HYPOTHESIS: command records contain an opcode and either an integer or a pointer.
extern const struct {
    s32 unk0;
    union {
        s32 unk4;
        const s32* unk4p;
    };
} lbl_1_rodata_1378[];

s32 sqAdventure::getStartStepId(s32 index) {
    if (index < 0 || index > 33) {
        return 0;
    }
    bool found = false;
    for (s32 i = 0; ; ++i) {
        if (found) {
            if (lbl_1_rodata_1378[i].unk0 == 0 ||
                lbl_1_rodata_1378[i].unk0 == 1 ||
                lbl_1_rodata_1378[i].unk0 == 3 ||
                lbl_1_rodata_1378[i].unk0 == 4 ||
                lbl_1_rodata_1378[i].unk0 == 27) {
                return *lbl_1_rodata_1378[i].unk4p;
            }
        } else if (lbl_1_rodata_1378[i].unk0 == 22) {
            if (index == lbl_1_rodata_1378[i].unk4 - 1) {
                found = true;
            }
        }
    }
}

void sqAdventure::exit() {
    if (unk24 != NULL) {
        delete unk24;
        unk24 = NULL;
    }
    if (unk28 != NULL) {
        delete unk28;
        unk28 = NULL;
    }
    g_GameGlobal->m_modeMelee->m_meleeInitData.m_0x5a_4 = false;
}

void sqAdventure::calculateClearPercent() {
    float percent = 0.0f;
    s32 clearNum;
    s32 totalNum;
    s32 stepId;
    u32 i = 0;
    for (; i < 34; ++i) {
        stepId = getStartStepId(i);
        percent += adKeepManager::getInstance()->setMapPointClearPercent(i, stepId, &clearNum, &totalNum);
    }
    gmAdvData* advData = g_GameGlobal->m_advData;
    if (g_GameGlobal->m_advData->m_globalFlags[31] & 1) {
        if (!((g_GameGlobal->m_advData->m_globalFlags[31] >> 2) & 1)) {
            if (advData->m_levels[8].m_kind == 4) {
                advData->m_levels[8].m_kind = 3;
            }
        }
        if (!((g_GameGlobal->m_advData->m_globalFlags[31] >> 6) & 1)) {
            if (advData->m_levels[13].m_kind == 4) {
                advData->m_levels[13].m_kind = 3;
            }
        }
        if (!((g_GameGlobal->m_advData->m_globalFlags[31] >> 4) & 1)) {
            if (advData->m_levels[17].m_kind == 4) {
                advData->m_levels[17].m_kind = 3;
            }
        }
    }
    advData = g_GameGlobal->m_advData;
    if ((advData->m_globalFlags[29] >> 12) & 1) {
        percent += 1.0f;
    }
    u32 flags = advData->m_globalFlags[31];
    if ((flags >> 2) & 1) {
        percent += 1.0f;
    }
    if ((flags >> 4) & 1) {
        percent += 1.0f;
    }
    if ((flags >> 6) & 1) {
        percent += 1.0f;
    }
    advData->m_unk2A8 = percent;
}

// TODO: clean up, give variables real names
namespace jj_305DC {
extern "C" void* getInstance__13adKeepManagerFv();
extern "C" void useStageClearInTiming__13adKeepManagerFv(void*);
extern "C" u32 getAdGotRareItemNum__13adKeepManagerFv(void*);
struct Elem { s32 type; u32 data; u8 count; };
extern "C" Elem* getAdGotRareItem__13adKeepManagerFUl(void*, u32);
extern void* g_GameGlobal;
struct Obj {
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void call(s32);
};
struct Self {
 u8 pad0[0x10]; u32 f10; u32 f14; u32 f18; u8 pad1c[4];
 Obj o20; u8 pad24[0x10]; Obj o34; u8 pad38[0x10]; Obj o48;
};
extern "C" void fn_1_305DC(void* unused, Self* self) {
 u8* base = *(u8**)((u8*)g_GameGlobal + 0x30);
 useStageClearInTiming__13adKeepManagerFv(getInstance__13adKeepManagerFv());
 for (u32 idx = 0; idx < getAdGotRareItemNum__13adKeepManagerFv(getInstance__13adKeepManagerFv()); idx++) {
  u32 j; u8 n; s32 d;
  Elem* e = getAdGotRareItem__13adKeepManagerFUl(getInstance__13adKeepManagerFv(), idx);
  if (e->type == 0x17) {
   j = 0; n = e->count; d = e->data;
   for (; j < n; j++) self->o20.call(d);
  }
  if (e->type == 0x33) {
   j = 0; n = e->count; d = e->data;
   for (; j < n; j++) self->o34.call(d);
  }
  if (e->type == 0xB) {
   j = 0; n = e->count; s32 d2 = e->data;
   for (; j < n; j++) self->o48.call(d2);
  }
 }
 self->f10 = *(u16*)(base + 0x4920);
 self->f14 = *(u32*)(base + 0x4910);
 self->f18 = *(u32*)(base + 0x60c);
}
}

// TODO: clean up, give variables real names
namespace jj_326E0 {

extern "C" {
unsigned int* getInfoAppFlag__10GameGlobalFv(void*);
unsigned int* getStageAppFlag__10GameGlobalFv(void*);
unsigned int* getMovieAppFlag__10GameGlobalFv(void*);
void gmSetChKindUseEnable__F15gmCharacterKind(int);
void gmSetBGMIDUseEnable__FUl(int);
void gmSetRecSealRareBit__FUc(int);
void setClearGetterOpen__Fi(int);
int fprintf(void*, const char*, ...);
void abort();
extern char __files[];
}

extern char lbl_1_data_62D0[];

struct IdList {
    unsigned char bytes[14];
};
extern IdList lbl_1_rodata_22C8;

struct BitPosition {
    unsigned int* bits;
    unsigned int index;
};

static inline bool testBitValue(BitPosition pos)
{
    if (pos.index >= 0x33) {
        fprintf(__files + 0xA0, lbl_1_data_62D0);
        abort();
    }
    return (pos.bits[pos.index / 32] & (1U << (pos.index % 32))) != 0;
}

static inline bool testBit(unsigned int* bits, unsigned int index)
{
    BitPosition pos = { bits, index };
    return testBitValue(pos);
}

extern "C" void fn_1_326E0()
{
    char* adventure = (char*)g_GameGlobal->m_advData;
    unsigned int* menu = getInfoAppFlag__10GameGlobalFv(g_GameGlobal);
    char* record = (char*)g_GameGlobal->m_record;
    unsigned int* flags = (unsigned int*)(record + 0x7F4);
    unsigned int* other = getStageAppFlag__10GameGlobalFv(g_GameGlobal);

    for (unsigned int i = 0; i < 0x33; ++i) {
        if (testBit((unsigned int*)(adventure + 0x4898), i)) {
            flags[2 + i / 32] |= 1U << (i % 32);
        }
    }

    if (*(unsigned int*)((char*)g_GameGlobal->m_advData + 0x6CC) & 1)
        flags[4] |= 0x80000000;

    bool complete = true;
    for (int i = 0; i < 34; ++i) {
        if (*(int*)(adventure + 8) != 4) {
            complete = false;
            break;
        }
        adventure += 0x14;
    }
    if (complete)
        flags[4] |= 0x40000000;

    IdList ids = lbl_1_rodata_22C8;

    for (int i = 0; i < 14; ++i) {
        int id = ids.bytes[i];
        unsigned int* bits = (unsigned int*)((char*)g_GameGlobal->m_record + 0x7FC);
        if (bits[(unsigned int)id / 32] & (1 << (id % 32)))
            gmSetChKindUseEnable__F15gmCharacterKind(id);
    }

    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x804) >> 31)
        setClearGetterOpen__Fi(0x42);
    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x7FC) & 0x400)
        setClearGetterOpen__Fi(0x75);
    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x7FC) & 0x800)
        setClearGetterOpen__Fi(0x35);
    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x800) & 0x20)
        setClearGetterOpen__Fi(0x39);

    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x804) >> 31)
        menu[4] |= 0x04000000;
    if ((*(unsigned int*)((char*)g_GameGlobal->m_record + 0x804) >> 30) & 1)
        menu[5] |= 0x00400000;

    bool all = true;
    for (int i = 0; i <= 0x73; ++i) {
        unsigned int* bits = getMovieAppFlag__10GameGlobalFv(g_GameGlobal);
        if (!(bits[i / 32] & (1 << (i % 32)))) {
            all = false;
            break;
        }
    }
    if (all)
        menu[6] |= 0x100;

    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x800) & 0x100)
        *other |= 4;
    if (*(unsigned int*)((char*)g_GameGlobal->m_record + 0x800) & 0x800)
        *other |= 0x40;

    if (*(unsigned int*)((char*)g_GameGlobal->m_advData + 0x6CC) & 1) {
        gmSetBGMIDUseEnable__FUl(0x2713);
        gmSetBGMIDUseEnable__FUl(0x2822);
        gmSetBGMIDUseEnable__FUl(0x2825);
        gmSetBGMIDUseEnable__FUl(0x282C);
        gmSetBGMIDUseEnable__FUl(0x282D);
    }

    if ((unsigned int)(*(int*)((char*)g_GameGlobal->m_advData + 0xF4) - 3) <= 1)
        gmSetRecSealRareBit__FUc(8);
    if ((unsigned int)(*(int*)((char*)g_GameGlobal->m_advData + 0x248) - 3) <= 1)
        gmSetRecSealRareBit__FUc(0x10);
    if (*(unsigned int*)((char*)g_GameGlobal->m_advData + 0x6CC) & 1)
        gmSetRecSealRareBit__FUc(0x20);
}
}

