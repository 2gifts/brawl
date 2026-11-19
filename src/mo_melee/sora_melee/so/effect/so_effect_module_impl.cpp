#define SO_ANIM_CMD_GET_FRAME_TAKES_INDEX
#define SO_EFFECT_ENUMERATOR_DTOR
#define SO_EFFECT_ENUMERATOR_OUT_OF_LINE
#define SO_ARRAY_LIST_DTOR_EXTERN
#define SO_EFFECT_ARRAY_DTOR_EXTERN
#define SO_EFFECT_LIST_UNIT_CTOR_EXTERN
#define SO_EFFECT_OBSERVER_ADD_EXTERN
#define SO_EFFECT_COMMON_LIST
#include <ec/ec_mgr.h>
#include <so/effect/so_effect_continual.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/effect/so_effect_screen.h>
#include <so/effect/so_effect_time.h>
#include <so/so_array.h>
#include <so/so_module_accesser.h>
#include <types.h>

// The common-effect queue is an embedded soArrayList at this+0x78. Virtual calls stay virtual
// when the static type is the abstract array base.
static inline soArray<soEffectModuleImpl::soEffectCommonPreserver>& commonEffects(soEffectModuleImpl* module) {
    return *reinterpret_cast<soArray<soEffectModuleImpl::soEffectCommonPreserver>*>(reinterpret_cast<char*>(module) + 0x78);
}


void soEffectModuleImpl::activate() {
    removeAllAfterImage(0, 0);
    removeAllContinual();
    removeAllTime();
    unk135_0 = false;
    unk54 = 0;
    unk135_2 = false;
    unk64 = 0;
    m_isSyncVisibility = true;
    unk74 = -1;
    unk134 = true;
}

void soEffectModuleImpl::pushGroup(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
}

void soEffectModuleImpl::reqTime(EfID effectID, int frame, Vec3f* pos, Vec3f* rot, float scale, u32 p6) {
    u32 handle = req(effectID, pos, rot, scale, p6, -1);
    if (handle != 0) {
        soEffectTime time;
        time.set(effectID, handle, frame);
        m_times->push(time);
    }
}

void soEffectModuleImpl::reqTimeFollow(EfID effectID, int nodeId, int frame, Vec3f* pos, Vec3f* rot, float scale, u32 p7) {
    u32 handle = reqFollow(effectID, nodeId, pos, rot, scale, false, p7, 0, -1);
    if (handle != 0) {
        soEffectTime time;
        time.set(effectID, handle, frame);
        m_times->push(time);
    }
}

void soEffectModuleImpl::removeContinual(u32 index) {
    soEffectContinual& continual = m_continuals->at(index);
    continual.m_isActive = false;
}

void soEffectModuleImpl::removeAllContinual() {
    int num = m_continuals->size();
    for (int i = 0; i < num; i++) {
        if (m_continuals->at(i).m_isActive == true) {
            m_continuals->at(i).m_isActive = false;
        }
    }
}

void soEffectModuleImpl::removeTime(int effectID) {
    for (int i = m_times->size() - 1; i >= 0; i--) {
        if (effectID == m_times->at(i).m_effectId) {
            m_times->at(i).end();
            m_times->erase(i);
        }
    }
}

void soEffectModuleImpl::removeAllTime() {
    int num = m_times->size();
    for (int i = 0; i < num; i++) {
        if (m_times->at(i).m_frame > 0) {
            m_times->at(i).end();
        }
    }
    m_times->clear();
}

void soEffectModuleImpl::remove(int handle) {
    g_ecMgr->endEffect(handle);
}

void soEffectModuleImpl::kill(int handle, bool p2, bool p3) {
    g_ecMgr->killEffect(handle, p2, p3);
}

void soEffectModuleImpl::setPos(int handle, Vec3f* pos) {
    g_ecMgr->setPos(handle, pos);
}

void soEffectModuleImpl::setRot(int handle, Vec3f* rot) {
    g_ecMgr->setRot(handle, rot);
}

