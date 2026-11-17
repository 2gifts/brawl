#define FT_LOG_ACTION_INFO_COPY_CTOR // the by-value log action info is built in place (see ft/ft_log.h)
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

// Helpers of the iceclimber REL that are not reconstructed yet.
// Not an infinity or a NaN (the weak copy of this helper sits in the special_s unit of the original).
inline bool mtIsNormal(float value) {
    bool result = true;
    if (((*reinterpret_cast<u32*>(&value) >> 23) & 0xFF) == 0xFF) result = false;
    return result;
}
#include <so/ground/so_ground_module_impl.h>
#include <ft/ft_owner.h>

// Link event carrying a status kind (HYPOTHESIS: 0x455 is the popo special-hi release event).
struct ftPopoSpecialHiStatusEvent : soLinkEventArgs {
    s32 m_status;
    ftPopoSpecialHiStatusEvent(int kind, int status) : soLinkEventArgs(kind), m_status(status) { }
};

// Link event carrying the ledge position the partner is pulled to (HYPOTHESIS names).
struct ftPopoSpecialHiPosEvent : soLinkEventArgs {
    Vec3f m_pos;
    ftPopoSpecialHiPosEvent(int kind, const Vec3f& pos) : soLinkEventArgs(kind), m_pos(pos) { }
};

// Link event carrying a state value and a flag (HYPOTHESIS names).
struct ftPopoSpecialHiStateEvent : soLinkEventArgs {
    s32 m_state;
    bool m_flag;
    ftPopoSpecialHiStateEvent(int kind, int state) : soLinkEventArgs(kind), m_state(state), m_flag(false) { }
};



class ftPopoStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftPopoStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner != 0 && ftPopo::isEnableCoupleTech(soExternalValueAccesser::getStatusKind(partner)) == 1) {
            Vec3f partnerPos = soExternalValueAccesser::getPos(partner);
            Vec2f partnerXY(partnerPos);
            Vec3f ownPos = moduleAccesser->getPostureModule().getPos();
            Vec2f diff = partnerXY - Vec2f(ownPos);
            bool normal = mtIsNormal(diff.m_x) && mtIsNormal(diff.m_y);
            if (!normal) {
                return;
            }
            float dist = diff.length();
            if (dist < soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                if (moduleAccesser->getLinkModule().isLink(6) == 1) {
                    ftPopoSpecialHiStatusEvent statusEvent(0x455, 0x11a);
                    moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
                    int state = 0x1de;
                    if (moduleAccesser->getSituationModule().getKind() != 0) {
                        state = 0x1df;
                    }
                    ftPopoSpecialHiStateEvent stateEvent(0x456, state);
                    moduleAccesser->getLinkModule().sendEventNodes(6, stateEvent, 0);
                }
            }
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner == 0) {
            return;
        }
        if (soExternalValueAccesser::getStatusKind(partner) != 0x11a) {
            return;
        }
        int nextStatus = 0x11b;
        if (moduleAccesser->getGroundModule().isStCliff(0) == true) {
            nextStatus = 0x11d;
        }
        self->getOwner()->setLogActionInfo(ftLogActionInfo(2, 0x3b));
        if (moduleAccesser->getLinkModule().isLink(6) == 1) {
            ftPopoSpecialHiStatusEvent statusEvent(0x455, nextStatus);
            moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
            if (nextStatus == 0x11d) {
                ftPopoSpecialHiPosEvent posEvent(0x459, moduleAccesser->getGroundModule().getCliffPos(0));
                moduleAccesser->getLinkModule().sendEventNodes(6, posEvent, 0);
            }
        }
    }
}

void ftPopoStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x119) {
        return;
    }
    ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (partner == 0) {
        return;
    }
    if (soExternalValueAccesser::getStatusKind(partner) != 0x11a) {
        return;
    }
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        ftPopoSpecialHiStatusEvent statusEvent(0x455, 0xe);
        moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
    }
}

ftPopoStatusUniqProcessSpecialHi g_ftPopoStatusUniqProcessSpecialHi;
