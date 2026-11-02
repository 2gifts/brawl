#include <types.h>

#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_syssave_task.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <string.h>
#include <ut/ut_relocate.h>

#include <mu/mu_proc_sound_test.h>

// The background: comes in, goes out.
static MuAnimIdxData sBackAnims[2] = {
    { 0.0f, 19.0f, 0, 0, 0x9 },
    { 20.0f, 39.0f, 0, 0, 0x9 },
};
// The two panels.
static MuAnimIdxData sPanelAnims[7] = {
    { 0.0f, 9.0f, 0, 2, 0x11 },     // the music panel comes in
    { 0.0f, 9.0f, 0, 0, 0x11 },     // the sound effect panel comes in (behind)
    { 0.0f, 129.0f, 1, 5, 0x11 },   // the music panel while a track plays
    { 0.0f, 29.0f, 0, 4, 0x11 },    // the panels change places
    { 0.0f, 9.0f, 0, 3, 0x11 },     // goes out
    { 0.0f, 9.0f, 0, 1, 0x11 },     // goes out (behind)
    { 0.0f, 19.0f, 0, 5, 0x11 },    // a sound effect plays
};

muProcSoundTest::muProcSoundTest(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* seriesMsgData, void* titleMsgData,
                                 gfArchive* archive) : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(5, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(Msg_Buf, MsgCount);
    m_seriesMsgData = seriesMsgData;
    m_titleMsgData = titleMsgData;
    m_infoMsgData = archive->getData(Data_Type_Misc, 8, 0xFFFE);
    m_altMsgData = archive->getData(Data_Type_Misc, 4, 0xFFFE);
    m_creditMsgData = archive->getData(Data_Type_Misc, 5, 0xFFFE);
    m_seCategoryMsgData = archive->getData(Data_Type_Misc, 6, 0xFFFE);
    int size;
    void* seList = archive->getData(Data_Type_Misc, 7, &size, 0xFFFE);
    utRelocate relocate(static_cast<u8*>(seList), size);
    m_seList = static_cast<sndSeListData*>(relocate.getPublicAddress("sndSeListData"));
    for (m_seCategoryCount = 0; m_seList[m_seCategoryCount].m_msgIndex != -1; m_seCategoryCount++) {
        m_seCount[m_seCategoryCount] = 0;
        for (u32 i = 0; i < m_seList[m_seCategoryCount].m_rangeCount / 2; i++) {
            m_seCount[m_seCategoryCount] += m_seList[m_seCategoryCount].m_ranges[i * 2 + 1] -
                                            m_seList[m_seCategoryCount].m_ranges[i * 2] + 1;
        }
    }

    for (int i = 0; i < MsgCount; i++) {
        m_msg->setFontWidthModeAuto(i);
    }
    m_msg->attachScnMdlSimple(Msg_Title, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Music]->m_sceneModel), 0,
                              1.0f / 16.0f);
    m_msg->setAlignMode(Msg_Title, MuMsg::Align_Center);
    m_msg->setFontColor(Msg_Title, 0xF0, 0xF0, 0xF0, 0xFF);
    m_msg->attachScnMdlSimple(Msg_TrackCount, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Music]->m_sceneModel),
                              1, 1.0f / 16.0f);
    m_msg->setAlignMode(Msg_TrackCount, MuMsg::Align_Center);
    m_msg->setFontColor(Msg_TrackCount, 0x03, 0x35, 0x66, 0xFF);
    m_msg->attachScnMdlSimple(Msg_Credits, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Music]->m_sceneModel), 2,
                              0.05f);
    m_msg->setAlignMode(Msg_Credits, MuMsg::Align_Left);
    m_msg->setFontColor(Msg_Credits, 0xE9, 0xE9, 0xE9, 0xFF);
    m_msg->getWScale(Msg_Credits, 49.0f);
    m_msg->attachScnMdlSimple(Msg_Series, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Music]->m_sceneModel), 3,
                              1.0f / 16.0f);
    m_msg->setAlignMode(Msg_Series, MuMsg::Align_Center);
    m_msg->setFontColor(Msg_Series, 0xF0, 0xF0, 0xF0, 0xFF);
    m_msg->attachScnMdlSimple(Msg_Info, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Music]->m_sceneModel), 4,
                              1.0f / 16.0f);
    m_msg->setAlignMode(Msg_Info, MuMsg::Align_Center);
    m_msg->setFontColor(Msg_Info, 0x03, 0x35, 0x66, 0xFF);
    m_msg->attachScnMdlSimple(Msg_SeCategory, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Se]->m_sceneModel), 0,
                              1.0f / 16.0f);
    m_msg->setAlignMode(Msg_SeCategory, MuMsg::Align_Center);
    m_msg->setFontColor(Msg_SeCategory, 0xF0, 0xF0, 0xF0, 0xFF);
    m_msg->attachScnMdlSimple(Msg_SeNumber, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Se]->m_sceneModel), 1,
                              0.125f);
    m_msg->setAlignMode(Msg_SeNumber, MuMsg::Align_Center);
    m_msg->setFontColor(Msg_SeNumber, 0xF0, 0xF0, 0xF0, 0xFF);
}

