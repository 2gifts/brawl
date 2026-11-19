#define SO_ANIM_CMD_ADDRESS_PACK_ARGS
#define SO_LINK_PHYSICS_SLOTS
#define SO_GLOW_MODULE_ATTACK_API
#define SO_LINK_MODULE_IMPL_FULL
#include <so/link/so_link_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>
#include <so/color/so_color_blend_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/glow/so_glow_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/shake/so_shake_module_simple.h>
#include <so/slow/so_slow_module_impl.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/so_module_hyp.h>

soLinkModule::~soLinkModule() {
}

soLinkModuleImpl::soLinkModuleImpl(soModuleAccesser* moduleAccesser, soArray<soLinkConnection>* connections)
    : soAnimCmdEventObserver(5, static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId()) {
    m_modelConstraintNo = -1;
    m_modelConstraintFlag.m_mask = 0;
    m_isModelConstraintMutual = false;
    m_anmFileTransactorLinkNo = -1;
    m_moduleAccesser = moduleAccesser;
    m_connections = connections;
    m_modelConstraintNode = -1;
    m_modelConstraintTargetNode = -1;
    m_serverNode.m_next = NULL;
    m_serverNode.m_prev = NULL;
    m_serverNodeOwner = this;
    for (s32 i = 0; i < m_connections->size(); i++) {
        soLinkConnection& connection = m_connections->at(i);
        connection.m_id = i;
        connection.reset();
    }
    m_nodes.m_tail = NULL;
    m_nodes.m_head = NULL;
    m_nodes.m_length = 0;
    m_parents.m_tail = NULL;
    m_parents.m_head = NULL;
    m_parents.m_length = 0;
}

soLinkModuleImpl::~soLinkModuleImpl() {
    deactivate();
}

void soLinkModuleImpl::adjustModelConstraintPosture(Vec3f* rot, Vec3f* pos) {
    static_cast<soLinkModule*>(this)->adjustModelConstraintPosture(m_modelConstraintFlag.m_mask, rot, pos);
}

void soLinkModuleImpl::adjustModelConstraintPosture(u16 flags, Vec3f* rot, Vec3f* pos) {
    Matrix mtx(true);
    static_cast<soLinkModule*>(this)->getModelConstraintTargetNodeWorldMtx(&mtx, false);
    if ((flags & 1) != 0) {
        Vec3f outPos;
        mtx.mulPos(rot, &outPos);
        m_moduleAccesser->getPostureModule().setPos(&outPos);
    }
    if ((flags & 2) != 0) {
        Vec3f outRot;
        mtx.getRotate(&outRot);
        outRot.m_x = 57.29578f * outRot.m_x + pos->m_x;
        outRot.m_y = 57.29578f * outRot.m_y + pos->m_y;
        outRot.m_z = 57.29578f * outRot.m_z + pos->m_z;
        m_moduleAccesser->getPostureModule().setRot(&outRot, 0);
    }
}

Vec3f soLinkModuleImpl::getModelConstraintTargetNodePosition() {
    return m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_modelConstraintTargetNode, false);
}

Vec3f soLinkModuleImpl::getModelConstraintTargetNodeRotation() {
    return m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalRotation(m_modelConstraintTargetNode, false);
}

bool soLinkModuleImpl::getNodeRotation(u32 index, u32 nodeIndex, Vec3f* rotation) {
    rotation->m_x = rotation->m_y = rotation->m_z = 0.0f;
    Vec3f rot = m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalRotation(nodeIndex, false);
    rotation->m_x = rot.m_x;
    rotation->m_y = rot.m_y;
    rotation->m_z = rot.m_z;
    return true;
}

bool soLinkModuleImpl::setUpModelConstraintTargetMtx() {
    soLinkConnection& connection = m_connections->at(m_modelConstraintNo);
    if (connection.m_parent.getLinkModule()->m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        connection.m_parent.getLinkModule()->m_moduleAccesser->getLinkModule().setUpModelConstraintTargetMtx();
    }
    connection.m_parent.getLinkModule()->m_moduleAccesser->getModelModule().setUpMtx(m_modelConstraintTargetNode);
    return true;
}

void soLinkModuleImpl::getCmdAddressPackParents(u32 linkConnectionIndex, soArray<soAnimCmdAddressPack>* pack, int motionKind, short* ids) {
    soLinkModuleImpl* parent = m_connections->at(linkConnectionIndex).m_parent.getLinkModule();
    int linkNo = parent->m_anmFileTransactorLinkNo;
    if (linkNo != -1) {
        static_cast<soLinkModule*>(parent)->getCmdAddressPackParents(linkNo, pack, motionKind, ids);
    } else {
        parent->m_moduleAccesser->getAnimCmdModule().setAddressPackList(pack, motionKind, ids);
    }
}

bool soLinkModuleImpl::correctParent2ndBoth(u32 index, Vec3f* pos, float value) {
    soLinkModuleImpl* parent = m_connections->at(index).m_parent.getLinkModule();
    return static_cast<soHyp::soPhysicsModuleHyp*>(parent->m_moduleAccesser->m_enumerationStart->m_physicsModule)->unk38(pos, value);
}

void soLinkModuleImpl::setConstraint2ndWind(u8 wind) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (static_cast<soLinkModule*>(node->getLinkModule())->isModelConstraint() == true) {
            static_cast<soHyp::soPhysicsModuleHyp*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_physicsModule)->unkD8(wind);
        }
    }
}

