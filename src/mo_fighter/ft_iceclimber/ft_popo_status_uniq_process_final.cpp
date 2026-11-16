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

// Link event carrying only its kind (HYPOTHESIS: kind 0x45b is the partner's final-smash release).
struct ftPopoFinalLinkEvent : soLinkEventArgs {
    ftPopoFinalLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// Link event carrying a status kind (HYPOTHESIS names; kinds 0x455 and 0x456 are the popo final-smash events).
struct ftPopoFinalStatusEvent : soLinkEventArgs {
    s32 m_status;
    ftPopoFinalStatusEvent(int kind, int status) : soLinkEventArgs(kind), m_status(status) { }
};

// Link event carrying a state value and a flag (HYPOTHESIS names).
struct ftPopoFinalStateEvent : soLinkEventArgs {
    s32 m_state;
    bool m_flag;
    ftPopoFinalStateEvent(int kind, int state) : soLinkEventArgs(kind), m_state(state), m_flag(false) { }
};


class ftPopoStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
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
        if (normal) {
            float dist = diff.length();
            if (dist < soValueAccesser::getConstantFloat(moduleAccesser, 0xfdb, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                if (moduleAccesser->getLinkModule().isLink(6) == 1) {
                    ftPopoFinalStatusEvent statusEvent(0x455, 0x120);
                    moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
                }
                if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
                    moduleAccesser->getSituationModule().setKind(SituationKind(2), false);
                }
                int state = 0x1e5;
                if (moduleAccesser->getSituationModule().getKind() != 0) {
                    state = 0x1e9;
                }
                ftPopoFinalStateEvent stateEvent(0x456, state);
                moduleAccesser->getLinkModule().sendEventNodes(6, stateEvent, 0);
            }
        }
    } else {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    }
}

void ftPopoStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) != true) {
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
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        ftPopoFinalLinkEvent event(0x45b);
        moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
    }
}

ftPopoStatusUniqProcessFinal g_ftPopoStatusUniqProcessFinal;
