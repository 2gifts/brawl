#include <so/event/so_event_manage_module_impl.h>
#include <types.h>

// soEventSystem::addManager and soEventSystem::eraseManager (sora_melee map), still unnamed in so_event_system.
extern "C" s16 fn_27_90238(soEventSystem* system, soEventManager& manager);
extern "C" void fn_27_90D80(soEventSystem* system, s16 manageId);

soEventManageModuleImpl::soEventManageModuleImpl(void* unitList) :
    m_unitList(static_cast<soInstanceManager<soEventUnit*>*>(unitList)), m_manageId(-1) {
    m_manageId = fn_27_90238(soEventSystem::getInstance(), *this);
}

soEventManageModuleImpl::~soEventManageModuleImpl() {
    fn_27_90D80(soEventSystem::getInstance(), m_manageId);
}

bool soEventManageModuleImpl::isExist(short id) {
    return m_unitList->isContain(id);
}

soEventUnit* soEventManageModuleImpl::getEventUnit(short id) {
    return m_unitList->at(id);
}

s16 soEventManageModuleImpl::addUnit(soEventUnit* unit) {
    return m_unitList->add(unit, unit->getEventId());
}

void soEventManageModuleImpl::eraseUnit(u32 id) {
    if (m_unitList->isContain(id)) {
        m_unitList->erase(id);
    }
}

bool soEventManageModuleImpl::isNullUnit(u32 id) {
    if (!m_unitList->isContain(id)) {
        return true;
    }
    soEventUnit* unit = m_unitList->at(id);
    if (unit == nullptr) {
        return true;
    }
    return unit->isNull();
}

// TODO: clean up, give variables real names
namespace jj_8ECC4 {
class A {
public:
    virtual ~A();
    virtual void third(int);    // slot 1  -> 0x0C
    virtual void p2();
    virtual void p3();
    virtual void p4();
    virtual void p5();
    virtual void p6();
    virtual void p7();
    virtual void p8();
    virtual void p9();
    virtual void p10();
    virtual void p11();
    virtual void p12();
    virtual void p13();
    virtual void p14();
    virtual void p15();
    virtual void p16();
    virtual void p17();
    virtual A* first();         // slot 18 -> 0x50
    virtual void p19();
    virtual A* second(int);     // slot 20 -> 0x58
};

extern "C" void fn_27_8ECC4(A* a, int b, int c) {
    A* r = a->first();
    if (r == (A*)1) {
        A* r2 = a->second(b);
        r2->third(c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8EEF4 {
class Leaf {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual s32 f3();
};

class Node {
public:
    Leaf* m_leaf;
};

class Inner {
public:
    virtual void f0();
    virtual Node* f1(void* arg);
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual s32 f6(void* arg);
};

class Owner {
public:
    char m_pad[0xc];
    Inner* m_inner;
};

extern "C" s32 fn_27_8EEF4(Owner* self, void* arg) {
    if (!self->m_inner->f6(arg)) {
        return 0;
    }
    Leaf* leaf = self->m_inner->f1(arg)->m_leaf;
    if (leaf == 0) {
        return 0;
    }
    return leaf->f3();
}
}

// TODO: clean up, give variables real names
namespace jj_8EF88 {
class Leaf {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual int f4();
};

struct Node {
    Leaf* m_leaf;
};

class Inner {
public:
    virtual void f0();
    virtual Node* f1(int arg);
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual int f6();
};

struct Owner {
    char m_pad[0xc];
    Inner* m_inner;
};

extern "C" int fn_27_8EF88(Owner* self, int arg) {
    if (self->m_inner->f6() == 0) {
        return 0;
    }

    Leaf* leaf = self->m_inner->f1(arg)->m_leaf;
    if (leaf == 0) {
        return 0;
    }

    return leaf->f4();
}
}

// TODO: clean up, give variables real names
namespace jj_8F024 {
extern "C" void fn_27_4FCD0(void*);

extern "C" void fn_27_8F024(void* self) {
    fn_27_4FCD0(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_8F02C {
extern "C" void fn_27_8EF88(void*);

extern "C" void fn_27_8F02C(void* self) {
    fn_27_8EF88(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_8F034 {
extern "C" void fn_27_8EEF4(void*);

extern "C" void fn_27_8F034(void* self) {
    fn_27_8EEF4(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_8F044 {
extern "C" void fn_27_8ECC4(void*);

extern "C" void fn_27_8F044(void* self) {
    fn_27_8ECC4(reinterpret_cast<u8*>(self) - 0x8);
}
}

