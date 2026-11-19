// Havok translation unit hkCollisionDispatcher.o (main.dol 0x802CB630-0x802CD000).
// Not yet decompiled. Functions in address order:
//   0x802CB630    16  create
//   0x802CB640     4  destroy
//   0x802CB644     8  process
//   0x802CB64C   708  resetCreationFunctions
//   0x802CB910  1424  __ct
//   0x802CBF2C    12  setEnableChecks
//   0x802CBF38   164  disableDebugging
//   0x802CBFDC   272  __dt
//   0x802CC0EC   296  registerCollisionAgent
//   0x802CC214   728  registerAgent3
//   0x802CC4EC   576  internalRegisterCollisionAgent
//   0x802CC72C   148  updateHasAlternateType
//   0x802CC7C0   284  registerAlternateShapeType
//   0x802CC8DC   288  registerContactMgrFactoryWithAll
//   0x802CC9FC   288  calcStatistics
//   0x802CCB1C  1252  initCollisionQualityInfo

#include <havok/hkCollisionDispatcher.h>

// Null agent: a registration target that never produces contacts (the class is only used in this unit).
struct hkNullAgent3 {
    static void* create(void* unk0, u8* flag, void* agent);
    static void destroy(void* unk0);
    static void* process(void* unk0, void* unk1, void* agent);
};

// HYPOTHESIS: the first parameter is unused. Clears the flag byte and returns the agent.
void* hkNullAgent3::create(void* unk0, u8* flag, void* agent) {
    *flag = 0;
    return agent;
}

void hkNullAgent3::destroy(void* unk0) {}

void* hkNullAgent3::process(void* unk0, void* unk1, void* agent) {
    return agent;
}

void hkCollisionDispatcher::setEnableChecks(const hkBool& enable) {
    m_enableChecks = enable;
}

#pragma push
#pragma auto_inline off
void hkCollisionDispatcher::disableDebugging() {
    if (m_buffers[0] != 0) {
        hkMemory::getInstance().deallocate(m_buffers[0]);
        hkMemory::getInstance().deallocate(m_buffers[1]);
        hkMemory::getInstance().deallocate(m_buffers[2]);
        hkMemory::getInstance().deallocate(m_buffers[3]);
        m_buffers[0] = 0;
        m_buffers[1] = 0;
        m_buffers[2] = 0;
        m_buffers[3] = 0;
    }
}
#pragma pop

// Releases the debug buffers and the contact manager factories; the shape inheritance array frees itself.
hkCollisionDispatcher::~hkCollisionDispatcher() {
    disableDebugging();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (m_contactMgrFactory[i][j] != 0) {
                m_contactMgrFactory[i][j]->removeReference();
            }
        }
    }
}

void hkCollisionDispatcher::registerCollisionAgent(const hkAgentFuncs* funcs, int typeA, int typeB) {
    m_agentFuncs[m_numAgents] = *funcs;
    internalRegisterCollisionAgent(m_lookupE94, 1, typeA, typeB, typeA, typeB, m_buffers[2], 0);
    internalRegisterCollisionAgent(m_lookup190, m_numAgents, typeA, typeB, typeA, typeB, m_buffers[0], 0);
    if (funcs->symmetricB != 0) {
        internalRegisterCollisionAgent(m_lookup1294, 1, typeA, typeB, typeA, typeB, m_buffers[3], 0);
        internalRegisterCollisionAgent(m_lookup590, m_numAgents, typeA, typeB, typeA, typeB, m_buffers[1], 0);
    }
    m_numAgents++;
}

#pragma push
#pragma auto_inline off
void hkCollisionDispatcher::updateHasAlternateType(hkShapeType primaryType, hkShapeType alternateType, int depth) {
    m_hasAlternateType[primaryType] |= m_hasAlternateType[alternateType];
    for (int i = 0; i < m_shapeInheritance.getSize(); i++) {
        ShapeInheritance& si = m_shapeInheritance[i];
        if (si.m_alternateType == primaryType) {
            updateHasAlternateType(si.m_primaryType, si.m_alternateType, depth + 1);
        }
    }
}
#pragma pop

void hkCollisionDispatcher::registerAlternateShapeType(hkShapeType primaryType, hkShapeType alternateType) {
    for (int i = 0; i < m_shapeInheritance.getSize(); i++) {
        const ShapeInheritance& si = m_shapeInheritance[i];
        if (si.m_primaryType == primaryType && si.m_alternateType == alternateType) {
            m_shapeInheritance.removeAtAndCopy(i);
            i--;
        }
    }
    updateHasAlternateType(primaryType, alternateType, 0);
    ShapeInheritance& si = m_shapeInheritance.expandOne();
    si.m_primaryType = primaryType;
    si.m_alternateType = alternateType;
}