void soLinkModuleImpl::getLinkParentCameraInfo(u32 index, u8* playerNo, u8* pri, u8* advPri) {
    soLinkConnection& connection = m_connections->at(index);
    *playerNo = connection.m_parent.getLinkModule()->m_moduleAccesser->getCameraModule().getPlayerNo(0);
    *pri = connection.m_parent.getLinkModule()->m_moduleAccesser->getCameraModule().getPri(0);
    *advPri = connection.m_parent.getLinkModule()->m_moduleAccesser->getCameraModule().getAdvPri(0);
}

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
        static_cast<soLinkModule*>(this)->removeModelConstraint(true);
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

// The deactivation events sent to both ends of every connection (soLinkEventArgs keeps its constructor protected).
struct soLinkDeactivateEvent : public soLinkEventArgs {
    soLinkDeactivateEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

// TODO: clean up, give variables real names
void soLinkModuleImpl::deactivate() {
    soLinkDeactivateEvent nodeEvent(StageObject::Link::Event_Deactivate_Node);
    sendEventParents(-1, nodeEvent);
    soLinkDeactivateEvent parentEvent(StageObject::Link::Event_Deactivate_Parent);
    sendEventNodes(-1, parentEvent, 0);
    unlinkAll();
    unlinkNodeAll();
    lbl_27_bss_110.exit(&m_serverNode);
}

// TODO: clean up, give variables real names
bool soLinkModuleImpl::link(u32 index, int taskId) {
    return lbl_27_bss_110.link(this, taskId, &m_connections->at(index));
}

void soLinkModuleImpl::unlinkNode(u32 index) {
    soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0));
    while (node != NULL) {
        soLinkNode* next = static_cast<soLinkNode*>(node->m_next);
        if (static_cast<int>(index) == static_cast<int>(node->m_connection->m_id)) {
            node->getLinkModule()->unlink(index);
        }
        node = next;
    }
}

void soLinkModuleImpl::setAttribute(u32 index, soLinkConnection::Attribute attribute, bool enable) {
    soLinkConnection& connection = m_connections->at(index);
    if (enable == true) {
        connection.m_attribute |= 1 << attribute;
    } else {
        connection.m_attribute &= ~(1 << attribute);
    }
}

bool soLinkModuleImpl::chkLinkedAttribute(soLinkConnection::Attribute attribute) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << attribute)) {
            return true;
        }
    }
    return false;
}

int soLinkModuleImpl::getNodeTaskId(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->getOwner()->m_taskId;
        }
    }
    return -1;
}

bool soLinkModuleImpl::isNodeDamageReaction(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->m_moduleAccesser->getDamageModule().isReaction();
        }
    }
    return false;
}

bool soLinkModuleImpl::isNodeDamageCaptureCut(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->m_moduleAccesser->getDamageModule().isCaptureCut();
        }
    }
    return false;
}

StageObject* soLinkModuleImpl::searchParentAttr(soLinkConnection::Attribute attribute) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << attribute)) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

bool soLinkModuleImpl::chkLinkStop() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
                return node->getLinkModule()->m_moduleAccesser->getStopModule().isStop();
            }
        }
    }
    return false;
}

void soLinkModuleImpl::toLinkStop() {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
            node->getLinkModule()->m_moduleAccesser->getStopModule().setLinkStop(1);
        }
    }
}

void soLinkModuleImpl::leaveLinkStop() {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
            node->getLinkModule()->m_moduleAccesser->getStopModule().setLinkStop(0);
        }
    }
}

bool soLinkModuleImpl::isValidLinkStop() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
                return true;
            }
        }
    }
    return false;
}

soColorBlend* soLinkModuleImpl::getLinkColorBlend() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Color_Blend)) {
                return node->getLinkModule()->m_moduleAccesser->getColorBlendModule().getColorBlend();
            }
        }
    }
    return NULL;
}

s8 soLinkModuleImpl::getLinkLightSet() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Light_Set)) {
                return node->getLinkModule()->m_moduleAccesser->getColorBlendModule().getLightSet();
            }
        }
    }
    return 0;
}

bool soLinkModuleImpl::chkLinkVisibility() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Visibility)) {
                return node->getLinkModule()->m_moduleAccesser->getVisibilityModule().isVisible();
            }
        }
    }
    return true;
}

soController* soLinkModuleImpl::getLinkController() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Unknown_4)) {
                return node->getLinkModule()->m_moduleAccesser->getControllerModule().getController();
            }
        }
    }
    return &g_soControllerNull;
}

float soLinkModuleImpl::getLinkScale() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Scale)) {
                return node->getLinkModule()->m_moduleAccesser->getPostureModule().getScale();
            }
        }
    }
    return 1.0f;
}

void soLinkModuleImpl::adjustLinkGroundCollision(int unk1, float* unk2) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Ground_Collision)) {
            node->getLinkModule()->getOwner()->adjustParentGroundCollision(unk1, unk2);
        }
    }
}

Vec3f soLinkModuleImpl::getParentModelNodeGlobalPosition(u32 index, u32 correctNodeId, bool p3) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(correctNodeId, p3);
}

Vec3f soLinkModuleImpl::getParentModelNodeGlobalPosition(u32 index, const char* nodeName, bool p3) {
    soLinkConnection& connection = m_connections->at(index);
    int nodeId = connection.m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeId(nodeName);
    return connection.m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeId, p3);
}

Vec3f soLinkModuleImpl::getParentRot(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getPostureModule().getRot(0);
}

nw4r::g3d::ResFile soLinkModuleImpl::getParentAnmFile(u32 index, u16 fileIndex) {
    return m_connections->at(index).m_parent.getLinkModule()->getAnmFile(fileIndex);
}

