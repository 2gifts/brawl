// Havok translation unit hkPoweredChainData.o (main.dol 0x802DAD74-0x802DB990).
// Not yet decompiled. Functions in address order:
//   0x802DAD74     8  getType
//   0x802DAD7C    36  __as
//   0x802DADA0   116  getConstraintInfo
//   0x802DAE14    44  getRuntimeInfo
//   0x802DAE40    20  getConstraintFlags
//   0x802DAE54    12  getRuntime
//   0x802DAE60  1788  buildJacobian
//   0x802DB55C     8  getEntityA
//   0x802DB564     8  hkAddByteOffset<21hkVelocityAccumulator>
//   0x802DB56C     8  getSize
//   0x802DB574     8  getMotion
//   0x802DB57C    12  __ct
//   0x802DB588     8  getTransform
//   0x802DB590     8  val
//   0x802DB598     4  __ct
//   0x802DB59C   132  _setTransformedPos
//   0x802DB620     4  __ct
//   0x802DB624     4  getRotation
//   0x802DB628   168  setMul
//   0x802DB6D0    20  set
//   0x802DB6E4    32  getMotorRuntimeQuaternions
//   0x802DB704    36  lengthSquared4
//   0x802DB728   220  estimateAngleToLs
//   0x802DB804    68  sub4
//   0x802DB848    48  setNeg3
//   0x802DB878     4  __ct
//   0x802DB87C    36  __ct
//   0x802DB8A0    12  getColumn
//   0x802DB8AC     8  getSolverResults
//   0x802DB8B4    92  hk1dAngularVelocityMotorCommitJacobianInMotorInfo
//   0x802DB910   108  swap<12hkQuaternion>
//   0x802DB97C     8  __ct
//   0x802DB984    12  __as

#include <havok/hkPoweredChainData.h>
#include <types.h>

int hkPoweredChainData::getType() const {
    return 102;
}

// Info block: the sizes are linear in the chain length n.
void hkPoweredChainData::getConstraintInfo(hkConstraintInfo* info) const {
    ((hkConstraintInfoWithAtoms*)info)->unk10 = (void*)&m_bridgeAtom;
    ((hkConstraintInfoWithAtoms*)info)->unk14 = (u32)((u8*)&m_unk18 - (u8*)&m_bridgeAtom);
    info->m_maxSizeOfJacobians = 0;
    info->m_sizeOfJacobians = 0;
    info->m_numSolverResults = 0;
    info->m_sizeOfSchemas = 0x18;
    s32 n = m_chainLength;
    s32 n1 = n + 1;
    s32 sizeA = n1 * 0x20 + n * 0x20;
    s32 sizeB = (n1 << 2) + n * 0x4C;
    info->m_numSolverResults = n * 6;
    info->m_sizeOfSchemas = sizeB + 0x30;
    s32 sizeC = n * 0xF0;
    s32 sizeD = n * 0x3C0;
    info->m_sizeOfJacobians = sizeA + (sizeC + sizeD);
}

void hkPoweredChainData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) const {
    s32 n = m_chainLength;
    s32 a = n * 6;
    out->unk04 = a;
    s32 b = ((n + 3) & ~3) + a * 8;
    out->unk00 = b + n * 16;
}

void* hkPoweredChainData::getConstraintFlags(void* base) const {
    return (u8*)base + ((m_chainLength * 6) << 3);
}

void* hkPoweredChainData::getMotorRuntimeQuaternions(void* base) const {
    s32 n = m_chainLength;
    s32 a = n * 6;
    return (((n + 3) & ~3)) + ((u8*)base + (a << 3));
}

// HYPOTHESIS: returns its argument unchanged (the target is a bare `mr r3, r4`).
void* hkPoweredChainData::getSolverResults(void* results) const {
    return results;
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" int fn_802DB564(int arg0, int arg1) {
    return arg0 + arg1;
}

extern "C" void fn_802DB57C(int* arg0, int arg1, int arg2) {
    *arg0 = arg2 - arg1;
}

extern "C" void fn_802DB984(int* destination, const int* source)
{
    *destination = *source;
}

