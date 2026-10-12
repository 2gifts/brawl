#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnPopoRubberStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnPopoRubberStatusUniqProcessCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPopoRubberStatusUniqProcessCommon g_wnPopoRubberStatusUniqProcessCommon;

// HYPOTHESIS: the physics module (soPhysicsModuleImpl, no header yet) is reached through the
// vtable stored at its +0x8 sub-object, as in ft_snake_status_uniq_process_final_end.cpp.
#define POPO_PHYSICS_CALL_VOID(acc, slot, arg) \
    do { \
        void* physics_ = (acc)->m_enumerationStart->m_physicsModule; \
        void** vt_ = *reinterpret_cast<void***>(reinterpret_cast<u8*>(physics_) + 8); \
        reinterpret_cast<void (*)(void*, int)>(vt_[(slot) / 4])(physics_, (arg)); \
    } while (0)

void wnPopoRubberStatusUniqProcessCommon::initStatus(soModuleAccesser* moduleAccesser) {
    POPO_PHYSICS_CALL_VOID(moduleAccesser, 0x4c, 1);
    if (moduleAccesser->getStatusModule().getStatusKind() == 1) {
        POPO_PHYSICS_CALL_VOID(moduleAccesser, 0x64, 0);
        void* physics = moduleAccesser->m_enumerationStart->m_physicsModule;
        Vec3f first(1.0f, 0.0f, 0.0f); // HYPOTHESIS: x from lbl_105_rodata_124 (value guessed), y/z from lbl_105_rodata_120 (0.0f guessed)
        Vec3f second(0.0f, 0.0f, 0.0f); // HYPOTHESIS: constant from lbl_105_rodata_120 (0.0f guessed)
        void** vt = *reinterpret_cast<void***>(reinterpret_cast<u8*>(physics) + 8);
        reinterpret_cast<void (*)(void*, Vec3f*, Vec3f*)>(vt[0x68 / 4])(physics, &second, &first);
    } else if (moduleAccesser->getStatusModule().getStatusKind() == 2) {
        POPO_PHYSICS_CALL_VOID(moduleAccesser, 0x64, 1);
    } else if (moduleAccesser->getStatusModule().getStatusKind() == 3) {
        POPO_PHYSICS_CALL_VOID(moduleAccesser, 0x64, 2);
    }
}

void wnPopoRubberStatusUniqProcessCommon::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 4) {
        void* physics = moduleAccesser->m_enumerationStart->m_physicsModule;
        if (reinterpret_cast<int (*)(void*)>((*reinterpret_cast<void***>(reinterpret_cast<u8*>(physics) + 8))[0x88 / 4])(physics) > 0) {
            POPO_PHYSICS_CALL_VOID(moduleAccesser, 0x90, 1);
        } else {
            // BLOCKED: the native path casts m_stageObject to the article class (dynamic_cast with article typeinfo)
            // and calls its virtual at slot 0x1F8; the article class is not reconstructed here.
            return;
        }
    }
    Vec3f pos = moduleAccesser->getLinkModule().getModelConstraintTargetNodePosition();
    if (moduleAccesser->getLinkModule().isLink(5)) {
        Vec3f node = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(5, 0x27, false);
        void* physics = moduleAccesser->m_enumerationStart->m_physicsModule;
        reinterpret_cast<void (*)(void*, Vec3f*, Vec3f*)>((*reinterpret_cast<void***>(reinterpret_cast<u8*>(physics) + 8))[0x68 / 4])(physics, &pos, &node);
    }
}
