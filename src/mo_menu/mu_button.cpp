#include <types.h>

#include <ef/ef_screen.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <ip/ip_padconfig.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <ms/ms_message.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmtexsrt.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <gf/gf_application.h>
#include <stdio.h>
#include <nw4r/ut/ut_Color.h>
#include <sc/sc_button.h>
#include <string.h>

#include <mu/mu_button.h>

// The actions a button can take, one list for each kind of button (each list ends with ActionNum).
static u8 sActionsCommon[] = { 0, 1, 2, 3, 4, 13 };
static u8 sActionsAppealHi[] = { 0, 1, 2, 3, 4, 10, 13 };
static u8 sActionsAppealS[] = { 0, 1, 2, 3, 4, 11, 13 };
static u8 sActionsAppealLw[] = { 0, 1, 2, 3, 4, 12, 13 };
static u8 sActionsSmash[] = { 0, 1, 2, 3, 4, 5, 13 };
static u8 sActionsWiiRemote[] = { 9, 0, 1, 2, 3, 4, 13 };
static u8 sActionsWiiRemoteSmash[] = { 9, 0, 5, 1, 2, 3, 4, 13 };
static u8 sActionsNunchukHi[] = { 0, 6, 1, 2, 3, 4, 10, 13 };
static u8 sActionsNunchukS[] = { 0, 7, 1, 2, 3, 4, 11, 13 };
static u8 sActionsNunchukLw[] = { 0, 8, 1, 2, 3, 4, 12, 13 };
static u8 sActionsWiiRemoteUp[] = { 2, 3, 4, 13 };

static u8* sGcActions[11] = {
    sActionsCommon, sActionsCommon, sActionsCommon, sActionsCommon, sActionsCommon, sActionsCommon,
    sActionsCommon, sActionsSmash, sActionsAppealHi, sActionsAppealS, sActionsAppealLw,
};
static u8* sNunchukActions[12] = {
    sActionsNunchukHi, sActionsNunchukS, sActionsNunchukLw, sActionsCommon, sActionsCommon, sActionsCommon,
    sActionsAppealHi, sActionsAppealLw, sActionsAppealS, sActionsCommon, sActionsCommon, NULL,
};
static u8* sWiiRemoteActions[8] = {
    sActionsWiiRemoteUp, sActionsWiiRemote, sActionsWiiRemote, sActionsWiiRemote, sActionsWiiRemote,
    sActionsWiiRemoteSmash, sActionsWiiRemote, sActionsWiiRemote,
};
static u8* sClassicActions[12] = {
    sActionsCommon, sActionsCommon, sActionsCommon, sActionsCommon, sActionsCommon, sActionsCommon,
    sActionsCommon, sActionsCommon, sActionsSmash, sActionsAppealHi, sActionsAppealS, sActionsAppealLw,
};

// The settings that "restore" puts back, for each controller kind in the order of the screen.
const u8 muButtonTask::sDefaultButtons[PadKind_Num][ButtonMax] = {
    { 3, 3, 4, 2, 2, 0, 1, 5, 10, 11, 12, 0 },
    { 2, 4, 3, 0, 1, 4, 10, 12, 11, 2, 3, 0 },
    { 2, 9, 4, 0, 1, 5, 3, 4, 0, 0, 0, 0 },
    { 3, 3, 4, 4, 2, 2, 0, 1, 5, 10, 11, 12 },
};

