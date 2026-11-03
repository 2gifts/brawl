#include <types.h>

#include <cstdio>
#include <cstring>
#include <gf/gf_model.h>
#include <gm/gm_syssave_task.h>
#include <nt/nt_etc_global.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <revolution/OS.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_wifi_opt.h>

// The animations of the panel: it comes in, it goes out.
static MuAnimIdxData sOptWifiAnims[2] = {
    { 0.0f, 9.0f, 0, 0, 1 },
    { 10.0f, 19.0f, 0, 0, 1 },
};

muProcOptWifi::muProcOptWifi(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData, void* errorMsgData,
                             nw4r::g3d::ResFile* resFile) : muProcMenu(objs, mdls) {
    m_resFile = resFile;
    m_wnd = NULL;
    m_exchange = NULL;
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x400, Msg_ListEnd);
    m_msg->setMsgData(msgData);
    m_errorMsg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_errorMsg->allocMsgBuf(0x400, 1);
    m_errorMsg->setMsgData(errorMsgData);
    // HYPOTHESIS: the titles; the third pass (line 2 on the list) is attached again to the first entry just after.
    for (int i = 0; i <= 2; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Entry0 + i]->m_sceneModel), 0,
                                  1.0f / 12.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Left);
        m_msg->setFontColor(i, 0, 0, 0, 0xFF);
        m_msg->setFontWidthModeAuto(i);
    }
    // The entries
    for (int i = 2; i < 4; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Entry0 - 2 + i]->m_sceneModel), 1,
                                  1.0f / 12.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->setFontColor(i, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msg->setFontWidthModeAuto(i);
        m_msg->setEdgeEnable(i, true);
        m_msg->setEdge(i, 0x3F, 0x39, 0x4F, 3.0f);
    }
    // The lines of the list
    for (int i = Msg_List0; i < Msg_ListEnd; i++) {
        m_msg->printIndex(i, 0xC, NULL);
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_List]->m_sceneModel),
                                  i - Msg_List0, 0.0625f);
        m_msg->setFontColor(i, 0, 0, 0, 0xFF);
    }
}

