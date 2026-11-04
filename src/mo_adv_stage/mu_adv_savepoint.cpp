// muAdvSavePoint: the menu of a save point of the adventure mode. Not written yet.
#include <types.h>
#include <mu/mu_adv_savepoint.h>
#include <if/if_adv_radar.h>
#include <if/if_adv_mngr.h>
#include <snd/snd_system.h>

extern IfAdvRadar* lbl_805A0348;

void muAdvSavePoint::moveCursor() {
    u32 buttons = unk150.m_pressed;
    if (buttons & 0xF) {
        int direction = 2;
        if (buttons & 2) direction = 3;
        else if (buttons & 1) direction = 2;
        else if (buttons & 8) direction = 0;
        else if (buttons & 4) direction = 1;
        int cursor = searchCursor(unk50, direction);
        if (cursor != 7) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            unk50 = cursor;
            unk224->setFrameVisible(static_cast<float>(cursor));
        }
    }
}

int muAdvSavePoint::searchCursor(int cursor, int direction) {
    for (int index = 0; index < 2; ++index) {
        int next = unk54[cursor].unk4[direction][index];
        if (next != 7 && unk54[next].unk0) return next;
    }
    for (u32 index = 0; index < 2; ++index) {
        int next = unk54[cursor].unk4[direction][index];
        if (next != 7) {
            int found = searchCursor(next, direction);
            if (found != 7) return found;
        }
    }
    return 7;
}

int muAdvSavePoint::checkState() {
    int state = 0;
    if (unk150.m_pressed & 0x100) {
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        switch (unk50) {
        case 0: state = 7; break;
        case 1: state = 5; break;
        case 2: state = 2; break;
        case 3: state = 8; break;
        case 4: state = 9; break;
        case 5: state = 10; break;
        case 6: state = 11; break;
        }
    } else if (unk150.m_pressed & 0x200) {
        g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
        state = 1;
    }
    return state;
}

muAdvSavePoint::~muAdvSavePoint() {
    g_IfAdvMngr->exitSavePointMenu();
    lbl_805A0348->hide();
}