const muButtonTask::CursorNode muButtonTask::sGcCursors[15] = {
    { 0, 9, { 10, 1, 8, 20 } },
    { 1, 8, { 7, 20, 2, 0 } },
    { 2, 0, { 1, 20, 3, 0 } },
    { 3, 0, { 2, 20, 4, 0 } },
    { 4, 0, { 3, 22, 5, 8 } },
    { 5, 0, { 4, 23, 6, 8 } },
    { 6, 0, { 5, 24, 7, 9 } },
    { 7, 4, { 6, 24, 1, 10 } },
    { 8, 1, { 0, 5, 9, 23 } },
    { 9, 1, { 8, 6, 10, 24 } },
    { 10, 5, { 9, 7, 0, 24 } },
    { 20, 10, { 24, 0, 22, 1 } },
    { 22, 2, { 20, 8, 23, 4 } },
    { 23, 2, { 22, 8, 24, 5 } },
    { 24, 6, { 23, 10, 20, 7 } },
};
const muButtonTask::CursorNode muButtonTask::sNunchukCursors[16] = {
    { 0, 9, { 10, 3, 1, 20 } },
    { 1, 1, { 0, 4, 2, 20 } },
    { 2, 1, { 1, 5, 9, 21 } },
    { 3, 8, { 8, 20, 4, 0 } },
    { 4, 0, { 3, 20, 5, 1 } },
    { 5, 0, { 4, 21, 6, 2 } },
    { 6, 0, { 5, 23, 7, 9 } },
    { 7, 0, { 6, 24, 8, 9 } },
    { 8, 4, { 7, 24, 3, 10 } },
    { 9, 1, { 2, 7, 10, 23 } },
    { 10, 5, { 9, 8, 0, 24 } },
    { 20, 10, { 24, 0, 21, 3 } },
    { 21, 2, { 20, 2, 22, 5 } },
    { 22, 2, { 21, 2, 23, 6 } },
    { 23, 2, { 22, 9, 24, 7 } },
    { 24, 6, { 23, 10, 20, 8 } },
};
const muButtonTask::CursorNode muButtonTask::sWiiRemoteCursors[11] = {
    { 0, 9, { 7, 2, 1, 2 } },
    { 1, 1, { 0, 3, 6, 3 } },
    { 2, 10, { 5, 0, 3, 0 } },
    { 3, 2, { 2, 1, 4, 1 } },
    { 4, 0, { 3, 23, 5, 6 } },
    { 5, 4, { 4, 24, 2, 7 } },
    { 6, 1, { 1, 4, 7, 23 } },
    { 7, 5, { 6, 5, 0, 24 } },
    { 22, 10, { 24, 6, 23, 4 } },
    { 23, 2, { 22, 6, 24, 4 } },
    { 24, 6, { 23, 7, 22, 5 } },
};
const muButtonTask::CursorNode muButtonTask::sClassicCursors[16] = {
    { 0, 9, { 11, 1, 2, 20 } },
    { 1, 8, { 8, 20, 3, 0 } },
    { 2, 1, { 0, 3, 9, 20 } },
    { 3, 0, { 1, 20, 4, 2 } },
    { 4, 0, { 3, 20, 5, 2 } },
    { 5, 0, { 4, 22, 6, 9 } },
    { 6, 0, { 5, 23, 7, 9 } },
    { 7, 0, { 6, 24, 8, 10 } },
    { 8, 4, { 7, 24, 1, 11 } },
    { 9, 1, { 2, 6, 10, 23 } },
    { 10, 1, { 9, 7, 11, 24 } },
    { 11, 5, { 10, 8, 0, 24 } },
    { 20, 10, { 24, 0, 22, 1 } },
    { 22, 2, { 20, 9, 23, 5 } },
    { 23, 2, { 22, 9, 24, 6 } },
    { 24, 6, { 23, 11, 20, 8 } },
};

const muButtonTask::CursorTable muButtonTask::sCursorTables[PadKind_Num] = {
    { sGcCursors, 15 },
    { sNunchukCursors, 16 },
    { sWiiRemoteCursors, 11 },
    { sClassicCursors, 16 },
};

// Folded into create; the REL linker drops the out-of-line copy.
muButtonTask::muButtonTask() : gfTask("Button", Category_Menu, 0xF, 8, true) {
    m_work = NULL;
    unk2C_b1 = false;
}

muButtonTask* muButtonTask::create(scButton* scene, Heaps::HeapType heap) {
    muButtonTask* task = new (heap) muButtonTask();
    task->m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    task->initWork();
    task->m_work->m_scene = scene;
    task->getNameButtonConfig(task->m_work->m_scene->m_buttonConfigNo);
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(scene->m_padNo, &status);
    task->m_work->m_padKind =
        status.m_error == gfPadError::NONE ? sPadKinds[status.m_controllerType] : PadKind_WiiRemote;
    const Step* step = &sSteps[0];
    if (step->m_init != NULL) {
        step->m_init(task);
    }
    task->m_work->m_step = 0;
    return task;
}

muButtonTask::~muButtonTask() {
    if (m_work != NULL) {
        m_work->m_scene->removeAllObj();
        for (int i = 0; i < GroupNum; i++) {
            if (m_work->m_groups[i] != NULL) {
                nw4r::g3d::ScnMdlExpand* group = m_work->m_groups[i];
                while (!group->Empty()) {
                    group->PopBack();
                }
                m_work->m_groups[i]->Destroy();
            }
        }
        for (int i = 0; i < ObjNum; i++) {
            delete m_work->m_objs[i];
        }
        if (m_work->m_resButton != NULL) {
            gfHeapManager::free(m_work->m_resButton);
        }
        if (m_work->m_resScene != NULL) {
            gfHeapManager::free(m_work->m_resScene);
        }
        if (m_work->m_msg != NULL) {
            delete m_work->m_msg;
        }
        if (m_work->m_msgData != NULL) {
            gfHeapManager::free(m_work->m_msgData);
        }
        gfHeapManager::free(m_work);
        m_work = NULL;
        for (int i = 0; i < 3; i++) {
            m_handles[i].cancelRequest();
        }
    }
}

