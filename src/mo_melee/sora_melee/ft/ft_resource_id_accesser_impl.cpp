#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_heap_module.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>

// The fighter's resource ids come from its common data (by fighter kind, resource slot and costume); the costume number
// gets bit 7 while the fighter wears the spycloak.
static inline u8 getCostume(soModuleAccesser* acc) {
    ftOwner* owner = g_ftManager->getOwner(acc->getWorkManageModule().getInt(0x10000000));
    u8 color = owner->getFighterColor();
    if (owner->getSpycloak() == true) {
        color |= 0x80;
    }
    return color;
}

u32 ftResourceIdAccesserImpl::getMdlResId() {
    u8 color = getCostume(m_moduleAccsr);
    soHeapModule& heap = m_moduleAccsr->getHeapModule();
    ftKind kind = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
    return g_ftCommonDataAccesser.getModelResId(heap.getSlotNo(), kind, color);
}

// The motion resource of the slot, unless the kind has a combined motion/etc resource.
u32 ftResourceIdAccesserImpl::getAnmResId() {
    soHeapModule& heap = m_moduleAccsr->getHeapModule();
    ftKind kind = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
    u32 id = g_ftCommonDataAccesser.getMotionEtcResId(heap.getSlotNo(), kind);
    if (id == 0xffff) {
        soHeapModule& heap2 = m_moduleAccsr->getHeapModule();
        ftKind kind2 = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
        id = g_ftCommonDataAccesser.getMotionResId(heap2.getSlotNo(), kind2);
    }
    return id;
}

u32 ftResourceIdAccesserImpl::getBinResId() {
    return g_ftCommonDataAccesser.getResId(static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind()));
}

u32 ftResourceIdAccesserImpl::getEtcResId() {
    soHeapModule& heap = m_moduleAccsr->getHeapModule();
    ftKind kind = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
    u32 id = g_ftCommonDataAccesser.getMotionEtcResId(heap.getSlotNo(), kind);
    if (id == 0xffff) {
        soHeapModule& heap2 = m_moduleAccsr->getHeapModule();
        ftKind kind2 = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
        id = g_ftCommonDataAccesser.getEtcResId(heap2.getSlotNo(), kind2);
    }
    return id;
}

u32 ftResourceIdAccesserImpl::getFinalResId() {
    return g_ftCommonDataAccesser.getFinalResId(static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind()));
}

// Only Kirby's accesser has copy-ability resources.
u32 ftResourceIdAccesserImpl::getKirbyResId(int fighterKind) {
    return 0xffff;
}

u32 ftResourceIdAccesserImpl::getEntryResId() {
    u8 color = getCostume(m_moduleAccsr);
    return g_ftCommonDataAccesser.getEntryResId(static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind()), color);
}

u32 ftResourceIdAccesserImpl::getResultResId() {
    u8 color = getCostume(m_moduleAccsr);
    return g_ftCommonDataAccesser.getResultResId(static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind()), color);
}

u32 ftResourceIdAccesserImpl::getMdlArchiveId() {
    return getArchiveId();
}

u32 ftResourceIdAccesserImpl::getTexArchiveId() {
    return getArchiveId();
}

// With a combined motion/etc resource the motions come from archive 1 and the etc data from archive 0.
u32 ftResourceIdAccesserImpl::getAnmArchiveId() {
    soHeapModule& heap = m_moduleAccsr->getHeapModule();
    ftKind kind = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
    if (g_ftCommonDataAccesser.getMotionEtcResId(heap.getSlotNo(), kind) != 0xffff) {
        return 1;
    }
    return getArchiveId();
}

u32 ftResourceIdAccesserImpl::getEtcArchiveId() {
    soHeapModule& heap = m_moduleAccsr->getHeapModule();
    ftKind kind = static_cast<ftKind>(m_moduleAccsr->getStageObject().soGetSubKind());
    if (g_ftCommonDataAccesser.getMotionEtcResId(heap.getSlotNo(), kind) != 0xffff) {
        return 0;
    }
    return getArchiveId();
}

u32 ftResourceIdAccesserImpl::getBinArchiveId() {
    return getArchiveId();
}
