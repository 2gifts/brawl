#include <it/item.h>
#include <it/it_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// itResourceIdAccesserImpl members from the item resource module unit (sora_melee 0x2BCDD0); the rest of that unit
// (itResourceModuleImpl) is not decompiled yet.

u32 itResourceIdAccesserImpl::getArchiveId() {
    return m_archiveId;
}

itKind itResourceIdAccesserImpl::getItKind() {
    return dynamic_cast<BaseItem*>(&m_moduleAccsr->getStageObject())->m_kind;
}
