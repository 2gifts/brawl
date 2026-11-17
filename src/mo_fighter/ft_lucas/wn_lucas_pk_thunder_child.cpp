#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructor expands the empty param customize module destructor (a plain delete)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/lucas/wn_lucas_pk_thunder_child.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// The status unique process of the bolt (wn_lucas_pk_thunder_child_status_uniq_process_move.cpp).
extern soStatusUniqProcess g_wnLucasPKThunderChildStatusUniqProcessMove;

// TODO(link): every function is exact (100%). WN_PARAM_CUSTOMIZE_INLINE_DTOR (below) gives the 0x40-byte builder destructor the original
//   calls (0x4A00 in ft_lucas's symbols.txt, renamed from a folded alias; the 0x60 symbol at 0xECA0 was mislabeled, it is
//   ftKineticEnergyController's destructor). Still does not link: the original's vtable at the @136 secondary base points at
//   ftFighterBuildData getters (getNWModelInstanceHeap at 0x15f9c/0xc6ac) where ours points at Weapon thunks, ours also emits a
//   soParamCustomizeModuleImpl vtable/RTTI (+0x100 data) and 0x38 bss, and .rodata differs at +0x141. Compare the build data
//   class and the data-owner base against the Ness twin.

wnLucasPKThunderChild::wnLucasPKThunderChild(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKThunderChildData* data) :
    wnWeaponBuilder<wnLucasPKThunderChildModuleAccesserBuildConfig>(
        "LucasPKThunderChild", articleId, info,
        wnModuleAccesserBuildData(80, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnLucasPKThunderChildStatusUniqProcessMove;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

// BYTES: the original builds the descriptor in an inlined helper that takes the start position as a temporary; as a named local
// the zero vector gets the wrong stack slot (above the link position instead of below it).
static inline void initChildDesc(wnActivateDesc& desc, int founderTaskId, int team, const Vec3f& pos) {
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // BYTES: a word copy; the unit's Vec3f assignment is the out-of-line float one.
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
    desc.lr = 1.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
}

void wnLucasPKThunderChild::activate(int founderTaskId, int linkTaskId, int team, int kind) {
    wnActivateDesc desc;
    initChildDesc(desc, founderTaskId, team, Vec3f(0.0f, 0.0f, 0.0f));
    Weapon::activate(&desc);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soStatusModule& status = m_moduleAccesser->getStatusModule();
    soLinkModule& link = m_moduleAccesser->getLinkModule();
    if (link.link(5, linkTaskId)) {
        Vec3f parentPos = link.getParentModelNodeGlobalPosition(5u, u32(0), true);
        posture.setPos(&parentPos);
        work.setFloat(parentPos.m_x, 0x21000000);
        work.setFloat(parentPos.m_y, 0x21000001);
        work.setFloat(parentPos.m_x, 0x21000002);
        work.setFloat(parentPos.m_y, 0x21000003);
        if (kind == 0) {
            m_moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(0x144d), true, 0);
        }
        soEffectModule& effect = m_moduleAccesser->getEffectModule();
        int effectId = (kind == 5) ? 0x1b0007 : 0x1b0006;
        effect.reqFollow(static_cast<EfID>(effectId), 0, &Vec3f(0.0f, 0.0f, 4.0f), &Vec3f(0.0f, 0.0f, 0.0f), 2.0f, false, 0, 0, -1);
        updateNodeSRT();
        link.setAttribute(5, static_cast<soLinkConnection::Attribute>(0xd), true);
    }
    status.changeStatusForce(0, m_moduleAccesser);
}

bool wnLucasPKThunderChild::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

void wnLucasPKThunderChild::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    float s = m_moduleAccesser->getPostureModule().getScale();
    scale.m_z = s;
    scale.m_y = s;
    scale.m_x = s;
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    rot.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}
