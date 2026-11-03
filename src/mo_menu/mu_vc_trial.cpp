#include <types.h>

#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_shutdown_manager.h>
#include <gm/gm_global.h>
#include <gm/gm_syssave_task.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <revolution/NAND/NANDCheck.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>

#include <mu/mu_vc_trial.h>

// muMenuController::getPadStatus(merge...): the pad status of the pads of the controller, merged (main.dol, no name).
extern "C" gfPadStatus fn_800B9F90(muMenuController* controller, int unk);
// The instance of gmSysSave and what ends it once it is done (main.dol, no names).
extern "C" gmSysSave* lbl_805A00FC;
extern "C" void fn_8005A6E8(gmSysSave* save);

// HYPOTHESIS: the object of the scene (module sora_scene) that starts the Virtual Console title; only the fields that the
// menu touches are known.
struct scVCTrialLoader {
    u32 m_unk00;
    u32 m_unk04;
    u32 m_unk08;
    u32 m_unk0C;
    u8 m_unk10[2];
    u8 m_isLoaded;                  // 0x12 the scene is loaded
    u8 m_unk13[0x30 - 0x13];
    u8 m_unk30;
};
extern scVCTrialLoader* lbl_1_bss_20;
// HYPOTHESIS: loads the scene (kind 3) on the two heaps; called each frame until m_isLoaded is set.
extern "C" void fn_1_4A44(scVCTrialLoader* loader, int kind, Heaps::HeapType heap, Heaps::HeapType resHeap);

// A name that is put together from a text and numbers; each step works on a copy.
// HYPOTHESIS: the sizes (17, 33 and 65 letters with the end) follow from how the compiler copies the names around.
template <int N>
struct muVCTrialName {
    char m_str[N];

    muVCTrialName(const char* str) {
        m_str[0] = '\0';
        strcat(m_str, str);
    }
    // The name with the separator before the number of an animation.
    muVCTrialName separated() const {
        muVCTrialName name = *this;
        strcat(name.m_str, "__");
        return name;
    }
    // The name with a number appended.
    friend muVCTrialName operator+(muVCTrialName name, int no) {
        sprintf(name.m_str + strlen(name.m_str), "%d", no);
        return name;
    }
};

// A text of the message file: where it is and how long it is.
struct muVCTrialMsgText {
    const char* m_text;
    int m_size;

    muVCTrialMsgText(MuMsg* msg, u32 index) { m_text = msg->getMsgData(index, &m_size); }
};

// A text that is put together from several texts of the message file.
template <int N>
struct muVCTrialText {
    char m_data[N];
    int m_size;

    muVCTrialText() : m_size(0) { }
    void append(const char* text, u32 size) {
        int used = m_size;
        memcpy(m_data + used, text, size);
        m_size = used + size;
    }
    void append(const muVCTrialMsgText& text) { append(text.m_text, text.m_size); }
    // Appends a text, a string and another text.
    void append(const muVCTrialMsgText& before, const char* str, const muVCTrialMsgText& after) {
        append(before);
        muVCTrialName<65> name(str);
        append(name.m_str, strlen(name.m_str));
        append(after);
    }
    // Appends three texts.
    void append(const muVCTrialMsgText& first, const muVCTrialMsgText& second, const muVCTrialMsgText& third) {
        append(first);
        append(second);
        append(third);
    }
};

// The name of the model of an object (from the resource of the model), which the names of its animations start with.
static inline const char* getMdlName(MuObject* object) {
    const nw4r::g3d::ResMdlData& data = object->m_resMdl.ref();
    return data.m_toName != 0 ? reinterpret_cast<const char*>(&data) + data.m_toName : NULL;
}

// The name of an animation of an object: the name of its model, the separator and the number of the animation.
static inline muVCTrialName<65> getAnimName(MuObject* object, int no) {
    muVCTrialName<65> name(getMdlName(object));
    return name.separated() + no;
}

