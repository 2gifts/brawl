#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/ness/wn_ness_yoyo_head.h>
#include <mt/mt_vector.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_lasso_status_view.h>

// The yo-yo head falling after being shot out: gravity-like acceleration along the owner's facing. HYPOTHESIS: all work variable
// meanings. The energy is the weapon variant (wnKineticEnergyNormal): its RTTI is what the original dynamic_cast names.
class wnNessYoyoHeadStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessShotFall g_wnNessYoyoHeadStatusUniqProcessShotFall;

void wnNessYoyoHeadStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessYoyoHeadParam* param = dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param;
    float lr = moduleAccesser->getPostureModule().getLr();
    Vec2f speed;
    speed.m_x = param->unkC * lr;
    speed.m_y = param->unk18;
    Vec2f accel;
    accel.m_x = param->unk10 * lr;
    accel.m_y = -param->unk1C;
    Vec2f limit;
    limit.m_x = param->unk14;
    limit.m_y = param->unk20;
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    energy.m_speed = speed;
    energy.m_accel = accel;
    energy.m_speedTarget = limit;
    energy.m_speedLimit = limit;
    energy.enable();
    moduleAccesser->getStageObject().updateNodeSRT();
}

void wnNessYoyoHeadStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoLinkView* link = (wnLassoLinkView*)modules->m_linkModule;
    wnLassoAltModelView* model = (wnLassoAltModelView*)modules->m_modelModule;
    const wnNessYoyoHeadParam* param = dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param;
    if (work.isFlag(0x22000004)) {
        Vec3f nodePos = link->unk98NodePos(5, work.getInt(0x20000001), false);
        moduleAccesser->getPostureModule().setPos(&nodePos);
    }
    float range = param->unk8 - param->unk4;
    float value = range * (moduleAccesser->getMotionModule().getFrame() / 60.0f);
    value = param->unk8 - value;
    work.addFloat(value, 0x21000001);
    work.setFloat(value, 0x21000002);
    model->unk70IntFloat(2, work.getFloat(0x21000001));
}

void wnNessYoyoHeadStatusUniqProcessShotFall::execFixPos(soModuleAccesser* moduleAccesser) {
    // The position is pulled back to the corrected ground position on the axes the head touches the ground on, then the node
    // transform is refreshed. BYTES: written out in each unit instead of an inline helper; the helper moved the two Vec3f
    // locals' stack slots.
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f pos = posture.getPos();
    Vec3f corrected = ground.getCorrectPos(0);
    if (ground.isTouch((grCollStatus::TouchMask)9, 0)) {
        pos.m_y = corrected.m_y;
    }
    if (ground.isTouch((grCollStatus::TouchMask)6, 0)) {
        pos.m_x = corrected.m_x;
    }
    posture.setPos(&pos);
    moduleAccesser->getStageObject().updateNodeSRT();
}
