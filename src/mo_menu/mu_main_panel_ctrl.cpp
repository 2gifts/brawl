#include <types.h>

#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>

#include <mu/mu_main_panel_ctrl.h>

// The background: a colour comes in and goes out for each of the six colours (colour c comes in at (c - 1) * 2 and goes
// out at (c - 1) * 2 + 1), and two that turn colour 5 into 6 and back.
static MuAnimIdxData sBgAnims[14] = {
    { 0.0f, 29.0f, 0, 0, 0x10 },    { 30.0f, 59.0f, 0, 0, 0x10 },   { 60.0f, 89.0f, 0, 0, 0x10 },
    { 90.0f, 119.0f, 0, 0, 0x10 },  { 120.0f, 149.0f, 0, 0, 0x10 }, { 150.0f, 179.0f, 0, 0, 0x10 },
    { 180.0f, 209.0f, 0, 0, 0x10 }, { 210.0f, 239.0f, 0, 0, 0x10 }, { 240.0f, 269.0f, 0, 0, 0x10 },
    { 270.0f, 299.0f, 0, 0, 0x10 }, { 300.0f, 329.0f, 0, 0, 0x10 }, { 330.0f, 359.0f, 0, 0, 0x10 },
    { 360.0f, 389.0f, 0, 0, 0x10 }, { 390.0f, 419.0f, 0, 0, 0x10 },
};
// The title: 0 the first panel comes in; 1 ... 6 the title moves from panel p to p + 1 ((p - 1) * 2 + 1) and back
// ((p - 1) * 2 + 2); 7 ... 10 panels 2 ... 5 stand; 11 ... 16 panels 2 ... 4 go out and come back (sTitleLeaveAnims,
// sTitleComeAnims).
// HYPOTHESIS: what the entries show is read from their frames and from where the functions use them.
static MuAnimIdxData sTitleAnims[17] = {
    { 0.0f, 69.0f, 0, 0, 0xB },     { 70.0f, 89.0f, 0, 0, 0xB },    { 90.0f, 109.0f, 0, 0, 0xB },
    { 110.0f, 129.0f, 0, 0, 0xB },  { 130.0f, 149.0f, 0, 0, 0xB },  { 150.0f, 169.0f, 0, 0, 0xB },
    { 170.0f, 189.0f, 0, 0, 0xB },  { 69.0f, 69.0f, 0, 0, 0xB },    { 89.0f, 89.0f, 0, 0, 0xB },
    { 129.0f, 129.0f, 0, 0, 0xB },  { 169.0f, 169.0f, 0, 0, 0xB },  { 210.0f, 219.0f, 0, 0, 0xB },
    { 220.0f, 229.0f, 0, 0, 0xB },  { 190.0f, 199.0f, 0, 0, 0xB },  { 200.0f, 209.0f, 0, 0, 0xB },
    { 230.0f, 239.0f, 0, 0, 0xB },  { 240.0f, 249.0f, 0, 0, 0xB },
};
// The panel: comes in, goes out, comes back, and stands.
static MuAnimIdxData sPanelAnims[4] = {
    { 0.0f, 69.0f, 0, 0, 0x1F },
    { 70.0f, 79.0f, 0, 0, 0x1F },
    { 80.0f, 89.0f, 0, 0, 0x1F },
    { 69.0f, 69.0f, 0, 0, 0x1F },
};