// Copies the button settings of a name into the order of the screen.
void muButtonTask::getNameButtonConfig(int no) {
    ipButtonConfig* src = g_GameGlobal->getButtonConfig(no);
    Config* gc = &m_work->m_configs[PadKind_Gc];
    gc->m_buttons[0] = src->gc.l;
    gc->m_buttons[1] = src->gc.r;
    gc->m_buttons[2] = src->gc.z;
    gc->m_buttons[3] = src->gc.y;
    gc->m_buttons[4] = src->gc.x;
    gc->m_buttons[5] = src->gc.a;
    gc->m_buttons[6] = src->gc.b;
    gc->m_buttons[7] = src->gc.substick;
    gc->m_buttons[8] = src->gc.up;
    gc->m_buttons[9] = src->gc.side;
    gc->m_buttons[10] = src->gc.down;
    gc->m_flickJump = (src->gc.flags >> 7) & 1;
    Config* nunchuk = &m_work->m_configs[PadKind_Nunchuk];
    nunchuk->m_buttons[0] = src->wii.nunchuk.up;
    nunchuk->m_buttons[1] = src->wii.nunchuk.side;
    nunchuk->m_buttons[2] = src->wii.nunchuk.down;
    nunchuk->m_buttons[3] = src->wii.nunchuk.a;
    nunchuk->m_buttons[4] = src->wii.nunchuk.b;
    nunchuk->m_buttons[5] = src->wii.nunchuk.ab;
    nunchuk->m_buttons[6] = src->wii.nunchuk.c;
    nunchuk->m_buttons[7] = src->wii.nunchuk.z;
    nunchuk->m_buttons[8] = src->wii.nunchuk.cz;
    nunchuk->m_buttons[9] = src->wii.nunchuk.motion;
    nunchuk->m_buttons[10] = src->wii.nunchuk.extra;
    nunchuk->m_flickJump = (src->wii.nunchuk.flags >> 6) & 1;
    nunchuk->m_flag6 = (src->wii.nunchuk.flags >> 7) & 1;
    Config* core = &m_work->m_configs[PadKind_WiiRemote];
    core->m_buttons[0] = src->wii.core.up;
    core->m_buttons[1] = src->wii.core.a;
    core->m_buttons[2] = src->wii.core.extra;
    core->m_buttons[3] = src->wii.core.two;
    core->m_buttons[4] = src->wii.core.one;
    core->m_buttons[5] = src->wii.core.oneTwo;
    core->m_buttons[6] = src->wii.core.b;
    core->m_buttons[7] = src->wii.core.ab;
    Config* classic = &m_work->m_configs[PadKind_Classic];
    classic->m_buttons[0] = src->wii.classic.l;
    classic->m_buttons[1] = src->wii.classic.r;
    classic->m_buttons[2] = src->wii.classic.zl;
    classic->m_buttons[3] = src->wii.classic.zr;
    classic->m_buttons[4] = src->wii.classic.x;
    classic->m_buttons[5] = src->wii.classic.y;
    classic->m_buttons[6] = src->wii.classic.a;
    classic->m_buttons[7] = src->wii.classic.b;
    classic->m_buttons[8] = src->wii.classic.substick;
    classic->m_buttons[9] = src->wii.classic.up;
    classic->m_buttons[10] = src->wii.classic.side;
    classic->m_buttons[11] = src->wii.classic.down;
    classic->m_flickJump = (src->wii.classic.flags >> 7) & 1;
}

void muButtonTask::initWork() {
    m_work->m_scene = NULL;
    m_work->m_resButton = NULL;
    m_work->m_resScene = NULL;
    for (int i = 0; i < ObjNum; i++) {
        m_work->m_objs[i] = NULL;
    }
    for (int i = 0; i < GroupNum; i++) {
        m_work->m_groups[i] = NULL;
    }
    fn_800B96E8(&m_work->m_controller, 0xF0);
    m_work->m_step = 1;
    m_work->m_padKind = PadKind_Gc;
    m_work->m_selectAnim = 0;
    m_work->m_msg = NULL;
    for (int i = 0; i < PadKind_Num; i++) {
        memset(&m_work->m_configs[i], 0, sizeof(Config));
    }
    memset(&m_work->m_edit, 0, sizeof(Config));
    m_work->m_cursor = 0;
    m_work->m_unk1B0 = 0;
    m_work->m_isListOpen = 0;
    m_work->m_dlgKind = 0;
    m_work->m_unk1BC = 0;
    m_work->m_unk1BE = 0;
    m_work->m_unk1C0 = 0;
    m_work->m_unk1C4 = 0;
    m_work->m_isSubStickOn = 0;
    m_work->m_unk1D4 = 0;
}

