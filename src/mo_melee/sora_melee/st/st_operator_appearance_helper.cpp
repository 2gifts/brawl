#include <st/operator/st_operator_appearance_helper.h>
#include <types.h>
#include <gm/gm_global.h>
#include <if/if_adv_gauge.h>
#include <if/if_adv_mngr.h>
#include <if/if_player.h>

// IfAdvGauge::removePressStart (the map name); src/sora/if/if_adv_mngr.cpp calls it by this stand-in name.
void fn_8010C0DC(IfAdvGauge* gauge, nw4r::g3d::ScnMdlExpand* locator);

stOperatorAppearanceHelper::~stOperatorAppearanceHelper() { }

void stOperatorAppearanceHelper::startOperator() {
    unk48 = 1;
    gmAdvData* advData = g_GameGlobal->m_advData;
    if ((s8)advData->unk5FA >= 1 && !advData->unk606_7) {
        IfAdvMngr* mngr = g_IfAdvMngr;
        if (mngr) {
            mngr->unk48_40 = true;
            if (mngr->m_gauge) {
                if (mngr->unk48_40 == 1) {
                    mngr->m_gauge->addPressStart(IfPlayer::getPlayerLocator());
                } else {
                    fn_8010C0DC(mngr->m_gauge, IfPlayer::getPlayerLocator());
                }
            }
        }
    }
}
