#include <gm/gm_global.h>
#include <ty/ty_seal_listmng.h>
#include <types.h>

tySealListDataManager::tySealListDataManager(bool load) {
    unk3EA8 = false;
    unk3EAC.m_request = NULL;
    unk3EB0 = true;
    if (load) {
        if (!unk3EAC.m_request) {
            unk3EA8 = false;
            unk3EB0 = false;
            unk3EAC.readRequest("/toy/seal/ty_seal_data.dat", Heaps::MenuResource, 0, 0);
        }
    }
}

bool tySealListDataManager::isLoadFinish() {
    if (!unk3EA8) {
        if (!unk3EAC.isReady()) {
            return false;
        }
        u32 size = unk3EAC.getSize();
        setData(static_cast<u8*>(unk3EAC.getBuffer()), size);
        unk3EA8 = true;
        return true;
    }
    return true;
}

void tySealListDataManager::sortNew() {
    for (u32 i = 0; i < unk2F04; ++i) {
        unk7E4[i] = unk2F08[i];
    }
}

void tySealListDataManager::clearNarrow() {
    for (u32 i = 0; i < unkC; ++i) {
        unk1784[i] = 0;
    }
    unkC = 0;
}

u32 tySealListDataManager::getSealList(u32 count, u32 start, void** output) {
    u32 i = 0;
    for (; i < count && i + start < unk2EF4; ++i) {
        *output++ = &unk0[unk7E4[i + start]];
    }
    return i;
}

void* tySealListDataManager::getSealDataId(u32 id) {
    if (!unk3EA8 && !unk3EB0) {
        return NULL;
    }
    for (u32 i = 0; i < unk2EF4; ++i) {
        if (id == unk0[i].unk0) {
            return &unk0[i];
        }
    }
    return NULL;
}

bool isPuttingOn(Vec2f* first, u32 firstCount, Vec2f* second, u32 secondCount, bool both) {
    if (isPut(first, firstCount, second, secondCount)) {
        return true;
    }
    if (both) {
        if (isPut(second, secondCount, first, firstCount)) {
            return true;
        }
    }
    if (isCross(first, firstCount, second, secondCount)) {
        return true;
    }
    return false;
}

bool tySealListDataManager::isPutChara(int chara, u32 id) {
    if (chara == 4) {
        chara = 3;
    }
    u32 mask;
    u32 flags;
    if (chara < 32) {
        mask = 1 << chara;
        flags = static_cast<__typeof__(unk0)>(getSealDataId(id))->unk1C;
    } else {
        mask = 1 << (chara - 32);
        flags = static_cast<__typeof__(unk0)>(getSealDataId(id))->unk20;
    }
    return (flags & mask) != 0;
}

u32 tySealListDataManager::getPutCharaNum(u32 id) {
    u32 count = 0;
    for (u32 chara = 0; chara < 62; ++chara) {
        if (isPutChara(chara, id)) {
            ++count;
        }
    }
    return count;
}

u32 tySealListDataManager::getPutCharaNo(u32 id, u32 index) {
    u32 count = 0;
    for (u32 chara = 0; chara < 62; ++chara) {
        if (isPutChara(chara, id)) {
            if (count == index) {
                return chara;
            }
            ++count;
        }
    }
    return 62;
}

// TODO: clean up, give variables real names
namespace jj_800AD8F8 {
extern "C" {
void readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(void* self, const char* name, int heap, int a, int b);
extern char lbl_80455180[];

// TODO: clean up, give variables real names
int fn_800AD8F8(unsigned char* arg0) {
    if (arg0[0x3EA8] != 0) {
        return 0;
    }
    arg0[0x3EA8] = 0;
    arg0[0x3EB0] = 0;
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(arg0 + 0x3EAC, lbl_80455180, 0x2b, 0, 0);
    return 1;
}
}
}

