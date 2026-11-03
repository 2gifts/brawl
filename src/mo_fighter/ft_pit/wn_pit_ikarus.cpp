#include <types.h>
#include <wn/pit/wn_pit_ikarus.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnPitIkarus::activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        pos, 0.0f, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagBit7 = true;
    desc.flagBit6 = false;
    desc.flagBit5 = false;
    desc.flagBit4 = false;
    desc.flagBit3 = false;
    desc.flagBit2 = false;
    desc.flagBit1 = false;
    desc.flagBit0 = false;
    desc.flag4DBit7 = false;
    desc.flag4DBit6 = false;
    Weapon::activate(&desc);
    m_moduleAccesser->getTeamModule().setHitTeam(-1);
    m_moduleAccesser->getTeamModule().set2nd(5);
    m_moduleAccesser->getTeamModule().setHit2nd(5);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000001);
    m_moduleAccesser->getPostureModule().setScale(unk2);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

bool wnPitIkarus::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags != 0) {
        m_moduleAccesser->getStatusModule().changeStatusForce(4, m_moduleAccesser);
        return true;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

wnPitIkarus::~wnPitIkarus() { }