// hkTransform::getRotation / hkRotation ctor (bare blr)
// TODO: cleanup (shape from jj-link/brawl fn_802DB624)
extern "C" void* fn_802DB624(void* t) { return t; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB878)
extern "C" void fn_802DB878(void*) {}
// hkConstraintInstance::getEntityA, hkArray<hkEntity*>::getSize, hkEntity::getMotion, hkMotion::getTransform,
// hkPadSpu<hkJacobianElement*>::val, hkPadSpuLong<hkConstraintInstance*> ctor
// TODO: cleanup (shape from jj-link/brawl fn_802DB55C)
extern "C" void* fn_802DB55C(const char* p) { return *(void* const*)(p + 0x14); }
// TODO: cleanup (shape from jj-link/brawl fn_802DB56C)
extern "C" int fn_802DB56C(const char* p) { return *(const int*)(p + 0x4); }
// TODO: cleanup (shape from jj-link/brawl fn_802DB574)
extern "C" char* fn_802DB574(char* p) { return p + 0xA0; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB588)
extern "C" char* fn_802DB588(char* p) { return p + 0x10; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB590)
extern "C" void* fn_802DB590(void* const* p) { return *p; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB97C)
extern "C" void fn_802DB97C(void** p, void* v) { *p = v; }
// hkConstraintInstance::getRuntime (m_internal->m_runtime), hkMatrix3::getColumn
// TODO: cleanup (shape from jj-link/brawl fn_802DAE54)
extern "C" void* fn_802DAE54(const char* p) { return *(void* const*)(*(const char* const*)(p + 0x28) + 0x20); }
// TODO: cleanup (shape from jj-link/brawl fn_802DB8A0)
extern "C" char* fn_802DB8A0(char* p, int i) { return p + i * 16; }
// hkVector4::set, hkQuaternion::operator=, hkVector4::lengthSquared4, setNeg3, sub4
// TODO: cleanup (shape from jj-link/brawl fn_802DB6D0)
extern "C" void fn_802DB6D0(float* p, float a, float b, float c, float d) {
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
}
// TODO: cleanup (shape from jj-link/brawl fn_802DAD7C)
extern "C" void fn_802DAD7C(float* d, const float* s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
}
// TODO: cleanup (shape from jj-link/brawl fn_802DB704)
extern "C" float fn_802DB704(const float* v) {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}
// TODO: cleanup (shape from jj-link/brawl fn_802DB848)
extern "C" void fn_802DB848(float* d, const float* s) {
    d[0] = -s[0];
    d[1] = -s[1];
    d[2] = -s[2];
    d[3] = s[3];
}
// TODO: cleanup (shape from jj-link/brawl fn_802DB804)
extern "C" void fn_802DB804(float* a, const float* b) {
    a[0] = a[0] - b[0];
    a[1] = a[1] - b[1];
    a[2] = a[2] - b[2];
    a[3] = a[3] - b[3];
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_802DB910.cpp.
// hkAlgorithm::swap<hkQuaternion>
struct hkFloat4A_802DB910 {
    float x, y, z, w;
} __attribute__((aligned(16)));
// TODO: cleanup (shape from jj-link/brawl fn_802DB910)
extern "C" void fn_802DB910(hkFloat4A_802DB910* a, hkFloat4A_802DB910* b) {
    hkFloat4A_802DB910 temp;
    temp.x = a->x;
    temp.y = a->y;
    temp.z = a->z;
    temp.w = a->w;
    *a = *b;
    *b = temp;
}


// fn_802DB8B4: body from https://github.com/jj-link/brawl src/havok/fn_802DB8B4.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DB8B4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DB8B4)
extern "C" void fn_802DB8B4(float* a, const float* b, float** c, float* d) {
    const float s = b[1];
    const float s2 = b[2];
    float* const p = *c;
    d[1] = a[2] * s;
    d[2] = a[3] * s;
    d[3] = a[1];
    d[4] = a[4];
    d[5] = a[5];
    d[0] = 1.0f;
    p[7] = a[0] * s2;
    *c = p + 8;
}
} // namespace jj_fn_802DB8B4

