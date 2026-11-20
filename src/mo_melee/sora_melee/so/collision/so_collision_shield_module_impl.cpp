#include <gf/gf_camera.h>
#include <so/collision/so_collision.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <so/collision/so_collision_shield_part.h>
#include <so/so_module_accesser.h>
#include <types.h>

soCollisionShieldModule::~soCollisionShieldModule() { }

void soCollisionShieldModuleImpl::clean() {
    for (int i = 0; i < m_collisionShieldGroupArray->size(); i++) {
        m_collisionShieldGroupArray->at(i).init();
    }
    _24 = 0;
    m_collision.clean();
}

void soCollisionShieldModuleImpl::deactivate() {
    if (_52 == 0) {
        clean();
    } else {
        m_collision.initLogAll();
    }
    m_collision.m_taskId = -1;
}

void soCollisionShieldModuleImpl::add(soCollisionShieldGroupData* data, u32 collisionShieldGroupIndex) {
    _24 += m_collisionShieldGroupArray->at(collisionShieldGroupIndex).add(&m_collision, m_partArray, data, collisionShieldGroupIndex, _24);
}

void soCollisionShieldModuleImpl::add(soCollisionReflectorGroupData* data, u32 collisionShieldGroupIndex) {
    _24 += m_collisionShieldGroupArray->at(collisionShieldGroupIndex).add(&m_collision, m_partArray, data, collisionShieldGroupIndex, _24);
}

void soCollisionShieldModuleImpl::update(double posX, double scale, double lr, u32 collisionShieldGroupIndex) {
    soModelModule& model = m_moduleAccesser->getModelModule();
    soCollisionShieldGroup& group = m_collisionShieldGroupArray->at(collisionShieldGroupIndex);
    if (group.update(&m_collision, m_partArray, &model, posX, scale, lr) == true && _50 == 0) {
        m_collision.addCheckList();
        _50 = 1;
    }
}

void soCollisionShieldModuleImpl::update(double posX, double scale, double lr) {
    int num = getGroupNum();
    for (int i = 0; i < num; i++) {
        update(posX, scale, lr, i);
    }
}

void soCollisionShieldModuleImpl::begin() {
    _50 = 0;
}

void soCollisionShieldModuleImpl::set(u32 index, soCollisionShieldData* data, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).set(m_partArray, index, data);
}

void soCollisionShieldModuleImpl::setReflectorData(soCollisionReflectorData* data, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).setReflectorData(data);
}

bool soCollisionShieldModuleImpl::isShield(u32 index, u32 collisionShieldGroupIndex) {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).isShield(m_partArray, index);
}

void soCollisionShieldModuleImpl::setStatus(u32 index, u32 status, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).setStatus(m_partArray, index, status);
}

void soCollisionShieldModuleImpl::setStatusAll(u32 status, u32 collisionShieldGroupIndex) {
    for (int i = 0; i < getPartSize(collisionShieldGroupIndex); i++) {
        setStatus(i, status, collisionShieldGroupIndex);
    }
}

u8 soCollisionShieldModuleImpl::isTurn(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isTurn;
}

u8 soCollisionShieldModuleImpl::isFront(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isFront;
}

bool soCollisionShieldModuleImpl::isHop(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isHop;
}

float soCollisionShieldModuleImpl::getHopAngle(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_hopAngle;
}

bool soCollisionShieldModuleImpl::isNoHop(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isNoHop;
}

void soCollisionShieldModuleImpl::setTurn(bool isTurn, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isTurn = isTurn;
}

void soCollisionShieldModuleImpl::setFront(bool isFront, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isFront = isFront;
}

void soCollisionShieldModuleImpl::setHop(float hopAngle, bool isHop, u32 collisionShieldGroupIndex) {
    soCollisionShieldGroup& group = m_collisionShieldGroupArray->at(collisionShieldGroupIndex);
    group.m_isHop = isHop;
    group.m_hopAngle = hopAngle;
}

double soCollisionShieldModuleImpl::getAttackMul(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_attackMul;
}

double soCollisionShieldModuleImpl::getSpeedMul(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_speedMul;
}

double soCollisionShieldModuleImpl::getLifeMul(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_lifeMul;
}

float soCollisionShieldModuleImpl::getAttackLimit(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_attackLimit;
}