// HYPOTHESIS: an object of sora_scene's loader is reset from it (nothing writes it).
// BYTES: the words are in .data (in front of the titles), as an explicitly initialized object is with this pragma.
#pragma explicit_zero_data on
static u32 sNullWork[2] = { 0, 0 };
#pragma explicit_zero_data reset

// The titles of the vault (the flags are set from the save data when the menu is made).
static muVCTrialTask::Title sTitles[muVCTrialTask::NumTitles] = {
    { 0x0D, 0, 40, 2.0f },
    { 0x0E, 0, 60, 3.0f },
    { 0x0F, 0, 30, 1.0f },
    { 0x10, 2, 180, 5.0f },
    { 0x11, 1, 120, 4.0f },
    { 0x12, 1, 90, 6.0f },
    { 0x13, 0, 90, 10.0f },
    { 0x14, 2, 40, 8.0f },
    { 0x15, 2, 120, 9.0f },
    { 0x16, 0, 120, 11.0f },
    { 0x17, 0, 120, 11.0f },
    { 0x18, 2, 300, 12.0f },
    { 0x19, 2, 0, 0.0f },
    { 0x1A, 3, 180, 13.0f },
    { 0x1B, 3, 300, 14.0f },
    { 0x1C, 2, 180, 7.0f },
};


inline const muVCTrialTask::Title& muVCTrialTask::getCurTitle() const {
    return sTitles[m_titles[m_page * NumPanels + m_cursor]];
}

muVCTrialTask* muVCTrialTask::create(const Param* param) {
    return new (Heaps::MenuInstance) muVCTrialTask(param);
}

int muVCTrialTask::getPlaySec(int title) {
    return sTitles[title].m_playSec;
}

muVCTrialTask::muVCTrialTask(const Param* param)
    : StandardMenu("VCTrial", 0xF0, Heaps::MenuResource), m_isEnd(0), m_cursor(0), m_wait(0), m_state(State_Start),
      m_page(0), m_numTitles(0), m_selected(-1), m_initTitle(param->m_title), m_pageAnim(1), m_isNandFull(false),
      m_nandWait(0), m_unk22C(0), m_unk230(0) {
    m_unk42_5 = true;
    m_notices[0] = -1;
    for (int i = 0; i < NumTitles; i++) {
        sTitles[i].m_flags = param->m_titleFlags[i];
    }
}

muVCTrialTask::~muVCTrialTask() {
    m_msg->detachMuObject(m_info.get());
}

void muVCTrialTask::initTitles() {
    const int order[NumShownTitles] = { 2, 0, 1, 4, 5, 7, 8, 6, 9, 3, 13, 14 };
    m_numTitles = 0;
    for (int i = 0; i < NumShownTitles; i++) {
        int title = order[i];
        if (sTitles[title].m_isAvailable) {
            m_titles[m_numTitles++] = title;
        }
    }
    int pos = 0;
    if (m_initTitle != -1) {
        for (const int* p = order; m_initTitle != *p; p++) {
            if (sTitles[*p].m_isAvailable) {
                pos++;
            }
        }
    }
    m_cursor = -1;
    m_page = -1;
    setPage(pos % NumPanels, pos / NumPanels);
}

void muVCTrialTask::loadResource() {
    m_wndRes.request("/menu/cmnwindow/wifi_notice_wnd.brres", Heaps::MenuResource, 0);
    m_resFile.request("/menu/collection/trial.brres", Heaps::MenuResource, 0);
    m_msgFile.request("/menu/msg/vc_trial.msbin", Heaps::MenuResource, 0);
}

