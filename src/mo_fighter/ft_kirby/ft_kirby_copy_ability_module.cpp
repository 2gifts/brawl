#define FT_KIRBY_COPY_ABILITY_MODULE_RESOURCE_ACCESSER
#define FT_EVENT_UNIT_INSTANCE_ASSIGN
#define SO_ARRAY_CONTRACTIBLE_DTORS_NOINLINE
#include <ft/builder/ft_builder_animcmd.h>
#include <ft/kirby/ft_kirby_copy_ability_module.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_heap_module.h>
#include <so/motion/so_motion_change_param.h>
#include <wn/wn_class_info.h>
#include <ogws/revolution/OS/OSError.h>
#include <ft/mario/ft_mario_transactor.h>
#include <ft/donkey/ft_donkey_transactor.h>
#include <ft/link/ft_link_transactor.h>
#include <ft/samus/ft_samus_transactor.h>
#include <ft/yoshi/ft_yoshi_transactor.h>
#include <ft/fox/ft_fox_transactor.h>
#include <ft/pikachu/ft_pikachu_transactor.h>
#include <ft/luigi/ft_luigi_transactor.h>
#include <ft/ness/ft_ness_transactor.h>
#include <ft/koopa/ft_koopa_transactor.h>
#include <ft/peach/ft_peach_transactor.h>
#include <ft/zelda/ft_sheik_transactor.h>
#include <ft/iceclimber/ft_iceclimber_transactor.h>
#include <ft/marth/ft_marth_transactor.h>
#include <ft/gamewatch/ft_gamewatch_transactor.h>
#include <ft/falco/ft_falco_transactor.h>
#include <ft/wario/ft_wario_transactor.h>
#include <ft/metaknight/ft_metaknight_transactor.h>
#include <ft/pit/ft_pit_transactor.h>
#include <ft/szerosuit/ft_szerosuit_transactor.h>
#include <ft/pikmin/ft_pikmin_transactor.h>
#include <ft/lucas/ft_lucas_transactor.h>
#include <ft/diddy/ft_diddy_transactor.h>
#include <ft/poke/ft_poke_lizardon_transactor.h>
#include <ft/poke/ft_poke_zenigame_transactor.h>
#include <ft/poke/ft_poke_fushigisou_transactor.h>
#include <ft/dedede/ft_dedede_transactor.h>
#include <ft/lucario/ft_lucario_transactor.h>
#include <ft/ike/ft_ike_transactor.h>
#include <ft/robot/ft_robot_transactor.h>
#include <ft/purin/ft_purin_transactor.h>
#include <ft/toonlink/ft_toonlink_transactor.h>
#include <ft/wolf/ft_wolf_transactor.h>
#include <ft/snake/ft_snake_transactor.h>
#include <ft/sonic/ft_sonic_transactor.h>
#include <types.h>


