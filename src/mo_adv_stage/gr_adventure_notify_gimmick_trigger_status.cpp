#include <memory.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>

// HYPOTHESIS: two stTrigger callback-list setters (sora_melee) shaped like stTrigger::setValidCallback but for the
// lists at +0x24 and +0x28 of the trigger.
extern "C" void fn_27_233F58(stTrigger* trigger, gfCallBack* cb);
extern "C" void fn_27_233FCC(stTrigger* trigger, gfCallBack* cb);

void grAdventureNotifyGimmickTriggerStatus::createGimmickStatusTrigger(grAdventureNotifyGimmickTriggerStatus* owner, stTrigger* trigger, stTriggerData* data, int index, int flag) {
    // MATCH-ONLY: the valid flag is read as the whole byte (the shared header declares it as a bit)
    m_triggers[index] = new (Heaps::StageInstance) stActTriggerGimmickStatus(owner, index, reinterpret_cast<u8*>(data)[2] != 0, flag);
    if (index == 1) {
        fn_27_233F58(trigger, m_triggers[index]);
    } else {
        fn_27_233FCC(trigger, m_triggers[index]);
    }
}

void grAdventureNotifyGimmickTriggerStatus::setGimmickStatus(const int* status) {
    m_status = *status;
    updateStatus();
}

void grAdventureNotifyGimmickTriggerStatus::updateStatus() { }