void muVCTrialTask::createModel() {
    muVCTrialName<17> pos("pos0");
    gfSceneRoot* root = g_gfSceneRoot;

    m_bg.reset(MuObject::create(&m_resFile, "MenTrial0000_TopN", 0x10, NULL, Heaps::MenuInstance));
    root->add(gfSceneRoot::Layer_Info, m_bg->m_sceneModel);

    m_info.reset(MuObject::create(&m_resFile, "MenTrial0001_TopN", 0x11, NULL, Heaps::MenuInstance));
    MuObject* info = m_info.get();
    info->changeNodeAnimNIf(getAnimName(info, 0).m_str);
    info = m_info.get();
    info->changeVisAnimNIf(getAnimName(info, 0).m_str);
    m_info->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);

    nw4r::g3d::ScnMdl* infoMdl = m_info->m_sceneModel;
    m_expand.reset(nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, 0x10, infoMdl));
    // BYTES: gfSceneRoot::add takes a ScnMdl (the symbol says so); the original passes the expanded group to it.
    root->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_expand.get()));

    for (u32 i = 0; i < NumPanels; i++) {
        m_panels[i].reset(MuObject::create(&m_resFile, "MenTrial0002_TopN", 0x12, NULL, Heaps::MenuInstance));
        MuObject* panel = m_panels[i].get();
        panel->changeTexPatAnimNIf(getAnimName(panel, 0).m_str);
        m_panels[i]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        panel = m_panels[i].get();
        panel->changeVisAnimNIf(getAnimName(panel, 0).m_str);
        m_panels[i]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    }

    int iconFrames[NumIcons] = { 1, 2, 3 };
    for (u32 i = 0; i < NumIcons; i++) {
        m_icons[i].reset(MuObject::create(&m_resFile, "MenTrial0003_TopN", 0x12, NULL, Heaps::MenuInstance));
        muVCTrialName<17> node = pos + (int)(i + NumPanels);
        m_expand->PushBack(m_icons[i]->m_sceneModel, node.m_str);
        MuObject* icon = m_icons[i].get();
        icon->changeTexPatAnimNIf(getAnimName(icon, 0).m_str);
        m_icons[i]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        m_icons[i]->setFrameTex(iconFrames[i]);
    }

    m_cursorObj.reset(MuObject::create(&m_resFile, "MenTrial0004_TopN", 0x11, NULL, Heaps::MenuInstance));
    MuObject* cursor = m_cursorObj.get();
    cursor->changeClrAnimNIf(getAnimName(cursor, 0).m_str);

    m_loading.reset(MuObject::create(&m_resFile, "MenTrialLoading_TopN", 0x1C, NULL, Heaps::MenuInstance));
    MuObject* loading = m_loading.get();
    loading->changeNodeAnimNIf(getAnimName(loading, 0).m_str);
    loading = m_loading.get();
    loading->changeVisAnimNIf(getAnimName(loading, 0).m_str);

    m_wait = m_info->getNodeAnimLength();

    m_msg.reset(MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource));
    m_msg->allocMsgBuf(0x100, 3);
    m_msg->setMsgData(m_msgFile.m_handle.getBuffer());
    for (int i = 0; i < 3; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_info->m_sceneModel), i, 0.0625f);
        m_msg->setAlignMode(i, MuMsg::Align_Left);
        m_msg->setFontColor(i, 0, 0, 0, 0xFF);
        m_msg->setEdgeEnable(i, false);
        m_msg->setEdge(i, 0, 0, 0, 2.0f);
    }
    initTitles();
}

void muVCTrialTask::dispInfo() {
    const Title& title = getCurTitle();
    int console = 0;
    switch (title.m_console) {
    case Console_NES:
        console = 0;
        break;
    case Console_SNES:
        console = 1;
        break;
    case Console_N64:
        console = 2;
        break;
    case Console_Unk3:
        console = 3;
        break;
    }
    m_msg->printIndex(0, console, NULL);
    m_msg->printIndex(1, title.m_nameMsg, NULL);

    char time[0x20];
    sprintf(time, "%d:%02d", title.m_playSec / 60, title.m_playSec % 60);
    muVCTrialText<0x40> text;
    text.append(muVCTrialMsgText(m_msg.get(), 4), time, muVCTrialMsgText(m_msg.get(), 5));
    m_msg->printData(2, text.m_data, text.m_size);
}

