// muAdvSaveLoadTask with muAdvSaveTask and muAdvLoadTask: the save and load menus of the adventure mode. Not written yet.
#include <types.h>

#include <mu/adv/mu_adv_saveload.h>

inline muRecorder::Data::~Data() { reset(0); }

void muAdvSaveLoadTask::update() {
    controllProc();
    updateScroll();
    updatePanel();
}

muAdvSaveLoadTask::SaveBox::SaveBox() {}
muAdvSaveLoadTask::SaveBox::~SaveBox() {}

void muAdvSaveLoadTask::moveScrollCore(int amount) {
    int maximum = unk1BC - 3;
    if (maximum <= 0) return;
    int selection = unk1414 - unk1428;
    unk1428 += amount;
    bool wrap = !unk1442 && unk1418 < 0;
    if (unk1428 < 0) {
        unk1443 = wrap && unk1428 <= amount;
        unk1428 = unk1443 ? maximum : 0;
    } else if (unk1428 > maximum) {
        unk1443 = wrap && unk1428 >= maximum + amount;
        unk1428 = unk1443 ? 0 : maximum;
    }
    unk1414 = unk1428 + selection;
    unk1442 = true;
}

extern const float lbl_40_rodata_B04;
extern const float lbl_40_rodata_B08;

void muAdvSaveLoadTask::initMsg() {
    unk1888.reset(MuMsg::create(1, unk1AC, unk1B0));
    unk1888->allocMsgBuf(0x100, 24);
    void* messages = unk1518.m_handle.getBuffer();
    unk1888->setMsgData(messages);
    u32 message = 0;
    for (u32 box = 0; box < 4; ++box) {
        for (int line = 0; line < 6; ++line, ++message) {
            float size = line <= 3 ? lbl_40_rodata_B04 : lbl_40_rodata_B08;
            unk1888->attachScnMdlSimple(message, unk1580[box].unk0->m_sceneModel, line, size);
            MuMsg::AlignMode align;
            switch (line) {
            case 0: case 1: case 2: case 3: align = MuMsg::Align_Center; break;
            case 4: case 5: align = MuMsg::Align_Left; break;
            }
            unk1888->setAlignMode(message, align);
        }
    }
}

bool muAdvSaveLoadTask::loading(bool advance) {
    if (unk1418 < 0) return true;
    if (unk1B4->unk100 != 0) return false;
    if (unk141C >= 0) return true;
    {
        muRecorder::Data data = unk1B4->getAdvData();
        muRecorder::Data& destination = unk1348[unk1418];
        void* buffer = data.unk0;
        data.unk0 = 0;
        destination.reset(buffer);
    }
    initSlotList(unk1418 + 1);
    if (unk1B4->unk104 != 0) {
        unk141C = unk1418;
        return false;
    }
    if (advance && unk1418 >= 0 && unk141C < 0) {
        if ((u32)unk1418 < 50) {
            ++unk1418;
            unk1B4->loadAdv(unk1418);
        } else {
            unk1418 = -1;
        }
    }
    return true;
}

muRecorder::Data muRecorder::getAdvData() {
    Data data;
    void* buffer = m_advData.unk0;
    m_advData.unk0 = 0;
    data.reset(buffer);
    return data;
}

bool utility::StandardTask::preLoad() { return true; }
bool utility::StandardTask::checkLoadFinish() const { return true; }