// fn_802DB59C: body from https://github.com/jj-link/brawl src/havok/fn_802DB59C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DB59C {
#pragma fp_contract on

extern "C" {

extern "C" const float lbl_805A426C;

// TODO: cleanup (shape from jj-link/brawl fn_802DB59C)
void fn_802DB59C(float *out, float *m, float *v)
{
    float a, b, c;
    float x = v[0];
    float y = v[1];
    float z = v[2];

    a = m[12] + ((z * m[8]) + ((x * m[0]) + (y * m[4])));
    b = m[13] + ((z * m[9]) + ((x * m[1]) + (y * m[5])));
    c = m[14] + ((z * m[10]) + ((x * m[2]) + (y * m[6])));

    out[3] = lbl_805A426C;
    out[0] = a;
    out[1] = b;
    out[2] = c;
}

}
} // namespace jj_fn_802DB59C

#include <types.h>
#include <havok/hkBallSocketChainData.h>
void cleanupLoadedObjecthkBallSocketChainData(hkBallSocketChainData* self);
#include <havok/hkConstraintAtom.h>

// fn_802DAB58: body from https://github.com/jj-link/brawl src/havok/fn_802DAB58.cpp (namespaced).
namespace jj_fn_802DAB58 {
struct Value { s32 value; };

extern "C" void fn_802DABE8(s32, Value*);

// TODO: cleanup (shape from jj-link/brawl fn_802DAB58)
extern "C" void fn_802DAB58(s32 arg0) {
    Value value;
    value.value = 1;
    if (arg0 != 0) {
        Value copy = value;
        fn_802DABE8(arg0, &copy);
    }
}
} // namespace jj_fn_802DAB58

// fn_802DAB8C: body from https://github.com/jj-link/brawl src/havok/fn_802DAB8C.cpp (namespaced).
namespace jj_fn_802DAB8C {
class Object {
public:
    virtual void slot0(int);
};

// TODO: cleanup (shape from jj-link/brawl fn_802DAB8C)
extern "C" void fn_802DAB8C(Object* self) {
    self->slot0(-1);
}
} // namespace jj_fn_802DAB8C

// fn_802DABA0: body from https://github.com/jj-link/brawl src/havok/fn_802DABA0.cpp (namespaced).
namespace jj_fn_802DABA0 {
struct Result {
    int value;
    char rest[60];
};
extern "C" void fn_802DABE8(Result*, int*);
// TODO: cleanup (shape from jj-link/brawl fn_802DABA0)
extern "C" int fn_802DABA0() {
    __attribute__((aligned(16))) Result out;
    int zero = 0;
    if (&out != 0) {
        int in = zero;
        fn_802DABE8(&out, &in);
    }
    return out.value;
}
} // namespace jj_fn_802DABA0

// fn_802DABE8: body from https://github.com/jj-link/brawl src/havok/fn_802DABE8.cpp (namespaced).
namespace jj_fn_802DABE8 {
extern "C" {
extern char lbl_80487A38[];
void fn_802D95E4(void* self, void* v);

// TODO: cleanup (shape from jj-link/brawl fn_802DABE8)
extern "C" void* fn_802DABE8(void* p) {
    u8* self = (u8*)p;
    void* saved = *(void**)(self + 0x14);
    *(s16*)(self + 6) = 1;
    *(void**)self = (void*)lbl_80487A38;
    ((hkBridgeConstraintAtom*)(self + 0xC))->init((hkConstraintData*)saved);
    ((hkBridgeConstraintAtom*)(self + 0xC))->init((hkConstraintData*)self);
    return p;
}
}
} // namespace jj_fn_802DABE8

