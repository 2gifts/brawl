#include <st/operator/st_operator_appearance_helper.h>
#include <types.h>
#include <gm/gm_global.h>
#include <if/if_adv_gauge.h>
#include <if/if_adv_mngr.h>
#include <if/if_player.h>

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
                    mngr->m_gauge->removePressStart(IfPlayer::getPlayerLocator());
                }
            }
        }
    }
}

// TODO: clean up, give variables real names
namespace jj_2450E4 {
extern "C" {
    extern u8* g_IfAdvMngr;
    void* getPlayerLocator__8IfPlayerFv();
    void addPressStart__10IfAdvGaugeFPQ34nw4r3g3d12ScnMdlExpand(void* a, void* b);
    void removePressStart__10IfAdvGaugeFPQ34nw4r3g3d12ScnMdlExpand(void* a, void* b);
}

extern "C" void endOperator__26stOperatorAppearanceHelperFv(u8* self) {
    u8* mgr = g_IfAdvMngr;
    if (mgr) {
        mgr[0x48] &= ~0x40;
        if (*(void**)(mgr + 0x104)) {
            u32 flag = (u32)(mgr[0x48] >> 6) & 1u;
            if (flag == 1u) {
                void* r = getPlayerLocator__8IfPlayerFv();
                addPressStart__10IfAdvGaugeFPQ34nw4r3g3d12ScnMdlExpand(*(void**)(mgr + 0x104), r);
            } else {
                void* r = getPlayerLocator__8IfPlayerFv();
                removePressStart__10IfAdvGaugeFPQ34nw4r3g3d12ScnMdlExpand(*(void**)(mgr + 0x104), r);
            }
        }
    }
    self[0x50] = 0;
}
}