#include <stl/bitset>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <ut/ut_list.h>
// TODO: clean up, give variables real names
namespace jj_42DF8 {
typedef unsigned int u32;
typedef int s32;
typedef unsigned char u8;

struct GXColor {
    u8 r, g, b, a;
};

struct CopiedMatAccess {
    u32 data[13];
};

struct Model {
    u8 pad[0xC];
    void* scn;
};

extern "C" {
void __ct__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFPQ34nw4r3g3d6ScnMdlUl(
    void*, void*, u32);
void* GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(void*, bool);
void GXGetChanCtrl__Q34nw4r3g3d10ResMatChanCF12_GXChannelIDPUcP11_GXColorSrcP11_GXColorSrcP10_GXLightIDP12_GXDiffuseFnP9_GXAttnFn(void*, u32, u8*, s32*, s32*, s32*, s32*, s32*);
void GXSetChanCtrl__Q34nw4r3g3d10ResMatChanF12_GXChannelIDUc11_GXColorSrc11_GXColorSrc10_GXLightID12_GXDiffuseFn9_GXAttnFn(void*, u32, u8, s32, s32, s32, s32, s32);
void GXGetChanMatColor__Q34nw4r3g3d10ResMatChanCF12_GXChannelIDP8_GXColor(void*, u32, GXColor*);
void GXSetChanMatColor__Q34nw4r3g3d10ResMatChanF12_GXChannelID8_GXColor(void*, u32, GXColor);
}

extern "C" void fn_40_42DF8(Model* self, u32 index, s32 alpha) {
    CopiedMatAccess access;
    __ct__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFPQ34nw4r3g3d6ScnMdlUl(
        &access, self->scn, index);
    void* channels = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, true);
    u32 channel = 2;