void muProcOptWifi::init(nw4r::g3d::ScnGroup* group, int from) {
    m_listKind[Entry_Watching][false] = 2;
    m_listKind[Entry_Watching][true] = 2;
    m_listKind[Entry_Service][false] = 2;
    m_listKind[Entry_Service][true] = 3;
    m_listMin[Entry_Watching][false] = 1;
    m_listMin[Entry_Watching][true] = 1;
    m_listMin[Entry_Service][false] = 1;
    m_listMin[Entry_Service][true] = 0;
    m_listMax[Entry_Watching][false] = 2;
    m_listMax[Entry_Watching][true] = 2;
    m_listMax[Entry_Service][false] = 2;
    m_listMax[Entry_Service][true] = 2;
    m_listStart[Entry_Watching][false] = 2;
    m_listStart[Entry_Watching][true] = 1;
    m_listStart[Entry_Service][false] = 2;
    m_listStart[Entry_Service][true] = 0;
    m_listPermit[Entry_Watching][false][0] = false;
    m_listPermit[Entry_Watching][false][1] = true;
    m_listPermit[Entry_Watching][false][2] = false;
    m_listPermit[Entry_Watching][true][0] = false;
    m_listPermit[Entry_Watching][true][1] = true;
    m_listPermit[Entry_Watching][true][2] = false;
    m_listPermit[Entry_Service][false][0] = false;
    m_listPermit[Entry_Service][false][1] = true;
    m_listPermit[Entry_Service][false][2] = false;
    m_listPermit[Entry_Service][true][0] = true;
    m_listPermit[Entry_Service][true][1] = false;
    m_listPermit[Entry_Service][true][2] = false;
    m_listDLTask[false][0] = 0;
    m_listDLTask[false][1] = 0;
    m_listDLTask[false][2] = 3;
    m_listDLTask[true][0] = 3;
    m_listDLTask[true][1] = 1;
    m_listDLTask[true][2] = 2;
    m_listMsg[Entry_Watching][false][0] = 0xC;
    m_listMsg[Entry_Watching][false][1] = 0xE;
    m_listMsg[Entry_Watching][false][2] = 0x15;
    m_listMsg[Entry_Watching][false][3] = 0x16;
    m_listMsg[Entry_Watching][false][4] = 0x17;
    m_listMsg[Entry_Watching][true][0] = 0xC;
    m_listMsg[Entry_Watching][true][1] = 0xF;
    m_listMsg[Entry_Watching][true][2] = 0x12;
    m_listMsg[Entry_Watching][true][3] = 0x13;
    m_listMsg[Entry_Watching][true][4] = 0x14;
    m_listMsg[Entry_Service][false][0] = 0xD;
    m_listMsg[Entry_Service][false][1] = 0x11;
    m_listMsg[Entry_Service][false][2] = 0x1B;
    m_listMsg[Entry_Service][false][3] = 0x1C;
    m_listMsg[Entry_Service][false][4] = 0x1D;
    m_listMsg[Entry_Service][true][0] = 0xD;
    m_listMsg[Entry_Service][true][1] = 0x10;
    m_listMsg[Entry_Service][true][2] = 0x18;
    m_listMsg[Entry_Service][true][3] = 0x19;
    m_listMsg[Entry_Service][true][4] = 0x1A;
    m_wnd = NULL;
    m_needSave = false;
    m_error = false;
    m_frame = 0;
    m_listShown = false;
    m_choice = 0;
    m_choiceMin = 0;
    m_choiceMax = 3;
    m_group = group;
    m_unk664 = Step_Select;
    m_cursor = Entry_Watching;
    m_permit[Entry_Watching] = nteGlobal::isPermitWatching();
    m_permit[Entry_Service] = nteGlobal::isPermitService();
    m_savedPermit[Entry_Service] = m_permit[Entry_Service];
    m_savedPermit[Entry_Watching] = m_permit[Entry_Watching];
    const char* nodes[EntryCount] = { "pos00", "pos01" };
    m_entryCount = 0;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Entries]));
    m_objs[Obj_Panel]->setAnimIdx(&sOptWifiAnims[0], true);
    nteGlobal::updateButtonCondition();
    if (nteGlobal::isEnableButton(0x20) == true) {
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Entries])
            ->PushBack(m_objs[Obj_Entry0 + Entry_Watching]->m_sceneModel, nodes[m_entryCount]);
        m_objs[Obj_Entry0 + Entry_Watching]->changeAnimN(0, 0x12, false, false);
        m_objs[Obj_Entry0 + Entry_Watching]->setFrameMatCol(m_permit[Entry_Watching]);
        m_objs[Obj_Entry0 + Entry_Watching]->setFrameVisible(1.0f);
        m_entryCount++;
    }
    if (nteGlobal::isEnableButton(0x40) == true) {
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Entries])
            ->PushBack(m_objs[Obj_Entry0 + Entry_Service]->m_sceneModel, nodes[m_entryCount]);
        m_objs[Obj_Entry0 + Entry_Service]->changeAnimN(0, 0x12, false, false);
        m_objs[Obj_Entry0 + Entry_Service]->setFrameMatCol(m_permit[Entry_Service]);
        m_objs[Obj_Entry0 + Entry_Service]->setFrameVisible(m_entryCount == 0 ? 1.0f : 0.0f);
        // Without the first entry the cursor starts on this one
        m_cursor = m_entryCount == 0;
        m_entryCount++;
    }
    m_panel->changeProcess1(0x1F, 0x96 + m_cursor * 2 + (m_cursor == Entry_Watching ? m_permit[Entry_Watching] == false
                                                                                    : m_permit[Entry_Service] == false));
    m_msg->printIndex(0, 0, NULL);
    m_msg->printIndex(1, 1, NULL);
    m_msg->printIndex(2, m_permit[Entry_Watching] ? 2 : 3, NULL);
    m_msg->printIndex(3, m_permit[Entry_Service] ? 4 : 5, NULL);
}

