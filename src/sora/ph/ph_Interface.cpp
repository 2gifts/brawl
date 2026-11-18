#include <types.h>
// Brawl physics wrapper translation unit ph_Interface.o (main.dol 0x8009A104-0x8009BA40).
// Not yet decompiled. Functions in address order with their map names:
//   0x8009A104   220  __ct
//   0x8009A1E0   200  __dt
//   0x8009A2A8   228  deleteInstance
//   0x8009A38C   352  init
//   0x8009A4EC    16  sysAlloc
//   0x8009A4FC     4  gfMemFree
//   0x8009A500    64  __dt
//   0x8009A540   540  removeHavokSystem
//   0x8009A75C   248  get2ndaryController
//   0x8009A854   152  get2ndary2Controller
//   0x8009A8EC   948  copy2ndaryMatrix
//   0x8009ACA0  1432  copy2ndaryMatrix2
//   0x8009B238   236  getIKController
//   0x8009B324     8  getArray2ndaryController
//   0x8009B32C     8  getArrayIKController
//   0x8009B334   632  initialize
//   0x8009B5AC   756  processDefault
//   0x8009B8A0    92  create2ndaryWorld
//   0x8009B8FC    84  delete2ndaryWorld
//   0x8009B950   116  delete2ndaryController
//   0x8009B9C4   100  renderDebug
//   0x8009BA28     4  notifiAllEndCalcWorldTimingC
//   0x8009BA2C    20  errorReport

#include <ph/ph2ndaryWorld.h>
#include <ph/ph_Interface.h>
#include <gf/gf_heap_manager.h>
#include <revolution/OS/OSError.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkWorld.h>
#include <havok/hkBaseSystem.h>

extern phInterface* lbl_8049DEC8[];

phInterface* phInterface::s_instance = nullptr;

phInterface::phInterface() : gfTask("PhysicsSystem", Category_Physics, 0xe, 0xd, true) {
    m_flags40[0] = 1;
    m_flags40[1] = 0;
    m_flags40[2] = 1;
    m_flags40[3] = 1;
    m_flags40[4] = 1;
    m_flags40[5] = 1;
    m_flags40[6] = 0;
    m_flags40[7] = 0;
    unk48 = 0;
    unk4C = 0;
    unk50 = 0;
    unk54 = 0;
    m_2ndaryWorld = nullptr;
    m_worldFlag = 0;
    m_2ndaryControllers.m_data = nullptr;
    m_2ndaryControllers.m_size = 0;
    m_2ndaryControllers.m_capacityAndFlags = hkArrayBase::DONT_DEALLOCATE_FLAG;
    m_ikControllers.m_data = nullptr;
    m_ikControllers.m_size = 0;
    m_ikControllers.m_capacityAndFlags = hkArrayBase::DONT_DEALLOCATE_FLAG;
    unk78 = 0;
    unk84[0] = 0;
    unk84[1] = 0;
    unk84[2] = 0;
    unk84[3] = 0;
    unk84[4] = 0;
    unk84[5] = 0;
    unk84[6] = 0;
    unk84[7] = 0;
    unk84[8] = 0;
    unk84[9] = 0;
}

phInterface::~phInterface() {
    reinterpret_cast<hkArray<phIKController*>*>(&m_ikControllers)->~hkArray();
    reinterpret_cast<hkArray<ph2ndaryController*>*>(&m_2ndaryControllers)->~hkArray();
    if (m_flags40) {
        s_instance = nullptr;
    }
}