// The titles of the first two consoles are played on a Wii Remote.
static inline bool usesWiiRemote(int console) {
    switch (console) {
    case muVCTrialTask::Console_NES:
    case muVCTrialTask::Console_SNES:
        return true;
    }
    return false;
}

void muVCTrialTask::setPage(int cursor, int page) {
    muVCTrialName<33> pos("pos0");
    if (page != m_page) {
        if (m_page >= 0) {
            if (page < m_page) {
                MuObject* info = m_info.get();
                info->changeClrAnimNIf(getAnimName(info, 1).m_str);
                m_pageAnim = 1;
            } else if (page > m_page) {
                MuObject* info = m_info.get();
                info->changeClrAnimNIf(getAnimName(info, 2).m_str);
                m_pageAnim = 2;
            }
            int count = m_numTitles - page * NumPanels;
            if (cursor >= count) {
                if (cursor / NumColumns > 0) {
                    cursor -= NumColumns;
                }
                if (cursor >= count) {
                    cursor = count - 1;
                }
            }
        }
        m_page = page;
        for (u32 i = 0; i < NumPanels; i++) {
            int index = m_page * NumPanels + i;
            nw4r::g3d::ScnMdl* mdl = m_panels[i]->m_sceneModel;
            if (index < m_numTitles) {
                const Title& title = sTitles[m_titles[index]];
                if (m_expand.get() != mdl->m_parent) {
                    muVCTrialName<33> node = pos + (int)i;
                    m_expand->PushBack(mdl, node.m_str);
                }
                m_panels[i]->setFrameVisible(title.m_isNew);
                m_panels[i]->setFrameTex(title.m_texFrame);
            } else {
                m_expand->Remove(mdl);
            }
        }
    }

    u32 arrows = 0;
    if (m_page > 0) {
        arrows |= 1;
    }
    if (m_page < (m_numTitles + NumPanels - 1) / NumPanels - 1) {
        arrows |= 2;
    }
    if (!m_info->isClrAnimFinished()) {
        arrows |= m_pageAnim == 1 ? 1 : 2;
    }
    m_info->setFrameVisible(arrows);

    int count = m_numTitles - m_page * NumPanels;
    if (cursor >= count) {
        int row = cursor / NumColumns;
        if (row == m_cursor / NumColumns) {
            if (row > 0) {
                cursor -= NumColumns;
            } else {
                cursor = m_cursor;
            }
        } else {
            cursor -= NumColumns;
            if (row == (count - 1) / NumColumns) {
                cursor = count - 1;
            }
        }
    }

    if (m_cursor != cursor) {
        m_cursor = cursor;
        nw4r::g3d::ScnMdl* mdl = m_cursorObj->m_sceneModel;
        m_expand->Remove(mdl);
        muVCTrialName<33> node = pos + m_cursor;
        m_expand->PushBack(mdl, node.m_str);
        m_icons[0]->setFrameTex(usesWiiRemote(getCurTitle().m_console));
    }
    dispInfo();
}

void muVCTrialTask::update() {
    main();
}

// Sets the "played" flag of a title in the save data (the flags have two bits for each entry); false if it was set.
// HYPOTHESIS: an inline helper of GameGlobal.
static inline bool setPlayedFlag(int no) {
    u32* flags = reinterpret_cast<u32*>(g_GameGlobal->getHideEtcAppFlag());
    int bit = no * 2;
    u32 mask = 1 << (bit % 32);
    if (flags[bit / 32] & mask) {
        return false;
    }
    flags[bit / 32] |= mask;
    return true;
}