bool soLinkModuleImpl::isParentEnableEnvMap(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().isEnvMap();
}

void soLinkModuleImpl::sendEnvMapNodes(bool enable) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Env_Map)) {
            node->getLinkModule()->m_moduleAccesser->getModelModule().switchEnvMap(enable);
        }
    }
}

bool soLinkModuleImpl::isParentEnableSubColor(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getColorBlendModule().isEnableSubColor();
}

nw4r::g3d::ResFile soLinkModuleImpl::getAnmFile(u16 fileIndex) {
    if (m_anmFileTransactorLinkNo != -1) {
        return getParentAnmFile(m_anmFileTransactorLinkNo, fileIndex);
    }
    return m_moduleAccesser->getResourceModule().getAnmFile(fileIndex, 0xFFFF, 0);
}

bool soLinkModuleImpl::getLinkSlopeRotation(int index, Vec3f* rotation) {
    rotation->m_x = rotation->m_y = rotation->m_z = 0.0f;
    if (index >= 0) {
        return getNodeRotation(index, 0, rotation);
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Slope)) {
                Vec3f rot = node->m_connection->m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalRotation(0, false);
                rotation->m_x = rot.m_x;
                rotation->m_y = rot.m_y;
                rotation->m_z = rot.m_z;
                return true;
            }
        }
    }
    return false;
}

float soLinkModuleImpl::getConstraintModelScale() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Model_Scale)) {
                return node->getLinkModule()->m_moduleAccesser->getModelModule().getScale();
            }
        }
    }
    return 1.0f;
}

void soLinkModuleImpl::setNodeScalePowerMul(float mul) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Scale_Power_Mul)) {
            node->getLinkModule()->m_moduleAccesser->getParamCustomizeModule().setAttackPowerMulScale(mul);
        }
    }
}

soArticle* soLinkModuleImpl::generateArticleLinkParents(u32 connectionIndex, int id, soModuleAccesser* moduleAccesser) {
    return m_connections->at(connectionIndex).m_parent.getLinkModule()->m_moduleAccesser->getGenerateArticleManageModule().generate(id, moduleAccesser, false);
}

bool soLinkModuleImpl::isValidParentShape(u32 index) {
    return !m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().isNull();
}

Vec2f soLinkModuleImpl::getParentShapeCenterPos(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().getCenterPos(0);
}

bool soLinkModuleImpl::isNodeGroundCrush(float p1) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Ground_Crush)) {
            if (node->getLinkModule()->m_moduleAccesser->getGroundModule().isCrush(p1, 0) == true) {
                return true;
            }
        }
    }
    return false;
}

bool soLinkModuleImpl::getParentShapeCenterPosTaskId(int taskId, Vec2f* outCenterPos) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        soLinkNode& parent = node->m_connection->m_parent;
        if (taskId == parent.m_taskId) {
            *outCenterPos = parent.getLinkModule()->m_moduleAccesser->getGroundModule().getCenterPos(0);
            return true;
        }
        if (parent.getLinkModule()->m_moduleAccesser->getLinkModule().getParentShapeCenterPosTaskId(taskId, outCenterPos) == true) {
            return true;
        }
    }
    return false;
}

// HYPOTHESIS: glow module slots past isGlow, named from the soLinkModuleImpl callers.
class soGlowModuleLinkView : public soGlowModule {
public:
    virtual int getWeaponGlowDefense();    // 0x24
    virtual int unk28();
    virtual int unk2C();
    virtual float getWeaponGlowEff();      // 0x30
    virtual float getWeaponGlowDamageMin(); // 0x34
    virtual float getWeaponGlowDamageMax(); // 0x38
    virtual int unk3C();
    virtual int unk40();
    virtual int unk44();
    virtual int unk48();
    virtual int unk4C();
    virtual int unk50();
    virtual int unk54();
    virtual int getGlowAttackAdd(soCollisionAttackData* data); // 0x58
    virtual int getGlowAttackData(soCollisionAttackData* data); // 0x5C
    virtual int unk60();
    virtual int getWeaponGlowAttack();     // 0x64
};

#define SO_PARENT_GLOW(index) (*reinterpret_cast<soGlowModuleLinkView*>(m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->m_enumerationStart->m_glowModule))

bool soLinkModuleImpl::isParentGlow(u32 index) {
    return SO_PARENT_GLOW(index).isGlow();
}

int soLinkModuleImpl::getParentWeaponGlowDefense(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowDefense();
}

int soLinkModuleImpl::getParentGlowAttack(u32 index, soCollisionAttackData* data) {
    return SO_PARENT_GLOW(index).getGlowAttackData(data);
}

int soLinkModuleImpl::getParentWeaponGlowAttack(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowAttack();
}

int soLinkModuleImpl::getParentGlowAttackAdd(soCollisionAttackData* data) {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Power_Up)) {
                return reinterpret_cast<soGlowModuleLinkView*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_glowModule)->getGlowAttackAdd(data);
            }
        }
    }
    return 0;
}

float soLinkModuleImpl::getParentWeaponGlowEff(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowEff();
}

float soLinkModuleImpl::getParentWeaponGlowDamageMin(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowDamageMin();
}

float soLinkModuleImpl::getParentWeaponGlowDamageMax(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowDamageMax();
}

Vec2f soLinkModuleImpl::getParentSumSpeed(u32 index) {
    soLinkConnection& connection = m_connections->at(index);
    soKineticEnergy::AttributeFlag flag(1);
    return connection.m_parent.getLinkModule()->m_moduleAccesser->getKineticModule().getSumSpeed(flag);
}