muMainPanelCtrlTask::muMainPanelCtrlTask(void* resFile, void* msgData)
    : m_titleFrame(1.0f), m_panel(0), m_bg(0), m_state(State_None), m_screen(0), m_msgData(NULL) {
    unk2C_b1 = false;
    muObjProcessTask* objProcess = muObjProcessTask::create(Heaps::MenuInstance);
    m_objProcess = objProcess;
    objProcess->m_objs = m_objs;
    objProcess->m_count = ObjCount;
    m_objs[Obj_Bg] = NULL;
    m_objs[Obj_Panel] = NULL;
    m_objs[Obj_Title] = NULL;
    m_resFile = nw4r::g3d::ResFile(resFile);
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_objs[Obj_Bg] = MuObject::create(&m_resFile, "MenMainBg_TopN", 0x10, NULL, Heaps::MenuInstance);
    m_objs[Obj_Panel] = MuObject::create(&m_resFile, "MenMainPanel_TopN", 0x1E, NULL, Heaps::MenuInstance);
    m_objs[Obj_Title] = MuObject::create(&m_resFile, "MenMainTitle_TopN", 0x1F, NULL, Heaps::MenuInstance);
    for (u32 i = 0; i < ObjCount; i++) {
        m_objs[i]->m_modelAnim->setUpdateRate(0.0f);
    }
    m_objs[Obj_Bg]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x200, 1);
    m_msg->setMsgData(msgData);
    m_msg->attachScnMdlSimple(0, m_objs[Obj_Title]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->setAlignMode(0, MuMsg::Align_Center);
    m_msg->setFontColor(0, 0x2F, 0x2E, 0x2E, 0xFF);
    m_msg->setEdgeEnable(0, false);
}

