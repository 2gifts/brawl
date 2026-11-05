// muAdvSavePoint: the menu of a save point of the adventure mode. Not written yet.
#include <types.h>
#include <mu/mu_adv_savepoint.h>
#include <if/if_adv_radar.h>
#include <if/if_adv_mngr.h>
#include <snd/snd_system.h>
#include <mu/adv/mu_adv_saveload.h>
#include <mu/mu_adv_selchr_c.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <nw4r/g3d/g3d_scngroup.h>

// muMenuController::main (the map name); include/mu/mu_menu_controller.h calls it by this stand-in name.
extern "C" void fn_800B9984(muMenuController* controller);
// MuMsgFontMngr::loadFont (the map name); src/sora/if/if_player.cpp calls it by this stand-in name.
extern "C" u8 fn_800B8648(void* mgr, u32 fontSetting, int p2, int p3);
// MuMsgFontMngr::unloadFont (the map name); src/sora/if/if_player.cpp calls it by this stand-in name.
extern "C" void fn_800B881C(void* mgr, u8 fontId);
extern u8 lbl_8049E01C[];

extern IfAdvRadar* lbl_805A0348;

void muAdvSavePoint::processDefault() {
    fn_800B9984(&unk150);
    switch (unk44) {
    case 0:
        unk44 = 178;
    case 178:
        if (unk20E) goto selectCharacters;
        unk44 = 188;
        break;
    case 188:
        g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
        g_gfSceneRoot->unk38C[8].unk4->Insert(g_gfSceneRoot->unk38C[8].unk4->sceneItemsCount,
            static_cast<nw4r::g3d::ScnObj*>(unk228.get()));
        lbl_805A0348->show();
        unk224->setFrameNode(0.0f);
        unk44 = 193;
        break;
    case 193:
        if (unk224->isNodeAnimFinished()) goto ready;
        break;
    save:
        unk44 = 203;
        break;
    case 203: {
        unk1F8 = fn_800B8648(lbl_8049E01C, 1, 43, 0);
        muAdvSaveTaskParam param;
        param.unk0 = unk48;
        unk1FC.reset(muAdvSaveTask::create(&param, Heaps::MenuInstance, 43));
        unk228->DetachFromParent();
        lbl_805A0348->hide();
        unk44 = 221;
        break;
    }
    case 221:
        g_IfAdvMngr->setCameraResAnm();
        if (static_cast<muAdvSaveTask*>(unk1FC.get())->unk1B8 != 1) break;
        g_gfSceneRoot->unk38C[8].unk4->Insert(g_gfSceneRoot->unk38C[8].unk4->sceneItemsCount,
            static_cast<nw4r::g3d::ScnObj*>(unk228.get()));
        lbl_805A0348->show();
        unk224->setFrameNode(0.0f);
        unk1FC.reset(NULL);
        fn_800B881C(lbl_8049E01C, unk1F8);
        unk44 = 243;
        break;
    case 243:
        unk44 = 244;
        break;
    case 244:
        g_gfApplication->unkD0.endKeepScreen();
        goto ready;
    selectCharacters:
        unk44 = 255;
        break;
    case 255:
        unk200.reset(muAdvSelchrCTask::create(unk204, Heaps::MenuInstance, 43));
        unk228->DetachFromParent();
        lbl_805A0348->hide();
        unk44 = 264;
        break;
    case 264:
        switch (static_cast<muAdvSelchrCTask*>(unk200.get())->m_state) {
        case 0:
        case 1:
            return;
        case 2:
            unk4C = 7;
            goto finish;
        case 4:
            unk4C = 6;
            goto finish;
        case 3:
        default:
            break;
        }
        g_gfSceneRoot->unk38C[8].unk4->Insert(g_gfSceneRoot->unk38C[8].unk4->sceneItemsCount,
            static_cast<nw4r::g3d::ScnObj*>(unk228.get()));
        lbl_805A0348->show();
        unk224->setFrameNode(0.0f);
        static_cast<muAdvSelchrCTask*>(unk200.get())->initProc();
        unk200.reset(NULL);
    ready:
        unk44 = 303;
        break;
    case 303: {
        moveCursor();
        int state = checkState();
        if (state == 5) goto save;
        if (state == 7) goto selectCharacters;
        if (state != 0) {
            unk4C = state;
        finish:
            unk44 = 324;
        }
        break;
    }
    case 324:
        break;
    }
}

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
