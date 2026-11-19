#include <mt/mt_matrix.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <types.h>

void soPostureModuleImpl::initRot() {
    int count = m_interpolationArray->size();
    for (int i = 0; i < count; i++) {
        initRot(i);
    }
}

void soPostureModuleImpl::initRotYLr() {
    m_rotYLr = 0.0f;
    updateRotYLr();
}

void soPostureModuleImpl::updateRotYLr() {
    m_rotYLr = 90.0f * m_lr;
}

void soPostureModuleSimple::setLr(float lr) {
    m_lr = lr;
}

bool soPostureModuleImpl::setStickLR(float stickLR) {
    float stickX = m_moduleAccesser->getControllerModule().getStickX();
    float lr = getLr();
    if (stickX > stickLR) {
        setLr(1.0f);
    } else if (stickX < -stickLR) {
        setLr(-1.0f);
    }
    return lr != getLr();
}

float soPostureModuleSimple::getLr() {
    return m_lr;
}

void soPostureModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    updateRotYLr();
    m_syncConstraintNode = -1;
}

bool soPostureModuleImpl::isObserv(char unk1) {
    return unk1 == 5;
}

void soPostureModuleSimple::reverseLr() {
    setLr(-m_lr);
}

void soPostureModuleSimple::setScale(float scale) {
    m_baseScale = scale;
}

void soPostureModuleImpl::reverseRotYLr() {
    m_rotYLr = -m_rotYLr;
}

void soPostureModuleImpl::setSyncConstraintNode(int node) {
    m_syncConstraintNode = node;
}

void soPostureModuleSimple::initScale() {
    m_baseScale = 1.0f;
}

float soPostureModuleSimple::getBaseScale() {
    return m_baseScale;
}

void soPostureModuleImpl::setOwnerScale(float ownerScale) {
    m_ownerScale = ownerScale;
}

float soPostureModuleImpl::getOwnerScale() {
    return m_ownerScale;
}

float soPostureModuleImpl::getRotYLr() {
    return m_rotYLr;
}

void soPostureModuleImpl::init(Vec3f* pos, float lr) {
    soPostureModuleSimple::init(pos, lr);
    initRot();
    initRotYLr();
    // MATCH-ONLY: the original loads all three components before storing any.
    float z, y, x, scale;
    x = pos->m_x;
    scale = 1.0f;
    y = pos->m_y;
    z = pos->m_z;
    m_ownerScale = scale;
    m_prevRoughPos.m_x = x;
    m_prevRoughPos.m_y = y;
    m_prevRoughPos.m_z = z;
}

void soPostureModuleImpl::initPos(Vec3f* pos) {
    soPostureModuleSimple::initPos(pos);
    m_moduleAccesser->getGroundModule().relocate(reinterpret_cast<Vec2f*>(pos), 0);
}

void soPostureModuleImpl::setPrevRoughPos(Vec3f* roughPos) {
    // MATCH-ONLY: the original loads all three components before storing any.
    float z, y, x;
    x = roughPos->m_x;
    y = roughPos->m_y;
    z = roughPos->m_z;
    m_prevRoughPos.m_x = x;
    m_prevRoughPos.m_y = y;
    m_prevRoughPos.m_z = z;
}

void soPostureModuleImpl::begin() {
    soPostureModuleSimple::begin();
    int count = m_interpolationArray->size();
    for (int i = 0; i < count; i++) {
        soInterpolation<Vec3f>& element = m_interpolationArray->at(i);
        element.m_elements[1] = element.m_elements[0];
    }
}

void soPostureModuleImpl::turnBack() {
    soPostureModuleSimple::turnBack();
    int count = m_interpolationArray->size();
    for (int i = 0; i < count; i++) {
        soInterpolation<Vec3f>& element = m_interpolationArray->at(i);
        element.m_elements[0] = element.m_elements[2];
    }
}


void soPostureModuleImpl::fixRot() {
    int count = m_interpolationArray->size();
    for (int i = 0; i < count; i++) {
        soInterpolation<Vec3f>& element = m_interpolationArray->at(i);
        element.m_elements[1] = element.m_elements[0];
    }
}

void soPostureModuleImpl::setLr(float lr) {
    m_lr = lr;
    soLinkEventArgs args(5);
    m_moduleAccesser->getLinkModule().sendEventNodes(-1, args, 0x200);
}

void soPostureModuleImpl::fixConstraintNodePosture(int node, void* mtx) {
    if (static_cast<u32>(node) == static_cast<u32>(m_syncConstraintNode)) {
        Matrix* matrix = static_cast<Matrix*>(mtx);
        float z = matrix->m[2][3];
        float y = matrix->m[1][3];
        float x = matrix->m[0][3];
        Vec3f pos;
        pos.m_y = y;
        pos.m_x = x;
        pos.m_z = z;
        setPos(&pos);
    }
}

