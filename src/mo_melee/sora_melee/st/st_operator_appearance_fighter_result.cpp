#include <st/operator/st_operator_appearance_fighter_result.h>
#include <types.h>
#include <st/operator/st_operator_rule_vsresult.h>
#include <if/if_vs_result_task.h>
#include <st/loader/st_loader_player.h>

// stLoader::isEntity (the map name); src/mo_melee/sora_melee/st/st_loader_sand_bag.cpp calls it by this stand-in name.
extern "C" bool fn_27_23AC10(stLoader* loader);

stOperatorAppearanceFighterResult::stOperatorAppearanceFighterResult() {
    unk1B5 = false;
    m_state = 0;
    unk1B8 = NULL;
}

stOperatorAppearanceFighterResult::~stOperatorAppearanceFighterResult() { }

bool stOperatorAppearanceFighterResult::isEnd() {
    return unk1B5;
}

void stOperatorAppearanceFighterResult::startAppearance() {
    stOperatorRuleVsResult* rule = static_cast<stOperatorRuleVsResult*>(unk1B8);
    m_state = 6;
    if (rule != NULL) {
        rule->startOperator();
    }
}

void stOperatorAppearanceFighterResult::processBegin() {
    if (m_state == 0) {
        return;
    }
    if (unk1B5 == true) {
        return;
    }
    if (unk1B8 != NULL && !static_cast<stOperatorRuleVsResult*>(unk1B8)->unkCC->isLoadEnd()) {
        return;
    }
    if (isEntryFighter(0)) {
        if (static_cast<stLoaderPlayer*>(unk74[0])->isEmpty() == true) {
            return;
        }
        if (!static_cast<stLoaderPlayer*>(unk74[0])->isLoad()) {
            return;
        }
        if (!fn_27_23AC10(static_cast<stLoaderPlayer*>(unk74[0]))) {
            static_cast<stLoaderPlayer*>(unk74[0])->entryEntityResult();
        }
    }
    if (isEntryFighter(1)) {
        if (static_cast<stLoaderPlayer*>(unk74[1])->isEmpty() == true) {
            return;
        }
        if (!static_cast<stLoaderPlayer*>(unk74[1])->isLoad()) {
            return;
        }
        if (!fn_27_23AC10(static_cast<stLoaderPlayer*>(unk74[1]))) {
            static_cast<stLoaderPlayer*>(unk74[1])->entryEntityResult();
        }
    }
    if (isEntryFighter(2)) {
        if (static_cast<stLoaderPlayer*>(unk74[2])->isEmpty() == true) {
            return;
        }
        if (!static_cast<stLoaderPlayer*>(unk74[2])->isLoad()) {
            return;
        }
        if (!fn_27_23AC10(static_cast<stLoaderPlayer*>(unk74[2]))) {
            static_cast<stLoaderPlayer*>(unk74[2])->entryEntityResult();
        }
    }
    if (isEntryFighter(3)) {
        if (static_cast<stLoaderPlayer*>(unk74[3])->isEmpty() == true) {
            return;
        }
        if (!static_cast<stLoaderPlayer*>(unk74[3])->isLoad()) {
            return;
        }
        if (!fn_27_23AC10(static_cast<stLoaderPlayer*>(unk74[3]))) {
            static_cast<stLoaderPlayer*>(unk74[3])->entryEntityResult();
        }
    }
    if (unk90 != NULL) {
        static_cast<stLoader*>(unk90)->removeEntity();
    }
    unk1B5 = true;
}