muMainPanelCtrlTask::~muMainPanelCtrlTask() {
    for (u32 i = 0; i < ObjCount; i++) {
        if (m_objs[i] != NULL) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    if (m_resFile.ptr() != NULL) {
        m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    delete m_msg;
    m_objProcess->exit();
    m_objProcess = NULL;
}

// Prints the text once the animation that changeProcess started is far enough.
void muMainPanelCtrlTask::processAnim() {
    if (m_state == State_None) {
        return;
    }
    m_timer++;
    switch (m_state) {
    case State_Show:
        if (static_cast<u32>(m_timer) >= 10) {
            m_msg->printIndex(0, m_msgNo, m_msgData);
            m_objs[Obj_Title]->setFrameTex(m_titleFrame);
            m_objs[Obj_Title]->setFrameMatCol(m_titleFrame);
            m_state = State_None;
        }
        break;
    case State_ShowNow:
        if (static_cast<u32>(m_timer) >= 1) {
            m_msg->printIndex(0, m_msgNo, m_msgData);
            m_objs[Obj_Title]->setFrameTex(m_titleFrame);
            m_objs[Obj_Title]->setFrameMatCol(m_titleFrame);
            m_state = State_None;
        }
        break;
    case State_Clear:
    case State_ShowText:
        if (static_cast<u32>(m_timer) >= 5) {
            if (m_state == State_Clear) {
                m_msg->printf(0, "");
            } else {
                m_msg->printIndex(0, m_msgNo, m_msgData);
            }
            m_state = State_None;
        }
        break;
    }
}

void muMainPanelCtrlTask::setMsg(int msgNo, int print, void* msgData) {
    if (print != 0) {
        m_msg->printIndex(0, msgNo, msgData);
    }
    m_msgNo = msgNo;
    m_msgData = msgData;
}

// bg 9 keeps the background; 7 and 8 turn colour 5 into 6 and back; 0 takes the colour away.
void muMainPanelCtrlTask::changeProcess(u32 panel, int msgNo, int bg, float titleFrame) {
    if (bg != 9) {
        if (bg == 0) {
            m_bgAnim = (m_bg - 1) * 2 + 1;
            m_bg = bg;
        } else if (bg == 7) {
            m_bgAnim = 12;
            m_bg = 6;
        } else if (bg == 8) {
            m_bgAnim = 13;
            m_bg = 5;
        } else {
            m_bg = bg;
            m_bgAnim = (bg - 1) * 2;
        }
        m_objs[Obj_Bg]->setAnimIdx(&sBgAnims[m_bgAnim], false);
        m_state = State_Show;
    }
    if (m_panel == 0) {
        if (panel == 1) {
            m_titleAnim = 0;
            m_panelAnim = 0;
        } else {
            m_titleAnim = panel + 6;
            m_panelAnim = 3;
        }
        m_objs[Obj_Title]->setAnimIdx(&sTitleAnims[m_titleAnim], false);
        m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[m_panelAnim], false);
        m_state = State_ShowNow;
    } else if (panel != m_panel) {
        if (m_panel > panel) {
            m_titleAnim = (panel - 1) * 2 + 2;
        } else {
            m_titleAnim = (panel - 2) * 2 + 1;
        }
        m_objs[Obj_Title]->setAnimIdx(&sTitleAnims[m_titleAnim], false);
        m_state = State_Show;
    } else {
        m_state = State_ShowNow;
    }
    m_panel = panel;
    m_titleFrame = titleFrame;
    m_msgNo = msgNo;
    m_msgData = NULL;
    m_timer = 0;
}

const muMainPanelCtrlTask::Screen muMainPanelCtrlTask::sScreens[ScreenCount] = {
    { 1, 1, 0 },  { 1, 2, 0 },  { 1, 3, 0 },  { 1, 4, 0 },  { 1, 5, 0 },  { 1, 6, 0 },  { 2, 7, 1 },
    { 2, 8, 2 },  { 2, 9, 3 },  { 2, 10, 4 }, { 2, 11, 5 }, { 2, 12, 6 }, { 3, 13, 1 }, { 3, 14, 1 },
    { 4, 15, 1 }, { 3, 16, 1 }, { 3, 17, 3 }, { 3, 18, 3 }, { 3, 19, 4 }, { 3, 20, 5 }, { 3, 21, 5 },
    { 3, 22, 5 }, { 3, 23, 5 }, { 3, 24, 5 }, { 3, 25, 5 }, { 3, 26, 5 }, { 3, 27, 5 }, { 3, 28, 6 },
    { 3, 29, 6 }, { 3, 30, 6 }, { 3, 31, 3 }, { 5, 0, 1 },  { 5, 0, 2 },  { 5, 0, 6 },
};

// The animation of the background from the colour of one screen to the colour of the next (14: none).
static inline int getBgAnim(int from, int to) {
    int anim = 14;
    if (from == to) {
        anim = 14;
    } else {
        if (from == 0) {
            anim = (to - 1) * 2;
        }
        if (to == 0) {
            anim = (from - 1) * 2 + 1;
        }
        if (from == 5 && to == 6) {
            anim = 12;
        }
        if (from == 6 && to == 5) {
            anim = 13;
        }
    }
    return anim;
}

// Screens 1 ... 31 are the screens of the menu where the panel is in, the others (0, 32 ...) are the ones without it.
void muMainPanelCtrlTask::changeProcess1(int screen, int msgNo) {
    int idx = screen - 1;
    if (screen >= 1 && screen < 32) {
        if (m_screen >= 0 && m_screen < 32) {
            u8 bg;
            if (m_screen != 0) {
                bg = sScreens[idx].m_bg;
                if (sScreens[m_screen - 1].m_bg == bg) {
                    bg = 9;
                }
            } else {
                bg = sScreens[idx].m_bg;
            }
            changeProcess(sScreens[idx].m_panel, msgNo, bg, sScreens[idx].m_titleFrame);
        } else {
            m_msgNo = msgNo;
            int bgAnim = getBgAnim(sScreens[m_screen - 1].m_bg, sScreens[idx].m_bg);
            u32 panel = m_panel;
            if (bgAnim != 14) {
                m_bgAnim = bgAnim;
                m_objs[Obj_Bg]->setAnimIdx(&sBgAnims[bgAnim], false);
            }
            m_objs[Obj_Title]->setAnimIdx(&sTitleAnims[sTitleComeAnims[panel]], false);
            m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[2], false);
            m_titleAnim = 14;
            m_panelAnim = 2;
            m_state = State_ShowText;
            m_timer = 0;
            changeProcess(sScreens[idx].m_panel, msgNo, 9, sScreens[idx].m_titleFrame);
        }
    } else if (m_screen == 0) {
        m_objs[Obj_Title]->setAnimIdx(&sTitleAnims[11], false);
        m_objs[Obj_Title]->setFrameFlg(sTitleAnims[11].m_endFrame, sTitleAnims[11].m_flags);
        m_objs[Obj_Title]->setUpdateRateFlg(0.0f, sTitleAnims[11].m_flags);
        m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[1], false);
        m_objs[Obj_Panel]->setFrameFlg(sPanelAnims[1].m_endFrame, sPanelAnims[1].m_flags);
        m_objs[Obj_Panel]->setUpdateRateFlg(0.0f, sPanelAnims[1].m_flags);
        u8 bg = sScreens[idx].m_bg;
        m_titleAnim = 11;
        m_panelAnim = 1;
        m_timer = 0;
        m_bg = bg;
        m_bgAnim = (bg - 1) * 2;
        m_objs[Obj_Bg]->setAnimIdx(&sBgAnims[m_bgAnim], false);
    } else {
        int bgAnim = getBgAnim(sScreens[m_screen - 1].m_bg, sScreens[idx].m_bg);
        if (m_screen >= 1 && m_screen < 32) {
            u32 panel = m_panel;
            if (bgAnim != 14) {
                m_bgAnim = bgAnim;
                m_objs[Obj_Bg]->setAnimIdx(&sBgAnims[bgAnim], false);
            }
            m_objs[Obj_Title]->setAnimIdx(&sTitleAnims[sTitleLeaveAnims[panel]], false);
            m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[1], false);
            m_titleAnim = 13;
            m_panelAnim = 1;
            m_state = State_Clear;
            m_timer = 0;
        } else {
            m_objs[Obj_Bg]->setAnimIdx(&sBgAnims[bgAnim], false);
        }
    }
    m_screen = screen;
}

