#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/sheik/wn_sheik_iron_wire.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessHad;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessShot;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessShotFall;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessFall;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessTakeup;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnSheikIronWire::wnSheikIronWire(s32 articleId, const wnArticleBuildInfo& info, const wnSheikIronWireData* data) :
    wnWeaponBuilder<wnSheikIronWireModuleAccesserBuildConfig>(
        "SheikIronWire", articleId, info,
        wnModuleAccesserBuildData(0x46, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9];
    uniqProcs[0] = &g_wnSheikIronWireStatusUniqProcessHad;
    uniqProcs[1] = &g_wnSheikIronWireStatusUniqProcessShot;
    uniqProcs[2] = &g_wnSheikIronWireStatusUniqProcessShotFall;
    uniqProcs[3] = &g_wnSheikIronWireStatusUniqProcessFall;
    uniqProcs[4] = &g_wnSheikIronWireStatusUniqProcessTakeup;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

// TODO: clean up, give variables real names
namespace jj_241F4 {

struct ZeldaModuleOwner {
    unsigned char prefix[0x60];
    soModuleAccesser* accesser;
};

extern "C" void __as__5Vec3fFRC5Vec3f(Vec3f* dst, const Vec3f* src);
extern "C" void updateNodeSRT__11StageObjectFv(void* object);

extern "C" void fn_104_241F4(ZeldaModuleOwner* object) {
    soModuleAccesser* accesser = object->accesser;
    if (accesser->getLinkModule().isLink(3) &&
        accesser->getWorkManageModule().isFlag(0x12000003)) {
        float scale = accesser->getPostureModule().getScale();
        Vec3f nodeScale;
        nodeScale.m_x = nodeScale.m_y = nodeScale.m_z = scale;
        Vec3f rotation;
        Vec3f position;
        u32 zero = 0;
        Vec3f rawPosition = accesser->getLinkModule().getParentModelNodeGlobalPosition(3, zero, false);
        __as__5Vec3fFRC5Vec3f(&position, &rawPosition);
        Vec3f rawRotation = accesser->getLinkModule().getParentRot(3);
        __as__5Vec3fFRC5Vec3f(&rotation, &rawRotation);
        rotation.m_y += accesser->getPostureModule().getRotYLr();
        accesser->getModelModule().setNodeSRT(0, &nodeScale, &rotation, &position);
    } else {
        updateNodeSRT__11StageObjectFv(object);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24474 {
extern "C" void getHeapSlotNo__25soModuleAccesserBuildDataCFv(void*);

extern "C" void fn_104_24474(void* self) {
    getHeapSlotNo__25soModuleAccesserBuildDataCFv(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_2448C {
extern "C" void __dt__15wnSheikIronWireFv(void*);

extern "C" void fn_104_2448C(void* self) {
    __dt__15wnSheikIronWireFv(reinterpret_cast<u8*>(self) - 0x88);
}
}