u8 soCollisionShieldModuleImpl::isNoMBall(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_isNoMBall;
}

int soCollisionShieldModuleImpl::getPartSize(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_partSize;
}

void soCollisionShieldModuleImpl::clearAll() {
    int num = m_collisionShieldGroupArray->size();
    for (int i = 0; i < num; i++) {
        m_collisionShieldGroupArray->at(i).clearAll(m_partArray);
    }
}

void soCollisionShieldModuleImpl::clearAll(u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).clearAll(m_partArray);
}

void soCollisionShieldModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    int num = m_partArray->size();
    for (int i = 0; i < num; i++) {
        if (m_partArray->at(i).m_isActive == 1) {
            m_partArray->at(i).m_isActive = 0;
        }
    }
}

bool soCollisionShieldModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

u32 soCollisionShieldModuleImpl::getGroupNum() {
    return m_collisionShieldGroupArray->size();
}

double soCollisionShieldModuleImpl::getPosX(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_posX;
}

void soCollisionShieldModuleImpl::setTargetProperty(u32 property, u32 collisionShieldGroupIndex) {
    m_collisionShieldGroupArray->at(collisionShieldGroupIndex).setTargetProperty(m_partArray, property);
}

bool soCollisionShieldModuleImpl::check() {
    if (m_collisionOccurred) {
        return _4C->notifyEventCollisionShieldCheck();
    }
    return false;
}

void soCollisionShieldModuleImpl::renderDebug() {
    nw4r::math::_MTX34 view = gfCameraManager::getManager()->getCamera(0)->m_viewMatrix;
    int num = m_collisionShieldGroupArray->size();
    for (int i = 0; i < num; i++) {
        m_collisionShieldGroupArray->at(i).renderDebug(&view, &m_collision, m_partArray);
    }
}

Vec3f soCollisionShieldModuleImpl::getCenterPos(u32 index, u32 collisionShieldGroupIndex) {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).getCenterPos(&m_collision, index);
}

float soCollisionShieldModuleImpl::getLR(u32 collisionShieldGroupIndex) const {
    return m_collisionShieldGroupArray->at(collisionShieldGroupIndex).m_Lr;
}