// fn_802DAC40: body from https://github.com/jj-link/brawl src/havok/fn_802DAC40.cpp (namespaced).
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
namespace jj_fn_802DAC40 {
#pragma use_lmw_stmw on
extern "C" void fn_8027EABC(void*, void*, unsigned int, int);
extern "C" char lbl_80487A38[];
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};
class Elem {
public:
    virtual void vf0(int);
    unsigned short m04;
    short m06;
};
struct Entry {
    char pad[0x40];
    Elem* e[3];
    char pad2[4];
};
struct Array {
    Entry* data;
    int count;
    unsigned int raw;
    bool isOwned() const { return (raw & 0x80000000) == 0; }
    void dealloc() { hkThreadMemory::s_instance->deallocateChunk(data, (raw & 0x3fffffff) * 0x50, 0x15); }
};
struct Obj {
    void* vtable;
    unsigned short type;
    char pad[0x12];
    Array array;
};
// TODO: cleanup (shape from jj-link/brawl fn_802DAC40)
extern "C" Obj* fn_802DAC40(Obj* self, int flag) {
    if (self) {
        self->vtable = lbl_80487A38;
        for (int i = 0; i < self->array.count; i++) {
            for (int j = 0; j < 3; j++) {
                Elem* e = self->array.data[i].e[j];
                if (e && e->m04 != 0) {
                    if (--e->m06 == 0 && e != 0) {
                        e->vf0(1);
                    }
                }
            }
        }
        if ((char*)&self->array != 0 && self->array.isOwned())
            self->array.dealloc();
        if (flag > 0)
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x2a);
    }
    return self;
}
} // namespace jj_fn_802DAC40

extern "C" char* fn_802DAD64(char* const* a, int i) { return *a + i * 0x50; }

#include <types.h>

// fn_802DB990: body from https://github.com/jj-link/brawl src/havok/fn_802DB990.cpp (namespaced).
namespace jj_fn_802DB990 {
struct fn_802DB990_work {
    void* m0;
    void* m4;
    unsigned int m8;
};

// TODO: cleanup (shape from jj-link/brawl fn_802DB990)
extern "C" void fn_802DB990(fn_802DB990_work* self, void* value)
{
    self->m0 = (char*)self + 0xc;
    self->m4 = value;
    self->m8 = 0x80000020;
}
} // namespace jj_fn_802DB990

// fn_802DB9AC: body from https://github.com/jj-link/brawl src/havok/fn_802DB9AC.cpp (namespaced).
namespace jj_fn_802DB9AC {
// TODO: cleanup (shape from jj-link/brawl fn_802DB9AC)
extern "C" u32 fn_802DB9AC(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DB9AC

// fn_802DB9B4: body from https://github.com/jj-link/brawl src/havok/fn_802DB9B4.cpp (namespaced).
namespace jj_fn_802DB9B4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DB9B4)
extern "C" u32 fn_802DB9B4(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DB9B4

struct hkInplaceArrayHeader_802DB9BC {
    void* m_data;
    int m_size;
    int m_capacityAndFlags;
};
// TODO: cleanup (shape from jj-link/brawl fn_802DB9BC)
extern "C" void fn_802DB9BC(hkInplaceArrayHeader_802DB9BC* a, int size) {
    a->m_data = (char*)a + 0xC;
    a->m_size = size;
    a->m_capacityAndFlags = 0x80000020;
}

// fn_802DB9D8: body from https://github.com/jj-link/brawl src/havok/fn_802DB9D8.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802DB9D8 {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned int, int);
};


struct Array {
    void* data;
    int size;
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, (raw & 0x3FFFFFFF) * 0x4C, 0x15);
    }

    ~Array() {
        if (isOwned())
            dealloc();
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802DB9D8)
extern "C" Array* fn_802DB9D8(Array* self, int flag) {
    if (self) {
        self->~Array();
        if (flag > 0)
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, 0x98c, 0x15);
    }
    return self;
}
} // namespace jj_fn_802DB9D8

#include <types.h>
#include <havok/hkArray.h>

// fn_802DBA6C: body from https://github.com/jj-link/brawl src/havok/fn_802DBA6C.cpp (namespaced).
namespace jj_fn_802DBA6C {
extern "C" void fn_8027CD94(void* a, s32 b, s32 c);

struct Fn802DBA6C_Class {
    s32 m_unk0;
    s32 m_unk4;
    s32 m_unk8;
};

// TODO: cleanup (shape from jj-link/brawl fn_802DBA6C)
extern "C" void fn_802DBA6C(Fn802DBA6C_Class* self, s32 arg1) {
    s32 val = self->m_unk8 & 0x3FFFFFFF;
    if (val < arg1) {
        s32 doubled = val * 2;
        hkArrayUtil::_reserve((hkArrayBase*)self, (arg1 < doubled) ? doubled : arg1, 0x4C);
    }
    self->m_unk4 = arg1;
}
} // namespace jj_fn_802DBA6C