void phInterface::deleteInstance() {
    if (lbl_8049DEC8[0] != nullptr) {
        if (m_2ndaryControllers.m_size != 0) {
            if (m_2ndaryControllers.mustDeallocate()) {
                hkThreadMemory::s_instance->deallocateChunk(m_2ndaryControllers.m_data,
                                                            m_2ndaryControllers.m_capacityAndFlags << 2, 0x15);
            }
            m_2ndaryControllers.m_data = nullptr;
            m_2ndaryControllers.m_size = 0;
            m_2ndaryControllers.m_capacityAndFlags =
                (m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                hkArrayBase::DONT_DEALLOCATE_FLAG;
        }
        if (m_ikControllers.m_size != 0) {
            if (m_ikControllers.mustDeallocate()) {
                hkThreadMemory::s_instance->deallocateChunk(m_ikControllers.m_data,
                                                            m_ikControllers.m_capacityAndFlags << 2, 0x15);
            }
            m_ikControllers.m_data = nullptr;
            m_ikControllers.m_size = 0;
            m_ikControllers.m_capacityAndFlags =
                (m_ikControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                hkArrayBase::DONT_DEALLOCATE_FLAG;
        }
        lbl_8049DEC8[0]->exit();
        lbl_8049DEC8[0] = nullptr;
    }
}

void* phInterface::sysAlloc(size_t a, size_t b) {
    return gfHeapManager::alloc((Heaps::HeapType)0xd, a + b);
}

void phInterface::gfMemFree(void* p) {
    gfHeapManager::free(p);
}

ph2ndaryController* phInterface::get2ndaryController(void* owner, int a, int b) {
    ph2ndaryController* controller = nullptr;
    if (s_instance != nullptr) {
        bool enabled = s_instance->m_flags40[0] && s_instance->m_flags40[3];
        if (enabled && owner != nullptr) {
            controller = new (Heaps::Physics) ph2ndaryController(owner, a, b);
            static_cast<hkArray<ph2ndaryController*>*>(&s_instance->m_2ndaryControllers)->pushBack(controller);
        }
    }
    return controller;
}

ph2ndary2Controller* phInterface::get2ndary2Controller(void* a, void* b) {
    ph2ndary2Controller* controller = nullptr;
    if (lbl_8049DEC8[0] != nullptr) {
        u8 enabled = lbl_8049DEC8[0]->m_flags40[0] ? lbl_8049DEC8[0]->m_flags40[3] : 0;
        if (enabled && a != nullptr) {
            controller = new (Heaps::Physics) ph2ndary2Controller(a, b);
        }
    }
    return controller;
}

hkArray<ph2ndaryController*>* phInterface::getArray2ndaryController() {
    return static_cast<hkArray<ph2ndaryController*>*>(&m_2ndaryControllers);
}

hkArray<phIKController*>* phInterface::getArrayIKController() {
    return static_cast<hkArray<phIKController*>*>(&m_ikControllers);
}

void phInterface::delete2ndaryController() {
    hkArray<ph2ndaryController*>* controllers = static_cast<hkArray<ph2ndaryController*>*>(&m_2ndaryControllers);
    if (controllers->getSize() == 0) {
        if (controllers->mustDeallocate()) {
            hkThreadMemory::s_instance->deallocateChunk(controllers->m_data,
                                                        (controllers->m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
        }
        controllers->m_data = nullptr;
        controllers->m_size = 0;
        controllers->m_capacityAndFlags =
            (controllers->m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    }
}

void phInterface::renderDebug() {
    for (int i = 0; i < m_2ndaryControllers.m_size; i++) {
        ((ph2ndaryController**)m_2ndaryControllers.m_data)[i]->draw();
    }
}

void phInterface::notifiAllEndCalcWorldTimingC() {
}

void phInterface::errorReport(const char* msg) {
    OSReport("[physics]:%s", msg);
}

ph2ndaryWorld* phInterface::create2ndaryWorld() {
    if (m_worldFlag && m_2ndaryWorld == nullptr) {
        m_2ndaryWorld = new (Heaps::Physics) ph2ndaryWorld();
    }
}

void phInterface::delete2ndaryWorld() {
    if (m_2ndaryWorld != nullptr && m_2ndaryControllers.m_size == 0) {
        delete m_2ndaryWorld;
        m_2ndaryWorld = nullptr;
    }
}

void phInterface::removeHavokSystem() {
    if (m_worldFlag) {
        unk78 = 1;
        if (m_2ndaryControllers.m_size == 0) {
            if (m_2ndaryControllers.mustDeallocate()) {
                hkThreadMemory::s_instance->deallocateChunk(m_2ndaryControllers.m_data,
                    m_2ndaryControllers.m_capacityAndFlags << 2, 0x15);
            }
            m_2ndaryControllers.m_data = nullptr;
            m_2ndaryControllers.m_size = 0;
            m_2ndaryControllers.m_capacityAndFlags =
                (m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                hkArrayBase::DONT_DEALLOCATE_FLAG;
        }
        if (m_ikControllers.m_size == 0) {
            if (m_ikControllers.mustDeallocate()) {
                hkThreadMemory::s_instance->deallocateChunk(m_ikControllers.m_data,
                    m_ikControllers.m_capacityAndFlags << 2, 0x15);
            }
            m_ikControllers.m_data = nullptr;
            m_ikControllers.m_size = 0;
            m_ikControllers.m_capacityAndFlags =
                (m_ikControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                hkArrayBase::DONT_DEALLOCATE_FLAG;
        }
        delete2ndaryWorld();
    } else if (unk78) {
        if (unk54 != nullptr) {
            unk54->removeReference();
            unk54 = nullptr;
        }
        delete2ndaryWorld();
        unk78 = 0;
    }
    if (unk54 != nullptr) {
        unk54->removeReference();
        unk54 = nullptr;
    }
    unk48->setStackArea(nullptr, 0);
    hkMemory::s_instance->deallocate(unk4C);
    unk48->removeReference();
    hkBaseSystem::quit();
    m_flags40[7] = 0;
}

void phInterface::processDefault() {
    if (m_worldFlag) {
        if (m_worldFlag) {
            unk78 = 1;
            if (m_2ndaryControllers.m_size == 0) {
                if (m_2ndaryControllers.mustDeallocate()) {
                    hkThreadMemory::s_instance->deallocateChunk(m_2ndaryControllers.m_data,
                        m_2ndaryControllers.m_capacityAndFlags << 2, 0x15);
                }
                m_2ndaryControllers.m_data = nullptr;
                m_2ndaryControllers.m_size = 0;
                m_2ndaryControllers.m_capacityAndFlags =
                    (m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                    hkArrayBase::DONT_DEALLOCATE_FLAG;
            }
            if (m_ikControllers.m_size == 0) {
                if (m_ikControllers.mustDeallocate()) {
                    hkThreadMemory::s_instance->deallocateChunk(m_ikControllers.m_data,
                        m_ikControllers.m_capacityAndFlags << 2, 0x15);
                }
                m_ikControllers.m_data = nullptr;
                m_ikControllers.m_size = 0;
                m_ikControllers.m_capacityAndFlags =
                    (m_ikControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                    hkArrayBase::DONT_DEALLOCATE_FLAG;
            }
            delete2ndaryWorld();
        } else if (unk78) {
            if (unk54 != nullptr) {
                unk54->removeReference();
                unk54 = nullptr;
            }
            delete2ndaryWorld();
            unk78 = 0;
        }
        initialize();
        if (m_worldFlag) {
            unk78 = 1;
            if (m_2ndaryControllers.m_size == 0) {
                if (m_2ndaryControllers.mustDeallocate()) {
                    hkThreadMemory::s_instance->deallocateChunk(m_2ndaryControllers.m_data,
                        m_2ndaryControllers.m_capacityAndFlags << 2, 0x15);
                }
                m_2ndaryControllers.m_data = nullptr;
                m_2ndaryControllers.m_size = 0;
                m_2ndaryControllers.m_capacityAndFlags =
                    (m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                    hkArrayBase::DONT_DEALLOCATE_FLAG;
            }
            if (m_ikControllers.m_size == 0) {
                if (m_ikControllers.mustDeallocate()) {
                    hkThreadMemory::s_instance->deallocateChunk(m_ikControllers.m_data,
                        m_ikControllers.m_capacityAndFlags << 2, 0x15);
                }
                m_ikControllers.m_data = nullptr;
                m_ikControllers.m_size = 0;
                m_ikControllers.m_capacityAndFlags =
                    (m_ikControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                    hkArrayBase::DONT_DEALLOCATE_FLAG;
            }
            delete2ndaryWorld();
        } else if (unk78) {
            if (unk54 != nullptr) {
                unk54->removeReference();
                unk54 = nullptr;
            }
            delete2ndaryWorld();
            unk78 = 0;
        }
    }
}

// TODO: clean up, give variables real names
namespace jj_8009A1E0 {
extern "C" void fn_8027EABC(void*, void*, unsigned int, int);
extern "C" void __dt__6gfTaskFv(void*, int);
extern "C" void __dl__FPv(void*);

extern void* lbl_805A0EC8;
extern void* lbl_8049DEC8[3];

struct Array {
    void* data;
    int size;
    unsigned int raw;

    unsigned int rawValue() const {
        return raw;
    }
};

struct Object {
    char pad[0x60];
    Array a;
    Array b;
};

extern "C" Object* fn_8009A1E0(Object* self, int flag) {
    if (self) {
        if ((unsigned int)self + 0x6c != 0 && !(self->b.raw & 0x80000000)) {
            fn_8027EABC(lbl_805A0EC8, self->b.data, self->b.rawValue() << 2, 0x15);
        }
        if ((unsigned int)self + 0x60 != 0 && !(self->a.raw & 0x80000000)) {
            fn_8027EABC(lbl_805A0EC8, self->a.data, self->a.rawValue() << 2, 0x15);
        }
        if ((unsigned int)self + 0x40 != 0) {
            lbl_8049DEC8[0] = 0;
        }
        __dt__6gfTaskFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_8009B238 {
struct Vec {
    void** data;
    int size;
    unsigned int capacity;
    void push(void* value);
};
struct Manager {
    unsigned char pad[0x40];
    unsigned char active;
    unsigned char pad41[3];
    unsigned char enabled;
    unsigned char pad45[0x27];
    Vec objects;
};
extern "C" Manager* lbl_8049DEC8[];
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_800966EC(void*, void*);
extern "C" void fn_8027CE3C(Vec*, int);
inline void Vec::push(void* value) {
    if (size == (int)(capacity & 0x3FFFFFFF))
        fn_8027CE3C(this, 4);
    data[size++] = value;
}
extern "C" void* fn_8009B238(void* arg) {
    if (lbl_8049DEC8[0]) {
        unsigned char enabled = lbl_8049DEC8[0]->active ? lbl_8049DEC8[0]->enabled : 0;
        if (enabled && lbl_8049DEC8[0] && arg) {
            void* obj = __nw__FUlQ25Heaps8HeapType(0x20, 0xD);
            if (obj)
                obj = fn_800966EC(obj, arg);
            if (obj) {
                lbl_8049DEC8[0]->objects.push(obj);
                return obj;
            }
        }
    }
    return 0;
}
}

namespace jj_8009A38C {
typedef unsigned char u8;
class Object_vptr_0 {
public:
    virtual void* slot0(void*, int);
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
    virtual void slot18(int);
    char pad[8];
    int count;
};
struct Self {
    char pad[0x47];
    u8 flag;
    Object_vptr_0* obj;
    void* value;
};
struct State {
    char pad[0x40];
    u8 flag;
    u8 value;
};
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" Object_vptr_0* fn_8027ED0C(void*);
extern "C" Object_vptr_0* fn_8027E5A8(void*, Object_vptr_0*, int);
extern "C" void fn_8027B8D8(Object_vptr_0*, Object_vptr_0*, void (*)(int), int);
extern "C" void fn_8027B89C(Object_vptr_0*);
extern "C" void* fn_8009A4EC(int, int);
extern "C" void fn_8009A4FC();
extern "C" void fn_8009BA2C(int);
extern void* (*lbl_8059EF40)(int, int);
extern void (*lbl_8059EF44)();
extern Object_vptr_0* lbl_805A0EB8;
extern void* lbl_8059DC18;
extern State* lbl_8049DEC8[32];
extern u8 lbl_805A0270;
extern "C" void fn_8009A38C(Self* self) {
    if (!self->flag) {
        self->flag = 1;
        lbl_8059EF40 = fn_8009A4EC;
        lbl_8059EF44 = fn_8009A4FC;
        Object_vptr_0* p = (Object_vptr_0*)__nw__FUlQ25Heaps8HeapType(0xD34, 0xD);
        if (p) p = fn_8027ED0C(p);
        Object_vptr_0* q = (Object_vptr_0*)__nw__FUlQ25Heaps8HeapType(0x320, 0xD);
        if (q) q = fn_8027E5A8(q, p, 0x10);
        self->obj = q;
        fn_8027B8D8(p, q, fn_8009BA2C, 0);
        if (--p->count == 0 && p) p->slot18(1);
        self->value = lbl_805A0EB8->slot0(lbl_8059DC18, 1);
        self->obj->slot0(self->value, (int)lbl_8059DC18);
        fn_8027B89C(self->obj);
    }
    if (self->flag) {
        lbl_805A0270 = lbl_8049DEC8[0]->flag ? lbl_8049DEC8[0]->value : 0;
    }
}
}