void soPostureModuleSimple::setPos(Vec3f* pos) {
    float z, y, x;
    x = pos->m_x;
    y = pos->m_y;
    z = pos->m_z;
    m_pos.m_x = x;
    m_pos.m_y = y;
    m_pos.m_z = z;
}

void soPostureModuleImpl::setRot(Vec3f* rot, u32 index) {
    Vec3f& element = m_interpolationArray->at(index).m_elements[0];
    float x = rot->m_x;
    float y = rot->m_y;
    element.m_x = x;
    float z = rot->m_z;
    element.m_y = y;
    element.m_z = z;
}

Vec3f soPostureModuleImpl::getRot(u32 index) {
    return m_interpolationArray->at(index).m_elements[0];
}

void soPostureModuleImpl::initRot(u32 index) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    soInterpolation<Vec3f>& first = m_interpolationArray->at(index);
    first.m_elements[0] = zero;
    first.m_elements[2] = first.m_elements[1] = zero;
    Vec3f last(0.0f, 0.0f, 0.0f);
    soInterpolation<Vec3f>& second = m_interpolationArray->at(index);
    second.m_elements[3] = last;
}

// TODO: clean up, give variables real names
namespace jj_2370C {
struct Fields
{
    unsigned int field0;
    unsigned int field1;
    unsigned int field2;
};

extern "C" void fn_27_2370C(Fields* dst, const Fields* src)
{
    const Fields* fields = reinterpret_cast<const Fields*>(
        reinterpret_cast<const unsigned char*>(src) + 0x6C);

    dst->field0 = fields->field0;
    dst->field1 = fields->field1;
    dst->field2 = fields->field2;
}
}

// TODO: clean up, give variables real names
namespace jj_23744 {
class VBase {
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
};
struct Inner { char pad[0x54]; VBase* m_54; };
struct Mid { char pad[0xd8]; Inner* m_d8; };
struct Outer { char pad[8]; Mid* m_08; char pad2[0x3c-0xc]; float m_3c; char pad3[0x68-0x40]; float m_68; };

extern const float lbl_27_rodata_38C;

extern "C" float fn_27_23744(const Outer* p) {
    float v = p->m_08->m_d8->m_54->getValue();
    if (lbl_27_rodata_38C != v) {
        return p->m_3c * v;
    }
    return p->m_3c * p->m_68;
}
}

// TODO: clean up, give variables real names
namespace jj_25088 {
extern "C" void fn_27_25088(void *dst, const void *src)
{
    const unsigned int *s = (const unsigned int *)src;
    unsigned int *d = (unsigned int *)dst;
    d[0] = s[3];
    d[1] = s[4];
    d[2] = s[5];
}
}

// TODO: clean up, give variables real names
namespace jj_25184 {
struct Data3
{
    unsigned char padding[0x18];
    unsigned int unk18;
    unsigned int unk1C;
    unsigned int unk20;
};

struct Data3Out
{
    unsigned int unk0;
    unsigned int unk4;
    unsigned int unk8;
};

extern "C" void fn_27_25184(Data3Out *out, const Data3 *in)
{
    out->unk0 = in->unk18;
    out->unk4 = in->unk1C;
    out->unk8 = in->unk20;
}
}

// TODO: clean up, give variables real names
namespace jj_251B8 {
extern "C" int fn_27_251B8(const float *p)
{
    extern const float lbl_27_rodata_388[];

    float value = p[0x40 / sizeof(float)];
    float reference = lbl_27_rodata_388[1];
    float threshold;

    if (reference == value)
        threshold = lbl_27_rodata_388[2];
    else
        threshold = lbl_27_rodata_388[8];

    return p[0x60 / sizeof(float)] != threshold;
}
}

// TODO: clean up, give variables real names
namespace jj_251F4 {
extern "C" void notifyEventChangeStatus__19soPostureModuleImplFiiP12soStatusDataP16soModuleAccesser(void*);

extern "C" void fn_27_251F4(void* self) {
    notifyEventChangeStatus__19soPostureModuleImplFiiP12soStatusDataP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x44);
}
}

// TODO: clean up, give variables real names
namespace jj_251FC {
extern "C" void fn_27_23EE0(void*);

extern "C" void fn_27_251FC(void* self) {
    fn_27_23EE0(reinterpret_cast<u8*>(self) - 0x50);
}
}

// TODO: clean up, give variables real names
namespace jj_25204 {
extern "C" void isObserv__19soPostureModuleImplFc(void*);

extern "C" void fn_27_25204(void* self) {
    isObserv__19soPostureModuleImplFc(reinterpret_cast<u8*>(self) - 0x50);
}
}

