#include <so/jostle/so_jostle.h>
#include <types.h>

class soModuleAccesser;
struct soStatusData;

// HYPOTHESIS: only the members the accessors below use are named; the bases (soJostleModule at 0x0, the status event
// observer at 0xC) are not declared here.
class soJostleModuleImpl {
    char m_base[0x18];
    soModuleAccesser* m_moduleAccesser;
    soJostle m_jostle;
    s16 m_0x44;
    s8 m_layer;
    u8 m_status;
    u8 m_fix;
    u8 m_refer;
    u8 m_sleep;
    u8 m_ignoreSpeedX;

public:
    float getSpeedX();
    float getPosZ();
    void setPushSpeedX(float speed);
    void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    void setForceGap(u8 gap);
    void setJostleData(s32 data);
    void setPriority(u8 priority);
    void setRefer(u8 refer);
    void sleep(u8 sleep);
    void setIgnoreSpeedX(u8 ignore);
    void setFix(u8 fix);
    void setLayer(s8 layer);
    void setStatus(u8 status);
};

float soJostleModuleImpl::getSpeedX() {
    if (m_fix == 0 && m_ignoreSpeedX == 0) {
        return m_jostle.m_0x8;
    }
    return 0.0f;
}

float soJostleModuleImpl::getPosZ() {
    if (m_fix == 0) {
        return m_jostle.m_0x10;
    }
    return 0.0f;
}

void soJostleModuleImpl::setPushSpeedX(float speed) {
    m_jostle.m_0xC = speed;
}

void soJostleModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    m_status = (*(u32*)((char*)statusData + 0xC) >> 1) & 1;
}

void soJostleModuleImpl::setForceGap(u8 gap) {
    m_jostle.m_0x1A = gap;
}

void soJostleModuleImpl::setJostleData(s32 data) {
    m_jostle.m_0x4 = data;
}

void soJostleModuleImpl::setPriority(u8 priority) {
    m_jostle.setPriority(priority);
}

void soJostleModuleImpl::setRefer(u8 refer) {
    m_refer = refer;
}

void soJostleModuleImpl::sleep(u8 sleep) {
    m_sleep = sleep;
}

void soJostleModuleImpl::setIgnoreSpeedX(u8 ignore) {
    m_ignoreSpeedX = ignore;
}

void soJostleModuleImpl::setFix(u8 fix) {
    m_fix = fix;
}

void soJostleModuleImpl::setLayer(s8 layer) {
    m_layer = layer;
}

void soJostleModuleImpl::setStatus(u8 status) {
    m_status = status;
}

