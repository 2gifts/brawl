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

#include <so/event/so_event_system.h>
#include <so/item/so_item_manage_event_presenter.h>
#include <so/item/so_item_pick_transactor_impl.h>
// TODO: clean up, give variables real names
namespace jj_B8184 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_B8184(void* p, s32 n) {
    if (p != nullptr && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_B874C {
extern "C" void* getInstance__9itManagerFv();
extern "C" void* getItemFromInstanceId__9itManagerFi(void*, int);

extern "C" void* fn_27_B874C(int id) {
    return getItemFromInstanceId__9itManagerFi(getInstance__9itManagerFv(), id);
}
}

// TODO: clean up, give variables real names
namespace jj_B8B44 {


namespace matching {

class soNullable {
public:
    virtual void isNull();
    virtual void __dt__22soItemManageModuleImplFv();
    virtual void activate__22soItemManageModuleImplFv();
    virtual void deactivate__22soItemManageModuleImplFv();
    virtual void begin__22soItemManageModuleImplFv();
    virtual void check__22soItemManageModuleImplFv();
    virtual void haveItem__22soItemManageModuleImplF6itKindUlUlb();
    virtual void fn_27_B88AC();
    virtual void haveItem__22soItemManageModuleImplFP8BaseItemUlbb();
    virtual void fn_27_B8B44();
    virtual void fn_27_BC19C();
    virtual void fn_27_B91FC();
    virtual void pickupItem__22soItemManageModuleImplFUcUlUlUl();
    virtual void isSuccessPickupItem__22soItemManageModuleImplFv();
    virtual unsigned int isHaveItem__22soItemManageModuleImplCFUl(int);

    char pad[0x2c];
    short a;
    short b;
    soInstanceManagerFullProperty<soItemManageEventObserver*>* cache;
    soArray<soItemInfo>* items;
    soItemNodeData* nodes;
    char pad40[0x21];
    bool flag;
    char pad62[6];
    soModuleAccesser* access;
};

struct ItemInfoLocal {
    itKind m_kind;
    BaseItem* m_item;
    itParam::TraitFlag m_trait;
    itParam::SizeKind m_size;
    bool m_isVisible : 1;
    bool m_isHoldAnim : 1;
    unsigned char m_attachGroup : 6;
    char _0x11[3];
};

}

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
};

extern "C" {
extern char lbl_27_bss_CC0[];
extern char lbl_27_data_175C8[];
extern char __RTTI__11soEventUnit[];

void* __dynamic_cast(void*, long, const void*, const void*, short);
int isSteal__8BaseItemFv(BaseItem*);
}

typedef soInstanceManagerFullProperty<soItemManageEventObserver*> ObserverList;

inline bool checkManageId(const matching::soNullable* self) {
    short id = self->a;
    return soEventSystem::getInstance()->getInstanceManager().isContain(id);
}

inline ObserverList* selectList(matching::soNullable* self) {
    short id1 = self->a;
    short id2;
    soEventManager* mgr;
    ObserverList* cur;
    soEventUnitWrapper<soItemManageEventObserver>* unit;
    int flag;

    if (id1 <= -1)
        return (ObserverList*)lbl_27_bss_CC0;

    if (self->cache != 0)
        goto CHECK_CACHE;

    if (id1 <= -1 || !checkManageId(self)) {
        flag = 0;
        goto CHECK;
    }

    if (soEventSystem::getInstance()
            ->getManager(self->a)
            ->getObserverCapacity(self->b) == 0) {
        self->cache = 0;
        flag = 0;
        goto CHECK;
    }

    id2 = self->b;
    mgr = soEventSystem::getInstance()->getManager(self->a);
    if (mgr->getObserverCapacity(id2) == 0) {
        cur = 0;
    } else {
        unit = (soEventUnitWrapper<soItemManageEventObserver>*)__dynamic_cast(
            mgr->getEventUnit(id2), 0, lbl_27_data_175C8,
            __RTTI__11soEventUnit, 0);
        if (unit == 0)
            cur = 0;
        else
            cur = unit->getObserverListSub();
    }

    self->cache = cur;
    flag = (cur != 0);

CHECK:
    if (flag == 0)
        return (ObserverList*)lbl_27_bss_CC0;

CHECK_CACHE:
    mgr = soEventSystem::getInstance()->getManager(self->a);
    if (mgr->getObserverCapacity(self->b) == 0) {
        self->cache = 0;
        return (ObserverList*)lbl_27_bss_CC0;
    }
    return self->cache;
}

inline void setInfo(
    matching::soNullable* self, int slot, BaseItem* item, int kind,
    int param, itParam::SizeKind size, int val)
{
    bool f = self->flag;
    matching::ItemInfoLocal& info = (matching::ItemInfoLocal&)self->items->at(slot);
    info.m_item = item;
    info.m_kind = (itKind)kind;
    info.m_trait.m_mask = param;
    info.m_size = size;
    info.m_isVisible = f;
    info.m_isHoldAnim = 0;
    info.m_attachGroup = val;
}

extern "C" int fn_27_B8B44(
    matching::soNullable* self, int node, BaseItem* item,
    int index, bool arg4, unsigned int notify)
{
    int param = item->getParam((itValueAccesser::ParamInt)0x5bce);
    int kind = item->soGetSubKind();
    int slot = index;
    int n;

    if (item->isHaved() == 1 && !isSteal__8BaseItemFv(item))
        return 0;

    if (notify == 1) {
        ObserverList* list = selectList(self);
        n = list->size();
        for (int i = 0; i < n; i++) {
            if (!list->atIndex(i)->notifyHaveItemPreCheck(item, (bool*)&slot))
                return 0;
        }
    }

    if (self->isHaveItem__22soItemManageModuleImplCFUl(slot) == 1)
        return 0;

    itParam::SizeKind size;
    switch (item->getParam((itValueAccesser::ParamInt)0x5b69)) {
    case 0:
        n = 0;
        break;
    case 1:
        n = 1;
        break;
    default:
        return 0;
    }

    item->have(
        *(int*)((char*)*(void**)((char*)self->access + 8) + 0x28),
        node, -1);

    if (!item->isHaved())
        return 0;

    item->setVisibilityWhole(self->flag);

    int val = 0xfe;
    if (kind == 0xc || kind == 0x38)
        val = 2;

    setInfo(self, slot, item, kind, param, (itParam::SizeKind)n, val);

    if (notify == 1) {
        ObserverList* list = selectList(self);
        int count = list->size();
        for (int i = 0; i < count; i++) {
            itParam::SizeKind sz = self->items->at(index).m_size;
            list->atIndex(i)->notifyHaveItem(
                sz, item, node, slot, arg4);
        }
    }

    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_B90F4 {
extern "C" u32 fn_27_B90F4(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x8c0);
}
}

// TODO: clean up, give variables real names
namespace jj_B90FC {
extern "C" s32 fn_27_B90FC(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_B9104 {
extern "C" void fn_27_B9104(void) {}
}

// TODO: clean up, give variables real names
namespace jj_B97F8 {
extern "C" void fn_27_B97F8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_B9D1C {
extern "C" void fn_27_B9D1C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_BA510 {
extern "C" void fn_27_BA510(void) {}
}

// TODO: clean up, give variables real names
namespace jj_BA9B4 {
extern "C" void fn_27_BA9B4(void) {}
}

// TODO: clean up, give variables real names
namespace jj_BAB24 {
extern "C" void OSReport(const char* format, ...);
extern "C" void setScaleAnimation__8BaseItemFffii(void* self, int x, int y, float a, float b);
extern "C" int getParam__8BaseItemFQ215itValueAccesser8ParamInt(void* self, int id);

extern const char lbl_27_data_17528[];
extern const float lbl_27_rodata_E68;

struct Result {
    s32 kind;
    void* item;
};

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual Result* slot1(int);
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual int slot14(int);
};

class Owner {
public:
    virtual void slot0();
    virtual Result* slot1(int);
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual int slot14(int);
    char pad[0x34];
    Object_vptr_0* child;
};

extern "C" void fn_27_BAB24(Owner* self, int arg1, int arg2, float farg0)
{
    if (self->slot14(arg2) != 0) {
        int param;
        switch (self->child->slot1(arg2)->kind) {
        case 4:
            param = getParam__8BaseItemFQ215itValueAccesser8ParamInt(self->child->slot1(arg2)->item, 0x5E88);
            break;
        default:
            OSReport(lbl_27_data_17528);
            return;
        }
        setScaleAnimation__8BaseItemFffii(self->child->slot1(arg2)->item, param, arg1, farg0, lbl_27_rodata_E68);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_BB03C {
extern "C" void fn_27_BB03C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_BB398 {

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8(BaseItem*, int, int);
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
};
struct ItemFlags {
    unsigned char f7 : 1;
    unsigned char f6 : 1;
    unsigned char f0 : 6;
};
namespace matching {
class soNullable {
public:
    virtual void isNull();
    virtual void __dt__22soItemManageModuleImplFv();
    virtual void activate__22soItemManageModuleImplFv();
    virtual void deactivate__22soItemManageModuleImplFv();
    virtual void begin__22soItemManageModuleImplFv();
    virtual void check__22soItemManageModuleImplFv();
    virtual void haveItem__22soItemManageModuleImplF6itKindUlUlb();
    virtual void fn_27_B88AC();
    virtual void haveItem__22soItemManageModuleImplFP8BaseItemUlbb();
    virtual void fn_27_B8B44();
    virtual void fn_27_BC19C();
    virtual void fn_27_B91FC();
    virtual void pickupItem__22soItemManageModuleImplFUcUlUlUl();
    virtual void isSuccessPickupItem__22soItemManageModuleImplFv();
    virtual int isHaveItem__22soItemManageModuleImplCFUl(int);
    virtual void getHaveItemSize__22soItemManageModuleImplCFUl();
    virtual void getHaveItemKind__22soItemManageModuleImplCFUl();
    virtual void getHaveItemTrait__22soItemManageModuleImplCFUl();
    virtual void getHaveItemHoldKind__22soItemManageModuleImplFUl();
    virtual void getHaveItemParamInt__22soItemManageModuleImplCFUlUl();
    virtual void getHaveItemParamFloat__22soItemManageModuleImplCFUlUl();
    virtual void getHaveItemInfo__22soItemManageModuleImplCFP10soItemInfoUl();
    virtual void getPickableItemSize__22soItemManageModuleImplFv();
    virtual void getPickableItemKind__22soItemManageModuleImplFv();
    virtual void getPickableItemTrait__22soItemManageModuleImplFv();
    virtual void getPickableItemInfo__22soItemManageModuleImplCFP10soItemInfo();
    virtual void removeItem__22soItemManageModuleImplFP8BaseItem(BaseItem*);
    char pad[0x2c];
    short a;
    short b;
    soInstanceManagerFullProperty<soItemManageEventObserver*>* cache;
    soArray<soItemInfo>* items;
};
}
extern "C" {
extern char lbl_27_bss_CC0[];
extern char lbl_27_data_175C8[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

typedef soInstanceManagerFullProperty<soItemManageEventObserver*> ObserverList;
inline short getManageId(const matching::soNullable* self) { return self->a; }
inline bool checkManageId(const matching::soNullable* self) {
    const int id = getManageId(self);
    return soEventSystem::getInstance()->getInstanceManager().isContain(id);
}
inline ObserverList* selectList(matching::soNullable* self) {
    short id1 = self->a;
    short id2;
    soEventManager* mgr;
    ObserverList* cur;
    soEventUnitWrapper<soItemManageEventObserver>* unit;
    int flag;
    if (id1 <= -1) return (ObserverList*)lbl_27_bss_CC0;
    if (self->cache != 0) goto C;
    if (id1 <= -1 || checkManageId(self) == 0) {
        flag = 0;
        goto CHECK;
    }
    if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 0) {
        self->cache = 0;
        flag = 0;
        goto CHECK;
    }
    id2 = self->b;
    mgr = soEventSystem::getInstance()->getManager(self->a);
    if (mgr->getObserverCapacity(id2) == 0) cur = 0;
    else {
        unit = (soEventUnitWrapper<soItemManageEventObserver>*)__dynamic_cast(mgr->getEventUnit(id2), 0, lbl_27_data_175C8, __RTTI__11soEventUnit, 0);
        if (unit == 0) cur = 0;
        else cur = unit->getObserverListSub();
    }
    self->cache = cur;
    flag = (cur != 0);
CHECK:
    if (flag == 0) return (ObserverList*)lbl_27_bss_CC0;
C:
    mgr = soEventSystem::getInstance()->getManager(self->a);
    if (mgr->getObserverCapacity(self->b) == 0) {
        self->cache = 0;
        return (ObserverList*)lbl_27_bss_CC0;
    }
    return self->cache;
}

extern "C" void fn_27_BB398(matching::soNullable* self, int a, unsigned int b, int c) {
    if (self->isHaveItem__22soItemManageModuleImplCFUl(a) == 0) return;
    BaseItem* item = self->items->at(a).m_item;
    ObserverList* sel = selectList(self);
    int n = sel->size();
    for (int i = 0; i < n; i++) {
        ((Object_vptr_0*)sel->atIndex(i))->slot8(item, a, c);
    }
    soItemInfo* ent = &self->items->at(a);
    ent->m_item = 0;
    ent->m_kind = (itKind)-1;
    ent->m_trait = (itParam::TraitFlag)0;
    ent->m_size = (itParam::SizeKind)2;
    ItemFlags* flags = (ItemFlags*)((char*)ent + 0x10);
    flags->f7 = 1;
    flags->f6 = 0;
    flags->f0 = 0xFE;
    if (b == 1U) self->removeItem__22soItemManageModuleImplFP8BaseItem(item);
}
}

// TODO: clean up, give variables real names
namespace jj_BB654 {
extern "C" void fn_27_BB654(void) {}
}

// TODO: clean up, give variables real names
namespace jj_BB658 {
#define D(n) virtual void d##n();
struct Msg { u32 id; u8 a; u8 b; };
struct Elem;
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual Elem* slot1(int);
    virtual void slot2();
    virtual int slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16(int, void*, int);
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
};
struct Obj4 {
    char pad[0x3c];
    D(0) D(1) D(2) D(3) D(4) D(5) D(6) D(7) D(8) D(9) D(10) D(11) D(12) D(13) D(14) D(15) D(16) D(17) D(18) D(19) D(20) D(21) D(22) D(23) D(24) D(25) D(26) D(27) D(28) D(29) D(30) D(31) D(32) D(33) D(34) D(35) D(36) D(37) D(38) D(39) D(40) D(41) D(42) D(43) D(44) D(45) D(46) D(47) D(48) D(49) D(50) D(51) D(52) D(53) D(54) D(55) D(56) D(57) D(58) D(59) D(60) D(61) D(62) D(63) D(64) D(65) D(66) D(67) D(68) D(69) D(70) D(71) D(72) D(73) D(74) D(75) D(76) D(77) D(78) D(79) D(80) D(81) D(82) D(83) D(84) D(85) D(86) D(87) D(88) D(89) D(90) D(91) D(92) D(93) D(94) D(95) D(96) D(97) D(98) D(99) D(100) D(101) D(102) D(103) D(104) D(105) D(106) D(107) D(108) D(109) D(110) D(111) D(112) D(113) D(114) D(115) D(116) D(117) D(118) D(119) D(120) D(121) D(122) D(123) D(124) D(125) D(126) D(127) D(128) D(129) D(130) D(131) D(132) D(133) D(134)
    virtual void change(u8);
};
struct Elem {
    char pad0[4]; Obj4* obj; char pad8[8];
    u8 bit7 : 1; u8 bit6 : 1; u8 low : 6;
    bool isEnabled() const { return bit7; }
    bool isIndex(u32 index) const { return index == low; }
};
struct Mid_d8 { char pad[0x54]; Object_vptr_0* unk54; };
struct Mid68 { char pad[0xd8]; Mid_d8* unkD8; };
class soNullable {
public:
    virtual void isNull();
    virtual void __dt__22soItemManageModuleImplFv();
    virtual void activate__22soItemManageModuleImplFv();
    virtual void deactivate__22soItemManageModuleImplFv();
    virtual void begin__22soItemManageModuleImplFv();
    virtual void check__22soItemManageModuleImplFv();
    virtual void haveItem__22soItemManageModuleImplF6itKindUlUlb();
    virtual void fn_27_B88AC();
    virtual void haveItem__22soItemManageModuleImplFP8BaseItemUlbb();
    virtual void fn_27_B8B44();
    virtual void fn_27_BC19C();
    virtual void fn_27_B91FC();
    virtual void pickupItem__22soItemManageModuleImplFUcUlUlUl();
    virtual void isSuccessPickupItem__22soItemManageModuleImplFv();
    virtual void isHaveItem__22soItemManageModuleImplCFUl();
    virtual void getHaveItemSize__22soItemManageModuleImplCFUl();
    virtual void getHaveItemKind__22soItemManageModuleImplCFUl();
    virtual void getHaveItemTrait__22soItemManageModuleImplCFUl();
    virtual void getHaveItemHoldKind__22soItemManageModuleImplFUl();
    virtual void getHaveItemParamInt__22soItemManageModuleImplCFUlUl();
    virtual void getHaveItemParamFloat__22soItemManageModuleImplCFUlUl();
    virtual void getHaveItemInfo__22soItemManageModuleImplCFP10soItemInfoUl();
    virtual void getPickableItemSize__22soItemManageModuleImplFv();
    virtual void getPickableItemKind__22soItemManageModuleImplFv();
    virtual void getPickableItemTrait__22soItemManageModuleImplFv();
    virtual void getPickableItemInfo__22soItemManageModuleImplCFP10soItemInfo();
    virtual void removeItem__22soItemManageModuleImplFP8BaseItem();
    virtual void removeItem__22soItemManageModuleImplFUl();
    virtual void removeAll__22soItemManageModuleImplFv();
    virtual void fn_27_B9918();
    virtual void fn_27_B9D20();
    virtual void fn_27_BA1C8();
    virtual void useItem__22soItemManageModuleImplFUl();
    virtual void warpItem__22soItemManageModuleImplFP5Vec3fUl();
    virtual void bornItem__22soItemManageModuleImplFUl();
    virtual void fn_27_BA714();
    virtual void shootItemBulletBlanks__22soItemManageModuleImplFUlUl();
    virtual void getShootItemBullet__22soItemManageModuleImplFUl();
    virtual void updateHaveItemActionInfo__22soItemManageModuleImplFUl();
    virtual void fn_27_BAB24();
    virtual void setHaveItemAction__22soItemManageModuleImplFUlfUl();
    virtual void fn_27_BACD4();
    virtual void setHaveItemHoldAnim__22soItemManageModuleImplFbUl();
    virtual void setHaveItemConstraintNode__22soItemManageModuleImplFUlUl();
    virtual void resetHaveItemConstraintNode__22soItemManageModuleImplFUl();
    virtual void attachItem__22soItemManageModuleImplF6itKindUlb();
    virtual void isAttachItem__22soItemManageModuleImplF6itKind();
    virtual void getAttachItemCount__22soItemManageModuleImplFv();
    virtual void getAttachItemInfo__22soItemManageModuleImplCFP10soItemInfoUl();
    virtual void fn_27_BB658(u8, u8);
    char pad4[0x3c]; Object_vptr_0* unk40;
    char pad44[0x1e]; u8 flag62; char pad63[5]; Mid68* unk68;
};
extern "C" void fn_27_BB658(soNullable* self, u8 arg1, u32 arg2) {
    u32 i;
    if (arg2 == 0xFF) {
        for (i = 0; i < 3U; i++) self->fn_27_BB658(arg1, (u8)i);
        return;
    }
    i = (u32)self + arg2;
    if (arg1 != *(u8*)(i + 0x62)) {
        if (arg2 == 2) {
            Msg msg;
            msg.b = arg1;
            msg.id = 0x37;
            msg.a = 0;
            self->unk68->unkD8->unk54->slot16(-1, &msg, 0);
        } else {
            int count = self->unk40->slot3();
            for (int i = 0; i < count; i++) {
                Elem* e = self->unk40->slot1(i);
                if (e->isIndex(arg2) && arg1 != e->isEnabled()) {
                    e->bit7 = arg1;
                    e->obj->change(arg1);
                }
            }
        }
        *(u8*)(i + 0x62) = arg1;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_BB7B4 {

class Observer {
public:
    virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3();
    virtual void a4(); virtual void a5(); virtual void a6(); virtual void a7();
    virtual void a8(); virtual void notify(void*, int, int);
};
class List {
public:
    virtual void a0(); virtual void a1(); virtual Observer** at(int);
    virtual void a3(); virtual int size();
};
class Unit {
public:
    virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3();
    virtual void a4(); virtual void a5(); virtual void a6(); virtual List* list();
};
struct Data { void* pad; void* value; };
class Module {
public:
    virtual void a0(); virtual Data* getData(); virtual void a2(); virtual void a3();
    virtual void a4(); virtual void a5(); virtual void a6(); virtual void a7();
    virtual void a8(); virtual void a9(); virtual void a10(); virtual void a11();
    virtual void action(int);
};
class Self {
public:
    virtual void isNull(); virtual void __dt__22soItemManageModuleImplFv(); virtual void activate__22soItemManageModuleImplFv();
    virtual void deactivate__22soItemManageModuleImplFv(); virtual void begin__22soItemManageModuleImplFv(); virtual void check__22soItemManageModuleImplFv();
    virtual void haveItem__22soItemManageModuleImplF6itKindUlUlb(); virtual void fn_27_B88AC(); virtual void haveItem__22soItemManageModuleImplFP8BaseItemUlbb();
    virtual void fn_27_B8B44(); virtual void fn_27_BC19C(); virtual void fn_27_B91FC();
    virtual void pickupItem__22soItemManageModuleImplFUcUlUlUl(); virtual void isSuccessPickupItem__22soItemManageModuleImplFv(); virtual void isHaveItem__22soItemManageModuleImplCFUl();
    virtual void getHaveItemSize__22soItemManageModuleImplCFUl(); virtual void getHaveItemKind__22soItemManageModuleImplCFUl(); virtual void getHaveItemTrait__22soItemManageModuleImplCFUl();
    virtual void getHaveItemHoldKind__22soItemManageModuleImplFUl(); virtual void getHaveItemParamInt__22soItemManageModuleImplCFUlUl(); virtual void getHaveItemParamFloat__22soItemManageModuleImplCFUlUl();
    virtual void getHaveItemInfo__22soItemManageModuleImplCFP10soItemInfoUl(); virtual void getPickableItemSize__22soItemManageModuleImplFv(); virtual void getPickableItemKind__22soItemManageModuleImplFv();
    virtual void getPickableItemTrait__22soItemManageModuleImplFv(); virtual void getPickableItemInfo__22soItemManageModuleImplCFP10soItemInfo(); virtual void removeItem__22soItemManageModuleImplFP8BaseItem(void*);
    char pad[0x2c]; short a; short b; List* cache; char pad38[8]; Module* module;
};
extern "C" {
extern char lbl_27_bss_CC0[], lbl_27_data_175C8[], __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}
inline bool checkManageId(const Self* self) { short id = self->a; return soEventSystem::getInstance()->getInstanceManager().isContain(id); }
inline List* selectList(Self* self) {
    short id1, id2;
    soEventManager* mgr;
    List* cur;
    Unit* unit;
    int flag;
    id1 = self->a;
    if (id1 <= -1) return (List*)lbl_27_bss_CC0;
    if (self->cache != 0) goto C;
    if (id1 <= -1 || checkManageId(self) == 0) {
        flag = 0; goto CHECK;
    }
    if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 0) {
        self->cache = 0; flag = 0; goto CHECK;
    }
    id2 = self->b;
    mgr = soEventSystem::getInstance()->getManager(self->a);
    if (mgr->getObserverCapacity(id2) == 0) cur = 0;
    else {
        unit = (Unit*)__dynamic_cast(mgr->getEventUnit(id2), 0, lbl_27_data_175C8, __RTTI__11soEventUnit, 0);
        if (unit == 0) cur = 0;
        else cur = unit->list();
    }
    self->cache = cur;
    flag = (cur != 0);
CHECK:
    if (flag == 0) return (List*)lbl_27_bss_CC0;
C:
    mgr = soEventSystem::getInstance()->getManager(self->a);
    if (mgr->getObserverCapacity(self->b) == 0) {
        self->cache = 0; return (List*)lbl_27_bss_CC0;
    }
    return self->cache;
}
extern "C" void fn_27_BB7B4(Self* self, int arg, unsigned mode, int last) {
    void* value = self->module->getData()->value;
    List* list = selectList(self);
    int n = list->size();
    for (int i = 0; i < n; i++) {
        (*list->at(i))->notify(value, arg, last);
    }
    self->module->action(arg);
    if (mode == 1) self->removeItem__22soItemManageModuleImplFP8BaseItem(value);
}
}

// TODO: clean up, give variables real names
namespace jj_BBA20 {
extern "C" void fn_27_BBA20(void) {}
}

// TODO: clean up, give variables real names
namespace jj_BBC60 {
extern "C" {
extern const float lbl_27_rodata_E60[];
void rot__5Vec2fCFfP5Vec2f(void*, float, void*);
void fall__8BaseItemFP5Vec3ff(void*, void*, float);
}
struct Entry { char pad[4]; void* item; char pad2[8]; unsigned char flags; };
class List { public: virtual void a(); virtual Entry* get(int); virtual void c(); virtual int count(); };
class Posture { public: virtual void a(); virtual void b(); virtual void c(); virtual void d(); virtual void e(); virtual void f(); virtual void g(); virtual void h(); virtual void i(); virtual float lr(); };
struct Param { int kind; unsigned char flag; float value; };
class Work { public: virtual void a(); virtual void b(); virtual void c(); virtual void d(); virtual void e(); virtual void f(); virtual void g(); virtual void h(); virtual void i(); virtual void j(); virtual void k(); virtual void l(); virtual void m(); virtual void n(); virtual void o(); virtual void p(); virtual void set(int, Param*, int); };
struct Modules { char pad[0xc]; Posture* posture; char pad2[0x44]; Work* work; };
struct Access { char pad[0xd8]; Modules* modules; };
class soNullable { public:
virtual void isNull();
virtual void __dt__22soItemManageModuleImplFv();
virtual void activate__22soItemManageModuleImplFv();
virtual void deactivate__22soItemManageModuleImplFv();
virtual void begin__22soItemManageModuleImplFv();
virtual void check__22soItemManageModuleImplFv();
virtual void haveItem__22soItemManageModuleImplF6itKindUlUlb();
virtual void fn_27_B88AC();
virtual void haveItem__22soItemManageModuleImplFP8BaseItemUlbb();
virtual void fn_27_B8B44();
virtual void fn_27_BC19C();
virtual void fn_27_B91FC();
virtual void pickupItem__22soItemManageModuleImplFUcUlUlUl();
virtual void isSuccessPickupItem__22soItemManageModuleImplFv();
virtual unsigned isHaveItem__22soItemManageModuleImplCFUl(int);
virtual void getHaveItemSize__22soItemManageModuleImplCFUl();
virtual void getHaveItemKind__22soItemManageModuleImplCFUl();
virtual void getHaveItemTrait__22soItemManageModuleImplCFUl();
virtual void getHaveItemHoldKind__22soItemManageModuleImplFUl();
virtual void getHaveItemParamInt__22soItemManageModuleImplCFUlUl();
virtual void getHaveItemParamFloat__22soItemManageModuleImplCFUlUl();
virtual void getHaveItemInfo__22soItemManageModuleImplCFP10soItemInfoUl();
virtual void getPickableItemSize__22soItemManageModuleImplFv();
virtual void getPickableItemKind__22soItemManageModuleImplFv();
virtual void getPickableItemTrait__22soItemManageModuleImplFv();
virtual void getPickableItemInfo__22soItemManageModuleImplCFP10soItemInfo();
virtual void removeItem__22soItemManageModuleImplFP8BaseItem();
virtual void removeItem__22soItemManageModuleImplFUl();
virtual void removeAll__22soItemManageModuleImplFv();
virtual void fn_27_B9918();
virtual void fn_27_B9D20();
virtual void fn_27_BA1C8(int, float, float);
virtual void useItem__22soItemManageModuleImplFUl();
virtual void warpItem__22soItemManageModuleImplFP5Vec3fUl();
virtual void bornItem__22soItemManageModuleImplFUl();
virtual void fn_27_BA714();
virtual void shootItemBulletBlanks__22soItemManageModuleImplFUlUl();
virtual void getShootItemBullet__22soItemManageModuleImplFUl();
virtual void updateHaveItemActionInfo__22soItemManageModuleImplFUl();
virtual void fn_27_BAB24();
virtual void setHaveItemAction__22soItemManageModuleImplFUlfUl();
virtual void fn_27_BACD4();
virtual void setHaveItemHoldAnim__22soItemManageModuleImplFbUl();
virtual void setHaveItemConstraintNode__22soItemManageModuleImplFUlUl();
virtual void resetHaveItemConstraintNode__22soItemManageModuleImplFUl();
virtual void attachItem__22soItemManageModuleImplF6itKindUlb();
virtual void isAttachItem__22soItemManageModuleImplF6itKind();
virtual void getAttachItemCount__22soItemManageModuleImplFv();
virtual void getAttachItemInfo__22soItemManageModuleImplCFP10soItemInfoUl();
virtual void fn_27_BB658();
virtual void getAttachItemVisibility__22soItemManageModuleImplFv();
virtual void fn_27_BB398();
virtual void fn_27_BB7B4(int, int, int);
virtual void eject__22soItemManageModuleImplFi();
virtual void ejectAttach__22soItemManageModuleImplF6itKindbb();
virtual void dropAttach__22soItemManageModuleImplF6itKindff();
virtual void fn_27_BBC60(unsigned char, float, float);
 char pad[0x34]; List* list38; char pad2[4]; List* list40; char pad3[0x24]; Access* access;
};
struct V2 { float x,y; };
struct V3 { float x,y,z; };
extern "C" void fn_27_BBC60(soNullable* self, unsigned kind, float angle, float speed) {
 const float* values = lbl_27_rodata_E60;
 if (kind == 255) {
   for (unsigned i=0; i<3; i++) self->fn_27_BBC60((unsigned char)i, angle, speed);
 } else if (kind == 2) {
   float lr = self->access->modules->posture->lr();
   Param p; p.kind=0x38; p.flag=0; p.value=lr;
   self->access->modules->work->set(-1, &p, 0);
   if (self->isHaveItem__22soItemManageModuleImplCFUl(0)==1 && (unsigned)(self->list38->get(0)->flags & 63)==2)
      self->fn_27_BA1C8(0, values[3], values[0]);
 } else {
   int i=self->list40->count()-1;
   for (; i>=0; --i) {
     if ((self->list40->get(i)->flags & 63)==kind) {
       void* item=self->list40->get(i)->item;
       float lr=self->access->modules->posture->lr();
       V2 v; v.x=speed; v.y=values[0];
       rot__5Vec2fCFfP5Vec2f(&v, values[1]*angle, &v);
       self->fn_27_BB7B4(i,0,0);
       V3 out; out.y=v.y; out.x=v.x*lr; out.z=values[0];
       fall__8BaseItemFP5Vec3ff(item,&out,lr);
     }
   }
 }
}
}

// TODO: clean up, give variables real names
namespace jj_BC3F0 {
extern "C" s32 fn_27_BC3F0(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_BE528 {
extern "C" void notifyEventLink__22soItemManageModuleImplFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(void*);

extern "C" void fn_27_BE528(void* self) {
    notifyEventLink__22soItemManageModuleImplFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_BE530 {
extern "C" void notifyEventChangeStatus__22soItemManageModuleImplFiiP12soStatusDataP16soModuleAccesser(void*);

extern "C" void fn_27_BE530(void* self) {
    notifyEventChangeStatus__22soItemManageModuleImplFiiP12soStatusDataP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x14);
}
}

// TODO: clean up, give variables real names
namespace jj_BE538 {
extern "C" void fn_27_BC4E0(void*);

extern "C" void fn_27_BE538(void* self) {
    fn_27_BC4E0(reinterpret_cast<u8*>(self) - 0x20);
}
}

// TODO: clean up, give variables real names
namespace jj_BE540 {
extern "C" void isObserv__22soItemManageModuleImplFc(void*);

extern "C" void fn_27_BE540(void* self) {
    isObserv__22soItemManageModuleImplFc(reinterpret_cast<u8*>(self) - 0x20);
}
}

// TODO: clean up, give variables real names
namespace jj_BE548 {
extern "C" void __dt__22soItemManageModuleImplFv(void*);

extern "C" void fn_27_BE548(void* self) {
    __dt__22soItemManageModuleImplFv(reinterpret_cast<u8*>(self) - 0x2c);
}
}

