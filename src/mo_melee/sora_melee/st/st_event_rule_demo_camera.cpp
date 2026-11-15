#include <mt/mt_prng.h>
#include <st/st_event_rule.h>
#include <types.h>

// HYPOTHESIS: an initial value of the rule (a sora_melee data word).
extern "C" u32 lbl_27_data_545A0;

// TODO: chechEventRule (the camera cut sequence).

stEventRuleDemoCamera::stEventRuleDemoCamera() : stEventRule(1) {
    m_started = false;
    unk28 = lbl_27_data_545A0;
    unk2C = 0;
    m_order[0] = 1;
    m_order[1] = 2;
    m_order[2] = 3;
    m_order[3] = 4;
    for (int i = 0; i < 4; i++) {
        int j = randi(3);
        int tmp = m_order[i];
        m_order[i] = m_order[j];
        m_order[j] = tmp;
    }
}

stEventRuleDemoCamera::~stEventRuleDemoCamera() { }

void stEventRuleDemoCamera::notifyEventStartGame() {
    m_started = true;
}
