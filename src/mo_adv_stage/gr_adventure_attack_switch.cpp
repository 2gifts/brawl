#include <memory.h>
#include <sora_adv_stage/gr_adventure_attack_switch.h>

grAdventureAttackSwitch* grAdventureAttackSwitch::create(int mdlIndex, bool* isOn, const char* taskName) {
    grAdventureAttackSwitch* ground = new (Heaps::StageInstance) grAdventureAttackSwitch(isOn, taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureAttackSwitch::grAdventureAttackSwitch(bool* isOn, const char* taskName) : grYakumonoSwitch(isOn, taskName) {
    m_coolTime = 0.0f;
    m_data = NULL;
    unk190[0] = 0;
    unk190[1] = 0;
    unk190[2] = 0;
    unk190[3] = 0;
    unk190[4] = 0;
}

grAdventureAttackSwitch::~grAdventureAttackSwitch() { }

void grAdventureAttackSwitch::setOn(int isTrigger) {
    grYakumonoSwitch::setOn(isTrigger);
    disableHit(0, 0);
    if (m_resFile.GetResAnmClrNumEntries() > 1) {
        changeMatColAnim(1, 0);
        if (isTrigger == 0) {
            gfModelAnimation* modelAnim = m_modelAnims[0];
            modelAnim->setFrame(modelAnim->getFrameCount());
        }
    }
}

void grAdventureAttackSwitch::setOff(int isTrigger) {
    grYakumonoSwitch::setOff(isTrigger);
    enableHit(0, 0);
    m_coolTime = 5.0f;
}

void grAdventureAttackSwitch::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (!(m_coolTime > 0.0f)) {
        if (grYakumono::isEnableAttackPersonAttacked((u32)damage, (u32)attackerInfo, (u32)&m_data->unk70) &&
            damage->m_damageAdd > 0.0f) {
            setOn(true);
        }
    }
}
