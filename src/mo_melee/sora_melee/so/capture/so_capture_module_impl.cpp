#define SO_CAPTURE_MODULE_IMPL_FULL
#include <so/capture/so_capture_module_impl.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

extern const float lbl_27_rodata_910[4];

// Matrix::mulVec (the map name); em_kinetic_energy.cpp calls it by this stand-in name.

soCaptureModule::~soCaptureModule() { }

soCaptureModuleImpl::~soCaptureModuleImpl() { }

void soCaptureModuleImpl::activate() {
    initInfo();
    m_sendCutEvent = true;
    unk2D = true;
    m_isThrown = false;
    unk31 = false;
    m_isReaction = false;
    unk28 = 0.0f;
}

void soCaptureModuleImpl::deactivate() { }

void soCaptureModuleImpl::setNodes(int node1, int node2, float unk) {
    m_node1 = node1;
    m_node2 = node2;
    unk20 = unk;
}

void soCaptureModuleImpl::initInfo() {
    m_isReaction = false;
}

bool soCaptureModuleImpl::isCapture() {
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == false) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return true;
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return true;
        }
    }
    return false;
}

int soCaptureModuleImpl::getCatchTaskId() {
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == false) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return m_moduleAccesser->getLinkModule().getNodeTaskId(0);
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return m_moduleAccesser->getLinkModule().getParentTaskId(0);
        }
    }
    return -1;
}

float soCaptureModuleImpl::getCatchPosY() {
    return m_moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(0, m_catchNode, true).m_y;
}

bool soCaptureModuleImpl::isIgnoreCatching() {
    return m_ignoreCatching;
}

void soCaptureModuleImpl::setIgnoreCatching(bool ignoreCatching) {
    m_ignoreCatching = ignoreCatching;
}

int soCaptureModuleImpl::getMotionOffset() {
    return m_motionOffset;
}

bool soCaptureModuleImpl::isReaction() {
    return m_isReaction;
}

void soCaptureModuleImpl::setUpdatePos(bool updatePos) {
    m_updatePos = updatePos;
}

void soCaptureModuleImpl::setCatchNode(int node) {
    m_catchNode = node;
}

Vec2f soCaptureModuleImpl::getNodeOffset(bool global) {
    Vec3f parent = m_moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(0, m_catchNode, global);
    Vec3f node = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_node1, global);
    Vec2f offset;
    offset.m_x = parent.m_x - node.m_x;
    offset.m_y = parent.m_y - node.m_y;
    return offset;
}

void soCaptureModuleImpl::captureCut(bool cut, bool constrained) {
    if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
        if (m_sendCutEvent == true) {
            soLinkEventArgs event(9);
            m_moduleAccesser->getLinkModule().sendEventParents(0, event);
        }
        if (cut == true && unk2D == true) {
            if (constrained == true) {
                Vec3f rot(0.0f, 0.0f, 0.0f);
                Vec3f pos(0.0f, 0.0f, 0.0f);
                m_moduleAccesser->getLinkModule().adjustModelConstraintPosture(&rot, &pos);
            }
            m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        }
        m_moduleAccesser->getLinkModule().unlink(0);
    }
    m_moduleAccesser->getLinkModule().unlinkNode(2);
}

bool soCaptureModuleImpl::checkDamage(int status) {
    unk28 += m_moduleAccesser->getDamageModule().getCaptureDamage();
    if (unk28 >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0x7f2, 0)) {
        return false;
    }
    bool cut = false;
    if (m_moduleAccesser->getDamageModule().isCaptureCut() == false) {
        if (m_moduleAccesser->getLinkModule().isParentDamageReaction(0) == true) {
            if (m_moduleAccesser->getLinkModule().isParentDamageCatchCut(0) == false) {
                cut = true;
            }
        } else {
            cut = true;
        }
    }
    if (cut == true) {
        if (status != -1) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(status, m_moduleAccesser);
        }
        return true;
    }
    return false;
}

bool soCaptureModuleImpl::checkDamageThrown() {
    unk28 += m_moduleAccesser->getDamageModule().getCaptureDamage();
    if (unk28 >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0x7f2, 0)) {
        return false;
    }
    bool cut;
    if (m_moduleAccesser->getDamageModule().isCaptureCut(&cut) == true) {
        if (cut == true) {
            unk31 = true;
        }
        return false;
    }
    return true;
}

