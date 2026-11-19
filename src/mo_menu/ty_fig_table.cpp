#include <types.h>

#include <gf/gf_pad_system.h>
#include <mu/mu_object.h>
#include <ty/ty_fig_listmng.h>
#include <snd/snd_system.h>
#include <gf/gf_pad_status.h>
// TODO: clean up, give variables real names
namespace jj_4A88 {

struct FigListMenu {
    char pad0[0x40];
    int state;
    int result;
    char pad48[4];
    tyFigListDataManager* mgr;
    u32 list[12];
    char pad80[0x128-0x80];
    int selection;
    u32 category;
    int group;
    int pages;
    u32 page;
    u32 count;
    u32 cursor;
    char pad144[4];
    MuObject* object;
    MuObject* controller;
    char pad150[0x20c-0x150];
    u16 owned[1];
};
extern const float lbl_2_rodata_F8[];
extern char lbl_2_data_8C0[];
extern "C" {
float getFrame__16gfModelAnimationFd(gfModelAnimation*);
u32 calcSelectPad__14tyFigTableTaskFv(FigListMenu*);
void drawListPage__14tyFigTableTaskFv(FigListMenu*);
int isMove__14tyFigTableTaskFv(FigListMenu*, int, int);
int isMovePageNext__14tyFigTableTaskFv(FigListMenu*, int, int);
int isMovePageBack__14tyFigTableTaskFv(FigListMenu*, int, int);
int isChangeCategory__14tyFigTableTaskFv(FigListMenu*, int, int);
int gmGetMenuDecisionPad__Fv();
void setAnimName__8MuObjectFP14MuAnimNameDatab(MuObject*, void*, int);
}

extern "C" void processAnim__14tyFigTableTaskFv(FigListMenu* self) {
    const float* ro = lbl_2_rodata_F8;
    int changed = 0;
    switch (self->state) {
    case 0:
        if (ro[9] == getFrame__16gfModelAnimationFd(self->object->m_modelAnim))
            self->state = 1;
        break;
    case 1: {
        u32 flags = calcSelectPad__14tyFigTableTaskFv(self);
        switch (self->selection) {
        case 0:
            if (self->category == 0 && (flags & 0x22)) {
                self->category = 1;
                self->cursor = 0;
                self->page = 0;
                self->group = 0;
                for (int i = 0x17; i < 0x21; i++) {
                    self->pages = self->mgr->getPageNumKind(i);
                    if (self->pages != 0) {
                        self->group = i - 0x17;
                        break;
                    }
                }
                changed = 1;
                g_sndSystem->playSE((SndID)0x23, -1, 0, 0, -1);
            } else if (self->category == 1 && (flags & 0x41)) {
                self->category = 0;
                self->cursor = 0;
                self->page = 0;
                self->group = 0;
                for (int i = 0; i < 0x16; i++) {
                    self->pages = self->mgr->getPageNumSeries(i);
                    if (self->pages != 0) {
                        self->group = i;
                        break;
                    }
                }
                changed = 1;
                g_sndSystem->playSE((SndID)0x23, -1, 0, 0, -1);
            } else if (flags & 4) {
                changed = 1;
                self->selection++;
                g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            }
            break;
        case 1:
            if (flags & 0x41) {
                changed = isChangeCategory__14tyFigTableTaskFv(self, 1, 1);
                if (changed) g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            } else if (flags & 0x22) {
                changed = isChangeCategory__14tyFigTableTaskFv(self, 0, 1);
                if (changed) g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            } else if (flags & 4) {
                changed = 1;
                g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
                self->selection++;
            } else if (flags & 8) {
                changed = 1;
                g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
                self->selection--;
            }
            break;
        case 2:
            if (flags & 1) changed = isMove__14tyFigTableTaskFv(self, 2, 1);
            else if (flags & 2) changed = isMove__14tyFigTableTaskFv(self, 3, 1);
            else if (flags & 4) changed = isMove__14tyFigTableTaskFv(self, 1, 1);
            else if (flags & 8) {
                changed = isMove__14tyFigTableTaskFv(self, 0, 1);
                if (!changed) {
                    changed = 1;
                    self->selection--;
                    g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
                }
            } else if (flags & 0x40) changed = isMovePageBack__14tyFigTableTaskFv(self, 1, 1);
            else if (flags & 0x20) changed = isMovePageNext__14tyFigTableTaskFv(self, 1, 1);
            break;
        }
        if (changed) {
            if (self->category == 0)
                self->count = self->mgr->getFigListSeries(self->group, self->page, self->list);
            else
                self->count = self->mgr->getFigListKind(self->group + 0x17, self->page, self->list);
            drawListPage__14tyFigTableTaskFv(self);
        } else if ((flags & 0x100) && (u32)self->selection == 2) {
            if (self->owned[self->list[self->cursor]]) {
                setAnimName__8MuObjectFP14MuAnimNameDatab(self->object, lbl_2_data_8C0 + 0x14, 0);
                self->state = 2;
                self->result = 1;
                g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
            } else {
                g_sndSystem->playSE((SndID)3, -1, 0, 0, -1);
            }
        } else if (flags & 0x200) {
            setAnimName__8MuObjectFP14MuAnimNameDatab(self->object, lbl_2_data_8C0 + 0x14, 0);
            self->state = 2;
            self->result = 2;
            g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
        }
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(gmGetMenuDecisionPad__Fv(), &status);
        if (status.m_error == 0) {
            float frame;
            switch (status.m_controllerType) {
            case 0: frame = ro[4]; break;
            case 1: frame = ro[6]; break;
            case 2: frame = ro[5]; break;
            case 3: frame = ro[10]; break;
            default: frame = ro[11]; break;
            }
            self->controller->setFrameVisible(frame);
        }
        break;
    }
    case 2:
        if (ro[12] == getFrame__16gfModelAnimationFd(self->object->m_modelAnim)) {
            self->state = 3;
            self->result = 3;
        }
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_507C {

extern "C" {
extern void clamp__11gfPadStatusFv(gfPadStatus* status);
extern void fn_800B9984(void* arg);
extern int fn_800FAD8C(void* arg);
}

extern gfPadSystem* g_gfPadSystem;

struct SubObj {
    char _0[0x10];
};

struct Self {
    char _0[0x80];
    SubObj sub;
    u32 _0x90;
    char _0x94[0x8];
    u32 _0x9c;
};

extern "C" u32 calcSelectPad__14tyFigTableTaskFv(Self* self) {
    u32 result = 0;
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    clamp__11gfPadStatusFv(&status);
    fn_800B9984(&self->sub);

    status.m_buttonsPressedThisFrame2.bits |= self->_0x90;
    u32 buttons2 = self->_0x9c;

    u32 buttons = status.m_buttonsPressedThisFrame2.bits;
    if (buttons & 0x8)
        result |= 0x8;
    if (buttons & 0x4)
        result |= 0x4;
    if (buttons & 0x1)
        result |= 0x1;
    if (buttons & 0x2)
        result |= 0x2;

    if (buttons2 & 0x1)
        result |= 0x40;
    if (buttons2 & 0x2)
        result |= 0x20;

    int ret = fn_800FAD8C(&self->sub);
    if (ret & 0x1)
        result |= 0x1000;
    if (ret & 0x2)
        result |= 0x100;
    if (ret & 0x4)
        result |= 0x200;
    if (ret & 0x8)
        result |= 0x40;
    if (ret & 0x10)
        result |= 0x20;

    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_5704 {
struct FigListMenu {
    char pad[0x13c];
    unsigned int count;
    unsigned int cursor;
};
extern "C" int isMovePageNext__14tyFigTableTaskFv(FigListMenu*, int, int);
extern "C" int isMovePageBack__14tyFigTableTaskFv(FigListMenu*, int, int);
extern "C" int isMove__14tyFigTableTaskFv(FigListMenu* self, int direction, int execute) {
    unsigned int current = self->cursor;
    unsigned int next = current;
    switch (direction) {
    case 0:
        if (current >= 3) next = current - 3;
        break;
    case 1:
        if (self->count - 1 - current >= 3) next = current + 3;
        break;
    case 2:
        if (current % 3 != 0) next = current - 1;
        else return isMovePageBack__14tyFigTableTaskFv(self, execute, 0);
        break;
    case 3:
        if (current % 3 != 2 && current != self->count - 1) next = current + 1;
        else return isMovePageNext__14tyFigTableTaskFv(self, execute, 0);
        break;
    }
    if (next != current) {
        if (execute) {
            self->cursor = next;
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
        }
        return 1;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_5838 {
extern "C" {
u32 getFigListSeries__20tyFigListDataManagerFlUlPUl(void* a, s32 b, u32 c, u32* d);
u32 getFigListKind__20tyFigListDataManagerFlUlPUl(void* a, s32 b, u32 c, u32* d);
s32 getPageNumSeries__20tyFigListDataManagerFl(void* a, s32 b);
s32 getPageNumKind__20tyFigListDataManagerFl(void* a, s32 b);
void playSE__9sndSystemF5SndIDiiii(void* a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void* g_sndSystem;
}

struct TrophiesPage {
    char pad0[0x4c];
    void* mgr;            // 0x4c
    u32 figList[12];      // 0x50 .. 0x80
    char pad1[0xac];      // 0x80 .. 0x12c
    s32 flag;             // 0x12c
    s32 page;             // 0x130
    s32 saved;            // 0x134
    s32 count;            // 0x138
    s32 result;           // 0x13c
    s32 cursor;           // 0x140
};

extern "C" s32 isMovePageNext__14tyFigTableTaskFv(TrophiesPage* self, s32 arg1, s32 arg2) {
    u32 newCount = (u32)self->count + 1;
    if ((u32)self->saved > newCount) {
        if (arg1 != 0) {
            u32 n;
            u32 k;
            self->count = (s32)newCount;
            if (self->flag == 0) {
                self->result = getFigListSeries__20tyFigListDataManagerFlUlPUl(self->mgr, self->page, newCount, self->figList);
            } else {
                self->result = getFigListKind__20tyFigListDataManagerFlUlPUl(self->mgr, self->page + 0x17, newCount, self->figList);
            }
            k = (u32)self->cursor / 3;
            if ((u32)self->result % 3 == 0) {
                n = (u32)self->result / 3 - 1;
            } else {
                n = (u32)self->result / 3;
            }
            self->cursor = 0;
            if (arg2 == 0) {
                u32 i = 0;
                while (i < n && i < k) {
                    i++;
                    self->cursor = self->cursor + 3;
                }
            }
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1ee8, -1, 0, 0, -1);
        }
        return 1;
    }
    if (self->flag == 0) {
        s32 page = self->page;
        while (page < 0x15) {
            s32 found = getPageNumSeries__20tyFigListDataManagerFl(self->mgr, page + 1);
            if (found != 0) {
                if (arg1 != 0) {
                    u32 r;
                    u32 n;
                    u32 k;
                    self->saved = found;
                    self->page = page + 1;
                    self->count = 0;
                    r = getFigListSeries__20tyFigListDataManagerFlUlPUl(self->mgr, page + 1, 0, self->figList);
                    self->result = (s32)r;
                    k = (u32)self->cursor / 3;
                    if (r % 3 == 0) {
                        n = r / 3 - 1;
                    } else {
                        n = r / 3;
                    }
                    self->cursor = 0;
                    if (arg2 == 0) {
                        u32 i = 0;
                        while (i < n && i < k) {
                            i++;
                            self->cursor = self->cursor + 3;
                        }
                    }
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1ee8, -1, 0, 0, -1);
                }
                return 1;
            }
            page++;
        }
    } else {
        s32 page = self->page;
        while (page < 9) {
            s32 found = getPageNumKind__20tyFigListDataManagerFl(self->mgr, page + 0x18);
            if (found != 0) {
                if (arg1 != 0) {
                    u32 r;
                    u32 n;
                    u32 k;
                    s32 np = page + 1;
                    self->saved = found;
                    self->page = np;
                    self->count = 0;
                    r = getFigListKind__20tyFigListDataManagerFlUlPUl(self->mgr, np + 0x17, 0, self->figList);
                    self->result = (s32)r;
                    k = (u32)self->cursor / 3;
                    if (r % 3 == 0) {
                        n = r / 3 - 1;
                    } else {
                        n = r / 3;
                    }
                    self->cursor = 0;
                    if (arg2 == 0) {
                        u32 i = 0;
                        while (i < n && i < k) {
                            i++;
                            self->cursor = self->cursor + 3;
                        }
                    }
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1ee8, -1, 0, 0, -1);
                }
                return 1;
            }
            page++;
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_5B90 {
extern "C" {
u32 getFigListSeries__20tyFigListDataManagerFlUlPUl(void* a, s32 b, u32 c, u32* d);
u32 getFigListKind__20tyFigListDataManagerFlUlPUl(void* a, s32 b, u32 c, u32* d);
s32 getPageNumSeries__20tyFigListDataManagerFl(void* a, s32 b);
s32 getPageNumKind__20tyFigListDataManagerFl(void* a, s32 b);
void playSE__9sndSystemF5SndIDiiii(void* a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void* g_sndSystem;
}

struct TrophiesPage {
    char pad0[0x4c];
    void* mgr;            // 0x4c
    u32 figList[12];      // 0x50 .. 0x80
    char pad1[0xac];      // 0x80 .. 0x12c
    s32 flag;             // 0x12c
    s32 page;             // 0x130
    s32 saved;            // 0x134
    s32 count;            // 0x138
    s32 result;           // 0x13c
    s32 cursor;           // 0x140
};

extern "C" s32 isMovePageBack__14tyFigTableTaskFv(TrophiesPage* self, s32 arg1, s32 arg2) {
    s32 count = self->count;
    if (count != 0) {
        if (arg1 != 0) {
            s32 flag = self->flag;
            s32 newCount = count - 1;
            self->count = newCount;
            if (flag == 0) {
                self->result = getFigListSeries__20tyFigListDataManagerFlUlPUl(self->mgr, self->page, (u32)newCount, self->figList);
            } else {
                self->result = getFigListKind__20tyFigListDataManagerFlUlPUl(self->mgr, self->page + 0x17, (u32)newCount, self->figList);
            }
            if (arg2 != 0) {
                self->cursor = 0;
            } else {
                self->cursor = self->cursor + 2;
            }
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1ee8, -1, 0, 0, -1);
        }
        return 1;
    }
    s32 page;
    s32 result;
    u32 R;
    u32 C;
    if (self->flag == 0) {
        page = self->page;
        while (page > 0) {
            result = getPageNumSeries__20tyFigListDataManagerFl(self->mgr, page - 1);
            if (result != 0) {
                if (arg1 != 0) {
                    s32 newPage = page - 1;
                    s32 newCount = result - 1;
                    self->saved = result;
                    self->page = newPage;
                    self->count = newCount;
                    R = getFigListSeries__20tyFigListDataManagerFlUlPUl(self->mgr, newPage, (u32)newCount, self->figList);
                    self->result = R;
                    if (arg2 != 0) {
                        self->cursor = 0;
                    } else {
                        C = (u32)self->cursor;
                        for (;;) {
                            if (R > C + 2) {
                                self->cursor = C + 2;
                                break;
                            }
                            if (R > C) {
                                self->cursor = C + (R % 3) - 1;
                                break;
                            }
                            C -= 3;
                        }
                    }
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1ee8, -1, 0, 0, -1);
                }
                return 1;
            }
            page--;
        }
    } else {
        page = self->page;
        while (page > 0) {
            result = getPageNumKind__20tyFigListDataManagerFl(self->mgr, page + 0x16);
            if (result != 0) {
                if (arg1 != 0) {
                    s32 newPage = page - 1;
                    s32 newCount = result - 1;
                    self->saved = result;
                    self->page = newPage;
                    self->count = newCount;
                    R = getFigListKind__20tyFigListDataManagerFlUlPUl(self->mgr, newPage + 0x17, (u32)newCount, self->figList);
                    self->result = R;
                    if (arg2 != 0) {
                        self->cursor = 0;
                    } else {
                        C = (u32)self->cursor;
                        for (;;) {
                            if (R > C + 2) {
                                self->cursor = C + 2;
                                break;
                            }
                            if (R > C) {
                                self->cursor = C + (R % 3) - 1;
                                break;
                            }
                            C -= 3;
                        }
                    }
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1ee8, -1, 0, 0, -1);
                }
                return 1;
            }
            page--;
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_5E50 {
typedef long s32;
typedef unsigned long u32;

class tyFigListDataManager {
public:
    s32 getPageNumSeries(long page);
    u32 getFigListSeries(long page, unsigned long idx, unsigned long* out);
    s32 getPageNumKind(long page);
    u32 getFigListKind(long page, unsigned long idx, unsigned long* out);
};

struct FigListMenu {
    char pad0[0x4c];
    tyFigListDataManager* mgr; // 0x4c
    u32 unk50;                 // 0x50
    char pad1[0x12c - 0x54];
    s32 unk12c;
    s32 unk130;
    s32 unk134;
    u32 unk138;
    u32 unk13c;
    u32 unk140;
};

extern "C" {
int isChangeCategory__14tyFigTableTaskFv(FigListMenu* self, long r4, long r5) {
    long i;
    long n;
    s32 page;

    if (self->unk12c == 0) {
        if (r4 != 0) {
            for (i = self->unk130; i > 0; i--) {
                page = self->mgr->getPageNumSeries(i - 1);
                if (page != 0) {
                    if (r5 != 0) {
                        self->unk134 = page;
                        self->unk130 = i - 1;
                        self->unk138 = 0;
                        self->unk140 = 0;
                        self->unk13c = self->mgr->getFigListSeries(i - 1, 0, &self->unk50);
                    }
                    return 1;
                }
            }
        } else {
            for (i = self->unk130; i < 0x15; i++) {
                page = self->mgr->getPageNumSeries(i + 1);
                if (page != 0) {
                    if (r5 != 0) {
                        self->unk134 = page;
                        self->unk130 = i + 1;
                        self->unk138 = 0;
                        self->unk140 = 0;
                        self->unk13c = self->mgr->getFigListSeries(i + 1, 0, &self->unk50);
                    }
                    return 1;
                }
            }
        }
    } else {
        if (r4 != 0) {
            for (i = self->unk130; i > 0; i--) {
                page = self->mgr->getPageNumKind(i + 0x16);
                if (page != 0) {
                    if (r5 != 0) {
                        n = i - 1;
                        self->unk134 = page;
                        self->unk130 = n;
                        self->unk138 = 0;
                        self->unk140 = 0;
                        self->unk13c = self->mgr->getFigListKind(n + 0x17, 0, &self->unk50);
                    }
                    return 1;
                }
            }
        } else {
            for (i = self->unk130; i < 9; i++) {
                page = self->mgr->getPageNumKind(i + 0x18);
                if (page != 0) {
                    if (r5 != 0) {
                        n = i + 1;
                        self->unk134 = page;
                        self->unk130 = n;
                        self->unk138 = 0;
                        self->unk140 = 0;
                        self->unk13c = self->mgr->getFigListKind(n + 0x17, 0, &self->unk50);
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_6050 {

struct FigListMenu {
    char pad0[0x40];
    int state;
    int result;
    char pad48[4];
    tyFigListDataManager* mgr;
    u32 list[12];
    char pad80[0x128-0x80];
    int selection;
    u32 category;
    int group;
    int pages;
    u32 page;
    u32 count;
    u32 cursor;
    char pad144[4];
    void* object;
    void* controller;
    char pad150[0x20c-0x150];
};

extern "C" void drawListPage__14tyFigTableTaskFv(FigListMenu*);
extern "C" void setFigureData__14tyFigTableTaskFv(FigListMenu*, u32, s32);

extern "C" void setFigureData__14tyFigTableTaskFv(FigListMenu* self, u32 id, s32 arg2) {
    tyFigListData* data = &self->mgr->m_data[id];
    self->selection = 2;
    self->category = (u32)arg2;

    if (arg2 == 0) {
        self->group = data->series;
        self->pages = (int)self->mgr->getPageNumSeries(data->series);
        u32 i = 0;
        while (i < (u32)self->pages) {
            u32 count = self->mgr->getFigListSeries(data->series, i, self->list);
            for (u32 j = 0; j < count; j++) {
                if (self->list[j] == id) {
                    self->count = count;
                    self->page = i;
                    self->cursor = j;
                    drawListPage__14tyFigTableTaskFv(self);
                    return;
                }
            }
            i++;
        }
        setFigureData__14tyFigTableTaskFv(self, self->mgr->nextIdSeries(id), arg2);
    } else {
        self->group = data->kind - 0x17;
        self->pages = (int)self->mgr->getPageNumKind(data->kind);
        u32 i = 0;
        while (i < (u32)self->pages) {
            u32 count = self->mgr->getFigListKind(data->kind, i, self->list);
            for (u32 j = 0; j < count; j++) {
                if (self->list[j] == id) {
                    self->count = count;
                    self->page = i;
                    self->cursor = j;
                    drawListPage__14tyFigTableTaskFv(self);
                    return;
                }
            }
            i++;
        }
        setFigureData__14tyFigTableTaskFv(self, self->mgr->nextIdKind(id), arg2);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_61FC {
typedef unsigned char u8;

class MenuObject {
    u8 padding[0x5C];

public:
    virtual ~MenuObject();
};

extern "C" {
extern u8 lbl_2_data_B00[];
extern void* g_gfSceneRoot;

void remove__11gfSceneRootFPQ34nw4r3g3d6ScnMdl(void*, void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void __dt__5MuMsgFv(void*, int);
void exit__6gfTaskFv(void*);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* __dt__14tyFigTableTaskFv(u8* object, int deleting) {
    if (object) {
        *reinterpret_cast<void**>(object + 0x3C) = lbl_2_data_B00;
        remove__11gfSceneRootFPQ34nw4r3g3d6ScnMdl(
            g_gfSceneRoot, *reinterpret_cast<void**>(object + 0x1C4));

        u8* cursor = object;
        for (unsigned int i = 0; i < 15; ++i, cursor += 4) {
            void* entry = *reinterpret_cast<void**>(cursor + 0x1C4);
            if (entry) {
                Destroy__Q34nw4r3g3d6G3dObjFv(entry);
                *reinterpret_cast<void**>(cursor + 0x1C4) = 0;
            }
        }

        cursor = object;
        for (unsigned int i = 0; i < 31; ++i, cursor += 4) {
            MenuObject* entry = *reinterpret_cast<MenuObject**>(cursor + 0x148);
            if (entry) {
                delete entry;
                *reinterpret_cast<void**>(cursor + 0x148) = 0;
            }
        }

        __dt__5MuMsgFv(*reinterpret_cast<void**>(object + 0x204), 1);
        exit__6gfTaskFv(*reinterpret_cast<void**>(object + 0x200));
        *reinterpret_cast<void**>(object + 0x200) = 0;
        __dt__6gfTaskFv(object, 0);

        if (deleting > 0)
            __dl__FPv(object);
    }

    return object;
}
}
}

