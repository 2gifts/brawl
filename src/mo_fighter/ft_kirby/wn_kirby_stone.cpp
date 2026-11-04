#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_stone.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ft/ft_common_data_accesser.h>
#include <revolution/OS/OSError.h>

extern char lbl_96_data_DA3C[8];

wnKirbyStone::wnKirbyStone(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyStoneData* data) :
    wnWeaponBuilder<wnKirbyStoneModuleAccesserBuildConfig>(
        "wnKirbyStone", articleId, info,
        wnModuleAccesserBuildData(0x83, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnKirbyStone::activate(int founderTaskId, int resourceId, int team, int kind) {
    if (kind < 0 || kind >= 5) {
        OSReport(lbl_96_data_DA3C, kind);
    }
    const wnKirbyStoneData* data = static_cast<const wnKirbyStoneData*>(g_ftCommonDataAccesser.getKirbyStoneData());
    __typeof__(data->unk38[0]) form = data->unk38[kind];
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 0.0f,
                        team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    desc.unk10 = kind + 4;
    desc.unk48 = form->unkC;
    desc.flag4DBit7 = 1;
    Weapon::activate(&desc);
    soModuleAccesser* accesser = m_moduleAccesser;
    Weapon::constraint(founderTaskId, form->unk8, form->unk4);
    accesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Visibility, false);
    soVisibilityModuleImpl& visibility = dynamic_cast<soVisibilityModuleImpl&>(m_moduleAccesser->getVisibilityModule());
    visibility.setData(form->unk0);
    for (int count = 0; count < 2; ++count) {
        visibility.setStatusDefault(0, 0, false);
    }
    visibility.setDefaultAll();
}