int soLinkModuleImpl::getParentTexResourceId(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getTexResId();
}

bool soLinkModuleImpl::chkLinkShake(Vec2f* offset) {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Shake)) {
                return reinterpret_cast<soShakeModule*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_shakeModule)->getOffset(offset);
            }
        }
    }
    return false;
}

void soLinkModuleImpl::setParentHitStopFrame(u32 index, u32 frame, bool unk) {
    m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getStopModule().setHitStop(static_cast<s32>(frame), unk);
}

void soLinkModuleImpl::setParentAttackerInfo(u32 index, StageObject* attacker) {
    soLinkConnection& connection = m_connections->at(index);
    soDamageAttackerInfo info;
    info.clear();
    connection.m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getAttackerInfo(&info);
    info.set(connection.m_parent.m_taskId, attacker);
    connection.m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().restoreAttackerInfo(&info);
}

bool soLinkModuleImpl::chkAttribute(int index, soLinkConnection::Attribute attribute) {
    if (index >= 0) {
        u32 bit = 1 << attribute;
        u32 attr = m_connections->at(index).m_attribute;
        return (attr & bit) != 0;
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << attribute)) {
                return true;
            }
        }
    }
    return false;
}

soSlopeStatusParam::Status soLinkModuleImpl::getLinkSlopeStatus(int index) {
    if (index >= 0) {
        return static_cast<soSlopeModule*>(m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->m_enumerationStart->m_slopeModule)->getStatus();
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Slope)) {
                return static_cast<soSlopeModule*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_slopeModule)->getStatus();
            }
        }
    }
    return static_cast<soSlopeStatusParam::Status>(0);
}

Vec3f soLinkModuleImpl::getModelConstraintNodeGlobalPosition(u32 nodeIndex) {
    return m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeIndex, false);
}

void soLinkModuleImpl::getModelConstraintTargetNodeWorldMtx(Matrix* mtx, bool unk) {
    m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_modelConstraintTargetNode, mtx, unk);
}

// HYPOTHESIS: attribute 0xD is the slow-motion reference (no other query tests it).
bool soLinkModuleImpl::isLinkParentSlow(int index) {
    if (index >= 0) {
        return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getSlowModule().isSlow();
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << 13)) {
                return node->getLinkModule()->m_moduleAccesser->getSlowModule().isSlow();
            }
        }
    }
    return false;
}

bool soLinkModuleImpl::getLinkParentSlowInfo(float* rate, bool* isSkip, int index) {
    if (index >= 0) {
        soLinkConnection& connection = m_connections->at(index);
        *rate = connection.m_parent.getLinkModule()->m_moduleAccesser->getSlowModule().getRate();
        *isSkip = connection.m_parent.getLinkModule()->m_moduleAccesser->getSlowModule().isSkip();
        return true;
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << 13)) {
                *rate = node->getLinkModule()->m_moduleAccesser->getSlowModule().getRate();
                *isSkip = node->getLinkModule()->m_moduleAccesser->getSlowModule().isSkip();
                return true;
            }
        }
    }
    return false;
}

Vec3f soLinkModuleImpl::getModelConstraintNodeGlobalPositionRecursive(u32 nodeIndex) {
    soModuleAccesser* accesser = m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser;
    soLinkModule& link = accesser->getLinkModule();
    if (link.isModelConstraint() == true && !link.isModelConstraintMutual()) {
        return link.getModelConstraintNodeGlobalPositionRecursive(nodeIndex);
    }
    return accesser->getModelModule().getNodeGlobalPosition(nodeIndex, false);
}

void soLinkModuleImpl::removeModelConstraint(bool resetParent) {
    if (!static_cast<soLinkModule*>(this)->isModelConstraint()) {
        return;
    }
    soLinkConnection& connection = m_connections->at(m_modelConstraintNo);
    static_cast<soLinkModule*>(this)->setModelConstraintAttribute(m_modelConstraintNo, false);
    m_modelConstraintNo = -1;
    m_modelConstraintNode = -1;
    m_modelConstraintTargetNode = -1;
    m_modelConstraintFlag.m_mask = 0;
    m_isModelConstraintMutual = false;
    if (resetParent == true) {
        if (!connection.m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().isNull()) {
            Vec2f center = connection.m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().getCenterPos(0);
            Vec3f pos;
            pos.m_x = center.m_x;
            pos.m_y = center.m_y;
            pos.m_z = 0.0f;
            m_moduleAccesser->getGroundModule().relocate(reinterpret_cast<Vec2f*>(&pos), 0);
        }
    }
    m_moduleAccesser->getModelModule().clearSetUpMtx();
}

void soLinkModuleImpl::setModelConstraintAttribute(u32 index, bool enable) {
    soLinkConnection& connection = m_connections->at(index);
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Stop;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Stop);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Shake;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Shake);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Color_Blend;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Color_Blend);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Visibility;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Visibility);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << 13;
    } else {
        connection.m_attribute &= ~(1 << 13);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << 14;
    } else {
        connection.m_attribute &= ~(1 << 14);
    }
}

