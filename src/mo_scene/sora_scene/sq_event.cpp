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

// TODO: clean up, give variables real names
namespace jj_32C3C {
class gfSceneManager;


extern "C" {
void* getInstance__14gfSceneManagerFv();
void* searchScene__14gfSceneManagerFPCc(gfSceneManager*, const char*);
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void* __dynamic_cast(void*, long, const void*, const void*, long);

unsigned int fn_1_331DC(void*);
void fn_1_33B24(void*);
void fn_1_33C08(void*);
void fn_1_33EDC();
void fn_1_3422C();
void fn_1_3D2D8();
int gmGetMenuDecisionPad__Fv();
void gmAddSingleTimeAll__FUlUl(unsigned int, int);
char* getCharaAppFlag__10GameGlobalFv(void*);
char* getInfoAppFlag__10GameGlobalFv(void*);
int fn_80055888(int, int);

extern char lbl_1_data_8000[];
extern char __RTTI__10scSelEvent[];
extern char __RTTI__7gfScene[];
extern char* g_GameGlobal;
extern char* g_gfApplication;
extern int lbl_1_bss_1A60;
}

extern "C" void fn_1_32C3C(char* self) {
    const char* str = lbl_1_data_8000;
    gfSceneManager* mgr = (gfSceneManager*)getInstance__14gfSceneManagerFv();
    char* p;
    int count;
    unsigned char a;
    unsigned char b;
    int cont = 1;

    while (cont) {
        cont = 0;
        switch (*(unsigned int*)(self + 8)) {
        default:
            *(int*)(self + 8) = 0;
        case 0:
            g_gfApplication[0x110] = 0;
            *(int*)(self + 0x14) = 0;
            *(int*)(self + 8) = 1;
            *(int*)(self + 0xc) = 3;
            self[0x10] = 13;
            cont = 1;
            break;

        case 1:
            setNextScene__14gfSceneManagerFv(
                mgr, str + 0x70, *(unsigned char*)(self + 0x10));
            *(int*)(self + 8) = 2;
            break;

        case 2:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 0xc);
            break;

        case 3: {
            setNextScene__14gfSceneManagerFv(
                mgr, str + 0x80, *(int*)(self + 0x14));
            char* p = *(char**)(g_GameGlobal + 0x10);
            int n = gmGetMenuDecisionPad__Fv();
            p[4] = 11;
            p[0] = n + 1;
            char* scene = (char*)__dynamic_cast(
                searchScene__14gfSceneManagerFPCc(mgr, str + 0x80),
                4, __RTTI__10scSelEvent, __RTTI__7gfScene, 0);
            *(int*)(scene + 0x394) = n;
            *(int*)(self + 8) = 4;
            break;
        }

        case 4:
            switch (*(int*)((char*)mgr + 0x284)) {
            case 1:
                if (fn_1_331DC(self) == 1) {
                    *(int*)(self + 8) = 1;
                    *(int*)(self + 0xc) = 5;
                    self[0x10] = 13;
                } else {
                    fn_1_33C08(self);
                    *(int*)(self + 8) = 1;
                    *(int*)(self + 0xc) = 7;
                    self[0x10] = 0;
                }
                break;
            case 0:
                *(int*)(self + 8) = 11;
                break;
            }
            cont = 1;
            *(int*)(self + 0x24) = gmGetMenuDecisionPad__Fv();
            break;

        case 5: {
            fn_1_3D2D8();
            p = *(char**)(g_GameGlobal + 0x10);
            char* scene = (char*)__dynamic_cast(
                searchScene__14gfSceneManagerFPCc(mgr, str + 0x80),
                4, __RTTI__10scSelEvent, __RTTI__7gfScene, 0);
            count = *(int*)(scene + 0x368);
            a = *(int*)(scene + 0x36c);
            if (count == 2)
                b = *(int*)(scene + 0x370);
            setNextScene__14gfSceneManagerFv(mgr, str + 0x8c, 0);
            p[4] = 11;
            p[0] = a + 1;
            p[8] = 1;
            p[0xbf] = a + 1;
            if (count == 2)
                p[0x11b] = b + 1;
            else
                p[0x11b] = 0;
            *(int*)(self + 8) = 6;
            break;
        }

        case 6: {
            char* p;
            char* scene = (char*)__dynamic_cast(
                searchScene__14gfSceneManagerFPCc(mgr, str + 0x80),
                4, __RTTI__10scSelEvent, __RTTI__7gfScene, 0);
            p = *(char**)(g_GameGlobal + 0x10);
            switch (*(int*)((char*)mgr + 0x284)) {
            case 0:
                *(int*)(self + 0x14) = 1;
                *(int*)(self + 8) = 3;
                break;
            case 1:
                fn_1_33B24(self);
                fn_1_33C08(self);
                *(int*)(self + 8) = 1;
                *(int*)(self + 0xc) = 7;
                self[0x10] = 0;
                {
                    char* dst = scene;
                    for (int i = 0; i < *(int*)(scene + 0x368); i++) {
                        int n = *(unsigned char*)(p + 0xd0);
                        if (n == 0x78)
                            n = -1;
                        *(int*)(dst + 0x374) = n;
                        p += 0x5c;
                        dst += 4;
                    }
                }
                break;
            }
            cont = 1;
            break;
        }

        case 7:
            g_gfApplication[0x110] = self[0x18];
            setNextScene__14gfSceneManagerFv(mgr, str + 0xa0, 0);
            *(int*)(self + 8) = 8;
            break;

        case 8: {
            g_gfApplication[0x110] = 0;
            fn_1_3422C();
            gmAddSingleTimeAll__FUlUl(
                *(unsigned int*)(*(char**)(g_GameGlobal + 0x18) + 0x1384) / 60,
                lbl_1_bss_1A60);
            unsigned int flag;
            if (!(*(int*)(getCharaAppFlag__10GameGlobalFv(g_GameGlobal) + 4) & 4) &&
                fn_80055888(1, 0) >= 20 &&
                ((*(unsigned int*)(getInfoAppFlag__10GameGlobalFv(g_GameGlobal) + 0x10) >> 26) & 1) == 1) {
                *(int*)(getCharaAppFlag__10GameGlobalFv(g_GameGlobal) + 4) |= 2;
                flag = 1;
            } else {
                flag = 0;
            }
            if (flag == 1)
                fn_1_33EDC();
            cont = 1;
            *(int*)(self + 8) = 9;
            break;
        }

        case 9:
            setNextSequence__14gfSceneManagerFv(mgr, str + 0xa8, 10);
            break;

        case 10: {
            *(int*)(*(char**)(g_GameGlobal + 0x1c) + 0x24) =
                *(int*)(self + 0x24);
            *(int*)(self + 0x14) = 1;
            *(int*)(self + 8) = 1;
            *(int*)(self + 0xc) = 3;
            self[0x10] = 13;
            unsigned char f = g_gfApplication[0xcc];
            unsigned int high = (f >> 7) & 1;
            unsigned int low = (f >> 6) & 1;
            if (!high && !low)
                g_gfApplication[0xcc] = f | 0xc0;
            cont = 1;
            break;
        }

        case 11:
            g_gfApplication[0x110] = self[0x18];
            setNextSequence__14gfSceneManagerFv(mgr, str + 0xb8, 8);
            *(int*)(self + 8) = 12;
            break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_33B24 {
extern "C" {
extern int lbl_1_bss_1A60;
struct Global { char pad[8]; char* dest; char pad2[4]; char* src; };
extern Global* g_GameGlobal;
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFiii(int, int);
void getNameUTF16__10GameGlobalFiPw(Global*, int, void*);
void fn_1_33B24() {
    char* s;
    char* d;
    char* data;
    int i = 0;
    d = g_GameGlobal->dest;
    s = g_GameGlobal->src;
    data = d + 0xa4;
    for (; i < lbl_1_bss_1A60; s += 0x5c, d += 0x5c, data += 0x5c, ++i) {
        if (*(u8*)(s + 0xb9) == 0) {
            u8 kind = *(u8*)(s + 0xb8);
            u8 costume = *(u8*)(s + 0xbd);
            int character = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
            int color = character == 0x28 ? 0 : getFighterColorFileNo__6muMenuFiii(character, costume);
            *(u8*)(d + 0x99) = 0;
            *(u8*)(d + 0x98) = kind;
            *(u8*)(d + 0xa3) = 0;
            *(u8*)(d + 0x9d) = color;
            *(u8*)(d + 0x9e) = costume;
            u8 stage = *(u8*)(s + 0xd0);
            *(u8*)(d + 0xb0) = stage;
            if (stage != 0x78)
                getNameUTF16__10GameGlobalFiPw(g_GameGlobal, stage, data);
            else
                *(u16*)(d + 0xa4) = 0;
            *(u8*)(d + 0x9f) = *(u8*)(s + 0xbf);
            *(u8*)(d + 0x9a) = i;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_3422C {
struct SceneData {
    unsigned char pad0[0x250];
    int value;
    int pad254;
    int total;
    unsigned char pad25C[0x4FC - 0x25C];
    int extra;
    unsigned char pad500[0x1378 - 0x500];
    unsigned char mode;
};

struct GameGlobal {
    unsigned char pad0[0x18];
    SceneData* data;
};

extern "C" {
extern GameGlobal* g_GameGlobal;
extern int lbl_1_bss_1A60;

int gmCountMeleeCoin__Fii(int, int);
void fn_80051440(int);

void fn_1_3422C() {
    SceneData* data = g_GameGlobal->data;
    int type = 2;
    int value = data->value;

    if (lbl_1_bss_1A60 == 2) {
        type = 3;
        value += data->extra;
    }

    if (data->mode == 7) {
        type = 0;
        if (lbl_1_bss_1A60 == 2) {
            type = 1;
        }
    }

    int amount = gmCountMeleeCoin__Fii(type, value);
    if (amount < 1) {
        amount = 1;
    }

    data->total += amount;
    fn_80051440(data->total);
}
}
}