int muProcOptWifi::proc() {
    u32 pad = padUpdate();
    switch (m_unk664) {
    case Step_List: {
        int prev = m_choice;
        if (pad & Pad_Up) {
            m_choice--;
        }
        if (pad & Pad_Down) {
            m_choice++;
        }
        if (m_choiceMax < m_choice) {
            m_choice = m_choiceMin;
        }
        if (m_choice < m_choiceMin) {
            m_choice = m_choiceMax;
        }
        if (prev != m_choice) {
            g_sndSystem->playSE(snd_se_system_cursor, -1, 0, 0, -1);
        }
        m_objs[Obj_List]->setFrameMatCol(m_choice);
        if ((pad & Pad_A) || (pad & Pad_Cancel)) {
            bool changed = false;
            if (pad & Pad_A) {
                g_sndSystem->playSE(snd_se_system_fixed, -1, 0, 0, -1);
                if (m_cursor == Entry_Watching) {
                    m_permit[Entry_Watching] = m_listPermit[m_cursor][m_permit[Entry_Watching]][m_choice];
                    if (m_savedPermit[Entry_Watching] != m_permit[Entry_Watching]) {
                        gmSysSaveTask::create(0x2E);
                    }
                } else {
                    m_permit[Entry_Service] = m_listPermit[m_cursor][m_permit[Entry_Service]][m_choice];
                    // BYTES: the original computes the flag and compares the two again for the branch (cmplw
                    // next to the subf/or/srwi of the flag); `if (changed)` branches on the flag (95.1 vs 95.6).
                    changed = m_permit[Entry_Service] != m_savedPermit[Entry_Service];
                    if (m_savedPermit[Entry_Service] != m_permit[Entry_Service]) {
                        m_needSave = true;
                    }
                }
                nteGlobal::setPermitWatching(m_permit[Entry_Watching]);
                nteGlobal::setPermitService(m_permit[Entry_Service]);
                if (changed == true) {
                    m_unk664 = Step_Exchange;
                    return 0;
                }
            } else if (pad & Pad_Cancel) {
                g_sndSystem->playSE(snd_se_system_cancel, -1, 0, 0, -1);
            }
            removeModelMessage();
            savePermit();
            m_unk664 = Step_Select;
        }
        return 0;
    }
    case Step_Exchange:
        m_exchange = new (Heaps::MenuInstance) nteExchange(Heaps::MenuInstance);
        m_exchange->setErrorHandler(this);
        if (m_exchange->_requestJob(4, 0) == false) {
            OSReport("\n");
            OSReport("ライブラリのオープンリクエストに失敗\n");
            delete m_exchange;
            m_exchange = NULL;
            removeModelMessage();
            restorePermit();
            m_unk664 = Step_Select;
            return 0;
        }
        m_frame = 0;
        m_unk664 = Step_WaitJob;
        return 0;
    case Step_WaitJob:
        if (m_exchange->m_jobDone == true) {
            if (m_exchange->m_result == 0) {
                m_unk664 = Step_Apply;
            } else {
                OSReport("nteExchange 初期化時に何らかのエラー発生\n");
                delete m_exchange;
                m_exchange = NULL;
            }
        } else if (m_frame >= 180) {
            m_unk664 = Step_OpenWnd;
        }
        m_frame++;
        return 0;
    case Step_OpenWnd:
        OSReport("OpenDialog\n");
        m_wnd = fn_800BEEB8(3, m_msg->getBinary(), 0x1E, m_resFile, m_group, 0x20, -1, Heaps::MenuInstance, 0x2B, 0,
                            0, 0xF0);
        m_unk664 = Step_WaitWnd;
        m_frame = 0;
        return 0;
    case Step_WaitWnd:
        if (m_exchange->m_jobDone == true && m_frame >= 60) {
            fn_800BF828(m_wnd, 2);
            m_unk664 = Step_CloseWnd;
        }
        m_frame++;
        return 0;
    case Step_CloseWnd:
        if (m_wnd->m_step == 3) {
            OSReport("CloseDialog\n");
            m_wnd->exit();
            m_wnd = NULL;
            m_unk664 = Step_Apply;
        }
        return 0;
    case Step_Apply: {
        int task = -1;
        if (m_cursor == Entry_Service) {
            task = m_listDLTask[m_savedPermit[Entry_Service]][m_choice];
        }
        if (m_permit[Entry_Service] == true) {
            if (m_exchange->check_getStatus(2) == true) {
                m_exchange->dl_registerDLTask();
            }
        } else {
            if (task == 1) {
                m_exchange->dl_deleteDLTask();
            }
            if (task == 2) {
                m_exchange->dl_deleteDLTaskAll();
            }
        }
        delete m_exchange;
        m_exchange = NULL;
        if (!m_error) {
            if (m_needSave == true) {
                gmSysSaveTask::create(0x2E);
                m_needSave = false;
            }
            removeModelMessage();
            savePermit();
            m_unk664 = Step_Select;
        }
        return 0;
    }
    case Step_Error:
        if (m_wnd->m_step == 3) {
            delete m_exchange;
            m_exchange = NULL;
            removeModelMessage();
            restorePermit();
            m_error = false;
            g_sndSystem->playSE(snd_se_system_InfoWindow_Close, -1, 0, 0, -1);
            m_wnd->exit();
            m_wnd = NULL;
            m_unk664 = Step_Select;
        }
        return 0;
    case Step_Leave:
        if (m_objs[Obj_Panel]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        return 0;
    }
    if (m_entryCount > 1 && ((pad & Pad_Up) || (pad & Pad_Down))) {
        g_sndSystem->playSE(snd_se_system_cursor, -1, 0, 0, -1);
        m_cursor ^= 1;
        if (m_cursor == Entry_Watching) {
            m_objs[Obj_Entry0 + Entry_Watching]->setFrameVisible(1.0f);
            m_objs[Obj_Entry0 + Entry_Service]->setFrameVisible(0.0f);
        } else {
            m_objs[Obj_Entry0 + Entry_Watching]->setFrameVisible(0.0f);
            m_objs[Obj_Entry0 + Entry_Service]->setFrameVisible(1.0f);
        }
        setHelpMsg();
    }
    if (pad & Pad_A) {
        g_sndSystem->playSE(snd_se_system_cursor, -1, 0, 0, -1);
        addModelMessage();
        m_unk664 = Step_List;
        setHelpMsg();
    }
    if (pad & Pad_Cancel) {
        m_result = 3;
        m_unk664 = Step_Leave;
        m_objs[Obj_Panel]->setAnimIdx(&sOptWifiAnims[1], true);
        return 1;
    }
    return 0;
}

void muProcOptWifi::exit() {
    removeModelMessage();
    nw4r::g3d::ScnMdlExpand* entries = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Entries]);
    while (!entries->Empty()) {
        entries->PopBack();
    }
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Entries]));
}

