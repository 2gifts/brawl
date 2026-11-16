#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_ARTICLE_BUILDER_DTOR_IN_CLASS // the module accesser builder destructor is a weak function of its own here
#define WN_WEAPON_NO_COMMON_ENERGIES // no kinetic module, so the weapon builder adds no energies
#define SO_OTHER_ANIM_CTOR // the motion builder owns one other-animation slot (constructed and destroyed as an array)
#define WN_TRANSITION_CTOR_INLINE // the motion builder constructor of this unit expands the transition module constructor
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define SO_BUILD_DATA_SLOPE_LIMIT_EXTERNAL // the build data's default slope limit is not emitted here (no 20.0 in the unit's pool)
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/zelda/wn_szerosuit_paralyzer.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/so_external_value_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_kind.h>
#include <ft/ft_entry.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) {}


// Registers the Zero Suit paralyzer article class (kind 98) in the class info table during static initialization.
wnClassInfoImpl<98, wnSZerosuitParalyzer, wnSZerosuitParalyzerData> g_wnSZerosuitParalyzerClassInfo;

wnSZerosuitParalyzer::wnSZerosuitParalyzer(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerData* data) :
    wnWeaponBuilder<wnSZerosuitParalyzerModuleAccesserBuildConfig>(
        "SZerosuitParalyzer", articleId, info,
        wnModuleAccesserBuildData(98, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The unit keeps the weak constructors of its twenty-motion motion builder and of its status builder.
typedef wnSZerosuitParalyzerModuleAccesserBuildConfig SpBC;
// BYTES: the original addresses each singleton's destructor-chain record by its own address (no pooled anchor).
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<SpBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop
template soStatusModuleBuilder<SpBC::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);

// BYTES: the .text order of the original unit is the article constructor (with its weak copies), the parameter accesser
// destructor, getModuleAccesser, the module accesser builder constructor, then the article members.
inline wnSZerosuitParalyzerParamAccesser::~wnSZerosuitParalyzerParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSZerosuitParalyzerModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnSZerosuitParalyzer::activate(int founderTaskId, int resourceId, int team) {
    Vec3f origin(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = origin;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = true;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = true;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.flag4DBit6 = 1;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000005);
}

void wnSZerosuitParalyzer::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser,
                                           StageObject* sender, int selector) {
    soLinkModule& link = m_moduleAccesser->getLinkModule();
    if (sender->soGetKind() == StageObject_Fighter && sender->soGetSubKind() == Fighter_SZeroSuit &&
        link.isLink(3)) {
        u32 senderTaskId = sender->m_taskId;
        u32 parentTaskId = link.getParentTaskId(3);
        if (parentTaskId == senderTaskId) {
            switch (event->m_eventKind) {
            case 0x455: {
                int motionKind = -1;
                switch (soExternalValueAccesser::getMotionKind(sender)) {
                case 0x1d5:
                    motionKind = 6;
                    break;
                case 0x1d6:
                    motionKind = 7;
                    break;
                case 0x1dc:
                    motionKind = 8;
                    break;
                case 0x1dd:
                    motionKind = 9;
                    break;
                }
                m_moduleAccesser->getWorkManageModule().setInt(motionKind, 0x10000005);
                break;
            }
            }
        }
    }
    Weapon::notifyEventLink(event, moduleAccesser, sender, selector);
}

void wnSZerosuitParalyzer::processFixPosition() {
    Weapon::processFixPosition();
    int motionKind = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    if (motionKind != -1) {
        soMotionChangeParam param(motionKind, m_moduleAccesser->getMotionModule().getFrame(), 1.0f, 0, 0, 1, 0);
        m_moduleAccesser->getMotionModule().changeMotionRequest(&param);
        m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000005);
    }
}

wnSZerosuitParalyzer::~wnSZerosuitParalyzer() { }
