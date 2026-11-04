#include <ac/ac_anim_cmd_impl.h>
#include <ft/poke/ft_poke_lizardon_article_pools.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnPokeLizardonRock::shoot(bool unk) {
    if (unk == true) {
        getModuleAccesser()->getStatusModule().changeStatusForce(3, getModuleAccesser());
        getModuleAccesser()->getWorkManageModule().offFlag(0x12000000);
        return;
    }
    getModuleAccesser()->getStatusModule().changeStatusForce(2, getModuleAccesser());
}

void wnPokeLizardonRock::activate(u32 founderTaskId, u32 resourceId, int team,
                                 const Vec3f& pos, float lr, float unk) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1,
                        0, 0, pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    Weapon::activate(&desc);
    getModuleAccesser()->getWorkManageModule().offFlag(0x12000003);
    getModuleAccesser()->getWorkManageModule().offFlag(0x12000004);
    getModuleAccesser()->getWorkManageModule().setInt(team, 0x10000005);
    getModuleAccesser()->getWorkManageModule().setFloat(unk, 0x11000002);
    getModuleAccesser()->getWorkManageModule().onFlag(0x12000000);
    getModuleAccesser()->getStatusModule().changeStatusForce(0, getModuleAccesser());
    updateNodeSRT();
}

void wnPokeLizardonRock::updateNodeSRT() {
    if (getModuleAccesser()->getStatusModule().getStatusKind() == 3) {
        Vec3f scale;
        Vec3f rotation;
        Vec3f nodeRotation;
        Vec3f position;
        scale.m_x = scale.m_y = scale.m_z = getModuleAccesser()->getPostureModule().getScale();
        position = getModuleAccesser()->getPostureModule().getPos();
        rotation = getModuleAccesser()->getPostureModule().getRot(0);
        getModuleAccesser()->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
        nodeRotation = getModuleAccesser()->getPostureModule().getRot(1);
        nodeRotation.m_x += getModuleAccesser()->getWorkManageModule().getFloat(0x11000001);
        getModuleAccesser()->getModelModule().setNodeRotate(2, &nodeRotation);
    } else {
        StageObject::updateNodeSRT();
    }
}

bool wnPokeLizardonRock::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, acc, index)) return true;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) return false;
    if (cmd->getType() <= -1 || cmd->getType() >= 11) return false;
    switch (cmd->getType()) {
    case 6:
        getModuleAccesser()->getStatusModule().changeStatusForce(4, getModuleAccesser());
        getModuleAccesser()->getWorkManageModule().onFlag(0x12000000);
        return true;
    }
    return false;
}

void wnPokeLizardonRock::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                       StageObject* sender, int index) {
    switch (event->m_eventKind) {
    case 0x838:
        getModuleAccesser()->getStatusModule().changeStatusForce(1, getModuleAccesser());
        return;
    }
    Weapon::notifyEventLink(event, acc, sender, index);
}

void wnPokeLizardonRock::notifyEventCollisionHit(float power, soCollisionAttackData* data,
                                               u32 index, int kind, soModuleAccesser* acc,
                                               soCollisionLog* log) {
    Weapon::notifyEventCollisionHit(power, data, index, kind, acc, log);
    if (!getModuleAccesser()->getWorkManageModule().isFlag(0x12000004)) {
        soLinkEventArgs event(0x456);
        getModuleAccesser()->getLinkModule().sendEventParents(3, event);
    }
}