void muProcSoundTest::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk664 = Step_Music;
    m_cursor = 0;
    m_isPlayingBGM = false;
    m_bgmPlayId = 0;
    m_seHandle = -1;
    m_series = 0;
    m_track = 0;
    m_seCategory = 0;
    m_seIndex = 0;
    m_unk680 = -1;
    if (static_cast<u32>(from) == 6) {
        m_code = 6;
    } else {
        // Back from another screen of the Vault: save what changed
        m_code = 0xB;
        if (gmIsAutosaveDirtyFlag()) {
            gmSysSaveTask::create(0x2E);
        }
    }
    gmResetAutosaveDirtyFlag();
    m_panel->changeProcess1(0x22, 0);
    m_result = m_code;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Back]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Music]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Se]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_State]->m_sceneModel));

    setAnimFrame(Obj_Back, 1.0f, 4);
    m_objs[Obj_Back]->setAnimIdx(&sBackAnims[0], false);
    m_objs[Obj_Music]->changeAnimN(0, 2, false, false);
    m_objs[Obj_Music]->setFrameVisible(2.0f);
    m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[0], true);
    m_objs[Obj_Se]->changeAnimN(0, 2, false, false);
    m_objs[Obj_Se]->setFrameVisible(2.0f);
    m_objs[Obj_Se]->setAnimIdx(&sPanelAnims[1], true);
    m_objs[Obj_State]->changeAnimN(0, 0x16, false, false);
    m_objs[Obj_State]->changeAnimN(0, 1, true, false);
    m_objs[Obj_State]->setFrameVisible(2.0f);
    g_gfPadSystem->getSysPadStatus(0xF0, &m_pad);
    printBgmData();
    showSeCategory();
    g_sndSystem->stopBGM(0x1E);
    lbl_805A01D8->setupBGMList();
}

int muProcSoundTest::proc() {
    gfPadStatus pad;
    if (m_unk664 == Step_Music || static_cast<u32>(m_unk664) == Step_Se) {
        // The icon of the buttons follows the kind of the controller
        g_gfPadSystem->getSysPadStatus(getFixedControllerID(), &pad);
        MuObject* state = m_objs[Obj_State];
        float frame;
        if (pad.m_error != gfPadError::NONE) {
            frame = -1.0f;
        } else {
            switch (pad.m_controllerType) {
            case gfPadType::GCC:
                frame = 0.0f;
                break;
            case gfPadType::WII_CLASSIC:
                frame = 2.0f;
                break;
            case gfPadType::WIIMOTE:
                frame = 3.0f;
                break;
            case gfPadType::NUNCHUK:
                frame = 1.0f;
                break;
            default:
                frame = 4.0f;
                break;
            }
        }
        if (frame >= 0.0f) {
            state->setFrameFlg(frame, 4);
        }
    }
    switch (m_unk664) {
    case Step_Music:
        if (selBGMProc(padUpdate())) {
            m_unk664 = Step_Leave;
            return 1;
        }
        break;
    case Step_Se:
        if (selectProc(padUpdate())) {
            m_unk664 = Step_Leave;
            return 1;
        }
        break;
    case Step_LoadSe:
        if (g_sndSystem->isFinishLoad()) {
            m_unk664 = Step_Se;
            m_objs[Obj_State]->setFrameVisible(3.0f);
        }
        break;
    case Step_Leave:
        if (m_objs[Obj_Music]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcSoundTest::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Music]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Se]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_State]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Back]->m_sceneModel));
}

