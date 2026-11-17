#define SO_ITEM_INFO_BYTE_ATTACH_GROUP
#define SO_ITEM_INFO_NO_PADDING
#include <it/it_manager.h>
#include <it/item.h>
#include <so/item/so_item_manage_module_impl.h>
#include <types.h>

bool soItemManageModuleImpl::isHaveItem(u32 index) const {
    return m_haveItemInfoArray->at(index).m_item != nullptr;
}

itParam::SizeKind soItemManageModuleImpl::getHaveItemSize(u32 index) const {
    return m_haveItemInfoArray->at(index).m_size;
}

itKind soItemManageModuleImpl::getHaveItemKind(u32 index) const {
    return m_haveItemInfoArray->at(index).m_kind;
}

itParam::TraitFlag soItemManageModuleImpl::getHaveItemTrait(u32 index) const {
    return m_haveItemInfoArray->at(index).m_trait;
}

itParam::HaveKind soItemManageModuleImpl::getHaveItemHoldKind(u32 index) {
    if (isHaveItem(index) == true) {
        return (itParam::HaveKind)m_haveItemInfoArray->at(index).m_item->getParam(itValueAccesser::Specialized_Param_Int_Have_Kind);
    }
    return (itParam::HaveKind)0;
}

int soItemManageModuleImpl::getHaveItemParamInt(u32 param, u32 index) const {
    return m_haveItemInfoArray->at(index).m_item->getParam((itValueAccesser::ParamInt)param);
}

float soItemManageModuleImpl::getHaveItemParamFloat(u32 param, u32 index) const {
    return m_haveItemInfoArray->at(index).m_item->getParam((itValueAccesser::ParamFloat)param);
}

u32 soItemManageModuleImpl::getAttachItemCount() {
    return m_attachItemInfoArray->size();
}

u8 soItemManageModuleImpl::getAttachItemVisibility() {
    return m_attachItemVisibility;
}

itParam::TraitFlag soItemManageModuleImpl::getPickableItemTrait() {
    return m_pickableItemInfo.m_trait;
}

itKind soItemManageModuleImpl::getPickableItemKind() {
    return m_pickableItemInfo.m_kind;
}

itParam::SizeKind soItemManageModuleImpl::getPickableItemSize() {
    return m_pickableItemInfo.m_size;
}

bool soItemManageModuleImpl::isSuccessPickupItem() {
    return m_isSuccessPickupItem;
}

bool soItemManageModuleImpl::isAttachItem(itKind kind) {
    s32 count = m_attachItemInfoArray->size();
    for (s32 i = 0; i < count; i++) {
        if (kind == m_attachItemInfoArray->at(i).m_kind) {
            return true;
        }
    }
    return false;
}

void soItemManageModuleImpl::warpItem(Vec3f* pos, u32 index) {
    if (isHaveItem(index)) {
        m_haveItemInfoArray->at(index).m_item->warp(pos);
    }
}

void soItemManageModuleImpl::bornItem(u32 index) {
    if (isHaveItem(index)) {
        m_haveItemInfoArray->at(index).m_item->born();
    }
}

bool soItemManageModuleImpl::removeItem(BaseItem* item) {
    itManager::getInstance()->removeItem(item);
    return true;
}

void soItemManageModuleImpl::deactivate() {
    removeAll();
}

void soItemManageModuleImpl::activate() {
    removeAll();
    m_pickableItemInfo.m_item = nullptr;
    m_pickableItemInfo.m_kind = (itKind)-1;
    m_pickableItemInfo.m_trait = (itParam::TraitFlag)0;
    m_pickableItemInfo.m_size = (itParam::SizeKind)2;
    m_pickableItemInfo.m_isVisible = true;
    m_pickableItemInfo.m_isHoldAnim = false;
    m_pickableItemInfo.m_attachGroup = (itParam::AttachGroup)-2;
    m_isSuccessPickupItem = false;
    _0x61[0] = 1;
    _0x61[1] = 1;
    _0x61[2] = 1;
    m_attachItemVisibility = 1;
}

void soItemManageModuleImpl::begin() {
    m_pickableItemInfo.m_item = nullptr;
    m_pickableItemInfo.m_kind = (itKind)-1;
    m_pickableItemInfo.m_trait = (itParam::TraitFlag)0;
    m_pickableItemInfo.m_size = (itParam::SizeKind)2;
    m_pickableItemInfo.m_isVisible = true;
    m_pickableItemInfo.m_isHoldAnim = false;
    m_pickableItemInfo.m_attachGroup = (itParam::AttachGroup)-2;
    m_isSuccessPickupItem = false;
}

