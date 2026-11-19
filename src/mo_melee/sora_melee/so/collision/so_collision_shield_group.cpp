#include <cl/cl_collision.h>
#include <so/collision/so_collision_shield_group.h>
#include <so/collision/so_collision_shield_part.h>
#include <types.h>

soCollisionShieldGroup::~soCollisionShieldGroup() { }

void soCollisionShieldGroup::init() {
    m_partStart = -1;
    m_partSize = 0;
    unk0 = 0;
    m_hopAngle = 0.0f;
    m_attackMul = 1.0f;
    m_speedMul = 1.0f;
    m_lifeMul = 0.0f;
    m_isFront = false;
    m_isHop = false;
    m_isTurn = false;
    m_isNoMBall = false;
    m_isNoHop = false;
    unkC = 0.0f;
    unk8 = 0.0f;
}

void soCollisionShieldGroup::set(soArray<soCollisionShieldPart>* parts, u32 index, soCollisionShieldData* data) {
    parts->at(m_partStart + index).setData(data);
}

void soCollisionShieldGroup::setReflectorData(const soCollisionReflectorData* data) {
    m_attackMul = data->m_attackMul;
    m_speedMul = data->m_speedMul;
    m_attackLimit = data->m_attackLimit;
    m_lifeMul = data->m_lifeMul;
    m_isNoMBall = data->m_isNoMBall;
}

bool soCollisionShieldGroup::isShield(soArray<soCollisionShieldPart>* parts, u32 index) const {
    return parts->at(m_partStart + index).m_isActive != 0;
}

void soCollisionShieldGroup::setStatus(soArray<soCollisionShieldPart>* parts, u32 index, u32 status) {
    parts->at(m_partStart + index).m_isActive = status;
}

void soCollisionShieldGroup::clearAll(soArray<soCollisionShieldPart>* parts) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).m_isActive = 0;
    }
}

void soCollisionShieldGroup::setTargetProperty(soArray<soCollisionShieldPart>* parts, u32 property) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setTargetProperty(property);
    }
}

soCollisionShieldGroup::soCollisionShieldGroup() {
    unk0 = -1;
    m_partStart = 0;
    m_partSize = 0;
    m_scaleX = 1.0f;
    m_scaleY = 1.0f;
    m_scaleZ = 1.0f;
    m_flags = 0;
    m_hopAngle = 0.0f;
    m_attackMul = 1.0f;
    m_speedMul = 1.0f;
    m_lifeMul = 0.0f;
    m_isFront = false;
    m_isHop = false;
    m_isTurn = false;
    m_isNoMBall = false;
    m_isNoHop = false;
    unkC = 0.0f;
    unk8 = 0.0f;
}

Vec3f soCollisionShieldGroup::getCenterPos(soCollision* collision, u32 index) {
    return collision->getCollision(index, unk0)->getCenterPos();
}

// TODO: clean up, give variables real names
namespace jj_46AE0 {
extern "C" void __dl__FPv(void* p);

extern "C" void* fn_27_46AE0(void* p, s32 n) {
    if (p != 0 && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_46BB4 {
struct Self {
    s32 id;
    s16 off;
    s16 cnt;
    char pad[0x88];
    float x, y, z;
};

class Class_3FE0 {
public:
    virtual void __dt__37soCollisionShieldEventPresenterShieldFv();
    virtual void* fn_27_4746C(s32);
};

extern "C" void* getCollision__11soCollisionFUlUl(void*, s32, s32);
extern "C" void reset__21soCollisionShieldPartFP25soCollisionShieldPartUnit(void*, void*);

extern "C" void fn_27_46BB4(Self* self, void* arg1, Class_3FE0* arg2, float x, float y, float z) {
    self->x = x;
    self->y = y;
    self->z = z;
    if (self->id != -1) {
        void* p;
        s32 i = 0;
        while (i < self->cnt) {
            p = arg2->fn_27_4746C(self->off + i);
            void* q = getCollision__11soCollisionFUlUl(arg1, i, self->id);
            reset__21soCollisionShieldPartFP25soCollisionShieldPartUnit(p, q);
            i++;
        }
    }
}
}