void soEffectModuleImpl::setVisible(int handle, bool isVisible) {
    g_ecMgr->setVisible(handle, isVisible);
}

void soEffectModuleImpl::setScale(int handle, Vec3f* scale) {
    g_ecMgr->setScl(handle, scale);
}

bool soEffectModuleImpl::isExistEffect(int handle) {
    return g_ecMgr->isExistEffect(handle);
}

bool soEffectModuleImpl::isObserv(char unk1) {
    return unk1 == 0x11;
}

void soEffectModuleImpl::notifyEventDestructInstance(soModuleAccesser* moduleAccesser) {
    killAll(4, true, true);
}

void soEffectModuleImpl::setShieldEffect(int shieldEffect) {
    m_shieldEffect = shieldEffect;
}

bool soEffectModuleImpl::isSyncScale() {
    return m_isSyncScale;
}

void soEffectModuleImpl::setSyncScale(bool sync) {
    m_isSyncScale = sync;
}

bool soEffectModuleImpl::isSyncVisibility() {
    return m_isSyncVisibility;
}

void soEffectModuleImpl::setSyncVisibility(bool sync) {
    m_isSyncVisibility = sync;
}

u32 soEffectModuleImpl::req2D(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32 group) {
    g_ecMgr->preSet2dScene(true);
    u32 handle = req(effectID, pos, rot, scale, group, -1);
    g_ecMgr->preSet2dScene(false);
    return handle;
}

u32 soEffectModuleImpl::req(EfID effectID, int node, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool follow, u32 group) {
    int handle = 0xFFFF;
    return req(effectID, node, &handle, pos, rot, scale, posRange, rotRange, follow, group);
}

void soEffectModuleImpl::reqPause(bool pause) {
    if (pause) {
        if (!unk135_0) {
            g_ecMgr->pauseEffectGroup(m_groupId | 2, 0xFFFF02);
            ecMgr::getInst()->m_traceMgr.pauseGroup(true, m_groupId | 2, -1);
            unk135_0 = true;
        }
    } else if (!m_moduleAccesser->getStopModule().isStop() && unk135_0 == true) {
        g_ecMgr->playEffectGroup(m_groupId | 2, 0xFFFF02);
        ecMgr::getInst()->m_traceMgr.pauseGroup(false, m_groupId | 2, -1);
        unk135_0 = false;
    }
    unk135_2 = pause;
}

void soEffectModuleImpl::begin() {
    unk135_1 = false;
    if (m_moduleAccesser->getStopModule().isSpecialStop()) {
        if (!unk135_2 && unk135_0 == true) {
            g_ecMgr->playEffectGroup(m_groupId | 2, 0xFFFF02);
            ecMgr::getInst()->m_traceMgr.pauseGroup(false, m_groupId | 2, -1);
            unk135_0 = false;
        }
    } else if (m_moduleAccesser->getStopModule().isStop() || unk135_2 == true) {
        if (!unk135_0) {
            g_ecMgr->pauseEffectGroup(m_groupId | 2, 0xFFFF02);
            ecMgr::getInst()->m_traceMgr.pauseGroup(true, m_groupId | 2, -1);
            unk135_0 = true;
        }
    } else if (unk135_0 == true) {
        g_ecMgr->playEffectGroup(m_groupId | 2, 0xFFFF02);
        ecMgr::getInst()->m_traceMgr.pauseGroup(false, m_groupId | 2, -1);
        unk135_0 = false;
    }
}

void soEffectModuleImpl::removeAll(u32 flags) {
    g_ecMgr->endEffectGroup(m_groupId | flags, flags | 0xFFFF00);
}

void soEffectModuleImpl::killKind(int effectKind, bool p2, bool p3) {
    g_ecMgr->killEffectGroup(m_groupId, 0xFFFF00, p2, p3, effectKind);
}

