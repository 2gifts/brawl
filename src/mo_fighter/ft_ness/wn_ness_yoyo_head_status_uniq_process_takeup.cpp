#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_ADD_OPERAND_ORDER_REGS
#include <wn/wn_module_accesser_builder.h>
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

// The yo-yo head being taken up again: it eases from where it is to the owner's node. HYPOTHESIS: all work variable meanings.
class wnNessYoyoHeadStatusUniqProcessTakeup : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessTakeup() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessTakeup g_wnNessYoyoHeadStatusUniqProcessTakeup;

void wnNessYoyoHeadStatusUniqProcessTakeup::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessYoyoHeadParam* param = dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param;
    moduleAccesser->getWorkManageModule().setFloat(param->unk0, 0x21000002);
}

void wnNessYoyoHeadStatusUniqProcessTakeup::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    wnLassoAltModelView* model = wnLassoGetAltModel(moduleAccesser);
    soPostureModule& posture = moduleAccesser->getPostureModule();
    wnLassoLinkView* link = wnLassoGetLink(moduleAccesser);
    Vec3f target = link->unk98NodePos(3, 0x42, true);
    Vec3f current = posture.getPos();
    Vec3f pos;
    if (work.getInt(0x20000002) < 10) {
        // The head eases to the node over ten frames.
        float t = (float)work.getInt(0x20000002) * 0.1f;
        if (t > 1.0f) {
            t = 1.0f;
        } else if (t < 0.0f) {
            t = 0.0f;
        }
        pos = target * t + current * (1.0f - t);
    } else {
        pos = target;
    }
    posture.setPos(&pos);
    float first = work.getFloat(0x21000001);
    float second = work.getFloat(0x21000002);
    float value = first + second;
    model->unk70IntFloat(2, value);
    work.setFloat(value, 0x21000001);
    if (work.isFlag(0x22000002)) {
        work.offFlag(0x22000002);
        work.setFloat(0.0f, 0x21000002);
    }
    work.incInt(0x20000002);
}

void wnNessYoyoHeadStatusUniqProcessTakeup::execFixPos(soModuleAccesser* moduleAccesser) {
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
