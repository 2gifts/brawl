#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <st/operator/st_operator_rule_kumite.h>
#include <types.h>

stOperatorRuleKumite::stOperatorRuleKumite(u8 arg) : stOperatorRuleMelee(arg) { }

stOperatorRuleKumite::~stOperatorRuleKumite() { }

bool stOperatorRuleKumite::isSuddenDeath() {
    return false;
}

void stOperatorRuleKumite::calcScore() {
    setNormalResultInfo();
}

void stOperatorRuleKumite::notifyEventSetupCorps() {
    if (m_condition->m_meleeInitData.m_corpsKind >= 5 &&
        m_condition->m_meleeInitData.m_corpsKind <= 10) {
        initCorpsRule(g_GameGlobal->m_corps);
    }
}

bool stOperatorRuleKumite::notifyEventDead(int entryId, u32 playerIndex, int arg) {
    if (m_condition->m_playersInitData[playerIndex].m_state == 1 &&
        m_condition->m_playersInitData[playerIndex].m_characterKind < 0x2E) {
        g_sndSystem->playSE(static_cast<SndID>(0x1B), -1, 0, 0, -1);
    }
    return stOperatorRuleMelee::notifyEventDead(entryId, playerIndex, arg);
}

void stOperatorRuleKumite::createRandomMyCharaList(u32 characterKind) {
    int i;
    int last = unk260 - 1;
    for (i = 0; i < unk260; i++) {
        int index = randi(last);
        u8 value = unk268[i];
        unk268[i] = unk268[index];
        unk268[index] = value;
    }
    if (unk268[0] == characterKind) {
        u8 value = unk268[0];
        unk268[0] = unk268[1];
        unk268[1] = value;
    }
    for (int i = 0; i < unk260; i++) {
        switch (unk268[i]) {
        case 3:
        case 4: {
            gmCharacterKind value = randi(99) < 50 ? Character_Samus : Character_ZeroSuitSamus;
            unk268[i] = value;
            break;
        }
        case 14:
        case 15: {
            gmCharacterKind value = randi(99) < 50 ? Character_Zelda : Character_Sheik;
            unk268[i] = value;
            break;
        }
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
            int value = randi(99);
            if (value < 33) {
                unk268[i] = 29;
            } else if (value < 66) {
                unk268[i] = 31;
            } else {
                unk268[i] = 33;
            }
            break;
        }
    }
    unk264 = 0;
}