// fn_802CC8DC: body from https://github.com/jj-link/brawl src/havok/fn_802CC8DC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CC8DC {
class Class_80487298 {
public:
    virtual void fn_802CAD00(int);
    unsigned short m04;
    short m06;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CC8DC)
extern "C" void fn_802CC8DC(char* self, Class_80487298* value, int index) {
    char* column = self + index * 4;
    char* row = self + (index << 5);
    int i = 0;
    do {
        if (value->m04 != 0) {
            value->m06++;
        }
        Class_80487298* old = *(Class_80487298**)(column + 0xC);
        if (old->m04 != 0) {
            if (--old->m06 == 0 && old != 0) {
                old->fn_802CAD00(1);
            }
        }
        *(Class_80487298**)(column + 0xC) = value;
        if (value->m04 != 0) {
            value->m06++;
        }
        old = *(Class_80487298**)(row + 0xC);
        if (old->m04 != 0) {
            if (--old->m06 == 0 && old != 0) {
                old->fn_802CAD00(1);
            }
        }
        i++;
        *(Class_80487298**)(row + 0xC) = value;
        column += 0x20;
        row += 4;
    } while (i < 8);
}
} // namespace jj_fn_802CC8DC

// fn_802CC9FC: body from https://github.com/jj-link/brawl src/havok/fn_802CC9FC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CC9FC {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1(int a, int b, void* c);
    virtual void slot2(char* name, int size, void* data, unsigned int count, unsigned int flags);
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
};

extern "C" char lbl_80410050[];

// TODO: cleanup (shape from jj-link/brawl fn_802CC9FC)
extern "C" void fn_802CC9FC(void* arg0, Object_vptr_0* obj) {
    obj->slot1(0, 2, arg0);
    void* data = *(void**)((char*)arg0 + 0x1C24);
    if (data != 0) {
        obj->slot2(lbl_80410050 + 0x147, 4, data, 0xC00, 0);
        obj->slot2(lbl_80410050 + 0x147, 4, *(void**)((char*)arg0 + 0x1C28), 0xC00, 0);
        obj->slot2(lbl_80410050 + 0x147, 4, *(void**)((char*)arg0 + 0x1C2C), 0xC00, 0);
        obj->slot2(lbl_80410050 + 0x147, 4, *(void**)((char*)arg0 + 0x1C30), 0xC00, 0);
    }
    obj->slot6();
}
} // namespace jj_fn_802CC9FC

// TODO: clean up, give variables real names
namespace jj_802CB64C {
extern "C" {
unsigned char *fn_802C05BC();
void fn_802C05B4();
void fn_802C05B0();
void fn_802C05B8();
int fn_802CB630(int, unsigned char *, int);
void fn_802CB640();
int fn_802CB644(int, int, int);
void fn_802CAA50(void *);
}
struct Cell { unsigned char a, b, c; };
struct Obj {
    char pad0[0x18c];
    int flag1;
    unsigned char a[32][32];
    unsigned char b[32][32];
    void *f1;
    void *f2;
    void *f3;
    void *f4;
    unsigned char f5, f6;
    char pad1[0xe90 - 0x9a2];
    int flag2;
    unsigned char c[32][32];
    unsigned char d[32][32];
    void *g1;
    void *g2;
    int g3, g4, g5, g6, g7, g8, g9, g10;
    void *g11;
    int g12;
    int g13;
    char pad2[0x1c08 - 0x16c8];
    unsigned char h;
    char pad3[0x1c24 - 0x1c09];
    Cell (*p1)[32];
    Cell (*p2)[32];
    Cell (*p3)[32];
    Cell (*p4)[32];
};
extern "C" void fn_802CB64C(Obj *o) {
    o->flag1 = 1;
    o->flag2 = 1;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (o->p1) {
                o->p1[i][j].c = 100;
                o->p2[i][j].c = 100;
                o->p3[i][j].c = 100;
                o->p4[i][j].c = 100;
            }
            o->a[i][j] = 0;
            o->c[i][j] = 0;
            o->b[i][j] = 0;
            o->d[i][j] = 0;
        }
    }
    o->f1 = (void *)fn_802C05BC;
    o->f2 = (void *)fn_802C05B4;
    o->f3 = (void *)fn_802C05B0;
    o->f4 = (void *)fn_802C05B8;
    o->f5 = 0;
    o->f6 = 1;
    o->g1 = (void *)fn_802CB630;
    o->g2 = (void *)fn_802CB640;
    o->g3 = 0;
    o->g4 = 0;
    o->g5 = 0;
    o->g6 = 0;
    o->g7 = 0;
    o->g10 = 0;
    o->g8 = 0;
    o->g9 = 0;
    o->g11 = (void *)fn_802CB644;
    o->g13 = 0;
    fn_802CAA50(o);
    o->h = 0;
    if (o->p3) {
        for (int i = 0; i < 32; i++)
            for (int j = 0; j < 32; j++) {
                o->p3[i][j].c = 100;
                o->p4[i][j].c = 100;
            }
    }
}
}


