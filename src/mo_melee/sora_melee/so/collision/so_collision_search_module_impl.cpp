#include <gf/gf_camera.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/collision/so_collision_search_part.h>
#include <types.h>
#include <so/so_module_accesser.h>

// HYPOTHESIS: the part layout as the module reads it; ft/builder/ft_dol_types.h keeps an opaque copy for the fighter RELs.
class soCollisionSearchPart {
public:
    int m_status; // HYPOTHESIS: nonzero while the part searches
    soCollisionSearchData m_data;
    soArrayVector<clTarget, 6> m_targets;
    int unk58;
    int unk5C;

    void reset();
    // HYPOTHESIS: parameter types follow the search-module calls.
    void set(soCollisionSearchData* data, newCollision* collision);
    void initPos(float scale, soModelModule* modelModule, newCollision* collision);
    void debugDisplay(nw4r::math::_MTX34* view, void* collision);
};
static_assert(sizeof(soCollisionSearchPart) == 0x60, "Class is wrong size!");

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soStatusDataView {
    char _0[0xc];
    u32 : 23;
    u32 m_keepSearch : 1;
};

soCollisionSearchModule::~soCollisionSearchModule() { }

void soCollisionSearchModuleImpl::begin() {
    m_nodeIndex = -1;
    unk104 = 0;
}

void soCollisionSearchModuleImpl::clearAll() {
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        m_collisionSearchPartArray->at(i).m_status = 0;
    }
}

void soCollisionSearchModuleImpl::clear(u32 partIndex) {
    soCollisionSearchPart& part = m_collisionSearchPartArray->at(partIndex);
    part.m_status = 0;
}

bool soCollisionSearchModuleImpl::isSearch(u32 partIndex) {
    return m_collisionSearchPartArray->at(partIndex).m_status != 0;
}

float soCollisionSearchModuleImpl::getSize(u32 partIndex) {
    return m_collisionSearchPartArray->at(partIndex).m_data.m_size;
}

void soCollisionSearchModuleImpl::setSize(u32 partIndex, float size) {
    m_collisionSearchPartArray->at(partIndex).m_data.m_size = size;
}

bool soCollisionSearchModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

void soCollisionSearchModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (!((soStatusDataView*)statusData)->m_keepSearch) {
        clearAll();
    }
}

void soCollisionSearchModuleImpl::setCheckInterval(int interval) {
    unk100 = interval;
    m_checkInterval = interval;
}

int soCollisionSearchModuleImpl::isInflict() {
    return m_isInflict;
}

void soCollisionSearchModuleImpl::activate(float param, int taskId) {
    m_collision.m_taskId = taskId;
    m_48 = param;
    unk100 = m_checkInterval;
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        m_collisionSearchPartArray->at(i).reset();
    }
    m_isInflict = 0;
}

void soCollisionSearchModuleImpl::deactivate() {
    if (unk105 == 0) {
        m_collision.clean();
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
    m_collision.m_taskId = -1;
}

void soCollisionSearchModuleImpl::setPos(int nodeIndex, Vec3f* startOffsetPos, Vec3f* endOffsetPos) {
    float ez, ey, ex, sz, sy, sx;
    sx = startOffsetPos->m_x;
    sy = startOffsetPos->m_y;
    sz = startOffsetPos->m_z;
    ex = endOffsetPos->m_x;
    ey = endOffsetPos->m_y;
    ez = endOffsetPos->m_z;
    m_startOffsetPos.m_x = sx;
    m_startOffsetPos.m_y = sy;
    m_startOffsetPos.m_z = sz;
    m_endOffsetPos.m_x = ex;
    m_endOffsetPos.m_y = ey;
    m_endOffsetPos.m_z = ez;
    m_nodeIndex = nodeIndex;
}

void soCollisionSearchModuleImpl::renderDebug() {
    int groupIndex;
    nw4r::math::_MTX34 view = gfCameraManager::getManager()->m_cameras[0].m_viewMatrix;
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        if (m_collisionSearchPartArray->at(i).m_status != 0) {
            if (m_checkInterval == 0 || unk100 == m_checkInterval) {
                groupIndex = m_collisionSearchPartArray->at(i).unk58;
                soCollisionSearchPart& part = m_collisionSearchPartArray->at(i);
                part.debugDisplay(&view, m_collision.getCollision(i, groupIndex));
            }
        }
    }
}