// Checks that the save data of the trial fits on the Wii system memory; false while the check is busy.
static inline bool checkNand(bool* isFull) {
    u32 answer;
    s32 result = NANDCheck(1, 4, &answer);
    if (result == NAND_RESULT_BUSY) {
        return false;
    }
    *isFull = result != NAND_RESULT_OK || (answer & (NAND_CHECK_TOO_MANY_USER_BLOCKS | NAND_CHECK_TOO_MANY_USER_FILES)) != 0;
    return true;
}

inline void muVCTrialTask::openNotice(int msg) {
    nw4r::g3d::ScnGroup* group = g_gfSceneRoot->unk38C[gfSceneRoot::Layer_Info].unk4;
    bool isQuestion = msg == 11;
    muNoticeWndTask* wnd;
    if (msg == 11) {
        int nameMsg = getCurTitle().m_nameMsg;
        muVCTrialText<0x200> text;
        text.append(muVCTrialMsgText(m_msg.get(), 11), muVCTrialMsgText(m_msg.get(), nameMsg),
                    muVCTrialMsgText(m_msg.get(), 12));
        wnd = fn_800BF048(isQuestion, text.m_data, text.m_size, &m_wndRes, group, 0x14, 0, Heaps::MenuInstance,
                          Heaps::MenuResource, 0, 0, 0xF0);
    } else {
        wnd = fn_800BEEB8(isQuestion, m_msg->getBinary(), msg, &m_wndRes, group, 0x14, 0, Heaps::MenuInstance,
                          Heaps::MenuResource, 0, 0, 0xF0);
    }
    m_notice.reset(wnd);
}

// HYPOTHESIS: the original is a coroutine whose states are source lines; it jumps to the line that waits for the next
// frame, so the code below jumps to the label in front of each state instead of setting the state again.
void muVCTrialTask::main() {
    switch (m_state) {
    case State_Start:
        m_state = State_Appear;
    case State_Appear:
        if (--m_wait <= 0) {
            m_nandWait = 0;
            m_state = State_CheckNand;
        }
        break;
    case State_CheckNand:
        if (--m_nandWait > 0) {
            break;
        }
        m_nandWait = 30;
        if (!checkNand(&m_isNandFull)) {
            break;
        }
        if (!m_isNandFull) {
            goto select;
        }
        m_notices[0] = 6;
        m_notices[1] = -1;
        openNotice(m_notices[0]);
        goto noticeOpen;
    select:
        m_state = State_Select;
        break;
    case State_Select:
        if (m_input & 2) {
            decide();
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        } else if (m_input & 4) {
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            goto leave;
        } else {
            int prev = getCurTitleNo();
            moveCursor();
            if (prev != getCurTitleNo()) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
            break;
        }
    noticeOpen:
        m_state = State_NoticeOpen;
        break;
    case State_NoticeOpen:
        m_state = State_Notice;
        break;
    case State_Notice: {
        if (m_notice.get() != NULL) {
            if (m_notice->m_step != 3) {
                break;
            }
            u8 selected = m_notice->m_selected;
            m_notice.reset(NULL);
            g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
            m_notices[0] = m_notices[1];
            m_notices[1] = m_notices[2];
            m_notices[2] = m_notices[3];
            if (m_notices[0] >= 0) {
                openNotice(m_notices[0]);
                goto noticeOpen;
            }
            if (selected == 0) {
                if (!m_isNandFull) {
                    goto select;
                }
                goto leave;
            }
        }
        m_selected = getCurTitleNo();
        m_fade.reset(utility::FadeOutTask::create(Heaps::MenuInstance));
        // The flag in the save data that tells that a title was played.
        // HYPOTHESIS: the "no flag" value is a plain local in the original (the table in .data has 0 in its entries and
        // the code stores it after copying the table).
        int noFlag = 17;
        int playedFlags[NumTitles] = { noFlag, noFlag, 14, noFlag, noFlag, noFlag, 15, 16,
                                       12, noFlag, noFlag, noFlag, noFlag, noFlag, 13, noFlag };
        int flag = playedFlags[getCurTitleNo()];
        bool changed;
        if (flag == noFlag) {
            changed = false;
        } else {
            changed = setPlayedFlag(flag);
        }
        if (!changed) {
            goto fadeOut;
        }
        g_gfShutdownManager.unkD = false;
        gmSysSave::create(Heaps::MenuInstance);
        m_state = State_Save;
        break;
    }
    case State_Save:
        if (lbl_805A00FC->returnStatus()) {
            fn_8005A6E8(lbl_805A00FC);
            g_gfShutdownManager.unkD = true;
            m_state = State_SaveDone;
        }
        break;
    case State_SaveDone:
        m_state = State_SaveDone2;
        break;
    case State_SaveDone2:
    fadeOut:
        m_state = State_FadeOut;
        break;
    case State_FadeOut:
        if (m_fade.get() == NULL || m_fade->isEnd()) {
            goto end;
        }
        break;
    leave:
        m_state = State_Leave;
        break;
    case State_Leave: {
        scVCTrialLoader* loader = lbl_1_bss_20;
        loader->m_unk30 = 0;
        loader->m_unk00 = sNullWork[0];
        loader->m_unk04 = 0;
        loader->m_unk08 = sNullWork[0];
        loader->m_unk0C = 0;
        if (!lbl_1_bss_20->m_isLoaded) {
            nw4r::g3d::ScnGroup* group = g_gfSceneRoot->unk38C[gfSceneRoot::Layer_Info].unk4;
            group->Insert(group->Size(), m_loading->m_sceneModel);
        }
        m_state = State_Loading;
        break;
    }
    case State_Loading:
        fn_1_4A44(lbl_1_bss_20, 3, Heaps::InfoExtraResource, Heaps::MenuResource);
        if (!lbl_1_bss_20->m_isLoaded) {
            break;
        }
    end:
        m_state = State_End;
        break;
    case State_End:
        m_isEnd = 1;
        break;
    }
}