#include <gf/gf_task_scheduler.h>
// TODO: clean up, give variables real names
namespace jj_48DA8 {
struct WorkObj {
    virtual float f1() = 0;
    virtual WorkObj* process(float f1, float f2, float f3) = 0;
};

struct ThisClass {
    u32 pad0[8];
    u32 m_20;
    u32 pad24;
    u32 m_28;
    u32 pad2c[7];
    WorkObj* m_work;
    u32 pad4c;
    u8 m_50;
    u8 m_51;
};

extern "C" void fn_27_46BB4(WorkObj* work, u32* ptr, u32 val, float f1, float f2, float f3);

extern "C" void fn_27_48DA8(ThisClass* self, float f1, float f2, float f3) {
    WorkObj* work = self->m_work->process(f1, f2, f3);
    fn_27_46BB4(work, &self->m_28, self->m_20, f1, f2, f3);
    self->m_50 = 0;
    self->m_51 = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_48E2C {
struct Obj {
    virtual void v0();
    virtual void call(int, float, float, float);
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual int count();

    unsigned char pad[0x30];
    int value;
};

extern "C" void fn_27_48E2C(Obj* self, int value, float a, float b, float c) {
    self->value = value;
    for (int i = 0; i < self->count(); ++i) {
        self->call(i, a, b, c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_496A4 {
class Inner {
public:
    virtual void v0();
    virtual void v1();
    virtual Inner* v2();
};

struct Mid {
    char pad[0x18];
    Inner* p18;
};

struct Outer {
    char pad[0xd8];
    Mid* pd8;
};

struct Top {
    char pad[0x1c];
    Outer* p1c;
};

extern "C" void fn_27_496A4(const Top* self) {
    Inner* p = self->p1c->pd8->p18;
    p->v2()->v2();
}
}

// TODO: clean up, give variables real names
namespace jj_496EC {
struct Entry {
 char pad[12]; unsigned long id; char pad10[17];
 unsigned char active, category, pad23, arg24, pad25[2], arg27;
};
class soNullable {
public:
 virtual void isNull();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual unsigned int slot6();
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
 virtual float slot26(unsigned char, unsigned char);
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
 virtual int slot42(unsigned char, unsigned char);
};
class Mode {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4(); virtual void slot5();
 virtual unsigned int slot6();
 virtual void slot7(); virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11(); virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15(); virtual void slot16(); virtual void slot17(); virtual void slot18(); virtual void slot19(); virtual void slot20(); virtual void slot21(); virtual void slot22(); virtual void slot23(); virtual void slot24(); virtual void slot25();
 virtual float slot26(int, Mode*, float);
};
struct Modules { char pad[0xc8]; Mode* mode; };
struct Access { char pad[0xd8]; Modules* modules; };
struct Data { char pad[0x98]; float value; };
class Array {
public:
 virtual void slot0(); virtual Data* slot1(int); virtual void slot2(); virtual int slot3();
};
class Handler {
public:
 virtual void slot0();
 virtual void slot1(soNullable*, Entry*, int, Access*, float, float, float);
 virtual void slot2(void*, Entry*, int, Access*);
};
struct Self {
 char pad[0x1c]; Access* access; char pad20[8]; char collection[0x20];
 Array* array; Handler* handler; char pad50; unsigned char flag;
};
extern "C" {
 extern const char __RTTI__11StageObject[];
 extern const char __RTTI__6gfTask[];
 void* __dynamic_cast(void*, long, const void*, const void*, long);
 void* getLogList__11soCollisionFUl(void*, int);
 float getPosX__11soCollisionFUl(void*, int);
 soNullable* getCollisionAttackModule__23soExternalValueAccesserFP11StageObject(void*);
 void* getCollisionSearchModule__23soExternalValueAccesserFP11StageObject(void*);
 Mode* getGlowModule__23soExternalValueAccesserFP11StageObject(void*);
 Entry* List_GetNext__Q24nw4r2utFPCQ34nw4r2ut4ListPCv(void*, Entry*);
}
extern "C" void fn_27_496EC(Self* self) {
 self->flag = 0;
 int count = self->array->slot3();
 for (int i = 0; i < count; ++i) {
  void* collection = getLogList__11soCollisionFUl(self->collection, i);
  Entry* entry = 0;
  while ((entry = List_GetNext__Q24nw4r2utFPCQ34nw4r2ut4ListPCv(collection, entry)) != 0) {
   gfTask* found = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)entry->category, entry->id);
   if (found != 0) {
    void* task = __dynamic_cast(found, 0x3c, __RTTI__11StageObject, __RTTI__6gfTask, 0);
    if (entry->active == 0) {
     soNullable* posture = getCollisionAttackModule__23soExternalValueAccesserFP11StageObject(task);
     Mode* mode = getGlowModule__23soExternalValueAccesserFP11StageObject(task);
     float v;
     float value = posture->slot26(entry->arg24, entry->arg27);
     if (self->access->modules->mode->slot6() == 1 && mode->slot6() == 1) {
      task = self->access->modules->mode;
      value = ((Mode*)task)->slot26(posture->slot42(entry->arg24, entry->arg27), mode, value);
     }
     v = self->array->slot1(i)->value;
     float other = getPosX__11soCollisionFUl(self->collection, i);
     self->handler->slot1(posture, entry, i, self->access, value, other, v);
    } else {
     self->handler->slot2(getCollisionSearchModule__23soExternalValueAccesserFP11StageObject(task), entry, i, self->access);
    }
    self->flag = 1;
   }
  }
 }
}
}

// TODO: clean up, give variables real names
namespace jj_4AC28 {
extern "C" void notifyEventChangeStatus__27soCollisionShieldModuleImplFiiP12soStatusDataP16soModuleAccesser(void*);

extern "C" void fn_27_4AC28(void* self) {
    notifyEventChangeStatus__27soCollisionShieldModuleImplFiiP12soStatusDataP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_4AC30 {
extern "C" void fn_27_49C60(void*);

extern "C" void fn_27_4AC30(void* self) {
    fn_27_49C60(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_4AC38 {
extern "C" void isObserv__27soCollisionShieldModuleImplFc(void*);

extern "C" void fn_27_4AC38(void* self) {
    isObserv__27soCollisionShieldModuleImplFc(reinterpret_cast<u8*>(self) - 0x10);
}
}