void soCollisionSearchModuleImpl::set(u32 partIndex, u32 groupIndex, soCollisionSearchData* collisionSearchData) {
    int index;
    int activeGroup;
    m_collision.allocateCollision(partIndex, collisionSearchData->m_unk14e, groupIndex);
    newCollision* collision = m_collision.getCollision(partIndex, groupIndex);
    if (m_collisionSearchPartArray->at(partIndex).m_status == 0 ||
        (int)groupIndex != m_collisionSearchPartArray->at(partIndex).unk58) {
        soCollisionSearchPart& newPart = m_collisionSearchPartArray->at(partIndex);
        newPart.m_status = 1;
        m_collisionSearchPartArray->at(partIndex).unk58 = groupIndex;
        m_collision.initLog(groupIndex);
        activeGroup = m_collisionSearchPartArray->at(partIndex).unk58;
        for (index = 0; index < m_collisionSearchPartArray->size(); index++) {
            if (m_collisionSearchPartArray->at(index).m_status != 0 && index != (int)partIndex &&
                activeGroup == m_collisionSearchPartArray->at(index).unk58) {
                goto setData;
            }
        }
        m_collision.initPastLog(activeGroup, false);
    }
setData:
    m_collisionSearchPartArray->at(partIndex).set(collisionSearchData, collision);
    m_collision.m_isNoTeam = collisionSearchData->m_unk14d & 1;
    if (unk104 == 1) {
        soCollisionSearchPart* part;
        soModelModule* modelModule = m_moduleAccesser->m_enumerationStart->m_modelModule;
        part = &m_collisionSearchPartArray->at(partIndex);
        part->initPos(m_48, modelModule, collision);
    }
}

#include <so/event/so_event_system.h>
#include <so/so_array_vector_calculator.h>
#include <so/collision/so_collision_group.h>
// TODO: clean up, give variables real names
namespace jj_4DCA4 {

class soCollisionSearchEventObserver;

extern "C" {
extern char lbl_27_data_4A88[], lbl_27_data_4958[], lbl_27_data_4A40[], lbl_27_data_46F8[], lbl_27_data_4950[], __RTTI__11soEventUnit[];
extern char __vt__21soStatusEventObserver[], __vt__22soAnimCmdEventObserver[];
extern float lbl_27_rodata_530;
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void __ct__16soCollisionGroupFv(void*);
void __dt__16soCollisionGroupFv(void*);
void fn_27_35C3C(void*, int, int, int, int, void*, int);
void* __dynamic_cast(void*, long, const void*, const void*, long);
}

struct Getter {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual short get();
};

struct Item {
    virtual void v0(); virtual void v1(); virtual int slot2();
};

struct Module {
    char pad[0x18];
    Item* item;
    char pad2[0x64];
    char* obj;
};

struct Owner {
    char pad[0xd8];
    Module* module;
};

struct Obs {
    virtual void addObserver(short, signed char);
    short id, type, link;
};

struct Obs3 {
    void* vt;
    short id, type;
    void* unit;
};

struct Self {
    void* vt;
    Obs obs1, obs2;
    Obs3 obs3;
    Owner* owner;
    unsigned int f2c;
    float f30;
    void* calc;
    void* calc2;
    unsigned int top : 4;
    unsigned int mode : 2;
    unsigned int enabled : 1;
    unsigned int rest : 25;
    char groups[0x78];
    char pad[0x3c];
    void* cache;
    int fF8, fFC;
    char pad3[4];
    unsigned char f104;
    unsigned char flag;
};

inline int getResult(Self* self) {
    short t = self->obs3.type;
    soEventManager* mgr = soEventSystem::getInstance()->getManager(self->obs3.id);
    if (mgr->getObserverCapacity(t) == 0) return 0;
    soEventUnitWrapper<soCollisionSearchEventObserver>* unit =
        (soEventUnitWrapper<soCollisionSearchEventObserver>*)__dynamic_cast(
            mgr->getEventUnit(t), 0, lbl_27_data_4950, __RTTI__11soEventUnit, 0);
    if (unit == 0) return 0;
    return (int)unit->getObserverListSub();
}

extern "C" Self* __ct__27soCollisionSearchModuleImplFP16soModuleAccesseriUcPvb(Self* self, Owner* owner, int a2, int a3, int a4, unsigned char a5) {
    int result = -1;
    self->vt = lbl_27_data_4A88;
    char* obj = owner->module->obj;
    short id = ((Getter*)(obj + 8))->get();
    self->obs1.id = result;
    self->obs1.type = 4;
    self->obs1.link = result;
    *(void**)&self->obs1 = __vt__21soStatusEventObserver;
    self->obs1.addObserver(id, -1);

    obj = owner->module->obj;
    id = ((Getter*)(obj + 8))->get();
    Obs* o2 = &self->obs2;
    self->obs2.id = result;
    self->obs2.type = 5;
    self->obs2.link = result;
    *(void**)o2 = __vt__22soAnimCmdEventObserver;
    o2->addObserver(id, -1);

    obj = owner->module->obj;
    int cid = (short)((Getter*)(obj + 8))->get();
    self->obs3.vt = lbl_27_data_4A40;
    self->obs3.id = cid;
    self->obs3.type = 17;
    result = 0;
    self->obs3.unit = (void*)result;

    if (cid > 0 && cid > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(cid)) {
            soEventManager* mgr = soEventSystem::getInstance()->getManager(self->obs3.id);
            if (mgr->getObserverCapacity(self->obs3.type) == 0) {
                self->obs3.unit = 0;
            } else {
                result = getResult(self);
                self->obs3.unit = (void*)result;
            }
        }
    }