    for (s32 i = 0; i < 2; ++i) {
        s32 source, light, diffuse, attenuation, mask;
        u8 enable;
        GXColor color;

        GXGetChanCtrl__Q34nw4r3g3d10ResMatChanCF12_GXChannelIDPUcP11_GXColorSrcP11_GXColorSrcP10_GXLightIDP12_GXDiffuseFnP9_GXAttnFn(&channels, channel, &enable, &source, &light,
                    &diffuse, &attenuation, &mask);
        light = 0;
        GXSetChanCtrl__Q34nw4r3g3d10ResMatChanF12_GXChannelIDUc11_GXColorSrc11_GXColorSrc10_GXLightID12_GXDiffuseFn9_GXAttnFn(&channels, channel, enable, source, light,
                    diffuse, attenuation, mask);
        GXGetChanMatColor__Q34nw4r3g3d10ResMatChanCF12_GXChannelIDP8_GXColor(&channels, channel, &color);
        color.a = color.a * alpha / 255;
        GXSetChanMatColor__Q34nw4r3g3d10ResMatChanF12_GXChannelID8_GXColor(&channels, channel, color);
        channel = 3;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_42F04 {

extern "C" {
extern char __files[];
extern char lbl_40_data_17D98[];
extern short lbl_40_data_17DC0[];
extern int lbl_40_rodata_A58[42];
int fprintf(void*, const char*, ...);
void abort();
void utf16to8__7MessageFPcPCw(char*, const void*);
}

struct BitReference {
    const unsigned int* words;
    unsigned int index;
    BitReference(const unsigned int* w, unsigned int i) : words(w), index(i) {}
    operator bool() const {
        unsigned int i = index;
        if (i >= 51) {
            fprintf(__files + 0xA0, lbl_40_data_17D98);
            abort();
        }
        return (words[i >> 5] & (1 << (i & 31))) != 0;
    }
};
struct Bits {
    unsigned int words[2];
    BitReference operator[](unsigned int i) const { return BitReference(words, i); }
};
typedef unsigned long long Pair;
struct Entry { int a; int used; char pad[12]; };
struct GameData {
    Entry entries[34];
    float value;
    int pad2AC;
    int selected;
    char pad2B4[0x5FC-0x2B4];
    unsigned char kind;
    char pad5FD[0x6CC-0x5FD];
    unsigned int mode;
    char pad6D0[0x4898-0x6D0];
    Bits bits;
    char pad48A0[0x4900-0x48A0];
    Pair pair;
};
struct Save {
    char pad[12];
    unsigned char enabled;
    char padD;
    char name[18];
    GameData data;
};
struct Menu {
    unsigned char enabled:1;
    unsigned char mode:1;
    unsigned char rest:6;
    unsigned char kind;
    char name[21];
    unsigned char list[42];
    unsigned char count;
    short id;
    short selected;
    short mapped;
    int value;
    int pad4C;
    Pair pair;
};
struct IdList { int values[42]; };
extern "C" void fn_40_42F04(Menu* self, short id, Save* save) {
    self->id = id;
    if (!save) {
        self->enabled = false;
        return;
    }
    IdList ids = *(const IdList*)lbl_40_rodata_A58;
    GameData* data = &save->data;
    self->enabled = save->enabled != 0;
    if (self->enabled) {
        self->kind = data->kind;
        self->pair = data->pair;
        int value = (int)data->value;
        self->count = 42;
        self->value = value;
        self->mode = data->mode;
        self->count = 0;
        for (int i=0; i<42; ++i) {
            int kind = ids.values[i];
            bool usable;
            if (i == 26) {
                usable = data->bits[29] || data->bits[31];
                if (!usable) usable = data->bits[33];
            } else {
                usable = kind >= 0;
                if (usable) usable = data->bits[kind];
            }
            if (usable) {
                int other = -1;
                switch (kind) {
                case 15: other = 14; break;
                case 4: other = 3; break;
                }
                bool hasOther = other >= 0;
                if (hasOther) hasOther = data->bits[other];
                if (hasOther) usable = false;
                if (usable) {
                    unsigned char count = self->count;
                    self->list[count] = i;
                    self->count = count + 1;
                }
            }
        }
        utf16to8__7MessageFPcPCw(self->name, save->name);
        int selected = 0;
        for (int i=0; i<34; ++i) {
            if (data->entries[i].used) selected = i;
        }
        if (self->mode) selected = data->selected;
        self->selected = selected;
        self->mapped = lbl_40_data_17DC0[(short)selected];
    }
}
}

// TODO: clean up, give variables real names
namespace jj_436B8 {
extern "C" void reset__Q210muRecorder4DataFPv(void*, int);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_40_436B8(void* p, int del) {
    if (p) {
        reset__Q210muRecorder4DataFPv(p, 0);
        if (del > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_43710 {
typedef unsigned char u8;

struct gfFileIOHandle {
    void release();
    ~gfFileIOHandle();
};

struct StageObject {
    u8 pad0[8];
    void* node;
    void* vtable;
    gfFileIOHandle handle;
};

extern "C" void clear__Q27utility8AutoFileFv(void*);
extern "C" void removeExist__6utListFP10utListNode(void*, void*);
extern "C" void __dl__FPv(void*);
extern "C" u8 lbl_40_data_182F8[];
extern "C" u8 lbl_8049E550[];

extern "C" StageObject* fn_40_43710(StageObject* self, int deleting) {
    if (self != 0) {
        self->vtable = lbl_40_data_182F8;
        clear__Q27utility8AutoFileFv(self);
        self->handle.release();
        self->handle.~gfFileIOHandle();
        if (self != 0) {
            if (self != 0) {
                removeExist__6utListFP10utListNode(lbl_8049E550, self);
                self->node = 0;
                if (self != 0) {
                    if (self->node != 0) {
                        removeExist__6utListFP10utListNode(0, self);
                        self->node = 0;
                    }
                }
            }
        }
        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_437D0 {
extern "C" void fn_40_437D0(int* value)
{
    *value = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_43A04 {

extern "C" {
void setClearDepthBufferState__11gfSceneRootFQ211gfSceneRoot9LayerTypeb(void*, int, unsigned);
void detachMuObject__5MuMsgFP8MuObject(void*, void*);
void __dt__5MuMsgFv(void*, int);
void __destroy_arr(void*, void (*)(void*, int), int, int);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void exit__6gfTaskFv(void*);
void clear__Q27utility8AutoFileFv(void*, void*);
void release__14gfFileIOHandleFv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void removeExist__6utListFP10utListNode(void*, void*);
void __dt__Q27utility12StandardTaskFv(void*, int);
void __dl__FPv(void*);
void __dt__Q217muAdvSaveLoadTask7SaveBoxFv(void*, int);
void fn_40_436B8(void*, int);
extern char lbl_40_data_181C0[];
extern char lbl_40_data_182F8[];
extern char __vt__Q27utility11AutoResFile[];
extern char lbl_8049E550[];
extern char* g_gfSceneRoot;
}

class Obj5C { char pad[0x5C]; public: virtual void f(int); };

extern "C" void* fn_40_43A04(void* self, int arg) {
    char* s = (char*)self;
    unsigned int i;
    char* p;
    if (self) {
        *(void**)(s + 0x3C) = lbl_40_data_181C0;
        setClearDepthBufferState__11gfSceneRootFQ211gfSceneRoot9LayerTypeb(g_gfSceneRoot, 8, 0);
        p = s;
        i = 0;
        do {
            detachMuObject__5MuMsgFP8MuObject(*(void**)(s + 0x1888), *(void**)(p + 0x1580));
            i++;
            p += 0xC0;
        } while (i < 4U);
        if ((void*)(s + 0x1890)) {
            void* t = *(void**)(s + 0x1890);
            if (t) exit__6gfTaskFv(t);
            *(void**)(s + 0x1890) = 0;
        }
        if ((void*)(s + 0x188C)) {
            void* t = *(void**)(s + 0x188C);
            if (t) exit__6gfTaskFv(t);
            *(void**)(s + 0x188C) = 0;
        }
        if ((void*)(s + 0x1888)) {
            __dt__5MuMsgFv(*(void**)(s + 0x1888), 1);
        }
        __destroy_arr(s + 0x1580, __dt__Q217muAdvSaveLoadTask7SaveBoxFv, 0xC0, 4);
        if ((void*)(s + 0x157C)) {
            void* o = *(void**)(s + 0x157C);
            if (o) Destroy__Q34nw4r3g3d6G3dObjFv(o);
            *(void**)(s + 0x157C) = 0;
        }
        if ((void*)(s + 0x1578)) {
            void* o = *(void**)(s + 0x1578);
            if (o) Destroy__Q34nw4r3g3d6G3dObjFv(o);
            *(void**)(s + 0x1578) = 0;
        }
        if ((void*)(s + 0x1574)) {
            void* o = *(void**)(s + 0x1574);
            if (o) ((Obj5C*)o)->f(1);
        }
        if ((void*)(s + 0x1570)) {
            void* o = *(void**)(s + 0x1570);
            if (o) ((Obj5C*)o)->f(1);
        }
        if ((void*)(s + 0x156C)) {
            void* o = *(void**)(s + 0x156C);
            if (o) ((Obj5C*)o)->f(1);
        }
        if ((void*)(s + 0x1568)) {
            void* o = *(void**)(s + 0x1568);
            if (o) ((Obj5C*)o)->f(1);
        }
        if ((void*)(s + 0x1564)) {
            void* o = *(void**)(s + 0x1564);
            if (o) ((Obj5C*)o)->f(1);
        }
        if ((void*)(s + 0x1560)) {
            void* o = *(void**)(s + 0x1560);
            if (o) Destroy__Q34nw4r3g3d6G3dObjFv(o);
            *(void**)(s + 0x1560) = 0;
        }
        if ((void*)(s + 0x1548)) {
            *(void**)(s + 0x1554) = lbl_40_data_182F8;
            clear__Q27utility8AutoFileFv(s + 0x1548, lbl_40_data_182F8);
            release__14gfFileIOHandleFv(s + 0x1558);
            __dt__14gfFileIOHandleFv(s + 0x1558, -1);
            if ((void*)(s + 0x1548)) {
                if ((void*)(s + 0x1548)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x1548);
                    *(void**)(s + 0x1550) = 0;
                    if ((void*)(s + 0x1548)) {
                        if (*(void**)(s + 0x1550)) {
                            removeExist__6utListFP10utListNode(0, s + 0x1548);
                            *(void**)(s + 0x1550) = 0;
                        }
                    }
                }
            }
        }
        if ((void*)(s + 0x1530)) {
            *(void**)(s + 0x153C) = lbl_40_data_182F8;
            clear__Q27utility8AutoFileFv(s + 0x1530, lbl_40_data_182F8);
            release__14gfFileIOHandleFv(s + 0x1540);
            __dt__14gfFileIOHandleFv(s + 0x1540, -1);
            if ((void*)(s + 0x1530)) {
                if ((void*)(s + 0x1530)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x1530);
                    *(void**)(s + 0x1538) = 0;
                    if ((void*)(s + 0x1530)) {
                        if (*(void**)(s + 0x1538)) {
                            removeExist__6utListFP10utListNode(0, s + 0x1530);
                            *(void**)(s + 0x1538) = 0;
                        }
                    }
                }
            }
        }
        if ((void*)(s + 0x1518)) {
            *(void**)(s + 0x1524) = lbl_40_data_182F8;
            clear__Q27utility8AutoFileFv(s + 0x1518, lbl_40_data_182F8);
            release__14gfFileIOHandleFv(s + 0x1528);
            __dt__14gfFileIOHandleFv(s + 0x1528, -1);
            if ((void*)(s + 0x1518)) {
                if ((void*)(s + 0x1518)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x1518);
                    *(void**)(s + 0x1520) = 0;
                    if ((void*)(s + 0x1518)) {
                        if (*(void**)(s + 0x1520)) {
                            removeExist__6utListFP10utListNode(0, s + 0x1518);
                            *(void**)(s + 0x1520) = 0;
                        }
                    }
                }
            }
        }
        if ((void*)(s + 0x14E4)) {
            *(void**)(s + 0x1514) = __vt__Q27utility11AutoResFile;
            *(void**)(s + 0x14F4) = __vt__Q27utility11AutoResFile + 8;
            clear__Q27utility8AutoFileFv(s + 0x14F8, __vt__Q27utility11AutoResFile);
            *(void**)(s + 0x14E4) = 0;
            *(char*)(s + 0x1510) = 0;
            if ((void*)(s + 0x14F8)) {
                *(void**)(s + 0x1504) = lbl_40_data_182F8;
                clear__Q27utility8AutoFileFv(s + 0x14F8, lbl_40_data_182F8);
                release__14gfFileIOHandleFv(s + 0x1508);
                __dt__14gfFileIOHandleFv(s + 0x1508, -1);
                if ((void*)(s + 0x14F8)) {
                    if ((void*)(s + 0x14F8)) {
                        removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x14F8);
                        *(void**)(s + 0x1500) = 0;
                        if ((void*)(s + 0x14F8)) {
                            if (*(void**)(s + 0x1500)) {
                                removeExist__6utListFP10utListNode(0, s + 0x14F8);
                                *(void**)(s + 0x1500) = 0;
                            }
                        }
                    }
                }
            }
            if ((void*)(s + 0x14E8)) {
                if ((void*)(s + 0x14E8)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x14E8);
                    *(void**)(s + 0x14F0) = 0;
                    if ((void*)(s + 0x14E8)) {
                        if (*(void**)(s + 0x14F0)) {
                            removeExist__6utListFP10utListNode(0, s + 0x14E8);
                            *(void**)(s + 0x14F0) = 0;
                        }
                    }
                }
            }
        }
        if ((void*)(s + 0x14B0)) {
            *(void**)(s + 0x14E0) = __vt__Q27utility11AutoResFile;
            *(void**)(s + 0x14C0) = __vt__Q27utility11AutoResFile + 8;
            clear__Q27utility8AutoFileFv(s + 0x14C4, __vt__Q27utility11AutoResFile);
            *(void**)(s + 0x14B0) = 0;
            *(char*)(s + 0x14DC) = 0;
            if ((void*)(s + 0x14C4)) {
                *(void**)(s + 0x14D0) = lbl_40_data_182F8;
                clear__Q27utility8AutoFileFv(s + 0x14C4, lbl_40_data_182F8);
                release__14gfFileIOHandleFv(s + 0x14D4);
                __dt__14gfFileIOHandleFv(s + 0x14D4, -1);
                if ((void*)(s + 0x14C4)) {
                    if ((void*)(s + 0x14C4)) {
                        removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x14C4);
                        *(void**)(s + 0x14CC) = 0;
                        if ((void*)(s + 0x14C4)) {
                            if (*(void**)(s + 0x14CC)) {
                                removeExist__6utListFP10utListNode(0, s + 0x14C4);
                                *(void**)(s + 0x14CC) = 0;
                            }
                        }
                    }
                }
            }
            if ((void*)(s + 0x14B4)) {
                if ((void*)(s + 0x14B4)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x14B4);
                    *(void**)(s + 0x14BC) = 0;
                    if ((void*)(s + 0x14B4)) {
                        if (*(void**)(s + 0x14BC)) {
                            removeExist__6utListFP10utListNode(0, s + 0x14B4);
                            *(void**)(s + 0x14BC) = 0;
                        }
                    }
                }
            }
        }
        if ((void*)(s + 0x147C)) {
            *(void**)(s + 0x14AC) = __vt__Q27utility11AutoResFile;
            *(void**)(s + 0x148C) = __vt__Q27utility11AutoResFile + 8;
            clear__Q27utility8AutoFileFv(s + 0x1490, __vt__Q27utility11AutoResFile);
            *(void**)(s + 0x147C) = 0;
            *(char*)(s + 0x14A8) = 0;
            if ((void*)(s + 0x1490)) {
                *(void**)(s + 0x149C) = lbl_40_data_182F8;
                clear__Q27utility8AutoFileFv(s + 0x1490, lbl_40_data_182F8);
                release__14gfFileIOHandleFv(s + 0x14A0);
                __dt__14gfFileIOHandleFv(s + 0x14A0, -1);
                if ((void*)(s + 0x1490)) {
                    if ((void*)(s + 0x1490)) {
                        removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x1490);
                        *(void**)(s + 0x1498) = 0;
                        if ((void*)(s + 0x1490)) {
                            if (*(void**)(s + 0x1498)) {
                                removeExist__6utListFP10utListNode(0, s + 0x1490);
                                *(void**)(s + 0x1498) = 0;
                            }
                        }
                    }
                }
            }
            if ((void*)(s + 0x1480)) {
                if ((void*)(s + 0x1480)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x1480);
                    *(void**)(s + 0x1488) = 0;
                    if ((void*)(s + 0x1480)) {
                        if (*(void**)(s + 0x1488)) {
                            removeExist__6utListFP10utListNode(0, s + 0x1480);
                            *(void**)(s + 0x1488) = 0;
                        }
                    }
                }
            }
        }
        if ((void*)(s + 0x1448)) {
            *(void**)(s + 0x1478) = __vt__Q27utility11AutoResFile;
            *(void**)(s + 0x1458) = __vt__Q27utility11AutoResFile + 8;
            clear__Q27utility8AutoFileFv(s + 0x145C, __vt__Q27utility11AutoResFile);
            *(void**)(s + 0x1448) = 0;
            *(char*)(s + 0x1474) = 0;
            if ((void*)(s + 0x145C)) {
                *(void**)(s + 0x1468) = lbl_40_data_182F8;
                clear__Q27utility8AutoFileFv(s + 0x145C, lbl_40_data_182F8);
                release__14gfFileIOHandleFv(s + 0x146C);
                __dt__14gfFileIOHandleFv(s + 0x146C, -1);
                if ((void*)(s + 0x145C)) {
                    if ((void*)(s + 0x145C)) {
                        removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x145C);
                        *(void**)(s + 0x1464) = 0;
                        if ((void*)(s + 0x145C)) {
                            if (*(void**)(s + 0x1464)) {
                                removeExist__6utListFP10utListNode(0, s + 0x145C);
                                *(void**)(s + 0x1464) = 0;
                            }
                        }
                    }
                }
            }
            if ((void*)(s + 0x144C)) {
                if ((void*)(s + 0x144C)) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, s + 0x144C);
                    *(void**)(s + 0x1454) = 0;
                    if ((void*)(s + 0x144C)) {
                        if (*(void**)(s + 0x1454)) {
                            removeExist__6utListFP10utListNode(0, s + 0x144C);
                            *(void**)(s + 0x1454) = 0;
                        }
                    }
                }
            }
        }
        __destroy_arr(s + 0x1348, fn_40_436B8, 4, 0x33);
        if ((void*)(s + 0x1B4)) {
            void* t = *(void**)(s + 0x1B4);
            if (t) exit__6gfTaskFv(t);
            *(void**)(s + 0x1B4) = 0;
        }
        if (self) __dt__Q27utility12StandardTaskFv(self, 0);
        if (arg > 0) {
            void* p2 = self;
            __dl__FPv(p2);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_47544 {
struct muAdvSaveTaskParam;

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void fn_40_432C0(void*, const muAdvSaveTaskParam*, int, int, int);
extern unsigned char lbl_40_data_180F0[0x94];
}

extern "C" void* create__13muAdvSaveTaskFPC18muAdvSaveTaskParamQ25Heaps8HeapTypeUl(const muAdvSaveTaskParam* param, int heapType, unsigned long p3) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x1898, heapType);
    if (p != 0) {
        fn_40_432C0(p, param, heapType, p3, 1);
        *(void**)((char*)p + 0x3c) = lbl_40_data_180F0;
        *(unsigned char*)((char*)p + 0x1422) = 1;
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_475CC {
extern "C" {
void fn_40_43A04(void* p, int flag);
void __dl__FPv(void* p);
}

extern "C" void* fn_40_475CC(void* p, int n) {
    if (p) {
        fn_40_43A04(p, 0);
        if (n > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_47624 {
extern "C" void* fn_40_47624(void* a, void* b)
{
    return b;
}
}

// TODO: clean up, give variables real names
namespace jj_4762C {
extern "C" void fn_40_42F04(void* a, int b, int c);

extern "C" void fn_40_4762C(void* arg1, int arg2) {
    char* base = (char*)arg1;
    int* field = (int*)(base + 0x1bc);
    *field = 0;
    int* ptr = (int*)(base + 4);
    for (int i = 1; i < arg2; i++) {
        int idx = *field;
        *field = idx + 1;
        int val = ptr[0x4D2];
        fn_40_42F04(base + idx * 0x58 + 0x1c0, i, val);
        ptr++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_476B8 {
extern "C" {
void exit__6gfTaskFv(void* task);
unsigned char fn_800BA3D8(void* a, void* b);
void* fn_800BEEB8(int a1, int a2, int a3, void* a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);
}

extern "C" s32 fn_40_476B8(u8* self) {
    u8* p = self + *(s32*)(self + 0x1414) * 0x58;
    s32 mode = 9;
    if ((p[0x1C0] >> 7) & 1) {
        mode = 0xB;
    }
    u8* p1888 = *(u8**)(self + 0x1888);
    s32 r30 = *(s32*)(self + 0x1560);
    s32 r29 = *(s32*)(p1888 + 4);
    s32 b = fn_800BA3D8(self + 0x100, p1888) & 0xFF;
    void* task = fn_800BEEB8(1, r29, mode, self + 0x147C, r30, *(s32*)(self + 0x143C), 0,
                             *(s32*)(self + 0x1AC), *(s32*)(self + 0x1B0), 0, 0, b);
    void* old = *(void**)(self + 0x188C);
    if (old != 0) {
        exit__6gfTaskFv(old);
    }
    *(void**)(self + 0x188C) = task;
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_47A64 {
extern "C" {
void saveAdv__10muRecorderFUl(void* a, int b);

void fn_40_47A64(void* self)
{
    saveAdv__10muRecorderFUl(*(void**)((char*)self + 0x1b4), 0);
}
}
}

// TODO: clean up, give variables real names
namespace jj_47A70 {
extern "C" {
void exit__6gfTaskFv(void* task);
unsigned char fn_800BA3D8(void* a, void* b);
void* fn_800BEEB8(int a1, int a2, int a3, void* a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);
}

extern "C" void fn_40_47A70(unsigned char* self) {
    if (self[0x1421] != 0) {
        unsigned char* p = *(unsigned char**)(self + 0x1888);
        int r31 = *(int*)(self + 0x1560);
        int r30 = *(int*)(p + 4);
        int b = fn_800BA3D8(self + 0x100, p);
        int b1b0 = *(int*)(self + 0x1B0);
        void* task = fn_800BEEB8(1, r30, 0xC, self + 0x147C, r31, *(int*)(self + 0x143C), 0, *(int*)(self + 0x1AC), b1b0, 0, 0, b & 0xFF);
        void* old = *(void**)(self + 0x188C);
        if (old != 0) {
            exit__6gfTaskFv(old);
        }
        *(void**)(self + 0x188C) = task;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_47B24 {
struct muAdvLoadTaskParam;

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void fn_40_432C0(void*, const muAdvLoadTaskParam*, int, int, int);
extern unsigned char lbl_40_data_18020[0x94];
}

extern "C" void* create__13muAdvLoadTaskFPC18muAdvLoadTaskParam(const muAdvLoadTaskParam* param) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x1898, 0x2a);
    if (p != 0) {
        fn_40_432C0(p, param, 0x2a, 0x2b, 0);
        *(void**)((char*)p + 0x3c) = lbl_40_data_18020;
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_47B90 {
extern "C" s32 fn_40_47B90(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_47B98 {
extern "C" void fn_40_42F04(void* a, int b, int c);

extern "C" void fn_40_47B98(char* base, int n) {
    *(int*)(base + 0x1bc) = 0;
    int* q = (int*)base;
    for (int i = 0; i < n; i++) {
        int cnt = *(int*)(base + 0x1bc);
        char* t = base;
        t = t + cnt * 0x58;
        char* p = t + 0x1c0;
        fn_40_42F04(p, i, q[0x1348 / 4]);
        if (i == 0 || ((p[0] >> 7) & 1)) {
            *(int*)(base + 0x1bc) = *(int*)(base + 0x1bc) + 1;
        }
        q++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_47C30 {
typedef unsigned char u8;
typedef int s32;

extern "C" {
void exit__6gfTaskFv(void* task);
unsigned char fn_800BA3D8(void* a, void* b);
void* fn_800BEEB8(int a1, int a2, int a3, void* a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);
}

extern "C" s32 fn_40_47C30(u8* self) {
    u8* p = self + *(s32*)(self + 0x1414) * 0x58;
    short idx = *(short*)(p + 0x202);
    s32 v = *(s32*)(self + idx * 4 + 0x1348);

    if (((p[0x1C0] >> 7) & 1) && v != 0) {
        u8* p1888 = *(u8**)(self + 0x1888);
        s32 r30 = *(s32*)(self + 0x1560);
        s32 r29 = *(s32*)(p1888 + 4);
        s32 b = fn_800BA3D8(self + 0x100, p1888) & 0xFF;
        void* task = fn_800BEEB8(1, r29, 0xA, self + 0x147C, r30, *(s32*)(self + 0x143C), 0,
                                 *(s32*)(self + 0x1AC), *(s32*)(self + 0x1B0), 0, 0, b);
        void* old = *(void**)(self + 0x188C);
        if (old != 0) {
            exit__6gfTaskFv(old);
        }
        *(void**)(self + 0x188C) = task;
    } else {
        u8* p1888 = *(u8**)(self + 0x1888);
        s32 r29 = *(s32*)(self + 0x1560);
        s32 r30 = *(s32*)(p1888 + 4);
        s32 b = fn_800BA3D8(self + 0x100, p1888) & 0xFF;
        void* task = fn_800BEEB8(0, r30, 0, self + 0x147C, r29, *(s32*)(self + 0x143C), 0,
                                 *(s32*)(self + 0x1AC), *(s32*)(self + 0x1B0), 0, 0, b);
        void* old = *(void**)(self + 0x188C);
        if (old != 0) {
            exit__6gfTaskFv(old);
        }
        *(void**)(self + 0x188C) = task;
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_47D80 {
extern "C" void fn_8004EFA8(void* a, void* b);
extern void* g_GameGlobal;

extern "C" {
void fn_40_47D80(char* self) {
    int idx = *(int*)(self + 0x1414);
    char* p = self + idx * 0x58;
    short s = *(short*)(p + 0x202);
    void* arg = *(void**)(self + (s << 2) + 0x1348);
    fn_8004EFA8(g_GameGlobal, arg);
    *(bool*)(self + 0x1422) = true;
}
}
}

// TODO: clean up, give variables real names
namespace jj_47DD8 {
extern "C" void fn_40_47DD8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_47DDC {
extern "C" void fn_40_47DDC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_47DE0 {
extern "C" u8 isLoaded__13muAdvLoadTaskCFv(void* self) {
    return *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x1420);
}
}

// TODO: clean up, give variables real names
namespace jj_47DE8 {
extern "C" s32 checkLoadFinish__13muVCTrialTaskCFv(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_47DF0 {
extern "C" s32 fn_40_47DF0(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_47DF8 {
extern "C" {
void fn_40_43A04(void* self, int arg);
void __dl__FPv(void* p);

void* fn_40_47DF8(void* self, int flag) {
    if (self != 0) {
        fn_40_43A04(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}
}