void muVCTrialTask::moveCursor() {
    // HYPOTHESIS: the merged pad status is not used.
    gfPadStatus pad = fn_800B9F90(&m_controller, 0);
    int cursor = m_cursor;
    int row = cursor / NumColumns;
    int column = cursor % NumColumns;
    int numPages = (m_numTitles + NumPanels - 1) / NumPanels;
    u32 trigger = m_controller.m_trigger;
    int page = m_page;
    if (m_input & 8) {
        page = page - 1 > 0 ? page - 1 : 0;
    } else if (m_input & 0x10) {
        page = page + 1 < numPages - 1 ? page + 1 : numPages - 1;
    } else if (trigger & 1) {
        if (column > 0) {
            column--;
        } else if (page > 0) {
            column = NumColumns - 1;
            page--;
        }
    } else if (trigger & 2) {
        if (column < NumColumns - 1) {
            column++;
        } else if (page < numPages - 1) {
            column = 0;
            page++;
        }
    } else if (trigger & 8) {
        row = row - 1 > 0 ? row - 1 : 0;
    } else if (trigger & 4) {
        row = row + 1 < 1 ? row + 1 : 1;
    }
    setPage(column + row * NumColumns, page);
}

void muVCTrialTask::decide() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0, &pad);
    bool hasPad = pad.m_error == 0;
    m_notices[0] = 9;
    m_notices[1] = 10;
    m_notices[2] = 11;
    m_notices[3] = -1;
    if (!hasPad) {
        gfPadStatus remote;
        g_gfPadSystem->getSysPadStatus(4, &remote);
        switch (getCurTitle().m_console) {
        case Console_NES:
        case Console_SNES:
            if (remote.m_error != 0) {
                m_notices[1] = -1;
                m_notices[0] = 7;
            }
            break;
        case Console_N64:
        case Console_Unk3:
            if (remote.m_error != 0 || remote.m_controllerType != 1) {
                m_notices[0] = 8;
                m_notices[1] = -1;
            }
            break;
        }
    }
    openNotice(m_notices[0]);
}