void soEffectModuleImpl::killAll(u32 flags, bool p2, bool p3) {
    g_ecMgr->killEffectGroup(m_groupId | flags, flags | 0xFFFF00, p2, p3, 0);
}

bool soEffectModuleImpl::removeAfterImage(int id, int p2) {
    return g_ecMgr->m_traceMgr.end(id, p2, 0);
}

void soEffectModuleImpl::removeAllAfterImage(u32 flags, int p2) {
    g_ecMgr->m_traceMgr.endGroup(m_groupId | flags, flags | 0xFFFF00, p2);
}

void soEffectModuleImpl::pushGroupIgnoreScaleZ(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
    g_ecMgr->setFlatMode(false);
}

void soEffectModuleImpl::pushGroupScaleZ(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
    g_ecMgr->setFlatMode(true);
}

void soEffectModuleImpl::clearScreen(int index, u32 frames) {
    reinterpret_cast<soEffectScreen*>(reinterpret_cast<char*>(this) + 0x124)->clear(index, frames);
}

bool soEffectModuleImpl::isEndCommon() {
    if (unk70 >= 0) {
        return m_moduleAccesser->getAnimCmdModule().getAnimCmdFrame(unk70);
    }
    return false;
}

void soEffectModuleImpl::resetCommon() {
    if (unk70 >= 0) {
        m_moduleAccesser->getAnimCmdModule().resetInterpreter3(unk70, 0, m_moduleAccesser);
    }
    commonEffects(this).clear();
    unk74 = -1;
}

bool soEffectModuleImpl::isExistCommon(int index) {
    if (unk74 != -1) {
        if (unk74 == index) {
            return true;
        }
    }
    for (s32 i = 0; i < static_cast<soArray<soEffectCommonPreserver>&>(m_commonEffects).size(); i++) {
        if (static_cast<soArray<soEffectCommonPreserver>&>(m_commonEffects).at(i).m_index == index) {
            return true;
        }
    }
    return false;
}

bool soEffectModuleImpl::removeCommonPreserver(int index) {
    soArrayListEnumerator<soEffectCommonPreserver, s8> enumerator;
    enumerator = m_commonEffects.getEnumerator();
    do {
        if (enumerator.getCurrent().m_index == index) {
            m_commonEffects.erase(enumerator);
            return true;
        }
    } while (enumerator.next());
    return false;
}

void soEffectModuleImpl::removeCommon(int index) {
    if (index == unk74) {
        m_moduleAccesser->getAnimCmdModule().resetInterpreter3(unk70, 0, m_moduleAccesser);
        unk74 = -1;
    }
    while (removeCommonPreserver(index)) {
    }
}

template s32 soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::size() const;
template s32 soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::capacity() const;
template bool soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::isFull() const;
template bool soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::isNull() const;
template soEffectModuleImpl::soEffectCommonPreserver& soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::at(s32);
template const soEffectModuleImpl::soEffectCommonPreserver& soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::at(s32) const;
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::shift();
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::pop();
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::clear();
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::unshift(soEffectModuleImpl::soEffectCommonPreserver const&);
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::push(soEffectModuleImpl::soEffectCommonPreserver const&);
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::insert(s32, soEffectModuleImpl::soEffectCommonPreserver const&);
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::erase(s32);
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::set(s32, soEffectModuleImpl::soEffectCommonPreserver const&, s32);
template s32 soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::shiftFreeArrayIndex(s32);
template s32 soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::getArrayIndex(s32) const;
template s32 soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::insertSub(s32, s32);
template void soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::eraseSub(s32);
template <typename T>
inline soArray<T>::~soArray() { }
template <typename T>
inline soArrayContractible<T>::~soArrayContractible() { }
template <typename T>
inline soArrayFixed<T>::~soArrayFixed() { }
template soArray<soEffectModuleImpl::soEffectCommonPreserver>::~soArray();
template soArrayContractible<soEffectModuleImpl::soEffectCommonPreserver>::~soArrayContractible();
template soArrayFixed<soEffectModuleImpl::soEffectCommonPreserver>::~soArrayFixed();
template <typename ElementTy, typename IndexTy>
soArrayListUnit<ElementTy, IndexTy>::soArrayListUnit() : m_inUse(false), m_prev(End), m_next(End) { }
template soArrayListUnit<soEffectModuleImpl::soEffectCommonPreserver, s8>::soArrayListUnit();
template <typename T>
void soEventObserver<T>::addObserver(s16, s8) { }
template void soEventObserver<soCollisionShieldEventObserver>::addObserver(s16, s8);
template <typename T, s32 C>
soArrayList<T, C>::~soArrayList() { }
template soArrayList<soEffectModuleImpl::soEffectCommonPreserver, 10>::~soArrayList();
template class soArrayListEnumerator<soEffectModuleImpl::soEffectCommonPreserver, s8>;

