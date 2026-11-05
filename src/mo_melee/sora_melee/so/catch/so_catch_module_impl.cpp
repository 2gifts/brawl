#include <so/catch/so_catch_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>

// Matrix::mulVec (the map name); em_kinetic_energy.cpp calls it by this stand-in name.

soCatchModule::~soCatchModule() { }

void soCatchModuleImpl::activate() {
    m_sendCutEvent = true;
    m_catchFlag = false;
}

void soCatchModuleImpl::deactivate() { }

void soCatchModuleImpl::setNodes(int node1, int node2, float unk) {
    m_node1 = node1;
    m_node2 = node2;
    unk1C = unk;
}

void soCatchModuleImpl::initInfo() {
    unk54 = 3.4028235e38f;
}

void soCatchModuleImpl::setCatch(int taskId) {
    m_sendCutEvent = true;
    if (static_cast<u32>(taskId) != 0xFFFFFFFF) {
        if (m_moduleAccesser->getLinkModule().link(0, taskId) == true) {
            onCatchFlag();
            UnkLinkEvent event(0x11, -1);
            m_moduleAccesser->getLinkModule().sendEventParents(0, event);
            m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(0, m_node1, event.unk8, 3, true);
        }
    }
}

bool soCatchModuleImpl::isCatch() {
    if (m_catchFlag == true) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return true;
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return true;
        }
    }
    return false;
}

void soCatchModuleImpl::onCatchFlag() {
    m_catchFlag = true;
}

void soCatchModuleImpl::offCatchFlag() {
    m_catchFlag = false;
}

void soCatchModuleImpl::setSendCutEvent(bool sendCutEvent) {
    m_sendCutEvent = sendCutEvent;
}

void soCatchModuleImpl::updatePosCling() {
    if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        Vec3f nodePos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_node2, false);
        pos.m_x = nodePos.m_x;
        pos.m_y = nodePos.m_y - unk1C * m_moduleAccesser->getPostureModule().getScale();
        m_moduleAccesser->getPostureModule().setPos(&pos);
    }
}

void soCatchModuleImpl::updateLRCling() {
    if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
        Matrix matrix(true);
        m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_node2, &matrix, false);
        Vec3f direction(0.0f, 0.0f, 0.0f);
        Vec3f forward(0.0f, 0.0f, 1.0f);
        matrix.mulVec(&forward, &direction);
        float y = direction.m_y;
        Vec2f xy(direction.m_x, y);
        Vec2f right(1.0f, 0.0f);
        float lr = -1.0f;
        if (right.m_x * xy.m_x + right.m_y * xy.m_y >= 0.00001f) {
            lr = 1.0f;
        }
        if (lr != m_moduleAccesser->getPostureModule().getLr()) {
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
}