// The sound effect page.
bool muProcSoundTest::selectProc(int pad) {
    if (m_seHandle != -1 && g_sndSystem->isPlay(m_seHandle)) {
        // HYPOTHESIS: the original reads the update rate of the panel and does nothing with it.
        m_objs[Obj_Se]->m_modelAnim->m_anmObjChrRes->GetUpdateRate();
    }
    if (pad & (Pad_Up | Pad_Cancel)) {
        if (m_cursor != 0) {
            m_cursor--;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            if (m_cursor == 1) {
                // Off the effects: they stop and their sound groups go
                if (m_seHandle != -1 && g_sndSystem->isPlay(m_seHandle)) {
                    g_sndSystem->stopSE(m_seHandle, 0);
                }
                m_seHandle = -1;
                g_sndSystem->freeGroup(0, SeGroupHeap);
            }
            m_objs[Obj_Se]->setFrameVisible((m_cursor + 2) % 3);
            m_objs[Obj_State]->setFrameVisible(2.0f);
        } else if (pad & Pad_Cancel) {
            // Back to the Vault
            if (!m_isPlayingBGM || g_sndSystem->getBGMId() == -1) {
                lbl_805A01D8->playBGM(0x2A, 0, 0);
            }
            m_objs[Obj_Back]->setAnimIdx(&sBackAnims[1], false);
            m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[5], true);
            m_objs[Obj_Se]->setAnimIdx(&sPanelAnims[4], true);
            m_objs[Obj_State]->changeAnimN(1, 1, true, false);
            if (m_code == 0xB) {
                m_result = 0xB;
            } else {
                m_result = 6;
            }
            return true;
        }
    } else if (pad & (Pad_Down | Pad_A)) {
        if (m_cursor < 2) {
            m_cursor++;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            if (m_cursor == 2) {
                // On the effects: the sound groups of the category load
                for (u32 i = 0; i < m_seList[m_seCategory].m_groupCount; i++) {
                    switch (m_seList[m_seCategory].m_groups[i]) {
                    case -1:
                    case 0:
                        break;
                    default:
                        g_sndSystem->loadSoundGroup(static_cast<SndGroupID>(m_seList[m_seCategory].m_groups[i]),
                                                    SeGroupHeap, true);
                        m_seHandle = -1;
                        break;
                    }
                }
                m_unk664 = Step_LoadSe;
            }
            m_objs[Obj_Se]->setFrameVisible((m_cursor + 2) % 3);
        } else if (pad & Pad_Ok) {
            // Plays the effect
            if (m_seHandle != -1 && g_sndSystem->isPlay(m_seHandle)) {
                g_sndSystem->stopSE(m_seHandle, 0);
                m_seHandle = -1;
            }
            m_seHandle = g_sndSystem->playSE(static_cast<SndID>(m_seId), -1, 0, 0, -1);
            m_objs[Obj_Se]->setAnimIdx(&sPanelAnims[6], true);
        }
    } else if (pad & Pad_Left) {
        switch (m_cursor) {
        case 0:
            // To the music page
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_unk664 = Step_Music;
            m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[3], true);
            m_objs[Obj_Se]->setAnimIdx(&sPanelAnims[1], true);
            m_objs[Obj_Se]->setFrameFlg(9.0f, 0x11);
            if (m_isPlayingBGM) {
                m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[2], true);
            }
            printBgmData();
            break;
        case 1:
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_seIndex = 0;
            do {
                if (--m_seCategory < 0) {
                    m_seCategory = SeCategoryCount - 1;
                }
            } while (!isEnableSEKategorie(m_seCategory));
            m_seId = m_seList[m_seCategory].m_ranges[0];
            showSeCategory();
            break;
        case 2:
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            if (m_seIndex == 0) {
                m_seIndex = m_seCount[m_seCategory] - 1;
            } else {
                m_seIndex--;
            }
            showSeCategory();
            setPlaySEId(false);
            break;
        }
    } else if (pad & Pad_Right) {
        switch (m_cursor) {
        case 1:
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_seIndex = 0;
            do {
                if (++m_seCategory >= SeCategoryCount) {
                    m_seCategory = 0;
                }
            } while (!isEnableSEKategorie(m_seCategory));
            m_seId = m_seList[m_seCategory].m_ranges[0];
            showSeCategory();
            break;
        case 2:
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_seIndex = (m_seIndex + 1) % m_seCount[m_seCategory];
            showSeCategory();
            setPlaySEId(true);
            break;
        }
    } else if (pad & Pad_Start) {
        // Everything stops
        if (m_seHandle != -1 && g_sndSystem->isPlay(m_seHandle)) {
            g_sndSystem->stopSE(m_seHandle, 0x3C);
            m_seHandle = -1;
        }
        g_sndSystem->stopBGM(0x78);
        m_isPlayingBGM = false;
        m_bgmPlayId = 0;
        m_objs[Obj_Music]->setUpdateRateFlg(0.0f, 0x11);
    }
    return false;
}

