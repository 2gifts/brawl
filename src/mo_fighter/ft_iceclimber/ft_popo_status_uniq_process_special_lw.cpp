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

class ftPopoStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

void ftPopoStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003e) == true) {
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self != 0) {
            Fighter* partner = self->getPartner();
            if (moduleAccesser->getSituationModule().getKind() == 0 && soExternalValueAccesser::getSituationKind(partner) == 0 &&
                partner != 0 && soExternalValueAccesser::getStatusKind(partner) == 0x115) {
                Vec3f partnerPos = soExternalValueAccesser::getPos(partner);
                Vec2f partnerXY(partnerPos);
                Vec3f ownPos = moduleAccesser->getPostureModule().getPos();
                Vec2f diff = partnerXY - Vec2f(ownPos);
                bool normal = mtIsNormal(diff.m_x) && mtIsNormal(diff.m_y);
                if (normal) {
                    float dist = diff.length();
                    if (dist < soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0)) {
                        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                        moduleAccesser->getPostureModule().setLr(-soExternalValueAccesser::getLr(partner));
                        moduleAccesser->getPostureModule().updateRotYLr();
                    }
                }
            }
        }
    }
}

bool ftPopoStatusUniqProcessSpecialLw::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return true;
        }
        Fighter* partner = self->getPartner();
        if (moduleAccesser->getSituationModule().getKind() == 0 && soExternalValueAccesser::getSituationKind(partner) == 0 &&
            partner != 0) {
            moduleAccesser->getPostureModule().setLr(-soExternalValueAccesser::getLr(partner));
            moduleAccesser->getPostureModule().updateRotYLr();
            moduleAccesser->getStageObject().updateNodeSRT();
            return false;
        }
    }
    return true;
}

ftPopoStatusUniqProcessSpecialLw g_ftPopoStatusUniqProcessSpecialLw;