namespace {
// The process of every copy ability, indexed by copy kind (a null entry has none).
// BYTES: the transactors derive from ftTransactor or from ftKirbyCopyAbilityProcess, which share the vtable layout.
#define FT_KIRBY_PROCESS_OF(T) reinterpret_cast<ftTransactor*>(soSingletonHolder<T>::getInstance())
// BYTES: the native singleton getters of the fighters' transactors stay out of line.
#pragma dont_inline on

ftTransactor* s_copyAbilityProcesses[55] = {
    FT_KIRBY_PROCESS_OF(ftMarioTransactor),
    FT_KIRBY_PROCESS_OF(ftDonkeyTransactor),
    FT_KIRBY_PROCESS_OF(ftLinkTransactor),
    FT_KIRBY_PROCESS_OF(ftSamusTransactor),
    FT_KIRBY_PROCESS_OF(ftYoshiTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftFoxTransactor),
    FT_KIRBY_PROCESS_OF(ftPikachuTransactor),
    FT_KIRBY_PROCESS_OF(ftLuigiTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftNessTransactor),
    FT_KIRBY_PROCESS_OF(ftKoopaTransactor),
    FT_KIRBY_PROCESS_OF(ftPeachTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftSheikTransactor),
    FT_KIRBY_PROCESS_OF(ftPopoTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftMarthTransactor),
    FT_KIRBY_PROCESS_OF(ftGameWatchTransactor),
    FT_KIRBY_PROCESS_OF(ftFalcoTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftWarioTransactor),
    FT_KIRBY_PROCESS_OF(ftMetaknightTransactor),
    FT_KIRBY_PROCESS_OF(ftPitTransactor),
    FT_KIRBY_PROCESS_OF(ftSZerosuitTransactor),
    FT_KIRBY_PROCESS_OF(ftPikminTransactor),
    FT_KIRBY_PROCESS_OF(ftLucasTransactor),
    FT_KIRBY_PROCESS_OF(ftDiddyTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftPokeLizardonTransactor),
    FT_KIRBY_PROCESS_OF(ftPokeZenigameTransactor),
    FT_KIRBY_PROCESS_OF(ftPokeFushigisouTransactor),
    FT_KIRBY_PROCESS_OF(ftDededeTransactor),
    FT_KIRBY_PROCESS_OF(ftLucarioTransactor),
    FT_KIRBY_PROCESS_OF(ftIkeTransactor),
    FT_KIRBY_PROCESS_OF(ftRobotTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftPurinTransactor),
    0, 0, 0,
    FT_KIRBY_PROCESS_OF(ftToonLinkTransactor),
    0, 0,
    FT_KIRBY_PROCESS_OF(ftWolfTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftSnakeTransactor),
    FT_KIRBY_PROCESS_OF(ftSonicTransactor),
    0,
    FT_KIRBY_PROCESS_OF(ftWarioTransactor),
};

#pragma dont_inline off

extern const ftKirbyCopyAbilityInfo lbl_96_rodata_D0[];
extern const int lbl_96_rodata_440[];
} // namespace

ftKirbyCopyAbilityModule::ftKirbyCopyAbilityModule(soModuleAccesser* moduleAccesser) :
    m_moduleAccesser(moduleAccesser),
    m_mediator(&m_groups),
    m_groups(7),
    unkB40(0),
    unkB58(0),
    unkB5C(0),
    m_copyKind(-1) {
    for (s32 i = 0; i < 7; i++) {
        m_groups.at(i).setUnitList(&m_units[i]);
    }
    m_moduleAccesser->getGenerateArticleManageModule().setDynamicArticleMediator(reinterpret_cast<soArticleMediator*>(&m_mediator));
    m_mediator.setAutoRecycle(true);
    m_mediator.setActivateTransactor(*this);
    m_mediator.setOperateTransactor(*this);
}

ftKirbyCopyAbilityModule::~ftKirbyCopyAbilityModule() {
    disposeCopyAbilityAll();
}

void ftKirbyCopyAbilityModule::disposeCopyAbilityAll() {
    m_mediator.deactivate();
    s32 count = m_mediator.getArrayNum();
    for (s32 i = 0; i < count; i++) {
        s32 groupId = m_mediator.getGroupId(i);
        if (groupId > -1) {
            disposeCopyAbility(groupId);
        }
    }
}

