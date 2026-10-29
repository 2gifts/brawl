#define SO_LINK_MODULE_IMPL_FULL
#include <so/link/so_link_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>

StageObject* soLinkModuleImpl::getOwner() {
    return m_moduleAccesser->m_stageObject;
}

void soLinkModuleImpl::activate() {
    lbl_27_bss_110.entry(&m_serverNode);
    unlinkAll();
    unlinkNodeAll();
}

void soLinkModuleImpl::unlinkAll() {
    lbl_27_bss_110.disconnectAll(this);
}

void soLinkModuleImpl::unlinkNodeAll() {
    lbl_27_bss_110.disconnectNodeAll(this);
}

void soLinkModuleImpl::unlink(u32 index) {
    lbl_27_bss_110.disconnect(&m_connections->at(index));
}

bool soLinkModuleImpl::isLink(int index) {
    if (index != -1) {
        return m_connections->at(index).m_isLinked;
    }
    return m_parents.m_length != 0;
}

bool soLinkModuleImpl::isLinked(int index) {
    if (index != -1) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (index == static_cast<int>(node->m_connection->m_id)) {
                return true;
            }
        }
        return false;
    }
    return m_nodes.m_length != 0;
}

void soLinkModuleImpl::onUnlink(int id) {
    if (isModelConstraint() && m_modelConstraintNo == id) {
        removeModelConstraint(true);
    }
}

bool soLinkModuleImpl::isModelConstraint() {
    return m_modelConstraintNo != -1;
}

StageObject* soLinkModuleImpl::getNode(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

s8 soLinkModuleImpl::getParentModelLoupeNo(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getLoupeNo();
}

StageObject* soLinkModuleImpl::getParent(u32 index) {
    soLinkConnection& connection = m_connections->at(index);
    if (connection.m_isLinked) {
        return connection.m_parent.getLinkModule()->getOwner();
    }
    return NULL;
}

StageObject* soLinkModuleImpl::searchParent(int taskId) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (taskId == node->m_taskId) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

StageObject* soLinkModuleImpl::searchNode(int taskId) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (taskId == node->m_taskId) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

float soLinkModuleImpl::getParentScale(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getPostureModule().getScale();
}

GXColor soLinkModuleImpl::getParentSubColor(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getColorBlendModule().getSubColor();
}

float soLinkModuleImpl::getParentMotionFrame(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getMotionModule().getFrame();
}

SituationKind soLinkModuleImpl::getParentSituationKind(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getSituationModule().getKind();
}

float soLinkModuleImpl::getParentLr(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getPostureModule().getLr();
}

int soLinkModuleImpl::getParentTaskId(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->getOwner()->m_taskId;
}

float soLinkModuleImpl::getParentDamage(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getDamage(0);
}

float soLinkModuleImpl::getParentDamageReaction(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getReaction(0);
}

float soLinkModuleImpl::getParentDamagePowerMax(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getPowerMax(0);
}

bool soLinkModuleImpl::isParentDamageReaction(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().isReaction();
}

bool soLinkModuleImpl::isParentDamageCatchCut(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().isCatchCut();
}

int soLinkModuleImpl::getParentTotalHitStatus(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getCollisionHitModule().getTotalStatus(0);
}

float soLinkModuleImpl::getOwnerAttackPowerMulArticle(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getParamCustomizeModule().getAttackPowerMulArticle();
}

bool soLinkModuleImpl::isParentRefFB(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().isRefFB();
}

bool soLinkModuleImpl::isParentEffectSyncScale(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getEffectModule().isSyncScale();
}

u8 soLinkModuleImpl::getParentAttachItemVisibility(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getItemManageModule().getAttachItemVisibility();
}

void soLinkModuleImpl::setAnmFileTransactorLinkNo(int no) {
    m_anmFileTransactorLinkNo = no;
}

u32 soLinkModuleImpl::getModelConstraintNo() {
    return m_modelConstraintNo;
}

void soLinkModuleImpl::setModelConstraintNode(int node) {
    m_modelConstraintNode = node;
}

void soLinkModuleImpl::setModelConstraintTargetNode(int node) {
    m_modelConstraintTargetNode = node;
}

bool soLinkModuleImpl::isObserv(char group) {
    return group == 15;
}

bool soLinkModuleImpl::isModelConstraintMutual() {
    return m_isModelConstraintMutual;
}

int soLinkModuleImpl::getModelConstraintNode() {
    return m_modelConstraintNode;
}

void soLinkModuleImpl::offModelConstraintFlag(u16 flag) {
    m_modelConstraintFlag.m_mask &= ~flag;
}

void soLinkModuleImpl::setModelConstraintFlag(u16 flag) {
    m_modelConstraintFlag.m_mask = flag;
}

u16 soLinkModuleImpl::getModelConstraintFlag() {
    return m_modelConstraintFlag.m_mask;
}