// TODO: clean up, give variables real names
namespace jj_C20D8 {
typedef unsigned char u8;
typedef unsigned int u32;

struct Observer {
    virtual void attach(short, int);
    short id;
    short type;
    short link;
};

struct Getter {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual int get();
};

struct Module {
    u8 pad[0x80];
    void* object;
};

struct Owner {
    u8 pad[0xD8];
    Module* module;
};

struct Object {
    void* vtable;
    u8 flag;
    u8 pad05[3];
    void* vtable08;
    Observer observer;
    Owner* owner;
    u8 data[0x28];
    short value;
    signed char flag46;
    u8 flag47;
    u8 flag48;
    u8 flag49;
    u8 flag4A;
    u8 flag4B;
};

extern "C" u8 lbl_27_data_98D0[];
extern "C" u8 lbl_27_data_17AA8[];
extern "C" void* __vt__21soStatusEventObserver[];
extern "C" void __ct__8soJostleFUcl(void*, u8, u32);

extern "C" Object* __ct__18soJostleModuleImplFP16soModuleAccesseriiPv(
    Object* self, Owner* owner, short value, u8 a, u32 b
) {
    self->flag = 0;
    self->vtable08 = lbl_27_data_98D0;
    self->vtable = lbl_27_data_98D0 + 8;

    void* sub = (u8*)owner->module->object + 8;
    int id = ((Getter*)sub)->get();

    self->observer.id = -1;
    self->observer.type = 4;
    self->observer.link = -1;
    *(void***)&self->observer = __vt__21soStatusEventObserver;
    self->observer.attach((short)id, -1);

    self->owner = owner;
    self->vtable08 = lbl_27_data_17AA8;
    self->vtable = lbl_27_data_17AA8 + 8;
    *(void***)&self->observer = (void**)(lbl_27_data_17AA8 + 0x50);

    __ct__8soJostleFUcl(self->data, a, b);

    self->value = value;
    self->flag46 = -1;
    self->flag47 = 0;
    self->flag48 = 0;
    self->flag49 = 0;
    self->flag4A = 0;
    self->flag4B = 0;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_C22AC {
extern "C" {

struct FnC1AB8Obj {
    u8 pad0[0x8];
    f32 f8;
    f32 fc;
    f32 f10;
    u8 pad14[8];
    s16 s1c;
};

void reset__8soJostleFv(FnC1AB8Obj* p);

class VtblObj {
public:
    virtual void m00();
    virtual void m01();
    virtual void m02();
    virtual void m03();
    virtual void m04();
    virtual void m05();
    virtual void m06();
    virtual void m07();
    virtual void m08();
    virtual void m09();
    virtual void m10();
    virtual void m11();
    virtual void m12();
    virtual void m13();
    virtual void m14();
    virtual void m15();
    virtual void m16();
    virtual void m17();
    virtual void m18();
    virtual void m19();
    virtual void m20();
    virtual void m21();
    virtual void m22();
    virtual u32 m23(s16);
    virtual u32 m24(s16);
};

void fn_27_C22AC(void* self) {
    u8* p = static_cast<u8*>(self);
    void* x = *(void**)(p + 0x18);
    void* y = *(void**)((u8*)x + 0xD8);
    VtblObj* v = *(VtblObj**)((u8*)y + 0x90);
    if (v->m23(*(s16*)(p + 0x44)) == 1) {
        void* x2 = *(void**)(p + 0x18);
        void* y2 = *(void**)((u8*)x2 + 0xD8);
        VtblObj* v2 = *(VtblObj**)((u8*)y2 + 0x90);
        *(u32*)(p + 0x1C) = v2->m24(*(s16*)(p + 0x44));
    } else {
        *(u32*)(p + 0x1C) = 0;
    }
    p[0x47] = 0;
    *(s8*)(p + 0x46) = -1;
    reset__8soJostleFv(reinterpret_cast<FnC1AB8Obj*>(p + 0x1C));
    p[0x4A] = 0;
}

}
}

// TODO: clean up, give variables real names
namespace jj_C2350 {
extern "C" {

extern void* m_instance__15soJostleManager;
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__15soJostleManagerFv(void*);
void addCheckList__15soJostleManagerFP8soJostlel(void*, void*, s32);
void addReferList__15soJostleManagerFP8soJostlel(void*, void*, s32);
void initLog__8soJostleFv(void*);
void updatePosZ__8soJostleFP11soJostleLog(void*, int);

class VBase {
    int a, b;
};

class VObj : public VBase {
public:
    virtual void* m0(int);
    virtual void* m1(int);
    virtual void* m2(int);
    virtual void* m3(int);
    virtual void* m4(int);
    virtual void* m5(int);
    virtual void* m6(int);
    virtual void* m7(int);
    virtual void* m8(int);
    virtual void* m9(int);
    virtual void* m10(int);
    virtual void* m11(int);
    virtual void* m12(int);
    virtual void* m13(int);
    virtual void* m14(int);
    virtual void* m15(int);
    virtual void* m16(int);
    virtual void* m17(int);
    virtual void* m18(int);
    virtual void* m19(int);
    virtual void* m20(int);
    virtual void* m21(int);
    virtual void* m22(int);
    virtual void* m23(int);
    virtual void* m24(int);
    virtual void* m25(int);
    virtual void* m26(int);
    virtual void* m27(int);
    virtual void* m28(int);
    virtual void* m29(int);
    virtual void* m30(int);
    virtual void* m31(int);
    virtual void* m32(int);
    virtual void* m33(int);
    virtual void* m34(int);
    virtual void* m35(int);
    virtual void* m36(int);
    virtual void* m37(int);
    virtual void* m38(int);
    virtual void* m39(int);
    virtual void* m40(int);
    virtual void* m41(int);
    virtual void* m42(int);
    virtual void* m43(int);
    virtual void* m44(int);
    virtual void* m45(int);
    virtual void* m46(int);
    virtual void* m47(int);
    virtual void* m48(int);
    virtual void* m49(int);
    virtual void* m50(int);
    virtual void* m51(int);
    virtual void* m52(int);
    virtual void* m53(int);
    virtual void* m54(int);
    virtual void* m55(int);
    virtual void* m56(int);
    virtual void* m57(int);
    virtual void* m58(int);
    virtual void* m59(int);
    virtual void* m60(int);
    virtual void* m61(int);
    virtual void* m62(int);
    virtual void* m63(int);
    virtual void* m64(int);
    virtual void* m65(int);
    virtual void* m66(int);
    virtual void* m67(int);
    virtual void* m68(int);
    virtual void* m69(int);
    virtual void* m70(int);
    virtual void* m71(int);
    virtual void* m72(int);
};

void fn_27_C2350(u8* self, s32 arg1) {
    s32 r29;
    if (self[0x4A] == 0) {
        r29 = 0;
        initLog__8soJostleFv(self + 0x1C);
        if (self[0x47] == 1) {
            s32 mode = arg1;
            s8 t = (s8)self[0x46];
            if (t != -1) {
                mode = t;
            }
            if (mode == 5) {
                if (self[0x49] == 0) {
                    s32 i;
                    for (i = 0; i < 4; i++) {
                        if (!m_instance__15soJostleManager) {
                            void* p = __nw__FUlQ25Heaps8HeapType(0xA4, 2);
                            if (p) {
                                p = __ct__15soJostleManagerFv(p);
                            }
                            m_instance__15soJostleManager = p;
                        }
                        addCheckList__15soJostleManagerFP8soJostlel(m_instance__15soJostleManager, self + 0x1C, i);
                    }
                } else {
                    s32 i;
                    for (i = 0; i < 4; i++) {
                        if (!m_instance__15soJostleManager) {
                            void* p = __nw__FUlQ25Heaps8HeapType(0xA4, 2);
                            if (p) {
                                p = __ct__15soJostleManagerFv(p);
                            }
                            m_instance__15soJostleManager = p;
                        }
                        addReferList__15soJostleManagerFP8soJostlel(m_instance__15soJostleManager, self + 0x1C, i);
                    }
                }
                r29 = 1;
            } else {
                if (arg1 == 0) {
                    void* A = *(void**)(self + 0x18);
                    void* B = *(void**)((char*)A + 0xD8);
                    void* C = *(void**)((char*)B + 0x10);
                    VObj* vobj = (VObj*)C;
                    *(void**)(self + 0x30) = vobj->m72(0);
                }
                if (self[0x49] == 0) {
                    if (!m_instance__15soJostleManager) {
                        void* p = __nw__FUlQ25Heaps8HeapType(0xA4, 2);
                        if (p) {
                            p = __ct__15soJostleManagerFv(p);
                        }
                        m_instance__15soJostleManager = p;
                    }
                    addCheckList__15soJostleManagerFP8soJostlel(m_instance__15soJostleManager, self + 0x1C, mode);
                } else {
                    if (!m_instance__15soJostleManager) {
                        void* p = __nw__FUlQ25Heaps8HeapType(0xA4, 2);
                        if (p) {
                            p = __ct__15soJostleManagerFv(p);
                        }
                        m_instance__15soJostleManager = p;
                    }
                    addReferList__15soJostleManagerFP8soJostlel(m_instance__15soJostleManager, self + 0x1C, mode);
                }
                r29 = 1;
            }
            self[0x37] = (u8)arg1;
        }
        if (r29 == 0) {
            updatePosZ__8soJostleFP11soJostleLog(self + 0x1C, 0);
        }
    }
}

}
}

// TODO: clean up, give variables real names
namespace jj_C2614 {
extern "C" void notifyEventChangeStatus__18soJostleModuleImplFiiP12soStatusDataP16soModuleAccesser(void*);

extern "C" void fn_27_C2614(void* self) {
    notifyEventChangeStatus__18soJostleModuleImplFiiP12soStatusDataP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0xc);
}
}

