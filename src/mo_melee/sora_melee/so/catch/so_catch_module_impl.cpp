#include <so/catch/so_catch_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>

// Matrix::mulVec (the map name); em_kinetic_energy.cpp calls it by this stand-in name.

soCatchModule::~soCatchModule() { }

void soCatchModuleImpl::activate() {
    m_sendCutEvent = true;
    m_catchFlag = false;
}

void soCatchModuleImpl::deactivate() { }

void soCatchModuleImpl::setNodes(int node1, int node2, float unk) {
    m_node1 = node1;
    m_node2 = node2;
    unk1C = unk;
}

void soCatchModuleImpl::initInfo() {
    unk54 = 3.4028235e38f;
}

void soCatchModuleImpl::setCatch(int taskId) {
    m_sendCutEvent = true;
    if (static_cast<u32>(taskId) != 0xFFFFFFFF) {
        if (m_moduleAccesser->getLinkModule().link(0, taskId) == true) {
            onCatchFlag();
            UnkLinkEvent event(0x11, -1);
            m_moduleAccesser->getLinkModule().sendEventParents(0, event);
            m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(0, m_node1, event.unk8, 3, true);
        }
    }
}

bool soCatchModuleImpl::isCatch() {
    if (m_catchFlag == true) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return true;
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return true;
        }
    }
    return false;
}

void soCatchModuleImpl::onCatchFlag() {
    m_catchFlag = true;
}

void soCatchModuleImpl::offCatchFlag() {
    m_catchFlag = false;
}

void soCatchModuleImpl::setSendCutEvent(bool sendCutEvent) {
    m_sendCutEvent = sendCutEvent;
}

void soCatchModuleImpl::updatePosCling() {
    if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        Vec3f nodePos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_node2, false);
        pos.m_x = nodePos.m_x;
        pos.m_y = nodePos.m_y - unk1C * m_moduleAccesser->getPostureModule().getScale();
        m_moduleAccesser->getPostureModule().setPos(&pos);
    }
}

void soCatchModuleImpl::updateLRCling() {
    if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
        Matrix matrix(true);
        m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_node2, &matrix, false);
        Vec3f direction(0.0f, 0.0f, 0.0f);
        Vec3f forward(0.0f, 0.0f, 1.0f);
        matrix.mulVec(&forward, &direction);
        float y = direction.m_y;
        Vec2f xy(direction.m_x, y);
        Vec2f right(1.0f, 0.0f);
        float lr = -1.0f;
        if (right.m_x * xy.m_x + right.m_y * xy.m_y >= 0.00001f) {
            lr = 1.0f;
        }
        if (lr != m_moduleAccesser->getPostureModule().getLr()) {
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
}

bool soCatchModuleImpl::checkDamage() {
    if (m_moduleAccesser->getLinkModule().isLinked(0) == true || m_moduleAccesser->getLinkModule().isLink(0) == true) {
        if (m_moduleAccesser->getDamageModule().isCatchCut() == false) {
            if (m_moduleAccesser->getLinkModule().isNodeDamageReaction(0) == true) {
                if (m_moduleAccesser->getLinkModule().isNodeDamageCaptureCut(0) == false) {
                    return true;
                }
            } else {
                return true;
            }
        } else if (m_moduleAccesser->getLinkModule().isNodeDamageReaction(0) == true) {
            m_moduleAccesser->getLinkModule().isNodeDamageCaptureCut(0);
        }
    }
    return false;
}