// fn_802DBAD4: body from https://github.com/jj-link/brawl src/havok/fn_802DBAD4.cpp (namespaced).
namespace jj_fn_802DBAD4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBAD4)
extern "C" void* fn_802DBAD4(void** p, int index) {
    return (char*)*p + index * 4;
}
} // namespace jj_fn_802DBAD4

// fn_802DBB04: body from https://github.com/jj-link/brawl src/havok/fn_802DBB04.cpp (namespaced).
namespace jj_fn_802DBB04 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB04)
extern "C" void fn_802DBB04(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0) = value;
}
} // namespace jj_fn_802DBB04

// fn_802DBB0C: body from https://github.com/jj-link/brawl src/havok/fn_802DBB0C.cpp (namespaced).
namespace jj_fn_802DBB0C {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB0C)
extern "C" void fn_802DBB0C(unsigned int* dst, const unsigned int* src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}
} // namespace jj_fn_802DBB0C

// fn_802DBB28: body from https://github.com/jj-link/brawl src/havok/fn_802DBB28.cpp (namespaced).
namespace jj_fn_802DBB28 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB28)
extern "C" void fn_802DBB28(float* dst, float* src) {
    dst[0] = src[0];
}
} // namespace jj_fn_802DBB28

// fn_802DBB34: body from https://github.com/jj-link/brawl src/havok/fn_802DBB34.cpp (namespaced).
namespace jj_fn_802DBB34 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB34)
extern "C" void fn_802DBB34(int* destination, const int* source)
{
    *destination = *source;
}
} // namespace jj_fn_802DBB34

// fn_802DBB40: body from https://github.com/jj-link/brawl src/havok/fn_802DBB40.cpp (namespaced).
namespace jj_fn_802DBB40 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB40)
extern "C" void fn_802DBB40(int* destination, const int* source)
{
    *destination = *source;
}
} // namespace jj_fn_802DBB40

// fn_802DBBB8: body from https://github.com/jj-link/brawl src/havok/fn_802DBBB8.cpp (namespaced).
namespace jj_fn_802DBBB8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBB8)
extern "C" u32 fn_802DBBB8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBBB8

// fn_802DBBC0: body from https://github.com/jj-link/brawl src/havok/fn_802DBBC0.cpp (namespaced).
namespace jj_fn_802DBBC0 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBC0)
extern "C" u32 fn_802DBBC0(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBBC0

// fn_802DBBC8: body from https://github.com/jj-link/brawl src/havok/fn_802DBBC8.cpp (namespaced).
namespace jj_fn_802DBBC8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBC8)
extern "C" u32 fn_802DBBC8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBBC8

// fn_802DBBD0: body from https://github.com/jj-link/brawl src/havok/fn_802DBBD0.cpp (namespaced).
namespace jj_fn_802DBBD0 {
extern "C" {
extern const float lbl_805A426C;

// TODO: cleanup (shape from jj-link/brawl fn_802DBBD0)
extern "C" void fn_802DBBD0(void* p) {
    float x = lbl_805A426C;
    *(float*)((char*)p + 8) = x;
    *(float*)((char*)p + 12) = x;
}
}
} // namespace jj_fn_802DBBD0

// fn_802DBBE0: body from https://github.com/jj-link/brawl src/havok/fn_802DBBE0.cpp (namespaced).
namespace jj_fn_802DBBE0 {
typedef struct Vec2f {
    float x;
    float y;
} Vec2f;

// TODO: cleanup (shape from jj-link/brawl fn_802DBBE0)
extern "C" void fn_802DBBE0(Vec2f *dst, const Vec2f *src)
{
    dst->x = src->x;
    dst->y = src->y;
}
} // namespace jj_fn_802DBBE0

