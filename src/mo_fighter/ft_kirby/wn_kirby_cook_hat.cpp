#define WN_STATUS_NO_TAIL
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_cook_hat.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/work/so_work_manage_module_impl.h>

wnKirbyCookHat::wnKirbyCookHat(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyCookHatData* data) :
    wnWeaponBuilder<wnKirbyCookHatModuleAccesserBuildConfig>(
        "KirbyCookHat", articleId, info,
        wnModuleAccesserBuildData(0x92, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    // HYPOTHESIS: the four statuses of the hat share the default (none) uniq process.
    soStatusUniqProcess* processes[4] = {0};
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 4);
    Weapon::deactivate(false);
}

void wnKirbyCookHat::activate(int founderTaskId, int resourceId, float lr, bool unk) {
    Vec3f position;
    position.m_x = 0.0f;
    position.m_y = 0.0f;
    position.m_z = 0.0f;
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        position, lr, -1, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (unk) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
    } else {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000002);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

void wnKirbyCookHat::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser,
                                   StageObject* linkedObject, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x83A:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 1) {
            moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        } else {
            moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        }
        break;
    case 0x838:
        moduleAccesser->getWorkManageModule().offFlag(0x22000002);
        moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        break;
    case 0x839:
        moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, linkedObject, unk4);
}
