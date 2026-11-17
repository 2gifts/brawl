#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE // BYTES: the Vec3f -> Vec2f constructor is the REL's weak out-of-line copy
#define MT_VEC2F_LENGTH_INLINE_SQ // BYTES: length() expands the squares in place
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define FT_FIGHTER_ANIMCMD_LONG
#define WN_WEAPON_ANIMCMD_LONG
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_POPO_ARTICLE_POOLS
#define MT_VEC3F_OPERATORS_NOINLINE
#define MT_VEC3F_SUB_NATIVE_ORDER
#define MT_VEC3F_LENGTH_SQ_NOINLINE
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define WN_KINETIC_GENERIC_BUILDER
#include <so/kinetic/so_kinetic_energy.h>
#undef WN_KINETIC_GENERIC_BUILDER
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_class_info_impl.h>
#include <ft/fighter.h>
#include <ft/iceclimber/ft_iceclimber.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/ground/so_ground_module_impl.h>

class ftPopoStatusUniqProcessSpecialHiPartner : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialHiPartner() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialHiPartner::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), true);
    }
}

// HYPOTHESIS: the native soMotionChangeParam constructor takes keepFrame as a bool (the target passes it
// unmasked); with the u8 declaration the call masks it (same blocker as final_partner / special_s_partner).
void ftPopoStatusUniqProcessSpecialHiPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000014) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000014);
        float frame = 0.0f;
        bool keepFrame = false;
        if (moduleAccesser->getMotionModule().getKind() == 0x1d7) {
            frame = moduleAccesser->getMotionModule().getFrame();
            keepFrame = true;
        }
        soMotionChangeParam param(0x1de, frame, 1.0f, 0, 0, keepFrame, 0);
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000015) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000015);
        float frame = 0.0f;
        bool keepFrame = false;
        if (moduleAccesser->getMotionModule().getKind() == 0x1d6) {
            frame = moduleAccesser->getMotionModule().getFrame();
            keepFrame = true;
        }
        soMotionChangeParam param(0x1df, frame, 1.0f, 0, 0, keepFrame, 0);
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
}

// On exit the partner drops its model constraint and is placed where the constraint node put it.
void ftPopoStatusUniqProcessSpecialHiPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
        int node = moduleAccesser->getLinkModule().getModelConstraintNode();
        Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
        pos.m_z = moduleAccesser->getGroundModule().getZ();
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xd), true);
        moduleAccesser->getStageObject().updateNodeSRT();
        pos = moduleAccesser->getModelModule().getTopNodeGlobalPosFromNode(node, &pos);
        moduleAccesser->getPostureModule().setPos(&pos);
        moduleAccesser->getStageObject().updateNodeSRT();
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner == 0) {
            return;
        }
        moduleAccesser->getPostureModule().setLr(soExternalValueAccesser::getLr(partner));
        moduleAccesser->getPostureModule().updateRotYLr();
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), false);
    }
}

ftPopoStatusUniqProcessSpecialHiPartner g_ftPopoStatusUniqProcessSpecialHiPartner;