// fn_802DBBF4: body from https://github.com/jj-link/brawl src/havok/fn_802DBBF4.cpp (namespaced).
namespace jj_fn_802DBBF4 {
extern "C" {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBF4)
extern "C" void *fn_802DBBF4(void *self, int index)
{
    char *base = *(char **)self;
    return base + index * 0x4c;
}
}
} // namespace jj_fn_802DBBF4

// fn_802DBC04: body from https://github.com/jj-link/brawl src/havok/fn_802DBC04.cpp (namespaced).
namespace jj_fn_802DBC04 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC04)
extern "C" void fn_802DBC04(int* destination, const int* source)
{
    *destination = *source;
}
} // namespace jj_fn_802DBC04

// fn_802DBC10: body from https://github.com/jj-link/brawl src/havok/fn_802DBC10.cpp (namespaced).
namespace jj_fn_802DBC10 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC10)
extern "C" u32 fn_802DBC10(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBC10

// fn_802DBC18: body from https://github.com/jj-link/brawl src/havok/fn_802DBC18.cpp (namespaced).
namespace jj_fn_802DBC18 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC18)
extern "C" u32 fn_802DBC18(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBC18

// fn_802DBC20: body from https://github.com/jj-link/brawl src/havok/fn_802DBC20.cpp (namespaced).
namespace jj_fn_802DBC20 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC20)
extern "C" u32 fn_802DBC20(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x1c);
}
} // namespace jj_fn_802DBC20

// fn_802DBC28: body from https://github.com/jj-link/brawl src/havok/fn_802DBC28.cpp (namespaced).
namespace jj_fn_802DBC28 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC28)
extern "C" unsigned int fn_802DBC28(void) {
    return 0x01000000;
}
} // namespace jj_fn_802DBC28

// fn_802DBC30: body from https://github.com/jj-link/brawl src/havok/fn_802DBC30.cpp (namespaced).
namespace jj_fn_802DBC30 {
extern "C" {
int fn_802DABA0(void);
void fn_802DAB58(void);
void fn_802DAB8C(void);
extern char lbl_804121F0[];
extern unsigned int lbl_80532E50[4];
}

// TODO: cleanup (shape from jj-link/brawl fn_802DBC30)
extern "C" void fn_802DBC30(void) {
    int temp_r3 = fn_802DABA0();
    lbl_80532E50[0] = (unsigned int)lbl_804121F0;
    lbl_80532E50[1] = (unsigned int)fn_802DAB58;
    lbl_80532E50[2] = (unsigned int)fn_802DAB8C;
    lbl_80532E50[3] = (unsigned int)temp_r3;
}

__declspec(section ".ctors") extern void* const fn_802DBC30_ctor = (void*)fn_802DBC30;
} // namespace jj_fn_802DBC30

// TODO: clean up, give variables real names
namespace jj_802DB598 {
extern "C" void __ct__9hkVector4Fv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_802DB620 {
extern "C" void __ct__12hkQuaternionFv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_802DB87C {
extern "C" {
void __ct__9hkVector4FRC9hkVector4(float* dst, const float* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}
}
}

// hkQuaternion::estimateAngleToLs: twice the vector part of a * conj(b) (w lane: twice b.w * a.w - a.w * b.w),
// negated when the two quaternions point into opposite hemispheres.
// TODO: clean up: hkQuaternion member once the unit's callers are written (map name; weak copy).
extern "C" void fn_802DB728(const hkVector4* a, const hkVector4* b, hkVector4* out) {
    hkReal x = a->y * b->z - a->z * b->y;
    x += b->w * a->x;
    x -= a->w * b->x;
    hkReal y = a->z * b->x - a->x * b->z;
    y += b->w * a->y;
    y -= a->w * b->y;
    hkReal z = a->x * b->y - a->y * b->x;
    z += b->w * a->z;
    z -= a->w * b->z;
    hkReal w = 0.0f;
    w += b->w * a->w;
    w -= a->w * b->w;
    out->x = x + x;
    out->y = y + y;
    out->z = z + z;
    out->w = w + w;
    hkReal dot = b->x * a->x + b->y * a->y + b->z * a->z + b->w * a->w;
    int mask = dot < 0.0f ? 8 : 0;
    if (mask) {
        out->setNeg4(*out);
    }
}