// Sets up the texts: the help lines, the switches, the buttons, the list of free actions, the dialog and the name.
void muButtonTask::initMsg(void* msgData) {
    MuMsg* msg = MuMsg::create(3, Heaps::MenuInstance, Heaps::MenuInstance);
    m_work->m_msg = msg;
    msg->allocMsgBuf(0x100, 0x1D);
    msg->setMsgData(msgData);
    MuObject* bg = m_work->m_objs[0];
    msg->attachScnMdlSimple(0, bg->m_sceneModel, 0, 0.0625f);
    msg->setFontType(0, 1);
    msg->setFontColor(0, 0xFF, 0xFF, 0xFF, 0xFF);
    msg->getMessage()->setBufField34(0, 7);
    msg->attachScnMdlSimple(1, bg->m_sceneModel, 1, 0.0625f);
    msg->getMessage()->setBufField34(1, 7);
    msg->attachScnMdlSimple(2, m_work->m_objs[6]->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(2, 0, 0, 0, 0xFF);
    msg->printIndex(2, 0x1B, NULL);
    msg->getMessage()->setBufField34(2, 7);
    msg->attachScnMdlSimple(3, m_work->m_objs[7]->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(3, 0, 0, 0, 0xFF);
    msg->printIndex(3, 0x1B, NULL);
    msg->attachScnMdlSimple(4, m_work->m_objs[8]->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(4, 0, 0, 0, 0xFF);
    msg->printIndex(4, 0x1C, NULL);
    msg->getMessage()->setBufField34(4, 7);
    for (int i = 0; i < ButtonMax; i++) {
        msg->attachScnMdlSimple(i + 5, m_work->m_objs[9 + i]->m_sceneModel, 0, 0.0625f);
        msg->getMessage()->setBufField34(i + 5, 7);
    }
    MuObject* list = m_work->m_objs[24];
    msg->attachScnMdlSimple(0x11, list->m_sceneModel, 0, 0.0625f);
    for (int j = 0; j < ListMax; j++) {
        msg->attachScnMdlSimple(j + 0x12, list->m_sceneModel, j + 1, 0.0625f);
        msg->setFontColor(j + 0x12, 0, 0, 0, 0xFF);
        msg->getMessage()->setBufField34(j + 0x12, 7);
    }
    msg->attachScnMdlSimple(0x17, m_work->m_objs[25]->m_sceneModel, 0, 0.0625f);
    msg->getWScale(0x17, 48.0f);
    msg->getMessage()->setBufField34(0x17, 7);
    msg->attachScnMdlSimple(0x18, m_work->m_objs[26]->m_sceneModel, 0, 0.0625f);
    msg->getMessage()->setBufField34(0x18, 7);
    msg->attachScnMdlSimple(0x19, m_work->m_objs[27]->m_sceneModel, 0, 0.0625f);
    msg->getMessage()->setBufField34(0x19, 7);
    msg->attachScnMdlSimple(0x1A, m_work->m_objs[28]->m_sceneModel, 0, 0.0625f);
    msg->getMessage()->setBufField34(0x1A, 7);
    msg->attachScnMdlSimple(0x1B, m_work->m_objs[29]->m_sceneModel, 0, 0.0625f);
    msg->getMessage()->setBufField34(0x1B, 7);
    msg->attachScnMdlSimple(0x1C, m_work->m_objs[34]->m_sceneModel, 0, 0.0625f);
    msg->setFontType(0x1C, 1);
    msg->setFontColor(0x1C, 0xFF, 0xFF, 0xFF, 0xFF);
    msg->getMessage()->setBufField34(0x1C, 7);
}

const muButtonTask::PadInfo muButtonTask::sPadInfos[PadKind_Num] = {
    { sGcActions, 11, 1, 2, 1, 30, 26, 12, 0xFF, 24 },
    { sNunchukActions, 11, 2, 3, 2, 31, 27, 12, 13, 25 },
    { sWiiRemoteActions, 8, 0, 4, 3, 32, 28, 0xFF, 0xFF, 26 },
    { sClassicActions, 12, 1, 5, 4, 33, 29, 13, 0xFF, 27 },
};

const muButtonTask::ActionMsg muButtonTask::sActionMsgs[ActionNum] = {
    { 0x0E, 0x06 }, { 0x13, 0x07 }, { 0x0F, 0x02 }, { 0x10, 0x03 }, { 0x11, 0x04 }, { 0x14, 0x08 }, { 0x15, 0x08 },
    { 0x16, 0x08 }, { 0x17, 0x08 }, { 0x12, 0x05 }, { 0x18, 0x05 }, { 0x19, 0x05 }, { 0x1A, 0x05 },
};

const muButtonTask::ObjData muButtonTask::sObjData[23] = {
    { "MenButton0000_TopN", 0, 0, 0x10 },
    { "MenButton0001_TopN", 1, 1, 0x11 },
    { "MenButton0010_TopN", 2, 2, 0x11 },
    { "MenButton0020_TopN", 3, 3, 0x11 },
    { "MenButton0030_TopN", 4, 4, 0x11 },
    { "MenButton0040_TopN", 5, 5, 0x11 },
    { "MenButton0011_TopN", 6, 6, 0x12 },
    { "MenButton0021_TopN", 7, 7, 0x12 },
    { "MenButton0021_TopN", 8, 8, 0x12 },
    { "MenButton0050_TopN", 9, 21, 0x13 },
    { "MenButton0051_TopN", 24, 24, 0x16 },
    { "MenButton0060_TopN", 22, 22, 0x14 },
    { "MenButton0061_TopN", 23, 23, 0x14 },
    { "MenButton0070_TopN", 25, 25, 0x17 },
    { "MenButton0100_TopN", 26, 26, 0x18 },
    { "MenButton0101_TopN", 27, 27, 0x18 },
    { "MenButton0102_TopN", 28, 28, 0x18 },
    { "MenButton0103_TopN", 29, 29, 0x18 },
    { "MenButton0012_TopN", 30, 30, 0x15 },
    { "MenButton0022_TopN", 31, 31, 0x15 },
    { "MenButton0032_TopN", 32, 32, 0x15 },
    { "MenButton0042_TopN", 33, 33, 0x15 },
    { "MenButton0002_TopN", 34, 34, 0x18 },
};

// Makes the models of the menu and the groups that hang the button texts on the controller models.
void muButtonTask::createModel(nw4r::g3d::ResFile* resFile) {
    createObjResFile(sObjData, 23, resFile);
    m_work->m_scene->setMenuObjs(m_work->m_objs, ObjNum);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    m_work->m_groups[0] = nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, 1, m_work->m_objs[0]->m_sceneModel);
    for (int i = 0; i < PadKind_Num; i++) {
        const PadInfo* info = &sPadInfos[i];
        m_work->m_groups[info->m_group] = nw4r::g3d::ScnMdlExpand::Construct(
            allocator, NULL, info->m_buttonNum + info->m_stickKind + 1, m_work->m_objs[info->m_padObj]->m_sceneModel);
    }
    m_work->m_objs[0]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
    m_work->m_objs[1]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_objs[6]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_objs[7]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_objs[8]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_objs[22]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_objs[23]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_objs[25]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
}

// Makes the objects of a list of models; each one starts on its first animation ("<node>__0"), stopped.
void muButtonTask::createObjResFile(const ObjData* data, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++) {
        int num = 1;
        if (data[i].m_first < data[i].m_last) {
            num = data[i].m_last - data[i].m_first;
        }
        for (int j = 0; j < num; j++) {
            MuObject* obj = MuObject::create(resFile, data[i].m_name, data[i].m_priority, NULL, Heaps::MenuInstance);
            m_work->m_objs[data[i].m_first + j] = obj;
            char animName[64];
            strcpy(animName, data[i].m_name);
            strcat(animName, "__0");
            obj->changeAnimN(animName);
            if (obj->m_modelAnim != NULL) {
                obj->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muButtonTask::processDefault() {
    fn_800B9984(&m_work->m_controller);
    const Step* step = &sSteps[m_work->m_step];
    if (step->m_main != NULL) {
        step->m_main(this);
    }
    m_work->m_scene->updateFrame();
}

// The settings kind of each gfPadType::PadType (GC, classic, Wii remote, nunchuk).
const u8 muButtonTask::sPadKinds[4] = { PadKind_Gc, PadKind_Classic, PadKind_WiiRemote, PadKind_Nunchuk };

const muButtonTask::Step muButtonTask::sSteps[] = {
    { initStepLoading, mainStepLoading },
    { initStepSelectAppear, mainStepSelectAppear },
    { initStepSelect, mainStepSelect },
    { initStepButtonAppear, mainStepButtonAppear },
    { initStepButtonSelect, mainStepButtonSelect },
    { initStepButtonAssign, mainStepButtonAssign },
    { initStepButtonLeave, mainStepButtonLeave },
    { initStepRestoreDlg, mainStepRestoreDlg },
    { initStepTryDlg, mainStepTryDlg },
    { initStepStoreDlg, mainStepStoreDlg },
    { initStepToPractice, mainStepToPractice },
    { initStepPractice, mainStepPractice },
    { initStepStoredDlg, mainStepStoredDlg },
    { initStepExit, mainStepExit },
};

// The animation of the controller choice that shows each controller kind, the animations that turn it to the next
// kind and back (10: none), and the animation that follows each one when it ends.
const u8 muButtonTask::sPadAnims[PadKind_Num] = { 0, 1, 2, 3 };
const u8 muButtonTask::sNextPadAnims[PadKind_Num] = { 4, 5, 6, 10 };
const u8 muButtonTask::sPrevPadAnims[PadKind_Num] = { 10, 9, 8, 7 };
const u8 muButtonTask::sAnimsAfter[SelectAnimNum] = { 10, 10, 10, 10, 1, 2, 3, 2, 1, 0 };

void muButtonTask::setSelectAnim(int anim) {
    static const char* sName = "MenButton0001_TopN__0";
    static MuAnimNameData sAnims[SelectAnimNum] = {
        { 0.0f, 0.0f, false, sName, 3 },   { 5.0f, 5.0f, false, sName, 3 },   { 10.0f, 10.0f, false, sName, 3 },
        { 15.0f, 15.0f, false, sName, 3 }, { 0.0f, 4.0f, false, sName, 3 },   { 5.0f, 9.0f, false, sName, 3 },
        { 10.0f, 14.0f, false, sName, 3 }, { 15.0f, 19.0f, false, sName, 3 }, { 20.0f, 24.0f, false, sName, 3 },
        { 25.0f, 29.0f, false, sName, 3 },
    };
    m_work->m_objs[1]->setAnimName(&sAnims[anim], false);
}

// Hangs the button texts (and the switches) of a controller kind on its model and prints the actions.
void muButtonTask::setupButtons(int padKind) {
    for (int i = 0; i < PadKind_Num; i++) {
        nw4r::g3d::ScnMdlExpand* group = m_work->m_groups[sPadInfos[i].m_group];
        while (!group->Empty()) {
            group->PopBack();
        }
    }
    const PadInfo* info = &sPadInfos[padKind];
    nw4r::g3d::ScnMdlExpand* group = m_work->m_groups[info->m_group];
    int num = info->m_buttonNum;
    for (int i = 0; i < num; i++) {
        char nodeName[16];
        sprintf(nodeName, "pos%02d", i);
        group->PushBack(m_work->m_objs[9 + i]->m_sceneModel, nodeName);
    }
    if (info->m_stickKind == 1) {
        group->PushBack(m_work->m_objs[6]->m_sceneModel, "pos20");
    }
    if (info->m_stickKind == 2) {
        group->PushBack(m_work->m_objs[7]->m_sceneModel, "pos20");
        group->PushBack(m_work->m_objs[8]->m_sceneModel, "pos21");
    }
    for (int i = 0; i < info->m_buttonNum; i++) {
        printButton(padKind, i, i + 5, m_work->m_edit.m_buttons[i], false);
    }
    if (info->m_stickKind >= 1) {
        bool on = m_work->m_edit.m_flickJump == 1;
        int obj;
        switch (info->m_stickKind) {
        case 1:
            obj = 6;
            break;
        case 2:
            obj = 7;
            break;
        }
        m_work->m_objs[obj]->setFrameTex(on ? 1.0f : 0.0f);
    }
    if (info->m_stickKind >= 2) {
        m_work->m_objs[8]->setFrameTex(m_work->m_edit.m_flag6 == 1 ? 1.0f : 0.0f);
    }
}

// The colours of a button text: plain, the default setting, an action that no other button has.
const u8 muButtonTask::sTextColors[3][3] = {
    { 0x00, 0x00, 0x00 },
    { 0x24, 0x2A, 0x82 },
    { 0xB3, 0x00, 0x00 },
};

void muButtonTask::printButton(int padKind, int button, int line, int action, bool markDuplicates) {
    const PadInfo* info = &sPadInfos[padKind];
    int color;
    if (markDuplicates) {
        int i;
        for (i = 0; i < info->m_buttonNum; i++) {
            if (i != button && action == m_work->m_edit.m_buttons[i]) {
                break;
            }
        }
        if (i < info->m_buttonNum) {
            color = 0;
        } else {
            color = 2;
        }
    } else {
        color = 0;
    }
    if (color == 0 && action == sDefaultButtons[padKind][button]) {
        color = 1;
    }
    m_work->m_msg->setFontColor(line, sTextColors[color][0], sTextColors[color][1], sTextColors[color][2], 0xFF);
    m_work->m_msg->printIndex(line, sActionMsgs[action].m_name, NULL);
}

// Moves the cursor of the button screen (-1: hides it).
void muButtonTask::setButtonCursor(int padKind, int cursor) {
    const PadInfo* info = &sPadInfos[padKind];
    MuObject* cursorObj = m_work->m_objs[22];
    nw4r::g3d::ScnMdlExpand* group = m_work->m_groups[info->m_group];
    group->Remove(cursorObj->m_sceneModel);
    m_work->m_scene->removeObj(m_work->m_objs[info->m_cursorObj]->m_sceneModel);
    if (info->m_stickKind == 1) {
        m_work->m_objs[6]->setFrameVisible(0.0f);
    }
    if (info->m_stickKind == 2) {
        m_work->m_objs[7]->setFrameVisible(0.0f);
        m_work->m_objs[8]->setFrameVisible(0.0f);
    }
    m_work->m_scene->removeObj(m_work->m_objs[23]->m_sceneModel);
    if (cursor == -1) {
        return;
    }
    if (cursor >= Cursor_Try) {
        MuObject* obj = m_work->m_objs[23];
        m_work->m_scene->addObj(obj->m_sceneModel);
        obj->setFrameNode(cursor - Cursor_Try);
    } else if (cursor == Cursor_FlickJump) {
        if (info->m_stickKind == 1) {
            m_work->m_objs[6]->setFrameVisible(1.0f);
        }
        if (info->m_stickKind == 2) {
            m_work->m_objs[7]->setFrameVisible(1.0f);
        }
    } else if (cursor == Cursor_Shake) {
        m_work->m_objs[8]->setFrameVisible(1.0f);
    } else {
        char nodeName[16];
        sprintf(nodeName, "pos%02d", cursor);
        group->PushBack(cursorObj->m_sceneModel, nodeName);
        MuObject* obj = m_work->m_objs[info->m_cursorObj];
        m_work->m_scene->addObj(obj->m_sceneModel);
        obj->setFrameVisible(cursor);
    }
}

// The actions that the list offers when no button of the controller has them.
const u8 muButtonTask::sListActions[ListMax] = { 0, 1, 2, 3, 4 };

// Collects the actions of sListActions that no button has (a jump is not missing while the stick jumps).
int muButtonTask::getFreeActions(int padKind, int* actions) {
    const PadInfo* info = &sPadInfos[padKind];
    int num = 0;
    for (int i = 0; i < ListMax; i++) {
        int action = sListActions[i];
        if (action == 2 && info->m_stickKind != 0 && m_work->m_edit.m_flickJump == 1) {
            continue;
        }
        int j;
        for (j = 0; j < info->m_buttonNum; j++) {
            if (action == m_work->m_edit.m_buttons[j]) {
                break;
            }
        }
        if (j >= info->m_buttonNum) {
            *actions++ = action;
            num++;
        }
    }
    return num;
}

// Shows the list of free actions (title: the text above it, frame: where it opens).
void muButtonTask::openActionList(int title, int frame, const int* actions, int num) {
    MuObject* list = m_work->m_objs[24];
    m_work->m_scene->removeObj(list->m_sceneModel);
    m_work->m_scene->addObj(list->m_sceneModel);
    m_work->m_msg->printIndex(0x11, title, NULL);
    m_work->m_objs[24]->setFrameTex(frame);
    if (actions == NULL || num <= 0) {
        num = 0;
    }
    int i;
    for (i = 0; i < num; i++) {
        m_work->m_msg->printIndex(i + 0x12, sActionMsgs[*actions].m_name, NULL);
        actions++;
    }
    for (; i < ListMax; i++) {
        m_work->m_msg->printf(i + 0x12, "");
    }
    list->setFrameNode(num);
    list->setFrameVisible(num);
    list->setFrameMatCol(0.0f);
    list->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_work->m_isListOpen = true;
}

// Opens a dialog (kind 1: restore the defaults, 2: try the buttons, 3: store the settings) with a text.
void muButtonTask::openDlg(int kind, int msg) {
    if (m_work->m_dlgKind != 0) {
        m_work->m_scene->removeObj(m_work->m_objs[25]->m_sceneModel);
        m_work->m_dlgKind = 0;
    }
    m_work->m_dlgKind = kind;
    MuObject* dlg = m_work->m_objs[25];
    float frame;
    switch (kind) {
    case 1:
        switch (m_work->m_dlgKind) {
        case 1:
            frame = 1.0f;
            break;
        case 3:
            frame = 4.0f;
            break;
        }
        dlg->setFrameNode(frame);
        dlg->setFrameVisible(frame);
        break;
    case 2:
        frame = 2.0f;
        dlg->setFrameNode(frame);
        dlg->setFrameVisible(frame);
        break;
    case 3:
        switch (m_work->m_dlgKind) {
        case 1:
            frame = 1.0f;
            break;
        case 3:
            frame = 4.0f;
            break;
        }
        dlg->setFrameNode(frame);
        dlg->setFrameVisible(frame);
        break;
    }
    m_work->m_scene->addObj(dlg->m_sceneModel);
    m_work->m_msg->printIndex(0x17, msg, NULL);
}

// Prints the explanation of what the cursor is on.
void muButtonTask::printExplByButtonCursor(int cursor) {
    int msg;
    switch (cursor) {
    case 22:
        msg = 11;
        break;
    case 23:
        msg = 12;
        break;
    case 24:
        msg = 13;
        break;
    case Cursor_FlickJump:
        msg = 9;
        break;
    case Cursor_Shake:
        msg = 10;
        break;
    default:
        msg = sActionMsgs[m_work->m_edit.m_buttons[cursor]].m_expl;
        break;
    }
    m_work->m_msg->printIndex(1, msg, NULL);
}

const u32 muButtonTask::sGcMasks[11] = {
    0x40, 0x20, 0x10, 0x800, 0x400, 0x100, 0x200, 0x40000, 0x8, 0x3, 0x4,
};
const u32 muButtonTask::sNunchukMasks[11] = {
    0x8, 0x3, 0x4, 0x200000, 0x400000, 0x80600000, 0x10000, 0x20000, 0x80030000, 0x40000, 0x10,
};
const u32 muButtonTask::sWiiRemoteMasks[8] = {
    0x8, 0x200000, 0x80000, 0x20000, 0x10000, 0x80030000, 0x400000, 0x80600000,
};
const u32 muButtonTask::sClassicMasks[12] = {
    0x40, 0x20, 0x4000, 0x8000, 0x400, 0x800, 0x100, 0x200, 0x40000, 0x8, 0x3, 0x4,
};
const u32* muButtonTask::sButtonMasks[PadKind_Num] = { sGcMasks, sNunchukMasks, sWiiRemoteMasks, sClassicMasks };

// The button of the screen that was pushed on a pad (-1: none). On the GC controller and the classic controller the
// C stick counts as a button (0x40000) that is pushed on the frame it is tilted.
int muButtonTask::getPushedButton(int padNo) {
    gfPadStatus pad;
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(padNo, &status);
    int padKind = status.m_error == gfPadError::NONE ? sPadKinds[status.m_controllerType] : PadKind_WiiRemote;
    g_gfPadSystem->getSysPadStatus(padNo, &pad);
    if (padKind == PadKind_Gc || padKind == PadKind_Classic) {
        pad.m_buttonsPressedThisFrame.bits &= ~0x40000;
        if (pad.m_subStickX * pad.m_subStickX + pad.m_subStickY * pad.m_subStickY >= 2500) {
            if (!m_work->m_isSubStickOn) {
                pad.m_buttonsPressedThisFrame.bits |= 0x40000;
            }
            pad.m_buttonsCurrentFrame2.bits |= 0x40000;
            m_work->m_isSubStickOn = true;
        } else {
            m_work->m_isSubStickOn = false;
        }
    } else {
        m_work->m_isSubStickOn = false;
    }
    const PadInfo* info = &sPadInfos[padKind];
    for (int i = 0; i < info->m_buttonNum; i++) {
        u32 mask = sButtonMasks[padKind][i];
        u32 bits = mask & 0x7FFFFFFF;
        if ((mask >> 31) && (bits & pad.m_buttonsCurrentFrame2.bits) == bits && (pad.m_buttonsPressedThisFrame.bits & bits)) {
            return i;
        }
    }
    for (int i = 0; i < info->m_buttonNum; i++) {
        u32 mask = sButtonMasks[padKind][i];
        if (!(mask & 0x80000000) && (pad.m_buttonsPressedThisFrame.bits & mask & 0x7FFFFFFF)) {
            return i;
        }
    }
    return -1;
}

void muButtonTask::initStepLoading(muButtonTask* task) {
    task->m_handles[0].readRequest("/menu/option/Button.brres", Heaps::MenuInstance, 0, 0);
    task->m_handles[1].readRequest(g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen
                                       ? "/menu/defaultcamera/DefaultSceneWide.brres"
                                       : "/menu/defaultcamera/DefaultScene.brres",
                                   Heaps::MenuInstance, 0, 0);
    task->m_handles[2].readRequest("/menu/msg/button.msbin", Heaps::MenuInstance, 0, 0);
    muMenu::loadMenuSound();
}

void muButtonTask::initStepSelectAppear(muButtonTask* task) {
    g_gfApplication->unkD0.endKeepScreen();
    task->m_work->m_scene->removeAllObj();
    nw4r::g3d::ScnMdlExpand* group = task->m_work->m_groups[0];
    task->m_work->m_scene->addObj(group);
    MuObject* bg = task->m_work->m_objs[0];
    bg->setFrameNode(0.0f);
    bg->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    bg->setFrameMatCol(0.0f);
    bg->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    group->PushBack(task->m_work->m_objs[1]->m_sceneModel, "pos00");
    int anim = sPadAnims[task->m_work->m_padKind];
    task->setSelectAnim(anim);
    task->m_work->m_selectAnim = anim;
    task->m_work->m_msg->printIndex(1, 0, NULL);
    char name[32];
    if (g_GameGlobal->getName(task->m_work->m_scene->m_buttonConfigNo, name)) {
        task->m_work->m_msg->printf(0, name);
        task->m_work->m_msg->printf(0x1C, name);
    }
}


void muButtonTask::initStepButtonSelect(muButtonTask* task) {
    int padKind = task->m_work->m_padKind;
    int cursor = task->m_work->m_cursor;
    task->setButtonCursor(padKind, cursor);
    task->printExplByButtonCursor(cursor);
    task->m_work->m_objs[22]->setFrameVisible(0.0f);
    cursor = task->m_work->m_cursor;
    if (cursor == 23 || cursor == 24) {
        int actions[ListMax];
        int num = task->getFreeActions(padKind, actions);
        if (num > 0) {
            int frame = cursor != 23;
            if (!task->m_work->m_isListOpen) {
                task->openActionList(0x1D, frame, actions, num);
            } else {
                task->m_work->m_objs[24]->setFrameTex(frame);
            }
        }
    }
}

void muButtonTask::initStepRestoreDlg(muButtonTask* task) {
    task->setButtonCursor(task->m_work->m_padKind, -1);
    task->openDlg(1, 0x1E);
    task->m_work->m_unk1BC = 0;
}

void muButtonTask::initStepTryDlg(muButtonTask* task) {
    task->setButtonCursor(task->m_work->m_padKind, -1);
    task->openDlg(2, 0x1F);
}

void muButtonTask::initStepStoreDlg(muButtonTask* task) {
    task->setButtonCursor(task->m_work->m_padKind, -1);
    task->openDlg(3, 0x20);
    task->m_work->m_unk1BC = 0;
}

void muButtonTask::initStepExit(muButtonTask* task) {
    task->m_work->m_exitWait = 0x20;
    GXColor black = nw4r::ut::Color(0, 0, 0, 0xFF);
    g_efScreen->requestFill(30.0f, 7, 0x80, &black);
}

void muButtonTask::mainStepExit(muButtonTask* task) {
    int wait = task->m_work->m_exitWait;
    if (wait > 0) {
        task->m_work->m_exitWait = wait - 1;
    } else if (wait == 0) {
        task->m_work->m_exitWait = wait - 1;
        task->m_work->m_scene->unkD8_b5 = false;
    }
}