    float f = lbl_27_rodata_530;
    self->vt = lbl_27_data_4958;
    *(void**)&self->obs1 = lbl_27_data_4958 + 0x54;
    *(void**)&self->obs2 = lbl_27_data_4958 + 0x64;
    *(void**)&self->obs3 = lbl_27_data_4958 + 0x78;
    self->owner = owner;
    self->f2c = a4;
    self->f30 = f;
    self->calc = lbl_27_data_46F8;
    self->calc2 = lbl_27_data_46F8 + 0x48;
    self->top = 0;
    self->mode = 1;
    self->enabled = 0;
    __construct_array(self->groups, __ct__16soCollisionGroupFv, __dt__16soCollisionGroupFv, 0x78, 1);
    soArrayVectorCalculator::postInitialize(*(soArrayVectorCalcInterface*)&self->calc2, 1, 1);
    fn_27_35C3C(self->pad, a2, a3, owner->module->item->slot2(), 5, &self->calc, 0);
    self->cache = (void*)-1;
    self->fF8 = 0;
    self->fFC = 0;
    self->f104 = 0;
    self->flag = a5;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_4DFF8 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_4DFF8(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_4E63C {
class Interface {
public:
    virtual void unused();
    virtual char* get() const;
};

struct Object {
    char padding[0x2c];
    Interface* member;
};

extern "C" char* fn_27_4E63C(const Object* self) {
    return self->member->get() + 4;
}
}

// TODO: clean up, give variables real names
namespace jj_4E670 {
struct Item {
    int w0;
    char pad4[0x54];
    int f58;
};

struct Vt44 {
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual int s9();
};

struct Mid {
    int unk0;
    int unk4;
    char pad8[0x44 - 8];
    Vt44* f44;
};

struct Owner {
    char pad0[0xd8];
    Mid* fd8;
};

struct Seq {
    virtual void s0();
    virtual Item* s1(int i);
    virtual void s2();
    virtual int s3();
};

struct SeqB {
    virtual void s0();
    virtual void* s1(int i, Mid* d);
};

struct Counter {
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual int s3();
};

struct Self {
    char pad0[0x28];
    Owner* f28;
    Seq* f2c;
    float f30;
    char pad34[0x84];
    char b8[0xdc - 0xb8];
    char dc[0xe8 - 0xdc];
    char e8[0xf4 - 0xe8];
    int fF4;
    int padF8;
    int fFC;
    int f100;
    unsigned char f104;
};

extern "C" {
void fn_27_3627C(void*, int);
void fn_27_37880(void*, int, float, float);
void fn_27_36768(void*, int, int);
int getCollision__11soCollisionFUlUl(void*, int, int);
void addCheckList__11soCollisionFv(void*);
unsigned int update__21soCollisionSearchPartFfP16soModuleAccesserP25soCollisionShieldPartUnitP5Vec3fP5Vec3f(void*, int, int, void*, void*, float);
}

extern const float lbl_27_rodata_534;

extern "C" void fn_27_4E670(Self* self, float a, float b) {
    int i;
    self->f30 = b;
    int r27 = self->f28->fd8->f44->s9();
    int r29 = ((Counter*)((char*)self + 0x34))->s3();
    for (int i = 0; i < r29; i++) {
        fn_27_3627C(self->b8, i);
        fn_27_37880(self->b8, i, a, lbl_27_rodata_534);
        fn_27_36768(self->b8, r27, i);
    }
    if (self->fFC > 0) {
        if (self->f100 == 0) {
            self->f100 = self->fFC;
        } else {
            self->f100 = self->f100 - 1;
            return;
        }
    }
    unsigned int r26 = 0U;
    int r30 = self->f2c->s3();
    for (int j = 0; j < r30; j++) {
        if (self->f2c->s1(j)->w0 != 0) {
            int v;
            int dv;
            void* obj;
            int res;
            if (j == self->fF4) {
                obj = (void*)self->f2c->s1(j)->f58;
                dv = self->f28->fd8->unk4;
                v = (int)((SeqB*)self->f2c)->s1(j, self->f28->fd8);
                res = getCollision__11soCollisionFUlUl(self->b8, j, (int)obj);
                if (update__21soCollisionSearchPartFfP16soModuleAccesserP25soCollisionShieldPartUnitP5Vec3fP5Vec3f((void*)v, dv, res, self->dc, self->e8, self->f30) == 1U) {
                    r26 = 1U;
                }
            } else {
                v = self->f2c->s1(j)->f58;
                dv = self->f28->fd8->unk4;
                obj = ((SeqB*)self->f2c)->s1(j, self->f28->fd8);
                res = getCollision__11soCollisionFUlUl(self->b8, j, v);
                if (update__21soCollisionSearchPartFfP16soModuleAccesserP25soCollisionShieldPartUnitP5Vec3fP5Vec3f(obj, dv, res, 0, 0, self->f30) == 1U) {
                    r26 = 1U;
                }
            }
        }
    }
    if (r26 == 1U) {
        addCheckList__11soCollisionFv(self->b8);
    }
    self->f104 = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_4EDB8 {
extern "C" s32 notifyEventCollisionSearchCheck__30soCollisionSearchEventObserverFv(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_4EDC0 {
extern "C" void fn_27_36510(void*);

extern "C" void fn_27_4EDC0(void* self) {
    fn_27_36510(reinterpret_cast<u8*>(self) + 0xb8);
}
}

// TODO: clean up, give variables real names
namespace jj_4FA94 {
extern "C" void notifyEventChangeStatus__27soCollisionSearchModuleImplFiiP12soStatusDataP16soModuleAccesser(void*);

extern "C" void fn_27_4FA94(void* self) {
    notifyEventChangeStatus__27soCollisionSearchModuleImplFiiP12soStatusDataP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_4FA9C {
extern "C" void fn_27_4EF28(void*);

extern "C" void fn_27_4FA9C(void* self) {
    fn_27_4EF28(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_4FAA4 {
extern "C" void isObserv__27soCollisionSearchModuleImplFc(void*);

extern "C" void fn_27_4FAA4(void* self) {
    isObserv__27soCollisionSearchModuleImplFc(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_4FAAC {
extern "C" void __dt__27soCollisionSearchModuleImplFv(void*);

extern "C" void fn_27_4FAAC(void* self) {
    __dt__27soCollisionSearchModuleImplFv(reinterpret_cast<u8*>(self) - 0x1c);
}
}