#include <havok/hkString.h>
#include <havok/hkShapeType.h>

// The warning text printed when a registration overrides a more specialized one (in the unit's .rodata).
// TODO: clean up: write it as a string literal once this unit's .rodata is split.
extern "C" const char lbl_80410050[];

// Writes value into the table entries of the pair (typeA, typeB); -1 stands for every shape type and counts as one
// level less specialized. Types that can be treated as typeA or typeB (m_shapeInheritance) are registered first, one
// level deeper. With a buffer, every entry remembers the pair and depth that wrote it; with checks enabled an entry
// written from a deeper (more specialized) registration is reported before it is overwritten.
void hkCollisionDispatcher::internalRegisterCollisionAgent(u8* table, int value, int typeA, int typeB, int typeA2,
                                                           int typeB2, void* buffer, int depth) {
    m_flagA = 1;
    for (int i = 0; i < m_shapeInheritance.m_size; i++) {
        ShapeInheritance& inheritance = m_shapeInheritance[i];
        int alternate = inheritance.m_alternateType;
        if (alternate == typeA || alternate == typeB) {
            if (alternate == typeA) {
                internalRegisterCollisionAgent(table, value, inheritance.m_primaryType, typeB, typeA2, typeB2, buffer,
                                               depth + 1);
            } else {
                internalRegisterCollisionAgent(table, value, typeA, inheritance.m_primaryType, typeA2, typeB2, buffer,
                                               depth + 1);
            }
        }
    }
    int startA = typeA;
    int endA = typeA + 1;
    int startB = typeB;
    int endB = typeB + 1;
    if (typeA == -1) {
        endA = 32;
        depth++;
        startA = 1;
    }
    if (typeB == -1) {
        startB = 1;
        endB = 32;
        depth++;
    }
    for (int a = startA; a < endA; a++) {
        u8* tableEntry = &table[a * 32 + startB];
        s8* entry = (s8*)buffer + (a * 32 + startB) * 3;
        for (int b = startB; b < endB; b++, tableEntry++, entry += 3) {
            *tableEntry = value;
            if (buffer != 0) {
                if (m_enableChecks && depth > entry[2]) {
                    const char* oldA = hkGetShapeTypeName(entry[0]);
                    const char* oldB = hkGetShapeTypeName(entry[1]);
                    const char* newA = hkGetShapeTypeName(typeA);
                    char text[1024];
                    hkString::snprintf(text, 1000, lbl_80410050, newA, hkGetShapeTypeName(typeB), oldA, oldB);
                }
                entry[2] = depth;
                entry[0] = typeA2;
                entry[1] = typeB2;
            }
        }
    }
}

// Agent3 registrations use the 0x200-byte agent sectors with 0x80-byte entries. A pair of different types whose
// agent does not ignore the symmetric version first gets a flipped twin registration for (typeB, typeA).
void hkCollisionDispatcher::registerAgent3(const hkAgent3Funcs& funcs, int typeA, int typeB) {
    Agent3FuncsIntern f;
    m_agentEntrySize = 0x80;
    m_agentSectorSize = 0x200;
    m_flagB = 1;
    (hkAgent3Funcs&)f = funcs;
    f.unk30 = 0;
    hkBool registerFlipped = false;
    if (typeA != typeB) {
        if (!funcs.m_ignoreSymmetricVersion) {
            registerFlipped = true;
        }
    }
    if (registerFlipped) {
        f.unk30 = 2;
        m_agent3Func[m_numAgent3Funcs] = f;
        internalRegisterCollisionAgent(m_lookupE94, m_numAgent3Funcs, typeB, typeA, typeB, typeA, m_buffers[2], 0);
        if (f.m_isPredictive) {
            internalRegisterCollisionAgent(m_lookup1294, m_numAgent3Funcs, typeA, typeB, typeA, typeB, m_buffers[3],
                                           0);
        }
        m_numAgent3Funcs++;
        f.unk30 = 1;
    }
    m_agent3Func[m_numAgent3Funcs] = f;
    internalRegisterCollisionAgent(m_lookupE94, m_numAgent3Funcs, typeA, typeB, typeA, typeB, m_buffers[2], 0);
    if (f.m_isPredictive) {
        internalRegisterCollisionAgent(m_lookup1294, m_numAgent3Funcs, typeA, typeB, typeA, typeB, m_buffers[3], 0);
    }
    m_numAgent3Funcs++;
}