void soItemManageModuleImpl::check() {
    m_itemPickTransactor->check(m_itemSearch, &m_pickableItemInfo, m_moduleAccesser);
}

bool soItemManageModuleImpl::pickupItem(u8 a, u32 b, u32 c, u32 index) {
    if ((int)a != 2) {
        if (m_itemPickTransactor->pickup(m_itemSearch, m_moduleAccesser, a, b, c, index) == true) {
            m_isSuccessPickupItem = true;
            return true;
        }
    }
    return false;
}

void soItemManageModuleImpl::getHaveItemInfo(soItemInfo* info, u32 index) const {
    *info = m_haveItemInfoArray->at(index);
}

void soItemManageModuleImpl::getPickableItemInfo(soItemInfo* info) const {
    *info = m_pickableItemInfo;
}

void soItemManageModuleImpl::getAttachItemInfo(soItemInfo* info, u32 index) const {
    *info = m_attachItemInfoArray->at(index);
}

void soItemManageModuleImpl::removeItem(u32 index) {
    ejectHaveItem(index, true, true);
}

void soItemManageModuleImpl::shootItemBulletBlanks(u32 count, u32 index) {
    if (isHaveItem(index)) {
        m_haveItemInfoArray->at(index).m_item->shootBlanks(count);
    }
}

u32 soItemManageModuleImpl::getShootItemBullet(u32 index) {
    if (isHaveItem(index) == false) {
        return 0;
    }
    return m_haveItemInfoArray->at(index).m_item->getBullet();
}

void soItemManageModuleImpl::updateHaveItemActionInfo(u32 index) {
    if (isHaveItem(index)) {
        m_haveItemInfoArray->at(index).m_item->updateLogAttackInfo();
    }
}

void soItemManageModuleImpl::setHaveItemHoldAnim(bool holdAnim, u32 index) {
    if (isHaveItem(index) == true) {
        m_haveItemInfoArray->at(index).m_isHoldAnim = holdAnim;
    }
}

void soItemManageModuleImpl::setHaveItemConstraintNode(u32 node, u32 index) {
    if (isHaveItem(index) == true) {
        m_haveItemInfoArray->at(index).m_item->setConstraintTargetNode(node);
    }
}

void soItemManageModuleImpl::scaleItem(BaseItem* item, float scale) {
    item->setScaleAnimation(scale, scale, item->getParam((itValueAccesser::ParamInt)23500), 1);
}

void soItemManageModuleImpl::setHaveItemTeam(int team, u32 index) {
    if (isHaveItem(index)) {
        m_haveItemInfoArray->at(index).m_item->setTeam(team);
    }
}

void soItemManageModuleImpl::setHaveItemAction(u32 action, float value, u32 index) {
    if (isHaveItem(index)) {
        m_haveItemInfoArray->at(index).m_item->action(action, value);
    }
}

bool soItemManageModuleImpl::haveItem(BaseItem* item, u32 index, bool a, bool b) {
    u32 nodeIndex;
    if (item->getParam((itValueAccesser::ParamInt)23401) == 0) {
        if ((item->getParam((itValueAccesser::ParamInt)23502) & 2) != 0) {
            nodeIndex = m_itemNodeData->m_haveNNodeIndex_;
        } else {
            nodeIndex = m_itemNodeData->m_haveNNodeIndex;
        }
    } else {
        nodeIndex = m_itemNodeData->m_throwNNodeIndex;
    }
    return haveItem(nodeIndex, item, index, a, b);
}

void soItemManageModuleImpl::removeAll() {
    s32 haveCount = m_haveItemInfoArray->size();
    for (s32 i = 0; i < haveCount; i++) {
        ejectHaveItem(i, true, true);
    }
    for (s32 i = m_attachItemInfoArray->size() - 1; i >= 0; i--) {
        ejectAttachItem(i, true, true);
    }
    m_attachItemInfoArray->clear();
}

void soItemManageModuleImpl::eject(int taskId) {
    s32 haveCount = m_haveItemInfoArray->size();
    for (s32 i = 0; i < haveCount; i++) {
        if (m_haveItemInfoArray->at(i).m_item != nullptr) {
            if (taskId == m_haveItemInfoArray->at(i).m_item->m_taskId) {
                ejectHaveItem(i, false, true);
            }
        }
    }
    for (s32 i = m_attachItemInfoArray->size() - 1; i >= 0; i--) {
        if (taskId == m_attachItemInfoArray->at(i).m_item->m_taskId) {
            ejectAttachItem(i, false, true);
        }
    }
}

