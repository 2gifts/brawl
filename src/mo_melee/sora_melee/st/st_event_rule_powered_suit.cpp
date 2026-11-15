#include <st/st_event_rule.h>
#include <types.h>

// HYPOTHESIS: an initial value of the rule (a sora_melee data word).
extern "C" u32 lbl_27_data_543A8;

// TODO: chechEventRule.

stEventRulePoweredSuit::stEventRulePoweredSuit() : stEventRule(1) {
    unk10 = lbl_27_data_543A8;
    unk14 = 0;
}

stEventRulePoweredSuit::~stEventRulePoweredSuit() { }