const u8 muMainPanelCtrlTask::sTitleLeaveAnims[PanelCount] = { 0, 0, 11, 13, 15 };
const u8 muMainPanelCtrlTask::sTitleComeAnims[PanelCount] = { 0, 0, 12, 14, 16 };

void muMainPanelCtrlTask::frameCancel() {
    if (m_objs[Obj_Bg]->m_modelAnim->m_anmObjMatClrRes->GetUpdateRate() > 0.0f) {
        m_objs[Obj_Bg]->setFrameMatCol(sBgAnims[m_bgAnim].m_endFrame);
    }
    if (m_objs[Obj_Title]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() > 0.0f) {
        m_objs[Obj_Title]->setFrameNode(sTitleAnims[m_titleAnim].m_endFrame);
        m_objs[Obj_Title]->setFrameTexSrt(sTitleAnims[m_titleAnim].m_endFrame);
        m_objs[Obj_Title]->setFrameVisible(sTitleAnims[m_titleAnim].m_endFrame);
    }
    if (m_objs[Obj_Panel]->m_modelAnim->getUpdateRate() > 0.0f) {
        m_objs[Obj_Panel]->setFrame(sPanelAnims[m_panelAnim].m_endFrame);
    }
    if (m_state == State_None) {
        return;
    }
    switch (m_state) {
    case State_Show:
    case State_ShowNow:
        m_msg->printIndex(0, m_msgNo, m_msgData);
        m_objs[Obj_Title]->setFrameTex(m_titleFrame);
        m_objs[Obj_Title]->setFrameMatCol(m_titleFrame);
        m_state = State_None;
        break;
    case State_Clear:
        m_msg->printf(0, "");
        m_state = State_None;
        break;
    case State_ShowText:
        m_msg->printIndex(0, m_msgNo, m_msgData);
        m_state = State_None;
        break;
    }
}

// The special melee is shown with the panel gone and everything stopped.
void muMainPanelCtrlTask::startupSpecialMelee() {
    m_objs[Obj_Title]->setAnimIdx(&sTitleAnims[11], false);
    m_objs[Obj_Title]->setFrameFlg(sTitleAnims[11].m_endFrame, sTitleAnims[11].m_flags);
    m_objs[Obj_Title]->setUpdateRateFlg(0.0f, sTitleAnims[11].m_flags);
    m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[1], false);
    m_objs[Obj_Panel]->setFrameFlg(sPanelAnims[1].m_endFrame, sPanelAnims[1].m_flags);
    m_objs[Obj_Panel]->setUpdateRateFlg(0.0f, sPanelAnims[1].m_flags);
    m_objs[Obj_Bg]->m_modelAnim->setUpdateRate(0.0f);
    m_objs[Obj_Panel]->m_modelAnim->setUpdateRate(0.0f);
    m_objs[Obj_Title]->m_modelAnim->setUpdateRate(0.0f);
    m_screen = 34;
}