namespace jj_800AD87C {
extern "C" {
void* getBuffer__14gfFileIOHandleCFv(void*);
void free__13gfHeapManagerFPv(void*);
void release__14gfFileIOHandleFv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void __dl__FPv(void*);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_800AD87C(char* self, int flag) {
    if (self) {
        unsigned char flagByte = self[0x3EB0];
        if (flagByte == 0) {
            free__13gfHeapManagerFPv(getBuffer__14gfFileIOHandleCFv(self + 0x3EAC));
            release__14gfFileIOHandleFv(self + 0x3EAC);
        }
        __dt__14gfFileIOHandleFv(self + 0x3EAC, -1);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_800AEC70 {
struct GG { char pad[0x30]; char* p30; };
extern GG* g_GameGlobal;
extern "C" void gmSetFlagCountSeal__FiUl(unsigned short, unsigned short);
extern "C" void gmSetFlagReferSeal__FiUl(unsigned short, int);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800AEC70(char* self) {
    char* p = self;
    int off = 0;
    unsigned i = 0;
    for (; i < *(unsigned*)(self + 0x2ef4); i++) {
        gmSetFlagCountSeal__FiUl(*(unsigned*)(*(char**)self + off), *(unsigned short*)(p + 0x14));
        gmSetFlagReferSeal__FiUl(*(unsigned*)(*(char**)self + off), (unsigned char)(self + i)[0x2724] == 0);
        p += 2;
        off += 0x64;
    }
    char* q = g_GameGlobal->p30;
    q[0x4924] |= 8;
}
}

// TODO: clean up, give variables real names
namespace jj_800AE200 {
extern "C" u8 getDistributionType__10GameGlobalFv(u8**);

struct Entry {
    u8 pad0[0x29];
    u8 mask;
    u8 pad2A[2];
    int kind;
    u8 pad30[0x34];
};

struct Context {
    Entry* entries;
    u8 pad4[8];
    u32 selectedCount;
    u32 count;
    u8 pad14[0x7D0];
    int indices[1000];
    u32 selected[1000];
    u8 flags[1000];
    u8 states[1000];
    u8 pad2EF4[0xC];
    u32 mask;
};

// TODO: clean up, give variables real names
extern "C" u32 fn_800AE200(Context* self, int mode, int skip) {
    u32 n = 0;
    switch (mode) {
    case 0: {
        int* src = (int*)self;
        u32* dst = (u32*)self;
        for (u32 i = 0; i < self->count; ++src, ++i) {
            int k = self->entries[src[0x7E4 / 4]].kind;
            if (k == 0 || (k >= 6 && k <= 0x17) ||
                (k >= 0x1C && k <= 0x1D) ||
                (k >= 0x2F && k <= 0x45 && k != 0x35 && k != 0x37)) {
                dst[0x1784 / 4] = i;
                ++dst;
                ++n;
            }
        }
        break;
    }
    case 1: {
        int* src = (int*)self;
        u32* dst = (u32*)self;
        for (u32 i = 0; i < self->count; ++src, ++i) {
            int k = self->entries[src[0x7E4 / 4]].kind;
            if (k == 1 || (k >= 0x18 && k <= 0x1B) || k == 0x35 || k == 0x37) {
                dst[0x1784 / 4] = i;
                ++dst;
                ++n;
            }
        }
        break;
    }
    case 2: {
        int* src = (int*)self;
        u32* dst = (u32*)self;
        for (u32 i = 0; i < self->count; ++src, ++i) {
            int k = self->entries[src[0x7E4 / 4]].kind;
            if (!(k == 0 || (k >= 6 && k <= 0x17) ||
                (k >= 0x1C && k <= 0x1D) ||
                (k >= 0x2F && k <= 0x45 && k != 0x35 && k != 0x37)) &&
                !(k == 1 || (k >= 0x18 && k <= 0x1B) || k == 0x35 || k == 0x37)) {
                dst[0x1784 / 4] = i;
                ++dst;
                ++n;
            }
        }
        break;
    }
    case 3: {
        int* src = (int*)self;
        u32* dst = (u32*)self;
        for (u32 i = 0; i < self->count; ++src, ++i) {
            if (self->flags[src[0x7E4 / 4]]) {
                dst[0x1784 / 4] = i;
                ++dst;
                ++n;
            }
        }
        break;
    }
    }
    if (skip == 0) {
        if (getDistributionType__10GameGlobalFv((u8**)g_GameGlobal) == 0)
            self->mask = 1;
        else
            self->mask = 2;
        u32* src = (u32*)self;
        u32* dst = (u32*)self;
        for (u32 i = 0, end = n; i < end; ++src, ++i) {
            u32 selected = src[0x1784 / 4];
            int index = self->indices[selected];
            if (self->states[index] == 1 && (self->mask & self->entries[index].mask)) {
                dst[0x1784 / 4] = selected;
                ++dst;
            } else {
                --n;
            }
        }
    }
    self->selectedCount = n;
    return n;
}
}

// TODO: clean up, give variables real names
namespace jj_800ADD1C {
struct Entry {
    int id;
    char pad04[0x10];
    u16 rank1, rank2, rank3;
    char pad1a[0xa];
    int score;
    char pad28;
    u8 mask;
    char pad2a[0x2e];
    u32 order;
    char pad5c[8];
};
struct Context {
    Entry* entries;
    char pad04[0xc];
    u32 selectedCount;
    u16 values[1000];
    u32 indices[1000];
    u32 selected[1000];
    u8 flags[1000];
    u8 states[1000];
    u32 count;
    char pad2ef8[8];
    u32 mask;
};
// TODO: clean up, give variables real names
extern "C" void fn_800ADD1C(Context* self, u32 mode, int skip) {
    switch (mode) {
    case 0:
        for (u32 i = 0; i < self->count; i++)
            self->indices[i] = self->entries[i].id - 1;
        break;
    case 1:
        for (u32 i = 0; i < self->count; i++)
            self->indices[self->entries[i].order] = i;
        break;
    case 2:
        for (u32 i = 0; i < self->count; i++)
            self->indices[i] = self->entries[i].id - 1;
        break;
    case 3:
        for (u32 i = 0; i < self->count; i++)
            self->indices[i] = self->entries[i].id - 1;
        u32 b, a;
        for (u32 i = 0; i < self->count - 1; i++) {
            for (u32 j = self->count - 1; j > i; j--) {
                a = self->indices[j-1];
                b = self->indices[j];
                if (self->entries[b].score > self->entries[a].score) {
                    self->indices[j] = a;
                    self->indices[j-1] = b;
                }
            }
        }
        break;
    case 4:
        for (u32 i = 0; i < self->count; i++)
            self->indices[i] = self->entries[i].id - 1;
        u32 d, c;
        for (u32 i = 0; i < self->count - 1; i++) {
            for (u32 j = self->count - 1; j > i; j--) {
                c = self->indices[j-1];
                d = self->indices[j];
                if (self->values[d] > self->values[c]) {
                    self->indices[j] = c;
                    self->indices[j-1] = d;
                }
            }
        }
        break;
    case 5:
        u32 value;
        u8* ptr;
        u32 previous;
        for (u32 i = 0; i < self->count; i++)
            self->indices[i] = self->entries[i].id - 1;
        for (u32 i = 0; i < self->count - 1; i++) {
            for (u32 j = self->count - 1; j > i; j--) {
                value = self->indices[j];
                ptr = (u8*)self + value;
                if ((ptr[0x2724] == 1 && self->flags[previous = self->indices[j-1]] == 0) ||
                    (ptr[0x2b0c] == 1 && self->states[previous = self->indices[j-1]] == 0)) {
                    self->indices[j] = previous;
                    self->indices[j-1] = value;
                }
            }
        }
        break;
    case 6:
        if (((u8*)g_GameGlobal->getGlobalRecordMenuDatap())[0x1a] == 0) {
            for (u32 i = 0; i < self->count; i++)
                self->indices[self->entries[i].rank1 - 1] = i;
        } else {
            for (u32 i = 0; i < self->count; i++)
                self->indices[self->entries[i].rank2 - 1] = i;
        }
        break;
    case 7:
        u32 second, first;
        for (u32 i = 0; i < self->count; i++)
            self->indices[i] = self->entries[i].id - 1;
        for (u32 i = 0; i < self->count - 1; i++) {
            for (u32 j = self->count - 1; j > i; j--) {
                first = self->indices[j-1];
                second = self->indices[j];
                if (self->entries[second].rank3 < self->entries[first].rank3) {
                    self->indices[j] = first;
                    self->indices[j-1] = second;
                }
            }
        }
        break;
    }
    if (skip == 0) {
        u32* src = (u32*)self;
        u32* dst = (u32*)self;
        u32 index;
        u32 n = 0;
        for (u32 i = 0; i < self->count; ++src, ++i) {
            index = src[0x7e4 / 4];
            if (self->states[index] == 1 && (self->mask & self->entries[index].mask)) {
                dst[0x7e4 / 4] = index;
                ++dst;
                ++n;
            }
        }
        self->selectedCount = n;
    } else {
        self->selectedCount = self->count;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800AE5E0 {
extern "C" const float lbl_805A21E8;
struct Point { float x, y; };
extern "C" int isPut__FP5Vec2fUlP5Vec2fUl(const Point* a, unsigned int na, const Point* b, unsigned int nb) {
    for (unsigned int i = 0; i < na - 1; i++) {
        Point p = { a[i].x, a[i].y };
        unsigned int count = 0;
        for (unsigned int j = 0; j < nb - 1; j++) {
            Point v = { b[j].x, b[j].y };
            Point w = { b[j+1].x, b[j+1].y };
            if (v.y == w.y && w.y == p.y) {
                float maxx = v.x < w.x ? w.x : v.x;
                if (p.x <= maxx) {
                    if (j == nb - 2) break;
                    unsigned int prev = j == 0 ? nb - 2 : j - 1;
                    float py = b[prev].y;
                    float ny = b[j+2].y;
                    if ((py < p.y && ny > p.y) || (py > p.y && ny < p.y)) count++;
                    j++;
                }
            } else {
                float low, high;
                if (v.y < w.y) { low = v.y; high = w.y; }
                else { low = w.y; high = v.y; }
                if (p.y < low || p.y > high) continue;
                if (v.y == p.y) {
                    if (!(v.x < p.x)) {
                        float py = j == 0 ? ((const float*)(b + nb))[-3] : b[j-1].y;
                        if ((v.y < w.y && v.y > py) || (v.y > w.y && v.y < py)) count++;
                    }
                } else if (w.y == p.y) {
                    if (j == nb - 2) break;
                    if (w.x < p.x) { j++; continue; }
                    float ny = b[j+2].y;
                    if (ny != p.y) {
                        if ((w.y < v.y && w.y > ny) || (w.y > v.y && w.y < ny)) count++;
                        j++;
                    }
                } else {
                    float minx, maxx;
                    if (v.x < w.x) { minx = v.x; maxx = w.x; }
                    else { minx = w.x; maxx = v.x; }
                    if (p.x < minx) count++;
                    else if (!(p.x > maxx)) {
                        if (v.x == w.x) {
                            if (p.x < v.x) count++;
                        } else {
                            float slope = (v.y - w.y) / (v.x - w.x);
                            float y = v.y + slope * p.x - slope * v.x;
                            if (slope < lbl_805A21E8) {
                                if (p.y <= y) count++;
                            } else {
                                if (p.y >= y) count++;
                            }
                        }
                    }
                }
            }
        }
        if ((count & 1) == 1) return 1;
    }
    return 0;
}
}