bool muProcSoundTest::selBGMProc(int pad) {
    if (pad & (Pad_Up | Pad_Cancel)) {
        if (m_cursor != 0) {
            m_cursor--;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_objs[Obj_Music]->setFrameVisible((m_cursor + 2) % 3);
            if (m_cursor == 1) {
                m_objs[Obj_State]->setFrameVisible(2.0f);
            }
        } else if (pad & Pad_Cancel) {
            // Back to the Vault
            if (!m_isPlayingBGM || g_sndSystem->getBGMId() == -1) {
                lbl_805A01D8->playBGM(0x2A, 0, 0);
            }
            m_objs[Obj_Back]->setAnimIdx(&sBackAnims[1], false);
            m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[4], true);
            m_objs[Obj_Se]->setAnimIdx(&sPanelAnims[5], true);
            m_objs[Obj_State]->changeAnimN(1, 1, true, false);
            if (m_code == 0xB) {
                m_result = 0xB;
            } else {
                m_result = 6;
            }
            return true;
        }
    } else if (pad & (Pad_Down | Pad_A)) {
        if (m_cursor < 2) {
            m_cursor++;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_objs[Obj_Music]->setFrameVisible((m_cursor + 2) % 3);
            if (m_cursor == 2) {
                m_objs[Obj_State]->setFrameVisible(2.0f);
            }
        } else if (pad & Pad_A) {
            if (m_bgmPlayId == lbl_805A01D8->getSeriesTitleData(m_series, m_track)->m_playId) {
                // A again on the track that plays: it stops
                g_sndSystem->stopBGM(0);
                m_isPlayingBGM = false;
                m_bgmPlayId = 0;
                m_objs[Obj_Music]->setUpdateRateFlg(0.0f, 0x11);
                m_objs[Obj_State]->setFrameVisible(0.0f);
            } else {
                if (m_isPlayingBGM) {
                    g_sndSystem->stopBGM(0);
                    m_isPlayingBGM = false;
                }
                m_bgmPlayId = lbl_805A01D8->getSeriesTitleData(m_series, m_track)->m_playId;
                g_sndSystem->playBGM(m_bgmPlayId, 0, true);
                m_isPlayingBGM = true;
                m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[2], true);
                m_objs[Obj_State]->setFrameVisible(1.0f);
            }
        }
    } else if (pad & Pad_Left) {
        switch (m_cursor) {
        case 1:
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            moveSeries(-1);
            m_track = 0;
            break;
        case 2:
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            if (m_track == 0) {
                moveSeries(-1);
                m_track = lbl_805A01D8->getSeriesTitleCount(m_series) - 1;
            } else {
                m_track--;
            }
            break;
        }
    } else if (pad & Pad_Right) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        switch (m_cursor) {
        case 0:
            // To the sound effect page
            m_unk664 = Step_Se;
            m_seId = m_seList[m_seCategory].m_ranges[0];
            m_seIndex = 0;
            showSeCategory();
            m_objs[Obj_Se]->setAnimIdx(&sPanelAnims[3], true);
            m_objs[Obj_Music]->setAnimIdx(&sPanelAnims[1], true);
            m_objs[Obj_Music]->setFrameFlg(9.0f, 0x11);
            m_objs[Obj_State]->setFrameMatCol(0.0f);
            m_objs[Obj_State]->setFrameVisible(2.0f);
            return false;
        case 1:
            moveSeries(1);
            m_track = 0;
            break;
        case 2:
            if (lbl_805A01D8->getSeriesTitleCount(m_series) <= ++m_track) {
                moveSeries(1);
                m_track = 0;
            }
            break;
        }
    } else if (pad & Pad_Start) {
        // The music stops
        g_sndSystem->stopBGM(0x78);
        m_isPlayingBGM = false;
        m_bgmPlayId = 0;
        m_objs[Obj_Music]->setUpdateRateFlg(0.0f, 0x11);
        m_objs[Obj_State]->setFrameVisible(0.0f);
    }
    printBgmData();
    return false;
}

