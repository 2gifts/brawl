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

#include <ft/ft_status_uniq_process_cliff.h>
#include <so/ground/so_ground_module_impl.h>

// Link event sent to the cliff-pulled partner: the kind, then the status kind it is moved to (HYPOTHESIS names).
struct ftPopoCliffPullStatusEvent : soLinkEventArgs {
    s32 m_status;
    ftPopoCliffPullStatusEvent(int kind, int status) : soLinkEventArgs(kind), m_status(status) { }
};

// Link event carrying the ledge position the partner is pulled to (HYPOTHESIS names).
struct ftPopoCliffPullPosEvent : soLinkEventArgs {
    Vec3f m_pos;
    ftPopoCliffPullPosEvent(int kind, const Vec3f& pos) : soLinkEventArgs(kind), m_pos(pos) { }
};

class ftPopoStatusUniqProcessSpecialHiCliffPullPartner : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialHiCliffPullPartner() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialHiCliffPullPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner != 0 && soExternalValueAccesser::getStatusKind(partner) == 0x119) {
            if (moduleAccesser->getLinkModule().isLink(6) == 1) {
                ftPopoCliffPullStatusEvent statusEvent(0x455, 0x11f);
                moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
                ftPopoCliffPullPosEvent posEvent(0x459, moduleAccesser->getGroundModule().getCliffPos(0));
                moduleAccesser->getLinkModule().sendEventNodes(6, posEvent, 0);
            }
        }
    }
}

void ftPopoStatusUniqProcessSpecialHiCliffPullPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (ftStatusUniqProcessCliff::isLeaveCliffStatus(nextStatus) == true) {
        moduleAccesser->getGroundModule().leaveCliff(0);
    }
}

ftPopoStatusUniqProcessSpecialHiCliffPullPartner g_ftPopoStatusUniqProcessSpecialHiCliffPullPartner;
