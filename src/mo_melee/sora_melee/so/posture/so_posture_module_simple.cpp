#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <types.h>

soPostureModuleSimple::soPostureModuleSimple(soModuleAccesser* moduleAccesser, soArray<soInterpolation<Vec3f> >* interpolations, soEventObserverRegistrationDesc* registrationDesc)
    : m_moduleAccesser(moduleAccesser), m_baseScale(1.0f), m_lr(0.0f) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    m_pos = zero;
    m_prevPos = zero;
    _36 = zero;
    _48 = Vec3f(0.0f, 0.0f, 0.0f);
}

soPostureModuleSimple::~soPostureModuleSimple() { }

void soPostureModuleSimple::init(Vec3f* pos, float lr) {
    initPos(pos);
    setLr(lr);
    initScale();
}

void soPostureModuleSimple::begin() {
    m_prevPos = m_pos;
}

Vec3f soPostureModuleSimple::getPrevRoughPos() {
    return getPos();
}

void soPostureModuleSimple::turnBack() {
    m_pos = _36;
}

void soPostureModuleSimple::fixRot() { }

void soPostureModuleSimple::fixConstraintNodePosture(int, void*) { }

void soPostureModuleSimple::setSyncConstraintNode(int) { }

bool soPostureModuleSimple::setStickLR(float stickLR) {
    return false;
}

void soPostureModuleSimple::setOwnerScale(float ownerScale) { }

float soPostureModuleSimple::getOwnerScale() {
    return 1.0f;
}

void soPostureModuleSimple::reverseRotYLr() { }

void soPostureModuleSimple::updateRotYLr() { }

float soPostureModuleSimple::getRotYLr() {
    return 90.0f * m_lr;
}

bool soPostureModuleSimple::isRotYLrDifferentInnerLr() {
    return false;
}

void soPostureModuleSimple::initRotYLr() { }

void soPostureModuleSimple::setRot(Vec3f* rot, u32 index) { }

Vec3f soPostureModuleSimple::getRot(u32 index) {
    return Vec3f(0.0f, 0.0f, 0.0f);
}

void soPostureModuleSimple::initRot() { }

void soPostureModuleSimple::initRot(u32 index) { }

void soPostureModuleSimple::setPrevRoughPos(Vec3f* roughPos) { }

// TODO: clean up, give variables real names
namespace jj_25380 {
extern "C" void initPos__21soPostureModuleSimpleFP5Vec3f(float* dst, const float* src) {
    float c;
    float b;
    float a;
    a = src[0];
    b = src[1];
    c = src[2];
    dst[3] = a;
    dst[4] = b;
    dst[5] = c;
    dst[6] = a;
    dst[7] = b;
    dst[8] = c;
    dst[9] = a;
    dst[10] = b;
    dst[11] = c;
}
}

// TODO: clean up, give variables real names
namespace jj_25470 {
class fn_27_25470Base {
public:
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual float getValue();
    virtual void v32(); virtual void v33();
};

struct fn_27_25470Inner {
    u8 pad[0x54];
    fn_27_25470Base* m_54;
};

struct fn_27_25470Mid {
    u8 pad[0xd8];
    fn_27_25470Inner* m_d8;
};

struct fn_27_25470Outer {
    u8 pad[0x8];
    fn_27_25470Mid* m_08;
    u8 pad2[0x3c - 0x0c];
    float m_3c;
};

extern "C" {
float fn_27_25470(const fn_27_25470Outer* self) {
    return self->m_3c * self->m_08->m_d8->m_54->getValue();
}
}
}