void soCaptureModuleImpl::updatePosThrown() {
    if (unk2D == true && m_updatePos == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        Vec3f node = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_node2, false);
        pos.m_x = node.m_x;
        pos.m_y = node.m_y - unk20 * m_moduleAccesser->getPostureModule().getScale();
        m_moduleAccesser->getPostureModule().setPos(&pos);
    }
}

void soCaptureModuleImpl::thrown() {
    if (unk2D == true && m_moduleAccesser->getLinkModule().isModelConstraint() == false) {
        m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(0, m_node1, m_catchNode, 3, true);
        m_moduleAccesser->getLinkModule().setAttribute(0, soLinkConnection::Attribute_Reference_Parent_Visibility, false);
        soLinkModule* link = soExternalValueAccesser::getLinkModule(m_moduleAccesser->getLinkModule().getParent(0));
        if (link->isLink(2) == false) {
            if (link->link(2, m_moduleAccesser->getStageObject().getTaskId()) == true) {
                link->setAttribute(2, soLinkConnection::Attribute_Reference_Parent_Stop, true);
            }
        }
    }
    m_isThrown = true;
    unk31 = false;
    unk28 = 0.0f;
}

void soCaptureModuleImpl::updateLRThrown() {
    const float* values = lbl_27_rodata_910;
    if (unk2D == true) {
        Matrix matrix(true);
        m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_node2, &matrix, false);
        float one;
        float zero = values[0];
        Vec3f direction(zero, zero, zero);
        one = values[1];
        Vec3f axis;
        axis.m_x = zero;
        axis.m_y = zero;
        axis.m_z = one;
        matrix.mulVec(&axis, &direction);
        float rightY;
        float rightX;
        float facingX;
        float facingY;
        facingY = direction.m_y;
        facingX = direction.m_x;
        Vec2f facing(facingX, facingY);
        rightX = values[1];
        rightY = values[0];
        Vec2f right(rightX, rightY);
        float lr = values[2];
        if (right.m_x * facing.m_x + right.m_y * facing.m_y >= values[3]) {
            lr = values[1];
        }
        if (lr != m_moduleAccesser->getPostureModule().getLr()) {
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
}

// TODO: clean up, give variables real names
namespace jj_65EE4 {
extern "C" {
extern char lbl_27_data_7700[];
extern char lbl_27_data_76C0[];
extern char lbl_27_data_7608[];
extern char lbl_27_data_7610[];
extern char __RTTI__11soEventUnit[];
extern const f32 lbl_27_rodata_910[];

void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void* __dynamic_cast(void*, long, const void*, const void*, long);
}

struct I9 {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual s16 get(void*);
};

struct Sys {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual s32 check(long);
};

struct Mgr {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual s32 check(long);
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void* s10(long);
};

struct Unit {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual s32 get();
};

struct Self {
    void* vt;
    void* p4;
    s16 f8;
    s16 fA;
    s32 fC;
    void* p10;
    s32 f14;
    s32 f18;
    s32 f1C;
    f32 f20;
    unsigned char pad24[11];
    unsigned char b2F;
    unsigned char pad30[2];
    unsigned char b32;
};

inline s32 getResult(Self* self) {
    s16 id = self->fA;
    Mgr* mgr2 = (Mgr*)getManager__13soEventSystemFl(
        getInstance__13soEventSystemFv(), self->f8);
    if (mgr2->check(id) == 0) {
        return 0;
    }
    Unit* u = (Unit*)__dynamic_cast(
        mgr2->s10(id), 0, lbl_27_data_7608, __RTTI__11soEventUnit, 0);
    if (u == 0) {
        return 0;
    }
    return u->get();
}

extern "C" Self* __ct__19soCaptureModuleImplFP16soModuleAccesser(Self* self, void* arg1) {
    s32 result = 0;
    s32 eid;
    Mgr* mgr;
    char* ctx;
    char* obj;

    self->vt = lbl_27_data_7700;
    ctx = *(char**)((char*)arg1 + 0xD8);
    obj = *(char**)(ctx + 0x80);
    eid = ((I9*)(obj + 8))->get(arg1);
    self->p4 = lbl_27_data_76C0;
    self->f8 = eid;
    self->fA = 13;
    self->fC = result;

    if (eid > 0 && eid > -1) {
        if (((Sys*)getInstance__13soEventSystemFv())->check(eid) != 0) {
            mgr = (Mgr*)getManager__13soEventSystemFl(
                getInstance__13soEventSystemFv(), self->f8);
            if (mgr->check(self->fA) == 0) {
                self->fC = result;
            } else {
                result = getResult(self);
                self->fC = result;
            }
        }
    }

    const f32& f = lbl_27_rodata_910[0];
    u8 t = 1;
    s32 n = -1;
    char* v2 = lbl_27_data_7610 + 0x70;
    char* v = lbl_27_data_7610;
    u8 z = 0;
    self->vt = v;
    self->p4 = v2;
    self->p10 = arg1;
    self->f14 = n;
    self->f18 = n;
    self->f1C = n;
    self->f20 = f;
    self->b2F = t;
    self->b32 = z;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_660D4 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_660D4(void* ptr, int size) {
    if (ptr != 0 && size > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_66BD4 {
extern "C" u8 fn_27_66BD4(void* self) {
    return *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x2e);
}
}

// TODO: clean up, give variables real names
namespace jj_66E08 {
extern const float lbl_27_rodata_910[];

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Buf {
    u32 a;
    u8 b;
    u8 c;
};

class soNullable {
public:
    virtual void isNull();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6(u32);
    virtual void slot7();
    virtual void slot8(u32);
    virtual u32 slot9(u32, void*);
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15(u32, void*, u8);
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
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void slot60();
    virtual void slot61();
    virtual void slot62();
    virtual void slot63();
    virtual void slot64();
    virtual void slot65();
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void slot69();
    virtual void slot70();
    virtual void slot71();
    virtual void slot72();
    virtual void slot73();
    virtual void slot74();
    virtual void slot75();
    virtual void slot76();
    virtual void slot77();
    virtual void slot78();
    virtual void slot79();
    virtual void slot80();
    virtual void slot81();
    virtual void slot82();
    virtual void slot83();
    virtual void slot84();
    virtual void slot85(void*, void*);
    virtual void slot86();
    virtual void slot87(u32);
};

class Inner {
public:
    char pad[0x54];
    soNullable* m_obj;
};

class Mid {
public:
    char pad[0xd8];
    Inner* m_inner;
};

class Self {
public:
    char pad0[0x10];
    Mid* m_mid;
    char pad1[0x19];
    u8 m_2d;
    char pad2[0x3];
    u8 m_31;
};

extern "C" void fn_27_66E08(Self* self, u32 arg) {
    if (self->m_mid->m_inner->m_obj->slot9(0, self->m_mid) == 1) {
        Buf buf;
        u8 val = self->m_31;
        buf.a = 0xe;
        buf.b = 0;
        buf.c = val;
        self->m_mid->m_inner->m_obj->slot15(0, &buf, val);
        if (self->m_2d == 1) {
            if (arg == 1) {
                Vec3 v1;
                Vec3 v2;
                v1.x = lbl_27_rodata_910[0];
                v1.y = lbl_27_rodata_910[0];
                v1.z = lbl_27_rodata_910[0];
                v2.x = lbl_27_rodata_910[0];
                v2.y = lbl_27_rodata_910[0];
                v2.z = lbl_27_rodata_910[0];
                self->m_mid->m_inner->m_obj->slot85(&v1, &v2);
            }
            self->m_mid->m_inner->m_obj->slot87(1);
        }
        self->m_mid->m_inner->m_obj->slot8(0);
    }
    self->m_mid->m_inner->m_obj->slot6(2);
}
}

// TODO: clean up, give variables real names
namespace jj_67220 {
extern "C" s32 notifyEventCaptureStatus__22soCaptureEventObserverFP16soModuleAccesseriii(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_67250 {
extern "C" void fn_27_67250(void* self, u8 value) {
    *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x2c) = value;
}
}