void soLinkModuleImpl::setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, int modelConstraintTargetNode, u16 modelConstraintFlag, bool modelConstraintAttribute) {
    soLinkConnection& connection = m_connections->at(modelConstraintNo);
    int correctedNode = m_moduleAccesser->getModelModule().getCorrectNodeId(modelConstraintNode);
    m_moduleAccesser->getModelModule().getNodeGlobalPosition(correctedNode, false);
    m_moduleAccesser->getModelModule().clearSetUpMtx();
    soLinkModuleImpl* parentImpl = connection.m_parent.getLinkModule();
    soLinkModule* parent = parentImpl;
    m_modelConstraintNo = modelConstraintNo;
    m_modelConstraintNode = correctedNode;
    m_modelConstraintTargetNode = parentImpl->m_moduleAccesser->getModelModule().getCorrectNodeId(modelConstraintTargetNode);
    m_modelConstraintFlag.m_mask = modelConstraintFlag;
    if (parent->isModelConstraint() == true && parent->getParentTaskId(parent->getModelConstraintNo()) == getOwner()->m_taskId) {
        m_isModelConstraintMutual = true;
    } else {
        m_isModelConstraintMutual = false;
    }
    static_cast<soLinkModule*>(this)->setModelConstraintAttribute(modelConstraintNo, modelConstraintAttribute);
}

