#define SO_CAPTURE_MODULE_IMPL_FULL
#include <so/capture/so_capture_module_impl.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

extern const float lbl_27_rodata_910[4];

// Matrix::mulVec (the map name); em_kinetic_energy.cpp calls it by this stand-in name.
extern "C" void fn_8003E3BC(const Matrix* mtx, const Vec3f* in, Vec3f* out);

soCaptureModule::~soCaptureModule() { }

soCaptureModuleImpl::~soCaptureModuleImpl() { }

void soCaptureModuleImpl::activate() {
    initInfo();
    m_sendCutEvent = true;
    unk2D = true;
    m_isThrown = false;
    unk31 = false;
    m_isReaction = false;
    unk28 = 0.0f;
}

void soCaptureModuleImpl::deactivate() { }

void soCaptureModuleImpl::setNodes(int node1, int node2, float unk) {
    m_node1 = node1;
    m_node2 = node2;
    unk20 = unk;
}

void soCaptureModuleImpl::initInfo() {
    m_isReaction = false;
}

bool soCaptureModuleImpl::isCapture() {
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == false) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return true;
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return true;
        }
    }
    return false;
}

int soCaptureModuleImpl::getCatchTaskId() {
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == false) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return m_moduleAccesser->getLinkModule().getNodeTaskId(0);
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return m_moduleAccesser->getLinkModule().getParentTaskId(0);
        }
    }
    return -1;
}

float soCaptureModuleImpl::getCatchPosY() {
    return m_moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(0, m_catchNode, true).m_y;
}

bool soCaptureModuleImpl::isIgnoreCatching() {
    return m_ignoreCatching;
}

void soCaptureModuleImpl::setIgnoreCatching(bool ignoreCatching) {
    m_ignoreCatching = ignoreCatching;
}

int soCaptureModuleImpl::getMotionOffset() {
    return m_motionOffset;
}

bool soCaptureModuleImpl::isReaction() {
    return m_isReaction;
}

void soCaptureModuleImpl::setUpdatePos(bool updatePos) {
    m_updatePos = updatePos;
}

void soCaptureModuleImpl::setCatchNode(int node) {
    m_catchNode = node;
}

Vec2f soCaptureModuleImpl::getNodeOffset(bool global) {
    Vec3f parent = m_moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(0, m_catchNode, global);
    Vec3f node = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_node1, global);
    Vec2f offset;
    offset.m_x = parent.m_x - node.m_x;
    offset.m_y = parent.m_y - node.m_y;
    return offset;
}

void soCaptureModuleImpl::captureCut(bool cut, bool constrained) {
    if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
        if (m_sendCutEvent == true) {
            soLinkEventArgs event(9);
            m_moduleAccesser->getLinkModule().sendEventParents(0, event);
        }
        if (cut == true && unk2D == true) {
            if (constrained == true) {
                Vec3f rot(0.0f, 0.0f, 0.0f);
                Vec3f pos(0.0f, 0.0f, 0.0f);
                m_moduleAccesser->getLinkModule().adjustModelConstraintPosture(&rot, &pos);
            }
            m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        }
        m_moduleAccesser->getLinkModule().unlink(0);
    }
    m_moduleAccesser->getLinkModule().unlinkNode(2);
}

bool soCaptureModuleImpl::checkDamage(int status) {
    unk28 += m_moduleAccesser->getDamageModule().getCaptureDamage();
    if (unk28 >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0x7f2, 0)) {
        return false;
    }
    bool cut = false;
    if (m_moduleAccesser->getDamageModule().isCaptureCut() == false) {
        if (m_moduleAccesser->getLinkModule().isParentDamageReaction(0) == true) {
            if (m_moduleAccesser->getLinkModule().isParentDamageCatchCut(0) == false) {
                cut = true;
            }
        } else {
            cut = true;
        }
    }
    if (cut == true) {
        if (status != -1) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(status, m_moduleAccesser);
        }
        return true;
    }
    return false;
}

bool soCaptureModuleImpl::checkDamageThrown() {
    unk28 += m_moduleAccesser->getDamageModule().getCaptureDamage();
    if (unk28 >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0x7f2, 0)) {
        return false;
    }
    bool cut;
    if (m_moduleAccesser->getDamageModule().isCaptureCut(&cut) == true) {
        if (cut == true) {
            unk31 = true;
        }
        return false;
    }
    return true;
}

void soCaptureModuleImpl::updatePosThrown() {
    if (unk2D == true && m_updatePos == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        Vec3f node = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_node2, false);
        pos.m_x = node.m_x;
        pos.m_y = node.m_y - unk20 * m_moduleAccesser->getPostureModule().getScale();
        m_moduleAccesser->getPostureModule().setPos(&pos);
    }
}

void soCaptureModuleImpl::thrown() {
    if (unk2D == true && m_moduleAccesser->getLinkModule().isModelConstraint() == false) {
        m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(0, m_node1, m_catchNode, 3, true);
        m_moduleAccesser->getLinkModule().setAttribute(0, soLinkConnection::Attribute_Reference_Parent_Visibility, false);
        soLinkModule* link = soExternalValueAccesser::getLinkModule(m_moduleAccesser->getLinkModule().getParent(0));
        if (link->isLink(2) == false) {
            if (link->link(2, m_moduleAccesser->getStageObject().getTaskId()) == true) {
                link->setAttribute(2, soLinkConnection::Attribute_Reference_Parent_Stop, true);
            }
        }
    }
    m_isThrown = true;
    unk31 = false;
    unk28 = 0.0f;
}

void soCaptureModuleImpl::updateLRThrown() {
    const float* values = lbl_27_rodata_910;
    if (unk2D == true) {
        Matrix matrix(true);
        m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_node2, &matrix, false);
        float one;
        float zero = values[0];
        Vec3f direction(zero, zero, zero);
        one = values[1];
        Vec3f axis;
        axis.m_x = zero;
        axis.m_y = zero;
        axis.m_z = one;
        fn_8003E3BC(&matrix, &axis, &direction);
        float rightY;
        float rightX;
        float facingX;
        float facingY;
        facingY = direction.m_y;
        facingX = direction.m_x;
        Vec2f facing(facingX, facingY);
        rightX = values[1];
        rightY = values[0];
        Vec2f right(rightX, rightY);
        float lr = values[2];
        if (right.m_x * facing.m_x + right.m_y * facing.m_y >= values[3]) {
            lr = values[1];
        }
        if (lr != m_moduleAccesser->getPostureModule().getLr()) {
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
}