void soItemManageModuleImpl::ejectAttach(itKind kind, bool a, bool b) {
    for (s32 i = m_attachItemInfoArray->size() - 1; i >= 0; i--) {
        if (kind == m_attachItemInfoArray->at(i).m_kind) {
            ejectAttachItem(i, a, b);
        }
    }
}

void soItemManageModuleImpl::scaleAttach(itKind kind, float scale) {
    for (s32 i = m_attachItemInfoArray->size() - 1; i >= 0; i--) {
        if (kind == m_attachItemInfoArray->at(i).m_kind) {
            scaleItem(m_attachItemInfoArray->at(i).m_item, scale);
        }
    }
}

void soItemManageModuleImpl::resetHaveItemConstraintNode(u32 index) {
    if (isHaveItem(index) == true) {
        int haveKind = m_haveItemInfoArray->at(index).m_item->getParam((itValueAccesser::ParamInt)23401);
        int flags = m_haveItemInfoArray->at(index).m_item->getParam((itValueAccesser::ParamInt)23502);
        u32 nodeIndex;
        if (haveKind == 0) {
            if ((flags & 2) != 0) {
                nodeIndex = m_itemNodeData->m_haveNNodeIndex_;
            } else {
                nodeIndex = m_itemNodeData->m_haveNNodeIndex;
            }
        } else {
            nodeIndex = m_itemNodeData->m_throwNNodeIndex;
        }
        m_haveItemInfoArray->at(index).m_item->setConstraintTargetNode(nodeIndex);
    }
}

void soItemManageModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    setHaveItemVisibility(true, -1);
    setAttachItemVisibility(true, (itParam::AttachGroup)0xFF);
}

bool soItemManageModuleImpl::isObserv(char kind) {
    return kind == 31;
}

bool soItemManageModuleImpl::haveItem(itKind kind, u32 variation, u32 index, bool a) {
    if (isHaveItem(index) == true) {
        return false;
    }
    BaseItem* item = itManager::getInstance()->createItem(kind, variation, m_moduleAccesser->m_stageObject->m_taskId);
    if (item != nullptr) {
        if (haveItem(item, index, a, true) == true) {
            return true;
        }
        removeItem(item);
    }
    return false;
}

bool soItemManageModuleImpl::attachItem(itKind kind, u32 variation, bool a) {
    if (isAttachItem(kind) == true) {
        return false;
    }
    BaseItem* item = itManager::getInstance()->createItem(kind, variation, m_moduleAccesser->m_stageObject->m_taskId);
    if (item != nullptr) {
        if (attachItem(item, a) == true) {
            return true;
        }
        removeItem(item);
    }
    return false;
}

void soItemManageModuleImpl::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    if (unk4 == 3) {
        if (stageObject->soGetKind() == StageObject_Item) {
            switch (eventInfo->m_eventKind) {
            case 60:
                eject(stageObject->m_taskId);
                break;
            }
        }
    }
}

void soItemManageModuleImpl::useItem(u32 index) {
    if (isHaveItem(index)) {
        BaseItem* item = m_haveItemInfoArray->at(index).m_item;
        ejectHaveItem(index, false, false);
        if (useItem(item) == false) {
            if (item->isHaved() == true) {
                item->setVisibilityWhole(1);
                Vec3f velocity(0.0f, 0.0f, 0.0f);
                item->fall(&velocity, m_moduleAccesser->getPostureModule().getLr());
            }
        }
    }
}

void soItemManageModuleImpl::dropAttach(itKind kind, float angle, float speed) {
    BaseItem* item;
    for (s32 i = m_attachItemInfoArray->size() - 1; i >= 0; i--) {
        if (kind == m_attachItemInfoArray->at(i).m_kind) {
            item = m_attachItemInfoArray->at(i).m_item;
            float lr = m_moduleAccesser->getPostureModule().getLr();
            Vec2f dir(speed, 0.0f);
            dir.rot(0.017453292f * angle, &dir);
            ejectAttachItem(i, false, false);
            Vec3f velocity(dir.m_x * lr, dir.m_y, 0.0f);
            item->fall(&velocity, lr);
        }
    }
}
