#include <ft/ft_common_data_accesser.h>
#include <ft/ft_external_value_accesser.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// TODO: the position, status and hit-target queries (getHipPos, getCursorPos, the down-wait / smash-hold status
// tests that sora_enemy calls by their REL names, getHitTarget...).

soDamageModule* ftExternalValueAccesser::getDamageModule(Fighter* fighter) {
    return &soExternalValueAccesser::getModuleAccesser(fighter)->getDamageModule();
}

soCollisionHitModule* ftExternalValueAccesser::getsoCollisionHitModule(Fighter* fighter) {
    return &soExternalValueAccesser::getModuleAccesser(fighter)->getCollisionHitModule();
}

float ftExternalValueAccesser::getWeight(Fighter* fighter) {
    return soValueAccesser::getConstantFloat(soExternalValueAccesser::getModuleAccesser(fighter), 0xbe1, 0);
}

int ftExternalValueAccesser::getMetroidNodeId() {
    return 0xc9;
}

int ftExternalValueAccesser::getNodeIdFromHitNo(u8 hitNo) {
    return hitNo + 0xcb;
}

// The audience table hangs off the common fighter data (word 0x40).
u32* ftExternalValueAccesser::getAudienceData() {
    return *reinterpret_cast<u32**>(&g_ftCommonData.dataCommon->unk40[0]);
}
