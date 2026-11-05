#include <st/loader/st_loader_info_stage_edit.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>
#include <if/if_stgedit_mngr.h>

class IfStgeditMngr;

extern IfStgeditMngr* lbl_805A0350;
extern "C" {
void fn_8010F4AC(IfStgeditMngr* mngr, int unk);
bool fn_8010F624(IfStgeditMngr* mngr);
void fn_8010F754(IfStgeditMngr* mngr);
void fn_8010F7EC(IfStgeditMngr* mngr);
void fn_8010F8FC(IfStgeditMngr* mngr);
}

static stLoaderSandBagMarker sMarkerA(0xFF, 0);
static stLoaderSandBagMarker sMarkerB(0xFF, 1);

stLoaderInfoStageEdit::stLoaderInfoStageEdit() {
    m_isVisible = false;
    m_isModelCreated = false;
    m_isVisibleZoomHelp = false;
}

stLoaderInfoStageEdit::~stLoaderInfoStageEdit() { }

void stLoaderInfoStageEdit::processBegin() {
    if ((u8)_0x45[1] != 0) {
        switch (m_state) {
            case 0:
                if ((u8)_0x45[2] != 1) {
                    m_state = 1;
                }
                break;
            case 1:
                fn_8010F4AC(lbl_805A0350, 1);
                m_state = 2;
                break;
            case 2:
                if (fn_8010F624(lbl_805A0350)) {
                    m_state = 3;
                }
                break;
            case 3:
                if ((u8)_0x45[2] == 1) {
                    m_state = 5;
                }
                break;
            case 5:
                removeEntity();
                fn_8010F754(lbl_805A0350);
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else {
                    m_state = 0;
                }
                break;
        }
    }
}

void stLoaderInfoStageEdit::entryEntity() {
    if (lbl_805A0350 != NULL) {
        if (!m_isModelCreated) {
            fn_8010F7EC(lbl_805A0350);
            fn_8010F8FC(lbl_805A0350);
            m_isModelCreated = true;
        }
        lbl_805A0350->setVisible(m_isVisible);
        lbl_805A0350->setVisibleZoomHelp(m_isVisibleZoomHelp);
    }
}

void stLoaderInfoStageEdit::removeEntity() {
    if (lbl_805A0350 != NULL && m_isModelCreated == true) {
        lbl_805A0350->destroyModel();
        m_isModelCreated = false;
    }
}