void ftKirbyCopyAbilityModule::disposeCopyAbility(int copyKind) {
    if (m_mediator.hasGroup(copyKind)) {
        OSReport("disposeCopyAbility %d 
", copyKind);
        const ftKirbyCopyAbilityInfo* info = getCopyAbilityInfo(copyKind);
        for (s32 i = 0; i < info->count; i++) {
            s32 articleId = info->getWeaponInfo(i).articleId;
            wnClassInfo::disposeArray(m_mediator.getArticleArray(copyKind, articleId));
        }
        m_mediator.disposeGroup(copyKind);
    }
}

// BYTES: the native module calls the weapon lookup out of line.
#pragma dont_inline on
inline const ftKirbyCopyAbilityWeaponInfo& ftKirbyCopyAbilityInfo::getWeaponInfo(int index) const {
    return weapons[index];
}
#pragma dont_inline off

soInstanceUnit<soEventUnit*>& soInstanceUnit<soEventUnit*>::operator=(const soInstanceUnit<soEventUnit*>& other) {
    m_element = other.m_element;
    m_id = other.m_id;
    return *this;
}

void soModelInfo::init() {
    unk0 = 0;
    unk4 = 0;
    unk8 = 0;
    unkC = 0;
    unk10 = 0;
}

const ftKirbyCopyAbilityInfo* ftKirbyCopyAbilityModule::getCopyAbilityInfo(int copyKind) {
    if (copyKind <= -1 || copyKind >= 55) {
        return &lbl_96_rodata_D0[0];
    }
    return &lbl_96_rodata_D0[copyKind];
}

int ftKirbyCopyAbilityModule::getCopyTopStatusKind(int copyKind) {
    if (copyKind <= -1 || copyKind >= 55) {
        return -1;
    }
    return lbl_96_rodata_440[copyKind];
}

ftTransactor* ftKirbyCopyAbilityModule::getCopyAbilityProcess() {
    if (m_copyKind <= -1) {
        return 0;
    }
    return s_copyAbilityProcesses[m_copyKind];
}

bool ftKirbyCopyAbilityModule::activeArticle(soArticle* article, soModuleAccesser* moduleAccesser) {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        return process->activeArticle(article, moduleAccesser);
    }
    return false;
}

bool ftKirbyCopyAbilityModule::shootArticle(soArticle* article, soModuleAccesser* moduleAccesser) {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        return process->shootArticle(article, moduleAccesser);
    }
    return false;
}

void ftKirbyCopyAbilityModule::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind) {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        process->notifyEventChangeSituation(kind, prevKind, m_moduleAccesser);
    }
}

void ftKirbyCopyAbilityModule::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        process->notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    }
}

bool ftKirbyCopyAbilityModule::notifyEventCollisionShieldCheck() {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        return process->unk9(m_moduleAccesser);
    }
    return false;
}

void ftKirbyCopyAbilityModule::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        process->notifyEventCollisionSearch(collisionLog, moduleAccesser);
    }
}

bool ftKirbyCopyAbilityModule::notifyEventCollisionSearchCheck() {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        return process->notifyEventCollisionSearchCheck(m_moduleAccesser);
    }
    return false;
}

void ftKirbyCopyAbilityModule::processUpdate() {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        process->processUpdate(m_moduleAccesser);
    }
}

void ftKirbyCopyAbilityModule::processCollision() {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        process->processCollision(m_moduleAccesser);
    }
}

void ftKirbyCopyAbilityModule::processFixPosition() {
    ftTransactor* process = getCopyAbilityProcess();
    if (process != 0) {
        process->processFixPosition(m_moduleAccesser);
    }
}

template class soArrayVectorAbstract<soArticle*>;
template class soArrayVector<soArticle*, 24>;

// Kirby's own accesser: the texture of a copied ability comes from the copy ability's resources.
u32 ftKirbyResourceIdAccesserImpl::getTexResId() {
    if (m_moduleAccsr->getWorkManageModule().isFlag(0x12000042) == true) {
        return ftKirbyResourceIdAccesserImpl::getKirbyResId(m_moduleAccsr->getWorkManageModule().getInt(0x10000048));
    }
    return getMdlResId();
}

u32 ftKirbyResourceIdAccesserImpl::getKirbyResId(int fighterKind) {
    soWorkManageModule& work = m_moduleAccsr->getWorkManageModule();
    soHeapModule& heap = m_moduleAccsr->getHeapModule();
    return g_ftCommonDataAccesser.getKirbyResId(heap.getSlotNo(), static_cast<ftKind>(fighterKind), work.getInt(0x10000049));
}

// The native copy ability code calls this constructor separately.
#pragma dont_inline on
soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, bool c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) { }
#pragma dont_inline reset