void muProcSoundTest::printBgmData() {
    gfPadStatus status;
    char text[Msg_Buf];
    int length;
    int pos;
    char* line;
    sndBgmTitleData* title = lbl_805A01D8->getSeriesTitleData(m_series, m_track);

    // HYPOTHESIS: while a pad holds the button (X; 1 on a Wii Remote, Z on a Nunchuk), the other text of the track shows.
    bool showAlt = false;
    for (int i = 0; i < 8; i++) {
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_controllerType == gfPadType::WIIMOTE) {
            if (status.m_buttonsCurrentFrame2.bits & 0x200000) {
                showAlt = true;
            }
        } else if (status.m_controllerType == gfPadType::NUNCHUK) {
            if (status.m_buttonsCurrentFrame2.bits & 0x10000) {
                showAlt = true;
            }
        } else if (status.m_buttonsCurrentFrame2.bits & 0x400) {
            showAlt = true;
        }
    }

    setAnimFrame(Obj_Music, getIconFrame(title->unk8), 4);
    if (m_cursor == 2) {
        if (m_bgmPlayId == title->m_playId) {
            m_objs[Obj_State]->setFrameVisible(1.0f);
        } else {
            m_objs[Obj_State]->setFrameVisible(0.0f);
        }
    } else {
        m_objs[Obj_State]->setFrameVisible(4.0f);
    }
    m_objs[Obj_State]->setFrameMatCol(title->m_unkC ? 1.0f : 0.0f);
    m_msg->printIndex(Msg_Series, m_series, m_seriesMsgData);
    m_msg->printIndex(Msg_Title, title->m_msgIndex, m_titleMsgData);
    printNumber(Msg_TrackCount, lbl_805A01D8->m_enabledTitleCount);

    if (showAlt) {
        if (title->m_altMsgIndex != -1) {
            m_msg->printf(Msg_Info, "");
            m_msg->setMsgData(m_altMsgData);
            m_msg->printIndex(Msg_Credits, title->m_altMsgIndex, NULL);
        }
        return;
    }

    // The game: a heading and the name
    m_msg->setMsgData(m_seriesMsgData);
    const char* data = m_msg->getMsgData(0x15, &length);
    memcpy(text, data, length);
    int headLength = length;
    m_msg->setMsgData(m_infoMsgData);
    data = m_msg->getMsgData(title->m_infoMsgIndex, &length);
    memcpy(text + headLength, data, length);
    m_msg->printData(Msg_Info, text, headLength + length);

    // The credits, one line each
    pos = 0;
    memset(text, 0, sizeof(text));
    if (title->m_creditMsgIndex[0] != -1) {
        m_msg->setMsgData(m_creditMsgData);
        data = m_msg->getMsgData(title->m_creditMsgIndex[0], &length);
        memcpy(text + pos, data, length);
        text[pos + length] = '\n';
        pos += length + 1;
    }
    if (title->m_creditMsgIndex[1] != -1) {
        m_msg->setMsgData(m_creditMsgData);
        data = m_msg->getMsgData(title->m_creditMsgIndex[1], &length);
        line = text + pos;
        memcpy(line, data, length);
        line[length] = '\n';
        pos += length + 1;
    }
    if (title->m_creditMsgIndex[2] != -1) {
        m_msg->setMsgData(m_creditMsgData);
        data = m_msg->getMsgData(title->m_creditMsgIndex[2], &length);
        line = text + pos;
        memcpy(line, data, length);
        line[length] = '\n';
        pos += length + 1;
    }
    if (title->m_creditMsgIndex[3] != -1) {
        m_msg->setMsgData(m_creditMsgData);
        data = m_msg->getMsgData(title->m_creditMsgIndex[3], &length);
        line = text + pos;
        memcpy(line, data, length);
        line[length] = '\n';
        pos += length + 1;
    }
    if (title->m_creditMsgIndex[4] != -1) {
        m_msg->setMsgData(m_creditMsgData);
        data = m_msg->getMsgData(title->m_creditMsgIndex[4], &length);
        memcpy(text + pos, data, length);
        pos += length;
    } else if (pos != 0) {
        // No line after the last one
        *(text + pos - 1) = '\0';   // BYTES: text[pos - 1] adds the index before the offset
        pos--;
    }
    m_msg->printData(Msg_Credits, text, pos);
}

