#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_nabe.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// HYPOTHESIS: the status unique process shared by all statuses of the pot (not yet named).
extern "C" soStatusUniqProcess lbl_96_bss_12EC;

wnKirbyNabe::wnKirbyNabe(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyNabeData* data) :
    wnWeaponBuilder<wnKirbyNabeModuleAccesserBuildConfig>(
        "wnKirbyNabe", articleId, info,
        wnModuleAccesserBuildData(0x91, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    soStatusUniqProcess* procs[6] = { &lbl_96_bss_12EC, &lbl_96_bss_12EC, &lbl_96_bss_12EC, &lbl_96_bss_12EC, &lbl_96_bss_12EC,
                                      &lbl_96_bss_12EC };
    m_moduleAccesser->getStatusModule().addRangeUniqProc(procs, 6);
    Weapon::deactivate(false);
}

void wnKirbyNabe::activate(int founderTaskId, int resourceId, const Vec3f* position, float lr, float unk0, float unk1) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = -1;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (m_moduleAccesser->getGroundModule().attachGround(0) == true) {
        m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    } else {
        m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setFloat(unk0, 0x21000000);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000001);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

void wnKirbyNabe::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* linkedObject, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x83A:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 1) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 2) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 3) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(4, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 4) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(5, m_moduleAccesser);
        }
        break;
    default:
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, linkedObject, unk4);
}
