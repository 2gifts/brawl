#include <so/glow/so_glow_module_impl.h>
#include <types.h>

class soCollisionAttackData;

// The fighter glow module: per-attribute and per-region attack glow, defense glow and queued attack statuses on top of
// the common glow module.
// HYPOTHESIS: the field roles follow the setters; the array lengths follow the field offsets.
// TODO: activate, setup, getGlowAttack, getGlowDefense, notifyEventChangeStatus and the weapon glow queries.
class ftGlowModuleImpl : public soGlowModuleImpl {
public:
    int m_attackStatusAdd;       // 0x24
    int m_attackStatus[4][2];    // 0x28
    int m_attackStatusCount;     // 0x48
    int m_weaponAttack;          // 0x4C
    int m_attackRegion[26];      // 0x50
    int m_attackAttr[24];        // 0xB8
    int m_defenseAttr[24];       // 0x118
    int unk178;                  // 0x178

    ftGlowModuleImpl(soModuleAccesser* acc);
    virtual ~ftGlowModuleImpl();
    void setGlowAttackAttr(int attr, int add);
    void setGlowAttackRegion(int region, int add);
    void setGlowAttackStatus(int status, int value);
    int getGlowAttackAdd(soCollisionAttackData* attackData);
    void setGlowDefenseAttr(int attr, int add);
    void setWeaponGlowAttack(int add);
    void addGlowAttackStatus(int add);
    void clearGlowAttackStatus();
};

ftGlowModuleImpl::ftGlowModuleImpl(soModuleAccesser* acc) : soGlowModuleImpl(acc) {
    unk178 = 0;
}

ftGlowModuleImpl::~ftGlowModuleImpl() { }

void ftGlowModuleImpl::setGlowAttackAttr(int attr, int add) {
    m_attackAttr[attr] += add;
}

void ftGlowModuleImpl::setGlowAttackRegion(int region, int add) {
    m_attackRegion[region] += add;
}

void ftGlowModuleImpl::setGlowAttackStatus(int status, int value) {
    int* entry = m_attackStatus[m_attackStatusCount];
    entry[0] = status;
    entry[1] = value;
    m_attackStatusCount++;
}

// HYPOTHESIS: the attack data words at 0x30 and 0x38 carry the attribute and the region in their low five bits.
int ftGlowModuleImpl::getGlowAttackAdd(soCollisionAttackData* attackData) {
    u32 attr = reinterpret_cast<u32*>(attackData)[0x30 / 4] & 0x1f;
    u32 region = reinterpret_cast<u32*>(attackData)[0x38 / 4] & 0x1f;
    return m_attackRegion[region] + m_attackAttr[attr];
}

void ftGlowModuleImpl::setGlowDefenseAttr(int attr, int add) {
    m_defenseAttr[attr] += add;
}

void ftGlowModuleImpl::setWeaponGlowAttack(int add) {
    m_weaponAttack += add;
}

void ftGlowModuleImpl::addGlowAttackStatus(int add) {
    m_attackStatusAdd += add;
}

void ftGlowModuleImpl::clearGlowAttackStatus() {
    m_attackStatusCount = 0;
}