void soLinkModuleImpl::setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, const char* modelConstraintTargetNode, u16 modelConstraintFlag, bool modelConstraintAttribute) {
    u32 nodeId = m_connections->at(modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeId(modelConstraintTargetNode);
    if (nodeId != 0xFFFFFFFF) {
        static_cast<soLinkModule*>(this)->setModelConstraintPosOrt(modelConstraintNo, modelConstraintNode, nodeId, modelConstraintFlag, modelConstraintAttribute);
    }
}

#include <ut/ut_list.h>
#include <so/event/so_event_system.h>
// TODO: clean up, give variables real names
namespace jj_6ADA4 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_6ADA4(void* ptr, int size) {
    if (ptr != 0 && size > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_6ADE4 {
extern "C" void __dl__FPv(void* p);

extern "C" void* fn_27_6ADE4(void* p, int n) {
    if (p != 0 && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_6B22C {
class Object_vptr_0 {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual int slot7(); virtual void slot8();
};
struct Observer {
    virtual void slot0();
    virtual void notify(int, void*, void*, int);
};
struct ObserverList {
    virtual void slot0(); virtual void slot1();
    virtual Observer** get(int);
    virtual void slot3();
    virtual int size();
};
struct EventUnitView {
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual ObserverList* getObserverList();
};
struct Node { Node* next; char pad4[8]; void* data; };
struct UnitData {
    char pad[0x30];
    union {
        u32 flags;
        struct { u32 : 24; u32 id : 7; u32 active : 1; };
        struct { u32 attributes : 24; u32 : 8; };
    };
};
struct Callback { char pad[8]; Object_vptr_0 object; };
struct ModuleRoot { char pad[0x80]; Callback* callback; };
struct ModuleAccessor { char pad[0xD8]; ModuleRoot* moduleRoot; };
struct Presenter { char pad[0x44]; ModuleAccessor* moduleAccessor; };
struct EventParams {
    void* vtable; short managerID; short eventID; ObserverList* cache; char pad[4];
    short getManagerID() const { return managerID; }
    bool checkManager() const {
        const short id = managerID;
        return soEventSystem::getInstance()->getInstanceManager().isContain(id);
    }
};
struct Work { EventParams params; Node* selected[64]; };
extern "C" {
extern char lbl_27_data_7BA0[], lbl_27_data_7B68[], lbl_27_data_480[];
extern char __RTTI__11soEventUnit[], lbl_27_bss_B40[];
Presenter* getLinkModule__10soLinkNodeFv(void*);
void* __dynamic_cast(void*, long, const void*, const void*, short);
}
inline ObserverList* getUnit(soEventManager* manager, short id) {
    EventUnitView* unit = (EventUnitView*)__dynamic_cast(
        manager->getEventUnit(id), 0, lbl_27_data_480, __RTTI__11soEventUnit, 0);
    if (unit == 0) return 0;
    return unit->getObserverList();
}
inline ObserverList* getObserverList(EventParams* params) {
    short id = params->eventID;
    soEventManager* manager = soEventSystem::getInstance()->getManager(params->getManagerID());
    if (manager->getObserverCapacity(id) == 0) return 0;
    return getUnit(manager, id);
}
inline bool updateCache(EventParams* params) {
    if (params->managerID <= -1 ||
        !params->checkManager())
        return false;
    soEventManager* manager = soEventSystem::getInstance()->getManager(params->getManagerID());
    if (manager->getObserverCapacity(params->eventID) == 0) {
        params->cache = 0;
        return false;
    }
    params->cache = getObserverList(params);
    return params->cache != 0;
}
extern "C" void sendEventNodes__16soLinkModuleImplFlRC15soLinkEventArgsUl(void* object, int eventID, int arg2, int mask) {
    void* presenterOwner;
    int id;
    void* ownerValue;
    Node** entry;
    int count;
    int i;
    Work work;
    Node* node = (Node*)((utList*)((char*)object + 0x14))->getNode(0);
    if (node == 0) return;
    count = 0;
    if (eventID == -1) {
        Node** out = work.selected;
        while (node != 0) {
            if (mask == 0 || (mask & (((UnitData*)node->data)->flags >> 8))) {
                ++count;
                *out++ = node;
                if (count >= 64) break;
            }
            node = node->next;
        }
    } else {
        Node** out = work.selected;
        while (node != 0) {
            UnitData* data = (UnitData*)node->data;
            if ((eventID == (int)data->id && mask == 0) ||
                (mask & data->attributes)) {
                ++count;
                *out++ = node;
                if (count >= 64) break;
            }
            node = node->next;
        }
    }
    entry = work.selected;
    for (i = 0; i < count; ++entry, ++i) {
        Node* current = *entry;
        u32 flags = ((UnitData*)current->data)->flags;
        if ((flags & 1) == 0) continue;
        id = (flags >> 1) & 0x7f;
        Presenter* presenter = getLinkModule__10soLinkNodeFv(current);
        short managerID = presenter->moduleAccessor->moduleRoot->callback->object.slot7();
        work.params.vtable = lbl_27_data_7BA0;
        work.params.managerID = managerID;
        work.params.eventID = 7;
        work.params.cache = 0;
        if (managerID > 0 && managerID > -1) {
            if (soEventSystem::getInstance()->getInstanceManager().isContain(managerID)) {
                soEventManager* manager = soEventSystem::getInstance()->getManager(work.params.managerID);
                if (manager->getObserverCapacity(work.params.eventID) == 0) {
                    work.params.cache = 0;
                } else {
                    soEventManager* manager2;
                    short id = work.params.eventID;
                    manager2 = soEventSystem::getInstance()->getManager(work.params.managerID);
                    ObserverList* result;
                    if (manager2->getObserverCapacity(id) == 0)
                        result = 0;
                    else
                        result = getUnit(manager2, id);
                    work.params.cache = result;
                }
            }
        }
        work.params.vtable = lbl_27_data_7B68;
        void* owner = *(void**)((char*)object + 0x44);
        ownerValue = *(void**)((char*)owner + 8);
        presenterOwner = presenter->moduleAccessor;
        ObserverList* list;
        if (work.params.managerID <= -1) {
            list = (ObserverList*)lbl_27_bss_B40;
        } else if (!(work.params.cache || updateCache(&work.params))) {
            list = (ObserverList*)lbl_27_bss_B40;
        } else {
            soEventManager* manager = soEventSystem::getInstance()->getManager(work.params.managerID);
            if (manager->getObserverCapacity(work.params.eventID) == 0) {
                work.params.cache = 0;
                list = (ObserverList*)lbl_27_bss_B40;
            } else {
                list = work.params.cache;
            }
        }
        for (int j = 0; j < list->size(); ++j) {
            Observer* observer = *list->get(j);
            observer->notify(arg2, presenterOwner, ownerValue, id);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6B66C {
class Object_vptr_0 {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual int slot7(); virtual void slot8();
};
struct Observer {
    virtual void slot0();
    virtual void notify(int, void*, void*, int);
};
struct ObserverList {
    virtual void slot0(); virtual void slot1();
    virtual Observer** get(int);
    virtual void slot3();
    virtual int size();
};
struct EventUnitView {
    virtual void slot0(); virtual void slot1(); virtual void slot2();
    virtual void slot3(); virtual void slot4(); virtual void slot5();
    virtual void slot6(); virtual ObserverList* getObserverList();
};
struct Node { Node* next; char pad4[4]; u32 key; void* data; };
struct UnitData {
    char pad[0x30];
    union {
        u32 flags;
        struct { u32 : 24; u32 id : 7; u32 active : 1; };
        struct { u32 attributes : 24; u32 : 8; };
    };
};
struct Callback { char pad[8]; Object_vptr_0 object; };
struct ModuleRoot { char pad[0x80]; Callback* callback; };
struct ModuleAccessor { char pad[0xD8]; ModuleRoot* moduleRoot; };
struct Presenter { char pad[0x44]; ModuleAccessor* moduleAccessor; };
struct EventParams {
    void* vtable; short managerID; short eventID; ObserverList* cache; char pad[4];
    short getManagerID() const { return managerID; }
    bool checkManager() const {
        const short id = managerID;
        return soEventSystem::getInstance()->getInstanceManager().isContain(id);
    }
};
extern "C" {
extern char lbl_27_data_7BA0[], lbl_27_data_7B68[], lbl_27_data_480[];
extern char __RTTI__11soEventUnit[], lbl_27_bss_B40[];
Presenter* getLinkModule__10soLinkNodeFv(void*);
void* __dynamic_cast(void*, long, const void*, const void*, short);
}
inline ObserverList* getUnit(soEventManager* manager, short id) {
    EventUnitView* unit = (EventUnitView*)__dynamic_cast(
        manager->getEventUnit(id), 0, lbl_27_data_480, __RTTI__11soEventUnit, 0);
    if (unit == 0) return 0;
    return unit->getObserverList();
}
inline ObserverList* getObserverList(EventParams* params) {
    short id = params->eventID;
    soEventManager* manager = soEventSystem::getInstance()->getManager(params->getManagerID());
    if (manager->getObserverCapacity(id) == 0) return 0;
    return getUnit(manager, id);
}
inline bool updateCache(EventParams* params) {
    if (params->managerID <= -1 ||
        !params->checkManager())
        return false;
    soEventManager* manager = soEventSystem::getInstance()->getManager(params->getManagerID());
    if (manager->getObserverCapacity(params->eventID) == 0) {
        params->cache = 0;
        return false;
    }
    params->cache = getObserverList(params);
    return params->cache != 0;
}
extern "C" void sendEventNodes__16soLinkModuleImplFliR15soLinkEventArgs(void* object, int manageID, u32 key, int arg3) {
    Node* node = (Node*)((utList*)((char*)object + 0x14))->getNode(0);
    while (node != 0) {
        Node* next = node->next;
        if ((manageID == -1 || manageID == (int)((UnitData*)node->data)->id) && key == node->key) {
            int id = ((UnitData*)node->data)->id;
            Presenter* presenter = getLinkModule__10soLinkNodeFv(node);
            short managerID = presenter->moduleAccessor->moduleRoot->callback->object.slot7();
            EventParams params;
            *(void**)&params = lbl_27_data_7BA0;
            params.managerID = managerID;
            params.eventID = 7;
            params.cache = 0;
            if (managerID > 0 && managerID > -1) {
                if (soEventSystem::getInstance()->getInstanceManager().isContain(managerID)) {
                    soEventManager* manager = soEventSystem::getInstance()->getManager(params.managerID);
                    if (manager->getObserverCapacity(params.eventID) == 0) {
                        params.cache = 0;
                    } else {
                        soEventManager* manager2;
                        short eid = params.eventID;
                        manager2 = soEventSystem::getInstance()->getManager(params.managerID);
                        ObserverList* result;
                        if (manager2->getObserverCapacity(eid) == 0)
                            result = 0;
                        else
                            result = getUnit(manager2, eid);
                        params.cache = result;
                    }
                }
            }
            *(void**)&params = lbl_27_data_7B68;
            void* owner = *(void**)((char*)object + 0x44);
            void* ownerValue = *(void**)((char*)owner + 8);
            void* presenterOwner = presenter->moduleAccessor;
            ObserverList* list;
            if (params.managerID <= -1) {
                list = (ObserverList*)lbl_27_bss_B40;
            } else if (!(params.cache || updateCache(&params))) {
                list = (ObserverList*)lbl_27_bss_B40;
            } else {
                soEventManager* manager = soEventSystem::getInstance()->getManager(params.managerID);
                if (manager->getObserverCapacity(params.eventID) == 0) {
                    params.cache = 0;
                    list = (ObserverList*)lbl_27_bss_B40;
                } else {
                    list = params.cache;
                }
            }
            for (int j = 0; j < list->size(); ++j) {
                Observer* observer = *list->get(j);
                observer->notify(arg3, presenterOwner, ownerValue, id);
            }
        }
        node = next;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6EAB8 {
struct ArgData { int type; int value; };
struct ArgResult { ArgData* data; unsigned char flag; };
struct PairArg { int value; signed char flag; PairArg(int v):value(v),flag(0) {} };
class Self { public:
virtual void v0(); virtual signed char category(); virtual signed char type(); virtual int count(); virtual void v4(); virtual void v5(); virtual bool getArg(ArgResult*, int) const;
};
class Cmd { public:
virtual void v0();
virtual void v1();
virtual void v2();
virtual void v3();
virtual void v4();
virtual void v5();
virtual void v6();
virtual void v7();
virtual void call8(int);
virtual void v9();
virtual void v10();
virtual void v11();
virtual void v12();
virtual void v13();
virtual void v14();
virtual void call15(int, const PairArg&);
virtual void call16(int, const PairArg&, int);
virtual void v17();
virtual void v18();
virtual void v19();
virtual void v20();
virtual void v21();
virtual void v22();
virtual void v23();
virtual void v24();
virtual void v25();
virtual void v26();
virtual void v27();
virtual void v28();
virtual void v29();
virtual void v30();
virtual void v31();
virtual void v32();
virtual void v33();
virtual void v34();
virtual void v35();
virtual void v36();
virtual void v37();
virtual void v38();
virtual void v39();
virtual void v40();
virtual void v41();
virtual void v42();
virtual void v43();
virtual void v44();
virtual void v45();
virtual void v46();
virtual void v47();
virtual void v48();
virtual void v49();
virtual void v50();
virtual void v51();
virtual void v52();
virtual void v53();
virtual void v54();
virtual void v55();
virtual void v56();
virtual void v57();
virtual void v58();
virtual void v59();
virtual void v60();
virtual void v61();
virtual void v62();
virtual void v63();
virtual void v64();
virtual void v65();
virtual void v66();
virtual void v67();
virtual void v68();
virtual void v69();
virtual void v70();
virtual void v71();
virtual void v72();
virtual void v73();
virtual void v74();
virtual void call75(int,int,int,unsigned short,bool);
virtual void v76();
virtual void v77();
virtual void v78();
virtual void v79();
virtual void v80();
virtual void v81();
virtual void call82(int);
virtual void call83(int);
virtual void v84();
virtual void call85(float*,float*);
virtual void v86();
virtual void call87(int);
virtual void v88();
virtual void v89();
virtual void v90();
virtual void v91();
virtual void v92();
virtual void v93();
virtual void v94();
virtual void v95();
virtual void v96();
virtual void v97();
virtual void v98();
virtual void call99(int,int,int);
virtual void v100();
virtual void v101();
virtual void v102();
virtual void v103();
virtual void v104();
virtual void v105();
virtual void v106();
virtual void v107();
virtual void v108();
virtual void v109();
virtual void v110();
virtual void v111();
virtual void v112();
virtual void v113();
virtual void v114();
virtual void v115();
virtual void v116();
virtual void v117();
virtual bool accept(signed char);
};
struct GroundBase { char pad[0x3c]; };
class Ground : public GroundBase { public:
virtual void v0();
virtual void v1();
virtual void v2();
virtual void v3();
virtual void v4();
virtual void v5();
virtual void v6();
virtual void v7();
virtual void v8();
virtual void v9();
virtual void v10();
virtual void v11();
virtual void v12();
virtual void v13();
virtual void v14();
virtual void v15();
virtual void v16();
virtual void v17();
virtual void v18();
virtual void v19();
virtual void v20();
virtual void v21();
virtual void v22();
virtual void v23();
virtual void v24();
virtual void v25();
virtual void v26();
virtual void v27();
virtual void v28();
virtual void v29();
virtual void v30();
virtual void v31();
virtual void v32();
virtual void v33();
virtual void v34();
virtual void v35();
virtual void v36();
virtual void v37();
virtual void v38();
virtual void v39();
virtual void v40();
virtual void v41();
virtual void v42();
virtual int getMdlIndex();
};
struct Access { void* pad; void* pad2; Ground* ground; };
extern const float lbl_27_rodata_950;
extern "C" int notifyEventAnimCmd__16soLinkModuleImplFP9acAnimCmdP16soModuleAccesseri(Cmd* cmd, Self* self, Access* acc) {
 if (cmd->accept(self->category()) == 0) return 0;
 if (self->type() <= -1 || self->type() >= 0xA) return 0;
 int shared;
 switch (self->type()) {
 case 0: {
 ArgResult local0; local0.data=0; local0.flag=0;
 if (self->getArg(&local0,0)==0) return 1;
 int val0; if(local0.flag==1) val0=0; else val0=local0.data->value;
 ArgResult local1; local1.data=0; local1.flag=0;
 if (self->getArg(&local1,1)==0) return 1;
 if(local1.flag==1) shared=0; else shared=local1.data->value;
 ArgResult local2; local2.data=0; local2.flag=0;
 if (self->getArg(&local2,2)==0) return 1;
 int val2; if(local2.flag==1) val2=0; else val2=local2.data->value;
 ArgResult local3; local3.data=0; local3.flag=0; int val3=3;
 if ((unsigned int)self->getArg(&local3,3)==1U) {
 int t; if(local3.flag==1) t=-1; else t=local3.data->type;
 if(t==0) { int v; if(local3.flag==1) v=0; else v=local3.data->value; val3=v & 0xFFFF; }
 }
 cmd->call75(val0,shared,val2,val3,1); return 1;
 }
 case 1: {
 if(self->count()<=0) cmd->call87(1);
 else { ArgResult local; local.data=0; local.flag=0; if(self->getArg(&local,0)==0) return 1; int val; if(local.flag==1) val=0; else val=local.data->value!=0; cmd->call87(val); }
 return 1;
 }
 case 2: {
 float arr0[3]; float arr1[3];
 arr0[0]=lbl_27_rodata_950; arr0[1]=lbl_27_rodata_950; arr0[2]=lbl_27_rodata_950;
 arr1[0]=lbl_27_rodata_950; arr1[1]=lbl_27_rodata_950; arr1[2]=lbl_27_rodata_950;
 cmd->call85(arr0,arr1); acc->ground->getMdlIndex(); return 1;
 }
 case 3: {
 ArgResult local0; local0.data=0; local0.flag=0; if(self->getArg(&local0,0)==0) return 1;
 if(local0.flag==1) shared=0; else shared=local0.data->value;
 ArgResult local1; local1.data=0; local1.flag=0; if(self->getArg(&local1,1)==0) return 1;
 int val1; if(local1.flag==1) val1=0; else val1=local1.data->value;
 cmd->call15(shared,PairArg(val1)); return 1;
 }
 case 4: {
 ArgResult local0; local0.data=0; local0.flag=0;
 if(self->getArg(&local0,0)==0) goto fail0;
 { int t; if(local0.flag==1) t=-1; else t=local0.data->type;
 if(t==0) goto next; }
 fail0: return 1;
 next:
 ArgResult local1; local1.data=0; local1.flag=0;
 if(self->getArg(&local1,1)==0) goto fail1;
 { int t1; if(local1.flag==1) t1=-1; else t1=local1.data->type;
 if(t1==0) goto call; }
 fail1: return 1;
 call:
 int val0; if(local0.flag==1) val0=0; else val0=local0.data->value;
 int val1; if(local1.flag==1) val1=0; else val1=local1.data->value;
 cmd->call16(val0,PairArg(val1),0); return 1;
 }
 case 5: case 6: {
 int extra;
 ArgResult local0; local0.data=0; local0.flag=0; if(self->getArg(&local0,0)==0) return 1;
 int val0; if(local0.flag==1) val0=0; else val0=local0.data->value;
 ArgResult local1; local1.data=0; local1.flag=0; if(self->getArg(&local1,1)==0) return 1;
 int val1; if(local1.flag==1) val1=0; else val1=local1.data->value;
 extra=1; if(self->type()==6) extra=0; cmd->call99(val0,val1,extra); return 1;
 }
 case 7: {
 ArgResult local; local.data=0; local.flag=0; if(self->getArg(&local,0)==0) return 1;
 int val; if(local.flag==1) val=0; else val=local.data->value; cmd->call8(val); return 1;
 }
 case 8: {
 ArgResult local; local.data=0; local.flag=0; if(self->getArg(&local,0)==0) return 1;
 int val; if(local.flag==1) val=0; else val=local.data->value; cmd->call82(val); return 1;
 }
 case 9: {
 ArgResult local; local.data=0; local.flag=0; if(self->getArg(&local,0)==0) return 1;
 int val; if(local.flag==1) val=0; else val=local.data->value; cmd->call83(val); return 1;
 }
 default: return 0;
 }
}
}

// TODO: clean up, give variables real names
namespace jj_6F2FC {
extern "C" void notifyEventAnimCmd__16soLinkModuleImplFP9acAnimCmdP16soModuleAccesseri(void*);

extern "C" void fn_27_6F2FC(void* self) {
    notifyEventAnimCmd__16soLinkModuleImplFP9acAnimCmdP16soModuleAccesseri(reinterpret_cast<u8*>(self) - 0x8);
}
}