// Moves the sound id under the cursor to the next or the previous one, through the ranges of the category.
void muProcSoundTest::setPlaySEId(bool next) {
    for (u32 i = 0; i < m_seList[m_seCategory].m_rangeCount / 2; i++) {
        if (m_seList[m_seCategory].m_ranges[i * 2] <= m_seId && m_seList[m_seCategory].m_ranges[i * 2 + 1] >= m_seId) {
            if (next) {
                if (m_seId == m_seList[m_seCategory].m_ranges[i * 2 + 1]) {
                    // Past the end of a range: the first of the next one, or of the first one
                    if (i * 2 + 2 == m_seList[m_seCategory].m_rangeCount) {
                        m_seId = m_seList[m_seCategory].m_ranges[0];
                    } else {
                        m_seId = m_seList[m_seCategory].m_ranges[i * 2 + 2];
                    }
                } else {
                    m_seId++;
                }
            } else {
                if (m_seId == m_seList[m_seCategory].m_ranges[i * 2]) {
                    // Before the start of a range: the last of the previous one, or of the last one
                    if (i == 0) {
                        m_seId = m_seList[m_seCategory].m_ranges[m_seList[m_seCategory].m_rangeCount - 1];
                    } else {
                        m_seId = m_seList[m_seCategory].m_ranges[i * 2 - 1];
                    }
                } else {
                    m_seId--;
                }
            }
            return;
        }
    }
}

