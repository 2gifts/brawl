#include <st/st_event_rule.h>
#include <types.h>

stEventRule::stEventRule(int flag) {
    unk4 = flag;
}

stEventRule::~stEventRule() { }

u8 stEventRule::notifyEventDead(int entryId, u32 playerIndex) {
    return unk4;
}

void stEventRule::notifyEventStartStage() { }

void stEventRule::notifyEventStartGame() { }

void stEventRule::notifyEventDeadCorpsMember(int index) { }

void stEventRule::notifyEventYoshiEgg() { }

void stEventRule::notifyEventPikminBloom() { }

bool stEventRule::chechEventRule() {
    return false;
}

// HYPOTHESIS: the event-wide tuning block (seven flag bits and five rates, all 1 by default).
struct GlobalEventRule {
    u8 m_flags : 7;
    u8 : 1;
    float m_rates[5];
    void init();
};

void GlobalEventRule::init() {
    m_flags = 0;
    m_rates[0] = 1.0f;
    m_rates[1] = 1.0f;
    m_rates[2] = 1.0f;
    m_rates[3] = 1.0f;
    m_rates[4] = 1.0f;
}