#include <so/model/so_model_module_simple.h>
#include <so/event/so_event_system.h>
#include <so/so_instance_manager.h>
// TODO: clean up, give variables real names
namespace jj_978AC {
extern "C" char lbl_27_data_14FE8[];
extern "C" void* getInstance__13soEventSystemFv();
extern "C" void* getManager__13soEventSystemFl(void*, long);
extern "C" void __dl__FPv(void* p);

struct Sys {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual u32 f6(s16);
};

struct Mgr {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3(s16, s16);
};

extern "C" void* fn_27_978AC(void* p, s32 flag) {
    if (p != nullptr) {
        if (p != nullptr) {
            s16 id = *(s16*)((char*)p + 4);
            *(void**)p = (void*)lbl_27_data_14FE8;
            Sys* sys = (Sys*)getInstance__13soEventSystemFv();
            u32 result = sys->f6(id);
            if (result == 1U) {
                Mgr* mgr = (Mgr*)getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), *(s16*)((char*)p + 4));
                mgr->f3(*(s16*)((char*)p + 6), *(s16*)((char*)p + 8));
            }
            *(s16*)((char*)p + 8) = -1;
            *(s16*)((char*)p + 4) = -1;
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_97CE0 {
class soNullable {
public:
    virtual void isNull();                 // 0x08
    virtual void __dt__18soEffectModuleImplFv();            // 0x0c
    virtual void activate__18soEffectModuleImplFv();            // 0x10
    virtual void fn_27_97CE0();            // 0x14
    virtual void begin__18soEffectModuleImplFv();            // 0x18
    virtual void fn_27_98194();            // 0x1c
    virtual void fn_27_986B0();            // 0x20
    virtual void req2D__18soEffectModuleImplF4EfIDP5Vec3fP5Vec3ffUl();            // 0x24
    virtual void req__18soEffectModuleImplF4EfIDiP5Vec3fP5Vec3ffP5Vec3fP5Vec3fbUl();            // 0x28
    virtual void fn_27_98924();            // 0x2c
    virtual void fn_27_991C4();            // 0x30
    virtual void fn_27_993AC();            // 0x34
    virtual void fn_27_997C4();            // 0x38
    virtual void removeContinual__18soEffectModuleImplFUl();            // 0x3c
    virtual void removeAllContinual__18soEffectModuleImplFv();            // 0x40
    virtual void reqTime__18soEffectModuleImplF4EfIDiP5Vec3fP5Vec3ffUl();            // 0x44
    virtual void reqTimeFollow__18soEffectModuleImplF4EfIDiiP5Vec3fP5Vec3ffUl();            // 0x48
    virtual void removeTime__18soEffectModuleImplFi();            // 0x4c
    virtual void removeAllTime__18soEffectModuleImplFv();            // 0x50
    virtual void fn_27_99D88();            // 0x54
    virtual void remove__18soEffectModuleImplFi();            // 0x58
    virtual void removeAll__18soEffectModuleImplFUl();            // 0x5c
    virtual void kill__18soEffectModuleImplFibb();            // 0x60
    virtual void killKind__18soEffectModuleImplFibb();            // 0x64
    virtual void killAll__18soEffectModuleImplFUlbb(s32, s32, s32); // 0x68
    virtual void fn_27_99F08();            // 0x6c
    virtual void removeAfterImage__18soEffectModuleImplFii();            // 0x70
    virtual void removeAllAfterImage__18soEffectModuleImplFUli(s32, s32);    // 0x74
    virtual void setPos__18soEffectModuleImplFiP5Vec3f();            // 0x78
    virtual void setRot__18soEffectModuleImplFiP5Vec3f();            // 0x7c
    virtual void setScale__18soEffectModuleImplFiP5Vec3f();            // 0x80
    virtual void isExistEffect__18soEffectModuleImplFi();            // 0x84
    virtual void setVisible__18soEffectModuleImplFib();            // 0x88
    virtual void fn_27_9A158();            // 0x8c
    virtual void reqPause__18soEffectModuleImplFb();            // 0x90
    virtual void fn_27_9A1A4();            // 0x94
    virtual void clearScreen__18soEffectModuleImplFiUl();            // 0x98
    virtual void fn_27_9A1F4();            // 0x9c
    virtual void fn_27_9A380();            // 0xa0
    virtual void removeCommon__18soEffectModuleImplFi();            // 0xa4
    virtual void resetCommon__18soEffectModuleImplFv();            // 0xa8
};

extern "C" void clearAll__14soEffectScreenFv(void* p);

extern "C" void fn_27_97CE0(soNullable* self) {
    self->removeAllAfterImage__18soEffectModuleImplFUli(0, 0);
    self->removeAllContinual__18soEffectModuleImplFv();
    self->removeAllTime__18soEffectModuleImplFv();
    self->killAll__18soEffectModuleImplFUlbb(4, 1, 1);
    clearAll__14soEffectScreenFv(reinterpret_cast<char*>(self) + 0x124);
    self->resetCommon__18soEffectModuleImplFv();
}
}

// TODO: clean up, give variables real names
namespace jj_97D84 {
extern "C" const float lbl_27_rodata_C5C;
struct Entry { int id; float x,y,z; int extra; };
template <class T> struct List { T* data; unsigned count; };
struct Owner {
 char pad[0x44]; char* access;
 char pad48[8]; List<int>* ids; unsigned index;
 char pad58[8]; List<Entry>* entries; unsigned entryIndex;
};
extern "C" int fn_27_97D84(Owner* self, int id, Vec3f* out) {
 out->m_x = out->m_y = out->m_z = lbl_27_rodata_C5C;
 switch(id) {
 case -1: {
 id = self->ids->data[self->index];
 if (++self->index >= self->ids->count) self->index = 0;
 return id;
 }
 case -2: {
 Entry* entry = &self->entries->data[self->entryIndex];
 int result = entry->id;
 out->m_x = entry->x; out->m_y = entry->y; out->m_z = entry->z;
 if (++self->entryIndex >= self->entries->count) self->entryIndex = 0;
 return result;
 }
 default:
 return (*(soModelModule***)(self->access + 0xd8))[1]->getCorrectNodeId(id);
 }
}
}

// TODO: clean up, give variables real names
namespace jj_99D88 {
typedef unsigned int u32;

struct Vec3 {
    float x, y, z;
};

struct Entry {
    int id;
    Vec3 position;
    float value;
};

struct Entries {
    Entry* data;
    u32 count;
};

class Object {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void apply(int, int, Vec3*, Vec3*, int, int, int, float, int);

    char pad[0x5c];
    Entries* entries;
};

extern "C" const float lbl_27_rodata_C5C;

extern "C" void fn_27_99D88(Object* self, int arg1, int arg2) {
    if (self->entries) {
        for (u32 i = 0; i < self->entries->count; ++i) {
            Vec3 position;
            Vec3 zero = {
                lbl_27_rodata_C5C,
                lbl_27_rodata_C5C,
                lbl_27_rodata_C5C
            };
            Entry* entry = &self->entries->data[i];
            position.x = entry->position.x;
            position.y = entry->position.y;
            position.z = entry->position.z;
            self->apply(arg1, self->entries->data[i].id,
                        &position, &zero, 0, arg2, 0,
                        self->entries->data[i].value, -1);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_9A158 {
typedef unsigned char u8;
typedef unsigned long u32;

struct Object {
    u8 pad0[0x48];
    u32 group;
    u8 pad4C[0x135 - 0x4C];
    u8 unused : 3;
    u8 visible : 1;
    u8 rest : 4;
};

struct ecMgr;
extern ecMgr* g_ecMgr;
extern "C" void setVisibleGroup__5ecMgrFUlbUlUl(ecMgr*, bool, u32, u32);

extern "C" void fn_27_9A158(Object* obj, bool visible) {
    if (obj->visible == visible) return;
    obj->visible = visible;
    setVisibleGroup__5ecMgrFUlbUlUl(g_ecMgr, obj->visible, obj->group | 4, 0x00FFFF04);
}
}

// TODO: clean up, give variables real names
namespace jj_9A1A4 {
struct Color {
    unsigned char r, g, b, a;
};

extern "C" void fill__14soEffectScreenFiiRC8_GXColorii(void*, int, int, Color, int, int);

extern "C" void fn_27_9A1A4(
    void* self, int arg1, int arg2, const Color& color, int arg4, int arg5)
{
    fill__14soEffectScreenFiiRC8_GXColorii(static_cast<char*>(self) + 0x124,
                arg1, arg2, color, arg4, arg5);
}
}

// TODO: clean up, give variables real names
namespace jj_9A1F4 {
extern const float lbl_27_rodata_C58[16];

extern "C" int checkDeadArea__12soGroundUtilFP5Vec3fff(void* pos, float radius, float ratio);
extern "C" float checkDeadDivisionRate__12soGroundUtilFP5Vec3f(void* self);

extern "C" float fn_27_9A1F4(void* unused_this, void* arg1, unsigned char* out, float farg0) {
    const float* rd = lbl_27_rodata_C58;
    int dead = checkDeadArea__12soGroundUtilFP5Vec3fff(arg1, rd[1], rd[1]);
    float res = checkDeadDivisionRate__12soGroundUtilFP5Vec3f(arg1);
    float ret = rd[1];
    switch (dead) {
        case 2:
            if (res > (rd[0] - farg0)) {
                *out = 1;
                ret = rd[9];
            } else if (res < farg0) {
                *out = 1;
                ret = rd[10];
            } else {
                *out = 0;
                ret = rd[11];
            }
            break;
        case 3:
            if (res > (rd[0] - farg0)) {
                *out = 1;
                ret = rd[12];
            } else if (res < farg0) {
                *out = 1;
                ret = rd[13];
            } else {
                *out = 0;
                ret = rd[4];
            }
            break;
        case 0:
            *out = 0;
            ret = rd[0];
            break;
        case 1:
            if (res < farg0) {
                *out = 1;
                ret = rd[14];
            } else if (res > (rd[0] - farg0)) {
                *out = 1;
                ret = rd[15];
            } else {
                *out = 0;
            }
            break;
    }
    return ret;
}
}

// TODO: clean up, give variables real names
namespace jj_9A380 {
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" const float lbl_27_rodata_C58;

struct Accesser;
struct Modules;

class Module6C {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4(void*, const Accesser*, const Modules*, float);
    virtual void v5();
    virtual void v6(void*, void*, s32, float);
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual float v13(void*);
};

class ModuleAC {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9(s32);
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15(u16, u8);
};

class Object78 {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9(void*);
};

struct Entry {
    s32 field0;
    u16 field4;
    u8 field6;
    u8 field7;
};

struct Modules {
    char pad[0x6C];
    Module6C* module6c;
    char pad2[0x3C];
    ModuleAC* moduleac;
};

struct Accesser {
    char pad[0xD8];
    Modules* modules;
};

class Self {
public:
    virtual void v0();
    virtual void v1();
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
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual s32 v42(s32);

    char pad0[0x40];
    Accesser* m_accesser;
    char pad1[0x24];
    Entry* m_entries;
    void* m_0x70;
    s32 m_index;
    Object78 m_obj78;
};

struct Pair {
    s32 first;
    float second;
};

extern "C" void fn_27_9A380(Self* self, s32 arg1, float farg) {
    if (self->m_index == -1 ||
        self->m_entries[self->m_index].field4 <=
            self->m_entries[arg1].field4) {
        if (self->m_index != -1 &&
            self->m_entries[self->m_index].field7 != 0 &&
            self->v42(arg1) == 0) {
            float result = self->m_accesser->modules->module6c->v13(
                self->m_0x70);
            Pair p;
            p.first = self->m_index;
            p.second = result;
            self->m_obj78.v9(&p);
        }

        self->m_accesser->modules->module6c->v6(
            self->m_0x70, self->m_accesser, arg1, farg);

        const Accesser* accesser = self->m_accesser;
        const Modules* const modules = accesser->modules;
        modules->module6c->v4(
            self->m_0x70, accesser, modules, lbl_27_rodata_C58);

        self->m_index = arg1;
        self->m_accesser->modules->moduleac->v9(0);
        self->m_accesser->modules->moduleac->v15(
            self->m_entries[self->m_index].field4,
            self->m_entries[self->m_index].field6);
    } else if (self->m_entries[arg1].field7 != 0 &&
               self->v42(arg1) == 0) {
        Pair p;
        p.first = arg1;
        p.second = farg;
        self->m_obj78.v9(&p);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_9A96C {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
void* __ct__21soEffectScreenManagerFv(void* p);
void req__21soEffectScreenManagerFPC13acAnimCmdConviUl(void* self, void* conv, s32 frame, s32 val);
extern void* lbl_27_bss_2B78;
}

extern "C" void fn_27_9A96C(void* self, u32 index) {
    if (lbl_27_bss_2B78 == 0) {
        void* p = __nw__FUlQ25Heaps8HeapType(0x140, 2);
        if (p != 0) {
            p = __ct__21soEffectScreenManagerFv(p);
        }
        lbl_27_bss_2B78 = p;
    }
    char* table = *(char**)((char*)self + 0x120);
    req__21soEffectScreenManagerFPC13acAnimCmdConviUl(lbl_27_bss_2B78, *(void**)(table + (index << 3)), (s32)*(u16*)(table + (index << 3) + 4), (s32)self);
}
}

// TODO: clean up, give variables real names
namespace jj_9A9F4 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
void* __ct__21soEffectScreenManagerFv(void* p);
void remove__21soEffectScreenManagerFPC13acAnimCmdConvUl(void* a, void* b, void* c);
extern void* lbl_27_bss_2B78;
}

extern "C" void fn_27_9A9F4(void* self, u32 index) {
    if (lbl_27_bss_2B78 == 0) {
        void* p = __nw__FUlQ25Heaps8HeapType(0x140, 2);
        if (p != 0) {
            p = __ct__21soEffectScreenManagerFv(p);
        }
        lbl_27_bss_2B78 = p;
    }
    char* table = *(char**)((char*)self + 0x120);
    remove__21soEffectScreenManagerFPC13acAnimCmdConvUl(lbl_27_bss_2B78, *(void**)(table + (index << 3)), self);
}
}

// TODO: clean up, give variables real names
namespace jj_9E700 {
typedef unsigned char u8;
typedef unsigned int u32;

extern "C" const float lbl_27_rodata_C5C;

struct Rec {
    char pad[0x28];
    u8 f28;
    char pad2;
    u8 f2a;
};

class Sub {
public:
    virtual void v0();
    virtual Rec* at(int i);
    virtual void v2();
    virtual int count();
};

class Inner {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6(int a, void* b, int c, float d);
};

struct Ext {
    char pad[0x6c];
    Inner* p6c;
};

struct Mid {
    char pad[0xd8];
    Ext* ext;
};

struct Entry8 {
    char pad[6];
    u8 flag;
    char pad2;
};

struct Arg {
    char pad[0xc];
    u32 flags;
};

class Self {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16();
    virtual void s17();
    virtual void s18();
    virtual void s19();
    virtual void s20();
    virtual void s21();
    virtual void s22();
    virtual void s23();
    virtual void s24(int, int, int);
    virtual void s25();
    virtual void s26();
    virtual void s27(int, int);

    char pad0[0x44 - 4];
    Mid* f44;
    char pad1[0x4c - 0x48];
    Sub* f4c;
    char pad2[0x6c - 0x50];
    Entry8* f6c;
    int f70;
    int f74;
};

extern "C" void fn_27_9E700(Self* self, int a, int b, const Arg* arg) {
    if (!((arg->flags >> 5) & 1)) {
        self->s24(1, 0, 1);
        self->s27(1, 0);
        int n = self->f4c->count();
        for (int i = 0; i < n; i++) {
            if (self->f4c->at(i)->f28 == 1) {
                if (self->f4c->at(i)->f2a == 1) {
                    self->f4c->at(i)->f28 = 0;
                }
            }
        }
        int idx = self->f74;
        if (idx != -1 && self->f6c[idx].flag == 1) {
            Mid* m = self->f44;
            Inner* in = m->ext->p6c;
            in->v6(self->f70, m, -1, lbl_27_rodata_C5C);
            self->f74 = -1;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_9EA6C {

extern "C" {
extern char lbl_27_data_3FD0[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};

class Object_vptr_0 {
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

extern "C" void addObserver__30soCollisionShieldEventObserverFsSc(Self* self, s32 arg1, s8 arg2) {
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
                    Object_vptr_0* cast = (Object_vptr_0*)__dynamic_cast(unit, 0, lbl_27_data_3FD0, __RTTI__11soEventUnit, 0);
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
namespace jj_9FA0C {
extern "C" void fn_27_9AB18(void*);

extern "C" void fn_27_9FA0C(void* self) {
    fn_27_9AB18(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_9FA14 {
extern "C" void isObserv__18soEffectModuleImplFc(void*);

extern "C" void fn_27_9FA14(void* self) {
    isObserv__18soEffectModuleImplFc(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_9FA1C {
extern "C" void fn_27_9E700(void*);

extern "C" void fn_27_9FA1C(void* self) {
    fn_27_9E700(reinterpret_cast<u8*>(self) - 0x14);
}
}

// TODO: clean up, give variables real names
namespace jj_9FA24 {
extern "C" void notifyEventDestructInstance__18soEffectModuleImplFP16soModuleAccesser(void*);

extern "C" void fn_27_9FA24(void* self) {
    notifyEventDestructInstance__18soEffectModuleImplFP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x20);
}
}

// TODO: clean up, give variables real names
namespace jj_9FA2C {
extern "C" void fn_27_9E870(void*);

extern "C" void fn_27_9FA2C(void* self) {
    fn_27_9E870(reinterpret_cast<u8*>(self) - 0x2c);
}
}

// TODO: clean up, give variables real names
namespace jj_9FA34 {
extern "C" void fn_27_9E9B8(void*);

extern "C" void fn_27_9FA34(void* self) {
    fn_27_9E9B8(reinterpret_cast<u8*>(self) - 0x38);
}
}

// TODO: clean up, give variables real names
namespace jj_9FA3C {
extern "C" void fn_27_9E900(void*);

extern "C" void fn_27_9FA3C(void* self) {
    fn_27_9E900(reinterpret_cast<u8*>(self) - 0x38);
}
}