void muProcOptWifi::notify_Error(int kind, int unk, int code) {
    int msgNo = 0;
    switch (kind) {
    case 0:
        return;
    case 1:
        msgNo = 6;
        break;
    case 2:
        msgNo = 7;
        break;
    case 3:
        msgNo = 8;
        break;
    case 4:
    case 7:
    case 8:
        msgNo = 9;
        break;
    case 5:
        msgNo = 10;
        break;
    case 6:
        msgNo = 11;
        break;
    }
    m_error = true;
    m_unk664 = Step_Error;
    u32 permitMsg = 5;
    if (m_permit[Entry_Service]) {
        permitMsg = 4;
    }
    m_msg->printIndex(3, permitMsg, NULL);
    int length;
    const char* text = m_msg->getMsgData(msgNo, &length);
    char msg[0x200];
    memset(msg, 0, sizeof(msg));
    memcpy(msg, text, length);
    text = m_errorMsg->getMsgData(0x13, &length);
    char codeText[0x200];
    memset(codeText, 0, sizeof(codeText));
    memcpy(codeText, text, length);
    length = sprintf(msg, "%s\n%s%d", msg, codeText, code);
    g_sndSystem->playSE(snd_se_system_Beep, -1, 0, 0, -1);
    if (m_wnd) {
        m_wnd->exit();
        m_wnd = NULL;
    }
    m_wnd = fn_800BF048(0, msg, length, m_resFile, m_group, 0x20, 0, Heaps::MenuInstance, 0x2B, 0, 0, 0xF0);
}

void muProcOptWifi::addModelMessage() {
    if (m_listShown != true) {
        m_msg->detachMuObject(m_objs[Obj_List]);
        int permit = m_cursor == Entry_Watching ? m_permit[Entry_Watching] : m_permit[Entry_Service];
        int kind = m_listKind[m_cursor][permit];
        m_choiceMin = m_listMin[m_cursor][permit];
        m_choiceMax = m_listMax[m_cursor][permit];
        m_choice = m_listStart[m_cursor][permit];
        for (int i = Msg_List0; i < Msg_ListEnd; i++) {
            int line = i - Msg_List0;
            if (kind == 2 && i == 6) {
                continue;
            }
            if (i == 5) {
                m_msg->setVerticalMode(i, true);
                m_msg->getWScale(i, 48.0f);
            }
            if (i >= 6 && i <= 8) {
                m_msg->getWScale(i, 42.0f);
            }
            m_msg->printIndex(i, m_listMsg[m_cursor][permit][line], NULL);
            m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_List]->m_sceneModel), line,
                                      0.0625f);
            m_msg->setFontColor(i, 0, 0, 0, 0xFF);
        }
        m_group->Insert(m_group->sceneItemsCount, m_objs[Obj_List]->m_sceneModel);
        m_objs[Obj_List]->changeAnimN(0, 0x12, false, false);
        m_objs[Obj_List]->setFrameVisible(kind == 3 ? 0.0f : 1.0f);
        m_objs[Obj_List]->setFrameMatCol(m_choice);
        m_listShown = true;
    }
}

void muProcOptWifi::playSE_CursorMove() {
    g_sndSystem->playSE(SND_SE_SYSTEM_CURSOR_L, -1, 0, 0, -1);
}