// Whether the category of the sound effects can be picked: the effects of a fighter, a stage or a mode show once it is
// unlocked.
// HYPOTHESIS: what each category is follows from the check it makes.
bool muProcSoundTest::isEnableSEKategorie(int category) {
    switch (category) {
    case 3:
        return gmCheckChKindUseEnable(Character_Mario);
    case 4:
        return gmCheckChKindUseEnable(Character_DonkeyKong);
    case 5:
        return gmCheckChKindUseEnable(Character_Link);
    case 6:
        return gmCheckChKindUseEnable(Character_Samus);
    case 7:
        return gmCheckChKindUseEnable(Character_Yoshi);
    case 8:
        return gmCheckChKindUseEnable(Character_Kirby);
    case 10:
        return gmCheckChKindUseEnable(Character_Fox);
    case 11:
        return gmCheckChKindUseEnable(Character_Pikachu);
    case 12:
        return gmCheckChKindUseEnable(Character_Bowser);
    case 13:
        return gmCheckChKindUseEnable(Character_Peach);
    case 14:
        return gmCheckChKindUseEnable(Character_Zelda);
    case 15:
        return gmCheckChKindUseEnable(Character_IceClimbers);
    case 16:
        return gmCheckChKindUseEnable(Character_Wario);
    case 17:
        return gmCheckChKindUseEnable(Character_MetaKnight);
    case 18:
        return gmCheckChKindUseEnable(Character_Pit);
    case 19:
        return gmCheckChKindUseEnable(Character_Olimar);
    case 20:
        return gmCheckChKindUseEnable(Character_Lucas);
    case 21:
        return gmCheckChKindUseEnable(Character_DiddyKong);
    case 22:
        return gmCheckChKindUseEnable(Character_PokeLizardon_Trainer);
    case 23:
        return gmCheckChKindUseEnable(Character_KingDedede);
    case 24:
        return gmCheckChKindUseEnable(Character_Ike);
    case 25:
        return gmCheckChKindUseEnable(Character_Luigi);
    case 26:
        return gmCheckChKindUseEnable(Character_CaptainFalcon);
    case 27:
        return gmCheckChKindUseEnable(Character_Ness);
    case 28:
        return gmCheckChKindUseEnable(Character_Marth);
    case 29:
        return gmCheckChKindUseEnable(Character_MrGameAndWatch);
    case 30:
        return gmCheckChKindUseEnable(Character_Falco);
    case 31:
        return gmCheckChKindUseEnable(Character_Ganondorf);
    case 32:
        return gmCheckChKindUseEnable(Character_Lucario);
    case 33:
        return gmCheckChKindUseEnable(Character_ROB);
    case 36:
        return gmCheckChKindUseEnable(Character_ToonLink);
    case 37:
        return gmCheckChKindUseEnable(Character_Wolf);
    case 38:
        return gmCheckChKindUseEnable(Character_Snake);
    case 39:
        return gmCheckChKindUseEnable(Character_Sonic);
    case 45:
        return gmCheckSrStageKind(Stages::DelfinoPlaza);
    case 46:
        return gmCheckSrStageKind(Stages::MushroomyKingdom);
    case 47:
        return gmCheckSrStageKind(Stages::MarioCircuit);
    case 48:
        return gmCheckSrStageKind(Stages::RumbleFalls);
    case 49:
        return gmCheckSrStageKind(static_cast<Stages::srStageKind>(0xA));
    case 50:
        return gmCheckSrStageKind(Stages::Norfair);
    case 51:
        return gmCheckSrStageKind(Stages::FrigateOrpheon);
    case 52:
        return gmCheckSrStageKind(Stages::YoshiIsland);
    case 53:
        return gmCheckSrStageKind(Stages::Halberd);
    case 54:
        return gmCheckSrStageKind(Stages::LylatCruise);
    case 55:
        return gmCheckSrStageKind(Stages::PokemonStadium2);
    case 56:
        return gmCheckSrStageKind(Stages::PortTown);
    case 57:
        return gmCheckSrStageKind(Stages::Summit);
    case 58:
        return gmCheckSrStageKind(Stages::CastleSiege);
    case 59:
        return gmCheckSrStageKind(Stages::WarioWare);
    case 60:
        return gmCheckSrStageKind(Stages::DistantPlanet);
    case 61:
        return gmCheckSrStageKind(Stages::Skyworld);
    case 62:
        return gmCheckSrStageKind(Stages::NewPorkCity);
    case 63:
        return gmCheckSrStageKind(Stages::Smashville);
    case 64:
        return gmCheckSrStageKind(Stages::ShadowMoses);
    case 65:
        return gmCheckSrStageKind(Stages::PictoChat);
    case 66:
        return gmCheckSrStageKind(Stages::LuigiMansion);
    case 67:
        return gmCheckSrStageKind(Stages::_75m);
    case 68:
        return gmCheckSrStageKind(Stages::PirateShip);
    case 69:
        return gmCheckSrStageKind(Stages::SpearPillar);
    case 70:
        return gmCheckSrStageKind(Stages::FlatZone2);
    case 71:
        return gmCheckSrStageKind(Stages::MarioBros);
    case 72:
        return gmCheckSrStageKind(Stages::GreenHillZone);
    case 73:
        return gmCheckSrStageKind(Stages::Hanenbow);
    case 74:
        return gmCheckSrStageKind(Stages::Brinstar);
    case 75:
        return gmCheckSrStageKind(Stages::Corneria);
    case 76:
        return gmCheckSrStageKind(Stages::Onett);
    case 77:
        return gmCheckSrStageKind(Stages::JungleJapes);
    case 78:
        return gmCheckSrStageKind(Stages::GreenGreens);
    case 79:
        return gmCheckSrStageKind(Stages::PokemonStadium);
    case 80:
        return gmCheckSrStageKind(Stages::BigBlue);
    case 34:
        return false;
    case 35:
        return gmCheckChKindUseEnable(Character_Jigglypuff);
    case 9:
        return gmCheckChKindUseEnableAll() != false;
    case 1:
        if (g_GameGlobal->checkHideMenuAllstar() == false) {
            return false;
        } else {
            return g_GameGlobal->checkHideMenuBossBattle() != false;
        }
    case 44:
        {
        // Every assist trophy
        int i;
        int count = 0;
        for (i = 0; i < 6; i++) {
            if (gmCheckAssistKindUseEnable(static_cast<u8>(i)) == true) {
                count++;
            }
        }
        return count >= 6;
    }
    case 93: case 94: case 95: case 96: case 97: case 98: case 99:
        return g_GameGlobal->m_record->m_unk804_31;
    case 100:
        return gmCheckChKindUseEnableAll() != false;
    }
    return true;
}

void muProcSoundTest::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