#include <so/event/so_event_system.h>
#include <so/so_instance_manager.h>
// TODO: clean up, give variables real names
namespace jj_64C8C {
extern "C" u8 lbl_27_data_7440[];
extern "C" u8 lbl_27_data_74D8[];
extern "C" u8 lbl_27_data_7558[];

struct Module {
    u8 pad[0x80];
    void* object;
};

struct Owner {
    u8 pad[0xD8];
    Module* module;
};

struct VirtualObj {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual s16 method(Owner*, s32, void*);
};

struct Observer {
    virtual void attach(s16, s32, void*);
    s16 id;
    s16 type;
    s16 link;
};

struct MainObj {
    void* vtable;
    Observer observer;
    void* field_10;
    u8 pad_14[0xC];
    s32 field_20;
    u8 pad_24[0x2C];
    void* field_50;
    u8 pad_54[0xD];
    u8 field_61;
};

extern "C" MainObj* __ct__17soCatchModuleImplFP16soModuleAccesseri(MainObj* self, Owner* owner, s32 arg2) {
    self->vtable = lbl_27_data_7558;

    VirtualObj* obj = (VirtualObj*)((u8*)owner->module->object + 8);
    s16 result = obj->method(owner, arg2, lbl_27_data_7558);

    self->observer.id = -1;
    self->observer.type = 2;
    self->observer.link = -1;
    *(void**)&self->observer = lbl_27_data_74D8;
    self->observer.attach(result, -1, lbl_27_data_74D8);

    self->field_10 = owner;
    self->vtable = lbl_27_data_7440;
    *(void**)&self->observer = lbl_27_data_7440 + 0x48;
    self->field_20 = arg2;
    self->field_50 = self;
    self->field_61 = 0;

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_64D9C {
extern "C" char lbl_27_data_75B8[];
extern "C" void* getInstance__13soEventSystemFv();
extern "C" void* getManager__13soEventSystemFl(void*, long);
extern "C" void __dl__FPv(void* p);

struct EvSys {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual s32 v6(s16 id);
};

struct EvMgr {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3(s16 a, s16 b);
};

struct Obj {
    void* vtbl;
    void* unk4;
    s16 m8;
    s16 mA;
    s16 mC;
};

extern "C" Obj* __dt__17soCatchModuleImplFv(Obj* p, s32 flag) {
    if (p != nullptr) {
        if ((u32)p + 4 != 0 && (u32)p + 4 != 0) {
            s16 id = p->m8;
            p->unk4 = lbl_27_data_75B8;
            u32 r = ((EvSys*)getInstance__13soEventSystemFv())->v6(id);
            if (r == 1) {
                EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), p->m8);
                mgr->v3(p->mA, p->mC);
            }
            p->mC = -1;
            p->m8 = -1;
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_65030 {
extern "C" {
extern float lbl_27_rodata_8FC;
extern char lbl_27_data_7438[];
void* getGlowModule__23soExternalValueAccesserFP11StageObject(void*);
void* getDamageModule__23soExternalValueAccesserFP11StageObject(void*);
}

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Hdr8 {
    int a;
    int b;
};

class Obj8 : public Hdr8 {
public:
    virtual void slot0();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11(); virtual void slot12();
    virtual void slot13(); virtual void slot14(); virtual void slot15(); virtual void slot16();
    virtual void slot17(unsigned int, float);
    virtual void slot18(); virtual void slot19(); virtual void slot20(); virtual void slot21();
    virtual void slot22(); virtual void slot23(); virtual void slot24(); virtual void slot25();
    virtual void slot26(); virtual void slot27(); virtual void slot28(); virtual void slot29();
    virtual void slot30(); virtual void slot31();
    virtual void* slot32(char*);
    virtual void slot33(); virtual void slot34(); virtual void slot35();
    virtual Vec3 slot36(void*, unsigned int);
};

class IfaceY {
public:
    virtual void isNull();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(unsigned int);
    virtual unsigned int slot9(unsigned int);
    virtual unsigned int slot10(unsigned int);
    virtual void slot11(); virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
    virtual void slot16(unsigned int, void*, unsigned int);
    virtual void slot17();
    virtual void* slot18(unsigned int);
    virtual void slot19(); virtual void slot20(); virtual void slot21(); virtual void slot22(); virtual void slot23();
    virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27(); virtual void slot28();
    virtual void slot29(); virtual void slot30(); virtual void slot31(); virtual void slot32(); virtual void slot33();
    virtual void slot34(); virtual void slot35(); virtual void slot36(); virtual void slot37(); virtual void slot38();
    virtual void slot39(); virtual void slot40(); virtual void slot41(); virtual void slot42(); virtual void slot43();
    virtual void slot44(); virtual void slot45(); virtual void slot46(); virtual void slot47(); virtual void slot48();
    virtual void slot49(); virtual void slot50(); virtual void slot51(); virtual void slot52(); virtual void slot53();
    virtual void slot54(); virtual void slot55(); virtual void slot56(); virtual void slot57(); virtual void slot58();
    virtual void slot59(); virtual void slot60(); virtual void slot61(); virtual void slot62(); virtual void slot63();
    virtual void slot64(); virtual void slot65(); virtual void slot66(); virtual void slot67(); virtual void slot68();
    virtual void slot69(); virtual void slot70(); virtual void slot71(); virtual void slot72(); virtual void slot73();
    virtual void slot74(); virtual void slot75(); virtual void slot76(); virtual void slot77(); virtual void slot78();
    virtual void slot79(); virtual void slot80(); virtual void slot81(); virtual void slot82(); virtual void slot83();
    virtual void slot84();
    virtual void slot85(Vec3*, Vec3*);
    virtual void slot86();
    virtual void slot87(unsigned int);
};

class IfaceZ {
public:
    virtual void isNull();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11(); virtual void slot12();
    virtual void slot13(); virtual void slot14(); virtual void slot15(); virtual void slot16();
    virtual void slot17(); virtual void slot18(); virtual void slot19(); virtual void slot20();
    virtual void slot21(); virtual void slot22(); virtual void slot23(); virtual void slot24();
    virtual void slot25();
    virtual float slot26(int, int);
    virtual void slot27(); virtual void slot28(); virtual void slot29(); virtual void slot30();
    virtual void slot31(); virtual void slot32(); virtual void slot33(); virtual void slot34();
    virtual void slot35(); virtual void slot36(); virtual void slot37(); virtual void slot38();
    virtual void slot39(); virtual void slot40(); virtual void slot41();
    virtual int slot42(int, int);
};

class IfaceW {
public:
    virtual void isNull();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5();
    virtual unsigned int slot6();
    virtual void slot7(); virtual void slot8(); virtual void slot9(); virtual void slot10();
    virtual void slot11(); virtual void slot12(); virtual void slot13(); virtual void slot14();
    virtual void slot15(); virtual void slot16(); virtual void slot17(); virtual void slot18();
    virtual void slot19(); virtual void slot20(); virtual void slot21(); virtual void slot22();
    virtual void slot23(); virtual void slot24(); virtual void slot25();
    virtual float slot26(float, int, void*);
};

struct X {
    char pad0[4];
    Obj8* p4;
    char pad1[0x14];
    IfaceZ* pZ;
    char pad2[0x34];
    IfaceY* pY;
    char pad3[0x70];
    IfaceW* pW;
};

struct Mod {
    char pad[0xd8];
    X* x;
};

struct Self {
    char pad0[0x10];
    Mod* mod;
    char pad1[0xc];
    int unk20;
    char pad2[0x3c];
    unsigned char unk60;
    unsigned char unk61;
};

struct Msg2 {
    int id;
    unsigned char flag;
};

struct Msg3 {
    int id;
    unsigned char flag;
    int idx;
    Vec3 pos;
    int a;
    int b;
};

extern "C" void fn_27_65030(Self* self, unsigned int arg1, int arg2) {
    int flag = 0;
    if (self->mod->x->pY->slot10(0) == 1) {
        if (self->mod->x->pY->slot9(0) == 1 && arg1 == 1) {
            Vec3 a;
            Vec3 b;
            a.x = lbl_27_rodata_8FC;
            a.y = lbl_27_rodata_8FC;
            a.z = lbl_27_rodata_8FC;
            b.x = lbl_27_rodata_8FC;
            b.y = lbl_27_rodata_8FC;
            b.z = lbl_27_rodata_8FC;
            self->mod->x->pY->slot85(&a, &b);
            flag = 1;
        }
        if (self->unk60 == 1) {
            if (self->unk20 >= 0) {
                if (arg2 == 0) {
                    IfaceZ* z;
                    void* r27 = self->mod->x->pY->slot18(0);
                    z = self->mod->x->pZ;
                    float f31 = z->slot26(self->unk20, 1);
                    if (self->mod->x->pW->slot6() == 1) {
                        arg2 = (int)getGlowModule__23soExternalValueAccesserFP11StageObject(r27);
                        if (((IfaceW*)arg2)->slot6() == 1) {
                            f31 = self->mod->x->pW->slot26(f31, z->slot42(self->unk20, 1), (void*)arg2);
                        }
                    }
                    Obj8* t = (Obj8*)getDamageModule__23soExternalValueAccesserFP11StageObject(r27);
                    t->slot17(0, f31);
                    Msg2 m;
                    m.id = 10;
                    m.flag = 0;
                    self->mod->x->pY->slot16(0, &m, 0);
                } else {
                    Msg3 m3;
                    m3.id = 12;
                    m3.flag = 0;
                    m3.idx = -1;
                    m3.a = -1;
                    m3.b = -1;
                    m3.idx = self->unk20;
                    Obj8* r = self->mod->x->p4;
                    void* res = r->slot32(lbl_27_data_7438);
                    m3.pos = r->slot36(res, 0);
                    m3.a = 0;
                    m3.b = 0;
                    self->mod->x->pY->slot16(0, &m3, 0);
                }
            } else {
                Msg2 m2;
                m2.id = 10;
                m2.flag = 0;
                self->mod->x->pY->slot16(0, &m2, 0);
            }
        }
        self->unk61 = 0;
    }
    if (self->mod->x->pY->slot9(0) == 1) {
        if (flag == 0 && arg1 == 1) {
            Vec3 c;
            Vec3 d;
            c.x = lbl_27_rodata_8FC;
            c.y = lbl_27_rodata_8FC;
            c.z = lbl_27_rodata_8FC;
            d.x = lbl_27_rodata_8FC;
            d.y = lbl_27_rodata_8FC;
            d.z = lbl_27_rodata_8FC;
            self->mod->x->pY->slot85(&c, &d);
        }
        self->mod->x->pY->slot87(1);
        self->mod->x->pY->slot8(0);
        self->unk61 = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_65690 {
extern "C" {
extern float lbl_27_rodata_8FC;
extern char lbl_27_data_7438[];
void* getGlowModule__23soExternalValueAccesserFP11StageObject(void*);
void* getDamageModule__23soExternalValueAccesserFP11StageObject(void*);
}

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Hdr8 {
    int a;
    int b;
};

class Obj8 : public Hdr8 {
public:
    virtual void slot0();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11(); virtual void slot12();
    virtual void slot13(); virtual void slot14(); virtual void slot15(); virtual void slot16();
    virtual void slot17(unsigned int, float);
    virtual void slot18(); virtual void slot19(); virtual void slot20(); virtual void slot21();
    virtual void slot22(); virtual void slot23(); virtual void slot24(); virtual void slot25();
    virtual void slot26(); virtual void slot27(); virtual void slot28(); virtual void slot29();
    virtual void slot30(); virtual void slot31();
    virtual void* slot32(char*);
    virtual void slot33(); virtual void slot34(); virtual void slot35();
    virtual Vec3 slot36(void*, unsigned int);
};

class IfaceY {
public:
    virtual void isNull();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(unsigned int);
    virtual unsigned int slot9(unsigned int);
    virtual unsigned int slot10(unsigned int);
    virtual void slot11(); virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
    virtual void slot16(unsigned int, void*, unsigned int);
    virtual void slot17();
    virtual void* slot18(unsigned int);
    virtual void slot19(); virtual void slot20(); virtual void slot21(); virtual void slot22(); virtual void slot23();
    virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27(); virtual void slot28();
    virtual void slot29(); virtual void slot30(); virtual void slot31(); virtual void slot32(); virtual void slot33();
    virtual void slot34(); virtual void slot35(); virtual void slot36(); virtual void slot37(); virtual void slot38();
    virtual void slot39(); virtual void slot40(); virtual void slot41(); virtual void slot42(); virtual void slot43();
    virtual void slot44(); virtual void slot45(); virtual void slot46(); virtual void slot47(); virtual void slot48();
    virtual void slot49(); virtual void slot50(); virtual void slot51(); virtual void slot52(); virtual void slot53();
    virtual void slot54(); virtual void slot55(); virtual void slot56(); virtual void slot57(); virtual void slot58();
    virtual void slot59(); virtual void slot60(); virtual void slot61(); virtual void slot62(); virtual void slot63();
    virtual void slot64(); virtual void slot65(); virtual void slot66(); virtual void slot67(); virtual void slot68();
    virtual void slot69(); virtual void slot70(); virtual void slot71(); virtual void slot72(); virtual void slot73();
    virtual void slot74(); virtual void slot75(); virtual void slot76(); virtual void slot77(); virtual void slot78();
    virtual void slot79(); virtual void slot80(); virtual void slot81(); virtual void slot82(); virtual void slot83();
    virtual void slot84();
    virtual void slot85(Vec3*, Vec3*);
    virtual void slot86();
    virtual void slot87(unsigned int);
};

class IfaceZ {
public:
    virtual void isNull();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
    virtual void slot9(); virtual void slot10(); virtual void slot11(); virtual void slot12();
    virtual void slot13(); virtual void slot14(); virtual void slot15(); virtual void slot16();
    virtual void slot17(); virtual void slot18(); virtual void slot19(); virtual void slot20();
    virtual void slot21(); virtual void slot22(); virtual void slot23(); virtual void slot24();
    virtual void slot25();
    virtual float slot26(int, int);
    virtual void slot27(); virtual void slot28(); virtual void slot29(); virtual void slot30();
    virtual void slot31(); virtual void slot32(); virtual void slot33(); virtual void slot34();
    virtual void slot35(); virtual void slot36(); virtual void slot37(); virtual void slot38();
    virtual void slot39(); virtual void slot40(); virtual void slot41();
    virtual int slot42(int, int);
};

class IfaceW {
public:
    virtual void isNull();
    virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
    virtual void slot5();
    virtual unsigned int slot6();
    virtual void slot7(); virtual void slot8(); virtual void slot9(); virtual void slot10();
    virtual void slot11(); virtual void slot12(); virtual void slot13(); virtual void slot14();
    virtual void slot15(); virtual void slot16(); virtual void slot17(); virtual void slot18();
    virtual void slot19(); virtual void slot20(); virtual void slot21(); virtual void slot22();
    virtual void slot23(); virtual void slot24(); virtual void slot25();
    virtual float slot26(float, int, void*);
};

struct X {
    char pad0[4];
    Obj8* p4;
    char pad1[0x14];
    IfaceZ* pZ;
    char pad2[0x34];
    IfaceY* pY;
    char pad3[0x70];
    IfaceW* pW;
};

struct Mod {
    char pad[0xd8];
    X* x;
};

struct Self {
    char pad0[0x10];
    Mod* mod;
    char pad1[0xc];
    int unk20;
    char pad2[0x3c];
    unsigned char unk60;
    unsigned char unk61;
};

struct Msg2 {
    int id;
    unsigned char flag;
};


extern "C" void fn_27_65690(Self* self, unsigned int arg1) {
    if (self->mod->x->pY->slot9(0) == 1) {
        if (self->unk60 == 1) {
            if (self->unk20 >= 0) {
                IfaceZ* z;
                void* task = self->mod->x->pY->slot18(0);
                z = self->mod->x->pZ;
                float f = z->slot26(self->unk20, 1);
                if (self->mod->x->pW->slot6() == 1) {
                    IfaceW* w = (IfaceW*)getGlowModule__23soExternalValueAccesserFP11StageObject(task);
                    if (w->slot6() == 1) {
                        f = self->mod->x->pW->slot26(f, z->slot42(self->unk20, 1), w);
                    }
                }
                Obj8* t = (Obj8*)getDamageModule__23soExternalValueAccesserFP11StageObject(task);
                t->slot17(0, f);
            }
            Msg2 m;
            m.id = 10;
            m.flag = 0;
            self->mod->x->pY->slot16(0, &m, 0);
        }
        self->unk61 = 0;
    }
    if (self->mod->x->pY->slot9(0) == 1) {
        if (arg1 == 1) {
            Vec3 a;
            Vec3 b;
            a.x = lbl_27_rodata_8FC;
            a.y = lbl_27_rodata_8FC;
            a.z = lbl_27_rodata_8FC;
            b.x = lbl_27_rodata_8FC;
            b.y = lbl_27_rodata_8FC;
            b.z = lbl_27_rodata_8FC;
            self->mod->x->pY->slot85(&a, &b);
        }
        self->mod->x->pY->slot87(1);
        self->mod->x->pY->slot8(0);
        self->unk61 = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_65A58 {
extern "C" {
extern void* lbl_27_bss_F8;
extern const float lbl_27_rodata_8F8;
void* __ct__14soCatchManagerFv(void* self);
void addCheckList__14soCatchManagerFP16soCatchCheckNode(void* self, void* arg);
void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
}

extern "C" int fn_27_65A58(void* self) {
    float f = *(float*)((char*)self + 0x54);
    if (f < lbl_27_rodata_8F8) {
        if (lbl_27_bss_F8 == 0) {
            void* p = __nw__FUlQ25Heaps8HeapType(0x4c, 2);
            if (p != 0) {
                p = __ct__14soCatchManagerFv(p);
            }
            lbl_27_bss_F8 = p;
        }
        addCheckList__14soCatchManagerFP16soCatchCheckNode(lbl_27_bss_F8, (void*)((char*)self + 0x50));
        return 1;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_65AE0 {
class VT {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void* s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void* s12(u8, void*);
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16();
    virtual void s17();
    virtual void s18(int, u32, int);
};

struct Result {
    char pad0[0x10];
    u32 w10;
    u32 w14;
};

struct Mid {
    char pad0[0x30];
    VT* a30;
    char pad1[0x40 - 0x34];
    VT* a40;
};

struct Owner {
    char pad0[8];
    int unk8;
    char pad1[0xd8 - 0xc];
    Mid* mid;
};

struct Self {
    char pad0[0x10];
    Owner* owner;
    char pad1[0x30 - 0x14];
    u32 taskId;
    char pad2[0x46 - 0x34];
    u8 cat;
    char pad3[0x49 - 0x47];
    u8 flag;
};

extern "C" {
void* getInstance__15gfTaskSchedulerFv();
void* getTaskById__15gfTaskSchedulerFQ26gfTask8CategoryUl(void* sched, int cat, u32 id);
void* __dynamic_cast(void*, long, const void*, const void*, short);
VT* getCaptureModule__23soExternalValueAccesserFP11StageObject();
}
extern "C" char __RTTI__11StageObject[];
extern "C" char __RTTI__6gfTask[];

extern "C" void checkOrder__17soCatchModuleImplFv(Self* self) {
    Owner* owner = self->owner;
    if (owner->mid->a40->s5() == 0) {
        void* task = getTaskById__15gfTaskSchedulerFQ26gfTask8CategoryUl(getInstance__15gfTaskSchedulerFv(), self->cat, self->taskId);
        __dynamic_cast(task, 0x3c, __RTTI__11StageObject, __RTTI__6gfTask, 0);
        VT* r31 = getCaptureModule__23soExternalValueAccesserFP11StageObject();
        Result* p = (Result*)self->owner->mid->a30->s12(self->flag, self->owner);
        r31->s18(self->owner->unk8, (p->w10 >> 14) & 0x1FF, (p->w14 >> 25) & 1);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_65BB8 {
struct Data {
    unsigned int w[7];
    unsigned short h[2];
    unsigned char b[11];
};
struct Object {
    char pad[0x24];
    Data data;
    char pad2[4];
    float value;
};
extern "C" void fn_27_65BB8(Object* obj, const Data* data, float value) {
    if (value < obj->value) {
        obj->data = *data;
        obj->value = value;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_65C94 {

extern "C" {
extern char lbl_27_data_4558[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};

class Class_4558 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual s16 slot6(Self*, s8);
    virtual void slot7();
    virtual void slot8();
};

extern "C" void fn_27_65C94(Self* self, s32 arg1, s8 arg2) {
    soEventManager* mgr;
    u32 f31 = 0;
    s32 f30 = 0;
    s32 f4 = 0;
    s32 f5 = 0;
    s16 ret;
    s16 id;
    s16 a = self->a;

    if (a >= 0 && self->b >= 0) {
        f5 = 1;
    }
    if (f5 != 0 && self->c > -1) {
        f4 = 1;
    }
    if (f4 != 0) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(a) == 1) {
            f30 = 1;
        }
    }
    if (f30 != 0) {
        if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 1) {
            f31 = 1;
        }
    }
    if (f31 == 1) {
        id = self->a;
        if (soEventSystem::getInstance()->getInstanceManager().isContain(id) == 1) {
            soEventSystem::getInstance()->getManager(self->a)->eraseObserver(self->b, self->c);
        }
        self->c = -1;
        self->a = -1;
    }
    if (arg1 > -1 && self->b > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(arg1) != 0) {
            if (soEventSystem::getInstance()->getManager(arg1)->isNullUnit(self->b) != 1) {
                mgr = soEventSystem::getInstance()->getManager(arg1);
                if (mgr->getObserverCapacity(self->b) == 0) {
                    ret = -1;
                } else {
                    soEventUnit* unit = mgr->getEventUnit(self->b);
                    Class_4558* cast = (Class_4558*)__dynamic_cast(unit, 0, lbl_27_data_4558, __RTTI__11soEventUnit, 0);
                    if (cast == 0) {
                        ret = -1;
                    } else {
                        ret = cast->slot6(self, arg2);
                    }
                }
                self->c = ret;
                if (ret > -1 && ret > -1) {
                    self->a = (s16)arg1;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_65ED8 {
extern "C" void fn_27_65ED8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_65EDC {
extern "C" void fn_27_65BB8(void*);

extern "C" void fn_27_65EDC(void* self) {
    fn_27_65BB8(reinterpret_cast<u8*>(self) - 0x4);
}
}

