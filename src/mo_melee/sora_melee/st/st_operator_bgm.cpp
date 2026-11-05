#include <st/operator/st_operator_bgm.h>
#include <types.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <st/stage.h>

// sndBgmRateSystem::stopBGM (the map name); src/mo_stage/st_madein/st_madein.cpp calls it by this stand-in name.
extern "C" void fn_800790FC(void* system, int a);

void stOperatorBgm::processBegin() {
    switch (m_state) {
    case 0:
        if (m_isSuddenDeath == true) {
            m_state = 1;
        }
        break;
    case 1: {
        u32 option;
        int bgmId;
        if (m_condition->m_meleeInitData.unk54 != 0) {
            u32 category = m_condition->m_meleeInitData.unk54 >> 24;
            if (category == 0xFF) {
                unk48 = m_condition->m_meleeInitData.unk54 & 0xFFFFFF;
                option = 0;
                bgmId = 0;
            } else {
                unk48 = category;
                option = 0;
                bgmId = m_condition->m_meleeInitData.unk54 & 0xFFFFFF;
            }
        } else if (g_Stage != NULL) {
            unk48 = g_Stage->getBgmID();
            option = g_Stage->getBgmOptionID();
            bgmId = 0;
        } else {
            unk48 = 0;
            option = 0;
            bgmId = 0;
        }
        unk4C = unk48 < 0x100;
        if (m_isSuddenDeath == true) {
            option = -1;
        }
        if (unk4C) {
            unk50 = lbl_805A01D8->prepareBGM(unk48, option, bgmId);
            unk60 = lbl_805A01D8->getPlayTiming();
            if (unk60 == 0xFFFF) {
                unk60 = 180;
            }
            m_state = 2;
        } else {
            lbl_805A01D8->m_category = 45;
            unk50 = unk48;
            if (!g_sndSystem->prepareBGM(unk48, 1)) {
                unk50 = -1;
            }
            m_state = 2;
        }
        if (m_isSuddenDeath == true) {
            unk60 = 60;
        }
        break;
    }
    case 2:
        if (unk50 >= 0) {
            if (unk4C) {
                if (lbl_805A01D8->isPreparedBGM()) {
                    goto preparedBgm;
                }
                break;
            } else if (!g_sndSystem->isPreparedBGM(unk48)) {
                break;
            }
        }
    preparedBgm:
        m_state = 3;
        break;
    case 3:
        if (m_isRequestPlay == true) {
            if (unk60 != 0) {
                --unk60;
            } else {
                if (unk50 >= 0) {
                    if (unk4C) {
                        lbl_805A01D8->startBGM(0);
                    } else {
                        g_sndSystem->startBGM(0, 0);
                    }
                }
                m_state = 4;
            }
        }
        break;
    case 4:
        if (m_isRequestStop == true) {
            if (unk50 >= 0) {
                if (unk4C) {
                    fn_800790FC(lbl_805A01D8, unk58);
                } else {
                    g_sndSystem->stopBGM(unk58);
                }
            }
            m_isRequestPlay = false;
            m_state = 5;
        }
        if (g_Stage != NULL && g_Stage->isBgmChange() == true) {
            u32 state;
            float fade;
            g_Stage->getBgmChangeID(&state, &fade);
            if (unk50 >= 0 && unk4C) {
                lbl_805A01D8->bgmStateChange(state, fade);
            }
            g_Stage->setBgmChange(0.0f, false, 0);
        }
        break;
    case 5:
        if (m_isRequestStop == true) {
            m_state = 0;
            m_isRequestStop = false;
        }
        break;
    }
}

stOperatorBgm::stOperatorBgm() :
    stOperator("StOperatorBgm"), unk4C(0), unk50(-1), unk54(0), unk58(60), m_isRequestPlay(false),
    m_isRequestStop(false), m_isSuddenDeath(false), unk60(0) {
    m_state = 0;
}

stOperatorBgm::~stOperatorBgm() { }

bool stOperatorBgm::isReady() {
    if (m_state == 3) {
        return true;
    }
    return m_state == 0;
}

void stOperatorBgm::requestPlay() {
    m_isRequestPlay = true;
    m_isSuddenDeath = false;
}

void stOperatorBgm::requestStop() {
    m_isRequestStop = true;
    m_isSuddenDeath = false;
}

void stOperatorBgm::requestPrepare() {
    if (m_state == 0) {
        m_state = 1;
        m_isSuddenDeath = false;
    }
}

void stOperatorBgm::requestPrepareSuddenDeath() {
    requestPrepare();
    m_isSuddenDeath = true;
}
