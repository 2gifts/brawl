#include <revolution/gx.h>
#include <so/collision/so_collision_shield_part.h>
#include <types.h>

soCollisionShieldPart::soCollisionShieldPart(u32 category1, u32 category2) : m_targets(6, 0) {
    {
        clTarget& target0 = m_targets.at(0);
        target0.m_0 = 1 << category2;
    }
    {
        clTarget& target1 = m_targets.at(1);
        target1.m_0 = 1 << category1;
    }
    if (m_targets.at(0).m_0 & 8) {
        clTarget& target5 = m_targets.at(5);
        target5.m_4 = 2;
    } else if (m_targets.at(0).m_0 & 0x10) {
        clTarget& target5 = m_targets.at(5);
        target5.m_4 = 4;
    }
}

soCollisionShieldPart::~soCollisionShieldPart() { }

void soCollisionShieldPart::reset(soCollisionShieldPartUnit* unit) {
    unit->m_targetArray = &m_targets;
    m_isActive = 0;
}

void soCollisionShieldPart::setData(soCollisionShieldData* data) {
    m_data = *data;
}

bool soCollisionShieldPart::update(soModuleAccesser* moduleAccesser, soCollisionShieldPartUnit* unit, float scale) {
    if (m_isActive) {
        Vec3f start;
        start.m_x = m_data.offset(0);
        start.m_y = m_data.offset(1);
        start.m_z = m_data.offset(2);
        Vec3f end;
        end.m_x = m_data.offset(3);
        end.m_y = m_data.offset(4);
        end.m_z = m_data.offset(5);
        soCollisionUtil::updateCollision(moduleAccesser, unit, m_data.m_shapeType, 0, m_data.m_nodeIndex, &start, &end, 0, false,
                                         m_data.m_size, scale);
        unit->m_isUpdated = true;
        return true;
    }
    unit->m_isUpdated = false;
    return false;
}

void soCollisionShieldPart::setTargetProperty(u32 property) {
    clTarget& target = m_targets.at(5);
    target.m_4 = 1 << property;
}

void soCollisionShieldPart::debugDisplay(int arg, soCollisionShieldPartUnit* unit) {
    if (m_isActive) {
        GXColor color0 = {0x00, 0xFF, 0xFF, 0x80};
        GXColor color1 = {0x00, 0x80, 0x80, 0x80};
        GXColor color2 = {0x00, 0xFF, 0x80, 0x80};
        GXColor color3 = {0x00, 0x80, 0x40, 0x80};
        GXColor color4 = {0x00, 0x80, 0xFF, 0x80};
        GXColor color5 = {0x00, 0x40, 0x80, 0x80};
        const GXColor* a;
        const GXColor* b;
        if (m_targets.at(0).m_0 & 4) {
            a = &color0;
            b = &color1;
        } else if (m_targets.at(0).m_0 & 8) {
            a = &color2;
            b = &color3;
        } else {
            a = &color4;
            b = &color5;
        }
        unit->m_debugVtable[9](unit, arg, a, b);
    }
}

#include <so/collision/so_collision.h>
// TODO: clean up, give variables real names
namespace jj_46588 {
extern "C" {
extern char lbl_27_data_3AD8[];
void __ct__8clTargetFv(void*);
void __dt__8clTargetFv(void*, int);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(void*, long, long);
}

class Holder {
public:
    virtual void slot0();
    virtual clTarget* slot1(int);
};

union Flags {
    u32 raw;
    struct { u32 pad : 8; u32 value : 4; u32 flag : 1; u32 rest : 19; } bits;
};

struct Obj {
    char pad[0x20];
    void* vt;
    void* calc;
    Flags flags;
    char targets[0x30];
};

extern "C" void* __ct__21soCollisionShieldPartFQ211soCollision8Categoryi(Obj* self, int a1, int a2) {
    self->calc = lbl_27_data_3AD8 + 0x48;
    self->vt = lbl_27_data_3AD8;
    self->flags.raw &= 0x00ffffff;
    self->flags.bits.value = 6;
    self->flags.bits.flag = 0;
    __construct_array(self->targets, (void (*)(void*))__ct__8clTargetFv,
                      (void (*)(void*))__dt__8clTargetFv, 8, 6);
    postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(&self->calc, 6, 6);
    clTarget* p = ((Holder*)&self->vt)->slot1(0);
    p->m_0 = 1 << a2;
    p = ((Holder*)&self->vt)->slot1(1);
    p->m_0 = 1 << a1;
    p = ((Holder*)&self->vt)->slot1(0);
    if (p->m_0 & 8) {
        p = ((Holder*)&self->vt)->slot1(5);
        p->m_4 = 2;
    } else {
        p = ((Holder*)&self->vt)->slot1(0);
        if (p->m_0 & 0x10) {
            p = ((Holder*)&self->vt)->slot1(5);
            p->m_4 = 4;
        }
    }
    return self;
}
}