#include <if/if_mngr.h>
// TODO: clean up, give variables real names
namespace jj_253194 {
typedef unsigned char u8;
typedef int s32;
typedef long long s64;

struct Info {
    u8 pad0[0x14];
    u8 kind;
    u8 pad15[0x40 - 0x15];
    s32 w40;
    s32 w44;
    u8 pad48[0x98 - 0x48];
    u8 b98;
    u8 pad99[0x9E - 0x99];
    u8 b9e;
    u8 pad9f[0xF4 - 0x9F];
    u8 bf4;
    u8 padf5[0xFA - 0xF5];
    u8 bfa;
};

struct Obj {
    u8 pad0[0x44];
    Info* info;
    u8 pad48[0xD3 - 0x48];
    u8 bd3;
    u8 padd4[0xF8 - 0xD4];
    s32 wf8;
    u8 padfc[0x100 - 0xFC];
    s32 w100;
    u8 pad104[0x1B4 - 0x104];
    s32 w1b4;
    s32 w1b8;
    u8 pad1bc[0x25C - 0x1BC];
    s32 w25c;
    s32 w260;
    s32 w264;
    u8 arr268[0x2C];
    u8 pad294[0x2A8 - 0x294];
    s32 w2a8;
    s32 w2ac;
    s32 w2b0;
    s32 w2b4;
};

extern "C" void fn_27_2498DC(Obj* p);
extern "C" void createRandomMyCharaList__20stOperatorRuleKumiteFUl(Obj* p, int v);
extern "C" int gmCheckCharaAccept__FUxi(int a, int b, int bit);
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
extern "C" int getNumCharColor__6muMenuFiii(int kind);
extern "C" s32 lbl_27_data_52980[];

extern "C" void fn_27_253194(Obj* self) {
    fn_27_2498DC(self);
    Info* info = self->info;
    self->w2a8 = info->b98;
    self->w2b0 = info->b9e;
    if (self->w1b8 == 1) {
        self->w2ac = 0x3E;
    } else {
        self->w2ac = info->bf4;
        self->w2b4 = info->bfa;
    }

    if (self->w2a8 == self->w2ac) {
        s32 n;
        s32 v = self->w2b0;
        s32 k = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(self->w2a8);
        s32 c = (k == 0x28) ? 1 : getNumCharColor__6muMenuFiii(k);
        s32 b4;
        s32 cur = self->w2b0;
        do {
            n = v + 1;
            v = (n >= c) ? 0 : n;
        } while (cur == v || ((b4 = self->w2b4), b4 == v));
        s32 b = b4;
        do {
            n = b + 1;
            b = (n >= c) ? 0 : n;
        } while (cur == b || b4 == b || v == b);
        self->w2b0 = v;
        self->w2b4 = b;
    } else {
        s32 k = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(self->w2a8);
        s32 c = (k == 0x28) ? 1 : getNumCharColor__6muMenuFiii(k);
        s32 n = self->w2b0 + 1;
        self->w2b0 = (n >= c) ? 0 : n;
        if (self->w2ac != 0x3E) {
            s32 k2 = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(self->w2ac);
            s32 c2 = (k2 == 0x28) ? 1 : getNumCharColor__6muMenuFiii(k2);
            s32 m = self->w2b4 + 1;
            self->w2b4 = (m >= c2) ? 0 : m;
        }
    }

    self->w260 = 0;
    self->w264 = 0;
    for (s32 i = 0; i < 0x2C; i++) {
        if (i != self->w2a8 && i != self->w2ac) {
            if (!((s64)(1 << i) & 0xFFFFFFFFC0068010LL)) {
                if (gmCheckCharaAccept__FUxi(self->info->w40, self->info->w44, i) != 0) {
                    self->arr268[self->w260] = (u8)i;
                    self->w260 = self->w260 + 1;
                }
            }
        }
    }

    createRandomMyCharaList__20stOperatorRuleKumiteFUl(self, 0xFF);
    switch (self->info->kind) {
    case 5:
        self->w25c = (self->w1b8 == 1) ? lbl_27_data_52980[0] : lbl_27_data_52980[1];
        self->bd3 = 1;
        self->w100 = 3;
        self->w1b4 = 2;
        break;
    case 6:
        self->w25c = (self->w1b8 == 1) ? lbl_27_data_52980[2] : lbl_27_data_52980[3];
        self->bd3 = 1;
        self->w100 = 3;
        self->w1b4 = 2;
        break;
    case 7:
        self->w25c = (self->w1b8 == 1) ? lbl_27_data_52980[4] : lbl_27_data_52980[5];
        self->wf8 = 2;
        self->w100 = 3;
        break;
    case 8:
        self->w25c = (self->w1b8 == 1) ? lbl_27_data_52980[6] : lbl_27_data_52980[7];
        self->wf8 = 2;
        self->w100 = 3;
        break;
    case 9:
        self->w25c = (self->w1b8 == 1) ? lbl_27_data_52980[8] : lbl_27_data_52980[9];
        break;
    case 10:
        self->w25c = (self->w1b8 == 1) ? lbl_27_data_52980[10] : lbl_27_data_52980[11];
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_253604 {
extern "C" float randf__Fv();
extern float lbl_27_rodata_4100;

struct Obj {
    char pad0[0x1d4];
    int idx;
    char pad1[0x1e0 - 0x1d8];
    unsigned char active;
    char pad2[0x25c - 0x1e1];
    unsigned char* table;
    char pad3[4];
    int off;
    unsigned char arr[0x40];
    int a8;
    int ac;
    int b0;
    int b4;
};

extern "C" int fn_27_253604(Obj* self, unsigned char* out1, unsigned char* out2) {
    while (self->active != 0) {
        unsigned char* e = self->table + self->idx * 16;
        unsigned char op = e[0];
        if (op == 0xFE) {
            *out1 = e[2];
            *out2 = 0;
            if (*out1 == 0xFC) {
                unsigned char v;
                if (lbl_27_rodata_4100 < randf__Fv()) {
                    v = 0xFD;
                } else {
                    v = 0xFE;
                }
                *out1 = v;
            }
            unsigned char c = *out1;
            if (c == 0xFD) {
                *out1 = self->a8;
                *out2 = self->b0;
            } else if (c == 0xFE) {
                *out1 = self->ac;
                *out2 = self->b4;
            } else if (c == 0xFF) {
                *out1 = ((unsigned char*)self + self->off)[0x268];
            }
            return 1;
        }
        if (op == 0xFF) {
            self->active = 0;
        } else {
            self->idx = op;
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_253724 {
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
extern "C" int getFighterColorFileNo__6muMenuFii(int kind, int costume);
extern "C" s32 randi__Fl(long range);
extern "C" void createRandomMyCharaList__20stOperatorRuleKumiteFUl(void *self, u32 arg1);
extern f32 lbl_27_rodata_4104;

class gfTask_base {
public:
    char pad[0x3c];
};

class Obj : public gfTask_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void *slot37();
};

struct MuFnObj {
    char pad0[0x1b8];
    s32 w1b8;
    s32 w1bc;
    char pad1[0x10];
    s32 w1d0;
    s32 w1d4;
    char pad2[0x84];
    u8 *p25c;
    s32 w260;
    s32 w264;
};

struct Ent {
    char p0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;
    char p5[3];
    f32 f8;
    f32 fC;
};

struct Out {
    u8 b0;
    u8 b1;
    s8 b2;
    char p3;
    u8 b4;
    s8 b5;
    s8 b6;
    char p7;
    s8 b8;
    s8 b9;
    char pA;
    u8 bB;
    char pC[0x12];
    u8 b1E;
    u8 b1F;
    char p20[0xC];
    f32 f2C;
    char p30[4];
    f32 f34;
    f32 f38;
};

extern "C" void fn_27_253724(MuFnObj *self, Obj *obj, u8 kind, u8 costume, s8 arg4) {
    Out *o = (Out *)obj->slot37();
    Ent *e = (Ent *)(self->p25c + (self->w1d4 << 4));
    s32 sel;
    s32 n;
    s32 avail;
    s32 t;

    o->b0 = kind;
    o->b1 = 1;
    o->b2 = arg4;
    o->b4 = 1;
    o->bB = 1;

    sel = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
    o->b5 = (sel == 0x28) ? 0 : getFighterColorFileNo__6muMenuFii(sel, costume);
    o->b6 = costume;
    o->b1E = e->b4;
    o->b1F = e->b3;
    o->f2C = 0.66666f;
    o->f34 = e->f8;
    o->f38 = e->fC;

    o->b9 = (randi__Fl(0x40) < 0x20) ? 1 : -1;

    if (kind >= 0x2e) {
        o->b8 = arg4 + 0x14;
    } else {
        o->b8 = arg4 + 0xa;
    }

    avail = self->w1bc - self->w1b8;
    n = e->b1;
    if (avail < n) {
        n = avail;
    }
    self->w1d0 = n;

    if (e->b2 == 0xff) {
        t = self->w264 + 1;
        self->w264 = t;
        if (self->w260 <= t) {
            createRandomMyCharaList__20stOperatorRuleKumiteFUl(self, kind);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_253888 {

struct Sub {
    char _00[0x08];
    u8 m_08;
    char _09[0x14 - 0x09];
    u8 m_14;
    char _15[0x98 - 0x15];
    u8 m_98;
    char _99[0x9E - 0x99];
    u8 m_9E;
    char _9F[0xF4 - 0x9F];
    u8 m_F4;
    char _F5[0xFA - 0xF5];
    u8 m_FA;
};

struct Self {
    char _00[0x44];
    Sub* m_44;
    char _48[0x75 - 0x48];
    u8 m_75;
    char _76[0xE0 - 0x76];
    s32 m_E0;
    u32 m_E4;
    char _E8[0x1B8 - 0xE8];
    s32 m_1B8;
    char _1BC[0x1C4 - 0x1BC];
    s32 m_1C4;
    char _1C8[0x258 - 0x1C8];
    u8 m_258;
};

struct Obj1 {
    char _00[0x9C];
    u16 m_9C;
    char _9E[0xB0 - 0x9E];
    u32 m_B0;
    u32 m_B4;
    u32 m_B8;
    u32 m_BC;
    u32 m_C0;
    u32 m_C4;
};

struct Rec {
    char _000[0x34D4];
    u16 m34D4;
    u16 m34D6;
    char _34D8[0x34E4 - 0x34D8];
    u32 m34E4;
    u32 m34E8;
    u32 m34EC;
    u32 m34F0;
    u32 m34F4;
    u32 m34F8;
    char _34FC[0x3530 - 0x34FC];
    u8 m3530[0x18];
    u16 m3548;
    char _354A[0x3554 - 0x354A];
    u32 m3554;
    u32 m3558;
    u32 m355C;
    u32 m3560;
    u32 m3564;
    u32 m3568;
    char _356C[0x3574 - 0x356C];
    u8 m3574[0x18];
};

struct IfC {
    u8 m_00;
    char _01[0x178 - 0x01];
    s32 m_178;
};

extern "C" {
int exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(int);
void* getGlobalRecordCharaNumDatap__10GameGlobalFi(void*, int);
int isClearGetterOpen__Fi(int);
void fn_80052AC0(int, int);

void fn_27_253888(Self* self) {
    if (self->m_75 != 0) {
        s32 r5 = 0;
        Sub* sub = self->m_44;
        switch (sub->m_14) {
        case 5:
            if (self->m_1C4 >= 0xA) {
                r5 = 1;
            }
            break;
        case 6:
            if (self->m_1C4 >= 0x64) {
                r5 = 1;
            }
            break;
        }
        if (r5 != 0 && g_IfMngr != 0) {
            IfC* c = (IfC*)g_IfMngr->m_ifCenter;
            if ((c->m_00 >> 7) & 1) {
                c->m_178 = 0x78;
            }
        }
    } else {
        Sub* sub = self->m_44;
        Rec* rec;
        u8 r30 = 0;
        s32 r29 = 0;
        u32 kind = ((u32)sub->m_08 >> 2) & 0x3F;
        if (kind == 0xA) {
            if (self->m_1B8 == 1) {
                Obj1* o1 = (Obj1*)getGlobalRecordCharaNumDatap__10GameGlobalFi(g_GameGlobal, exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(sub->m_98));
                switch (self->m_44->m_14) {
                case 5:
                    if (self->m_1C4 >= 0xA) {
                        r29 = 1;
                        if (!((o1->m_9C >> 14) & 1) || self->m_E4 < o1->m_B0) {
                            r30 = 1;
                            o1->m_9C |= 0x4000;
                            o1->m_B0 = self->m_E4;
                        }
                    } else if (!((o1->m_9C >> 14) & 1) && o1->m_B0 < (u32)self->m_1C4) {
                        o1->m_B0 = self->m_1C4;
                        r30 = 1;
                    }
                    break;
                case 6:
                    if (self->m_1C4 >= 0x64) {
                        r29 = 1;
                        if (!((o1->m_9C >> 13) & 1) || self->m_E4 < o1->m_B4) {
                            r30 = 1;
                            o1->m_9C |= 0x2000;
                            o1->m_B4 = self->m_E4;
                        }
                        if (self->m_E4 <= 0x2A30 && isClearGetterOpen__Fi(0x14) != 0) {
                            fn_80052AC0(0x209, 1);
                        }
                    } else if (!((o1->m_9C >> 13) & 1) && o1->m_B4 < (u32)self->m_1C4) {
                        o1->m_B4 = self->m_1C4;
                        r30 = 1;
                    }
                    break;
                case 7:
                    if (!((o1->m_9C >> 12) & 1) && self->m_E0 == 0) {
                        o1->m_9C |= 0x1000;
                    }
                    if (o1->m_B8 < (u32)self->m_1C4) {
                        o1->m_B8 = self->m_1C4;
                        r30 = 1;
                    }
                    break;
                case 8:
                    if (!((o1->m_9C >> 11) & 1) && self->m_E0 == 0) {
                        o1->m_9C |= 0x800;
                    }
                    if (o1->m_BC < (u32)self->m_1C4) {
                        o1->m_BC = self->m_1C4;
                        r30 = 1;
                    }
                    break;
                case 9:
                    if (o1->m_C0 < (u32)self->m_1C4) {
                        o1->m_C0 = self->m_1C4;
                        r30 = 1;
                    }
                    break;
                case 10:
                    if (o1->m_C4 < (u32)self->m_1C4) {
                        o1->m_C4 = self->m_1C4;
                        r30 = 1;
                    }
                    if (self->m_1C4 >= 0xA && isClearGetterOpen__Fi(0x3F) != 0) {
                        fn_80052AC0(0x2A5, 1);
                    }
                    break;
                }
            } else {
                rec = (Rec*)g_GameGlobal->m_record;
                switch (sub->m_14) {
                case 5:
                    if (self->m_1C4 >= 0xA) {
                        r29 = 1;
                        if (!((rec->m34D4 >> 14) & 1) || self->m_E4 < rec->m34E4) {
                            r30 = 1;
                            rec->m34D4 |= 0x4000;
                            rec->m34E4 = self->m_E4;
                        }
                    } else if (!((rec->m34D4 >> 14) & 1) && rec->m34E4 < (u32)self->m_1C4) {
                        rec->m34E4 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m34D6 |= 0x4000;
                        rec->m3530[0] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3530[1] = self->m_44->m_9E;
                        rec->m3530[2] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3530[3] = self->m_44->m_FA;
                    }
                    break;
                case 6:
                    if (self->m_1C4 >= 0x64) {
                        r29 = 1;
                        if (!((rec->m34D4 >> 13) & 1) || self->m_E4 < rec->m34E8) {
                            r30 = 1;
                            rec->m34D4 |= 0x2000;
                            rec->m34E8 = self->m_E4;
                        }
                        if (self->m_E4 <= 0x2A30 && isClearGetterOpen__Fi(0x14) != 0) {
                            fn_80052AC0(0x209, 1);
                        }
                    } else if (!((rec->m34D4 >> 13) & 1) && rec->m34E8 < (u32)self->m_1C4) {
                        rec->m34E8 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m34D6 |= 0x2000;
                        rec->m3530[4] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3530[5] = self->m_44->m_9E;
                        rec->m3530[6] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3530[7] = self->m_44->m_FA;
                    }
                    break;
                case 7:
                    if (!((rec->m34D4 >> 12) & 1) && self->m_E0 == 0) {
                        rec->m34D4 |= 0x1000;
                    }
                    if (rec->m34EC < (u32)self->m_1C4) {
                        rec->m34EC = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m34D6 |= 0x1000;
                        rec->m3530[8] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3530[9] = self->m_44->m_9E;
                        rec->m3530[10] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3530[11] = self->m_44->m_FA;
                    }
                    break;
                case 8:
                    if (!((rec->m34D4 >> 11) & 1) && self->m_E0 == 0) {
                        rec->m34D4 |= 0x800;
                    }
                    if (rec->m34F0 < (u32)self->m_1C4) {
                        rec->m34F0 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m34D6 |= 0x800;
                        rec->m3530[12] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3530[13] = self->m_44->m_9E;
                        rec->m3530[14] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3530[15] = self->m_44->m_FA;
                    }
                    break;
                case 9:
                    if (rec->m34F4 < (u32)self->m_1C4) {
                        rec->m34F4 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m34D6 |= 0x400;
                        rec->m3530[16] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3530[17] = self->m_44->m_9E;
                        rec->m3530[18] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3530[19] = self->m_44->m_FA;
                    }
                    break;
                case 10:
                    if (rec->m34F8 < (u32)self->m_1C4) {
                        rec->m34F8 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m34D6 |= 0x200;
                        rec->m3530[20] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3530[21] = self->m_44->m_9E;
                        rec->m3530[22] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3530[23] = self->m_44->m_FA;
                    }
                    if (self->m_1C4 >= 0xA && isClearGetterOpen__Fi(0x3F) != 0) {
                        fn_80052AC0(0x2A5, 1);
                    }
                    break;
                }
            }
        } else if (kind == 0x10) {
            if (self->m_1B8 == 2) {
                rec = (Rec*)g_GameGlobal->m_record;
                switch (sub->m_14) {
                case 5:
                    if (self->m_1C4 >= 0xA) {
                        r29 = 1;
                        if (!((rec->m3548 >> 14) & 1) || self->m_E4 < rec->m3554) {
                            r30 = 1;
                            rec->m3548 |= 0x4000;
                            rec->m3554 = self->m_E4;
                        }
                    } else if (!((rec->m3548 >> 14) & 1) && rec->m3554 < (u32)self->m_1C4) {
                        rec->m3554 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m3548 |= 0x20;
                        rec->m3574[0] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3574[1] = self->m_44->m_9E;
                        rec->m3574[2] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3574[3] = self->m_44->m_FA;
                    }
                    break;
                case 6:
                    if (self->m_1C4 >= 0x64) {
                        r29 = 1;
                        if (!((rec->m3548 >> 13) & 1) || self->m_E4 < rec->m3558) {
                            r30 = 1;
                            rec->m3548 |= 0x2000;
                            rec->m3558 = self->m_E4;
                        }
                    } else if (!((rec->m3548 >> 13) & 1) && rec->m3558 < (u32)self->m_1C4) {
                        rec->m3558 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m3548 |= 0x10;
                        rec->m3574[4] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3574[5] = self->m_44->m_9E;
                        rec->m3574[6] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3574[7] = self->m_44->m_FA;
                    }
                    break;
                case 7:
                    if (!((rec->m3548 >> 12) & 1) && self->m_E0 == 0) {
                        rec->m3548 |= 0x1000;
                    }
                    if (rec->m355C < (u32)self->m_1C4) {
                        rec->m355C = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m3548 |= 0x8;
                        rec->m3574[8] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3574[9] = self->m_44->m_9E;
                        rec->m3574[10] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3574[11] = self->m_44->m_FA;
                    }
                    break;
                case 8:
                    if (!((rec->m3548 >> 11) & 1) && self->m_E0 == 0) {
                        rec->m3548 |= 0x800;
                    }
                    if (rec->m3560 < (u32)self->m_1C4) {
                        rec->m3560 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m3548 |= 0x4;
                        rec->m3574[12] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3574[13] = self->m_44->m_9E;
                        rec->m3574[14] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3574[15] = self->m_44->m_FA;
                    }
                    break;
                case 9:
                    if (rec->m3564 < (u32)self->m_1C4) {
                        rec->m3564 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m3548 |= 0x2;
                        rec->m3574[16] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3574[17] = self->m_44->m_9E;
                        rec->m3574[18] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3574[19] = self->m_44->m_FA;
                    }
                    break;
                case 10:
                    if (rec->m3568 < (u32)self->m_1C4) {
                        rec->m3568 = self->m_1C4;
                        r30 = 1;
                    }
                    if (r30 != 0) {
                        rec->m3548 |= 0x1;
                        rec->m3574[20] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_98);
                        rec->m3574[21] = self->m_44->m_9E;
                        rec->m3574[22] = exchangeGmCharacterKindToMuCharKind__14stOperatorRuleF15gmCharacterKind(self->m_44->m_F4);
                        rec->m3574[23] = self->m_44->m_FA;
                    }
                    break;
                }
            }
        }
        if (r29 != 0) {
            if (g_IfMngr != 0) {
                IfC* c = (IfC*)g_IfMngr->m_ifCenter;
                if ((c->m_00 >> 7) & 1) {
                    c->m_178 = 0x78;
                }
            }
        }
        self->m_258 = r30;
    }
}
}
}

