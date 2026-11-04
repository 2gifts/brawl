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