// TODO: clean up, give variables real names
namespace jj_14CD88 {
extern "C" void clear__16soGlowModuleImplFv(void* p);

extern "C" void fn_27_14CD88(void* self) {
    s32* p = (s32*)self;
    clear__16soGlowModuleImplFv(self);
    p[19] = 12;
    p[9] = 0;
    p[18] = 0;
    p[20] = 0;
    p[21] = 0;
    p[22] = 0;
    p[23] = 0;
    p[24] = 0;
    p[25] = 0;
    p[26] = 0;
    p[27] = 0;
    p[28] = 0;
    p[29] = 0;
    p[30] = 0;
    p[31] = 0;
    p[32] = 0;
    p[33] = 0;
    p[34] = 0;
    p[35] = 0;
    p[36] = 0;
    p[37] = 0;
    p[38] = 0;
    p[39] = 0;
    p[40] = 0;
    p[41] = 0;
    p[42] = 0;
    p[43] = 0;
    p[44] = 0;
    p[45] = 0;
    p[46] = 0;
    p[47] = 0;
    p[48] = 0;
    p[49] = 0;
    p[50] = 0;
    p[51] = 0;
    p[52] = 0;
    p[53] = 0;
    p[54] = 0;
    p[55] = 0;
    p[56] = 0;
    p[57] = 0;
    p[58] = 0;
    p[59] = 0;
    p[60] = 0;
    p[61] = 0;
    p[62] = 0;
    p[63] = 0;
    p[64] = 0;
    p[65] = 0;
    p[66] = 0;
    p[67] = 0;
    p[68] = 0;
    p[69] = 0;
    p[70] = 0;
    p[71] = 0;
    p[72] = 0;
    p[73] = 0;
    p[74] = 0;
    p[75] = 0;
    p[76] = 0;
    p[77] = 0;
    p[78] = 0;
    p[79] = 0;
    p[80] = 0;
    p[81] = 0;
    p[82] = 0;
    p[83] = 0;
    p[84] = 0;
    p[85] = 0;
    p[86] = 0;
    p[87] = 0;
    p[88] = 0;
    p[89] = 0;
    p[90] = 0;
    p[91] = 0;
    p[92] = 0;
    p[93] = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_14CEF0 {
struct ftManager;
struct ftOwner;

extern "C" ftOwner* getOwner__9ftManagerFi(ftManager*, int);
extern ftManager* g_ftManager;

struct ftSub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual int v4(int arg);
};

struct ftMid {
    unsigned char pad[0x64];
    ftSub* sub;
};

struct ftSrc {
    unsigned char pad[0xd8];
    ftMid* mid;
};

struct ftSelf {
    unsigned char pad[0x178];
    ftOwner* owner;
};

extern "C" {
void fn_27_14CEF0(ftSelf* self, ftSrc* src) {
    ftManager* mgr = g_ftManager;
    ftSub* sub = src->mid->sub;
    int id = sub->v4(0x10000000);
    self->owner = getOwner__9ftManagerFi(mgr, id);
}
}
}

// TODO: clean up, give variables real names
namespace jj_14CFD0 {
struct Object {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual int first();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual int second(int);

    unsigned char padding0[0x20];
    int value;
    unsigned char padding1[0x150];
    void* component;
};

extern "C" int getGlowAttackAdd__7ftOwnerFv(void*);

extern "C" int fn_27_14CFD0(Object* self, int arg) {
    int first = self->first();
    return getGlowAttackAdd__7ftOwnerFv(self->component)
         + (self->value + (first + self->second(arg)));
}
}

// TODO: clean up, give variables real names
namespace jj_14D050 {
extern "C" u32 fn_27_14D050(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x4c);
}
}

// TODO: clean up, give variables real names
namespace jj_14D070 {
extern "C" int getGlowDefenseAdd__7ftOwnerFv(void*);

struct Object {
    virtual int method0();
    virtual int method1();
    virtual int method2();
    virtual int method3();
    virtual int method4();
    virtual int method5();
    virtual int method6();
    virtual int method7();

    unsigned char padding[0x114];
    int values[24];
    void* helper;
};

struct Argument {
    unsigned char padding[0x30];
    unsigned int index;
};

extern "C" int fn_27_14D070(Object* self, Argument* argument) {
    int base = self->method7();
    int offset = getGlowDefenseAdd__7ftOwnerFv(self->helper);
    return self->values[argument->index & 31] + (offset + base);
}
}

// TODO: clean up, give variables real names
namespace jj_14D0E0 {
struct Entry {
    int key;
    int value;
};

class Work {
public:
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual unsigned int getType();

    unsigned char padding[0x20];
    int total;
    Entry entries[4];
    int count;
};

extern "C" void fn_27_14D0E0(Work* work, int key) {
    if (work->getType() == 1) {
        work->total = 0;
        for (int i = 0; i < work->count; ++i) {
            if (work->entries[i].key == key) {
                work->total += work->entries[i].value;
            }
        }
    }
}
}

