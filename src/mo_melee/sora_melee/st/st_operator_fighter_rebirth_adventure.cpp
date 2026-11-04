#include <st/loader/st_loader_player.h>
#include <st/operator/st_operator_fighter_rebirth_adventure.h>
#include <types.h>

stOperatorFighterRebirthAdventure::stOperatorFighterRebirthAdventure() :
    unk54(0), unk5C(0), unk60(0), unk64(0) {
    m_state = 0;
    m_rebirthFrames = 0.0f;
    unk58 = 60;
}

stOperatorFighterRebirthAdventure::~stOperatorFighterRebirthAdventure() { }

void stOperatorFighterRebirthAdventure::startOperator() {
    m_state = 1;
    m_rebirthCounter = m_rebirthFrames;
}

void stOperatorFighterRebirthAdventure::resetRebirth() {
    u8 stateIndex = m_state + 0xFF;
    if (stateIndex <= 1) {
        gmPlayerInitData* initData = static_cast<stLoaderPlayer*>(unk50)->getWatchPlayerInitData();
        initData->m_characterKind = unk64;
    }
    m_state = 0;
}
