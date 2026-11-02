#include <types.h>

#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <mu/mu_menu_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <mu/mu_name.h>
#include <mu/mu_msg.h>
#include <snd/snd_system.h>

#include <nw4r/g3d/g3d_scnobj.h>

#include <mu/mu_proc_name.h>

// The animations of the window: hidden, comes, shown, leaves.
static MuAnimIdxData sConfirmAnims[4] = {
    { 0.0f, 0.0f, 0, 0, 0x1 },
    { 0.0f, 4.0f, 0, 0, 0x1 },
    { 4.0f, 4.0f, 0, 0, 0x1 },
    { 5.0f, 9.0f, 0, 0, 0x1 },
};

void muProcName::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk670 = 0;
    m_unk66C_0 = false;
    m_unk710 = 0;
    m_unk712 = 0;
    m_nameTask = NULL;
    m_shown = false;
    m_panel->changeProcess1(0x10, 0x28);

    // The question of the window
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x100, 1);
    m_msg->setMsgData(m_msgFile);
    m_msg->attachScnMdlSimple(0, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Confirm]->m_sceneModel), 0,
                              1.0f / 16.0f);
    m_msg->getWScale(0, 48.0f);
    m_msg->printIndex(0, 0x2B);

    MuObject* objects[4] = {
        m_objs[Obj_NameList0], m_objs[Obj_NameList0 + 1], m_objs[Obj_NameList0 + 2], m_objs[Obj_NameRemote],
    };
    nw4r::g3d::ScnMdl* mdls[2] = {
        reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[Mdl_Name0]), reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[Mdl_Name0 + 1]),
    };
    muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
    if (m_nameTask != NULL) {
        m_nameTask->exit();
        m_nameTask = NULL;
    }
    m_nameTask = muNameTask::create1(group, 1, objects, mdls, &menu->m_resFileA98, 0xF0, 0x19, NULL, NULL, m_remoteRes,
                                     m_msgData, m_wndRes);
    m_unk664 = Step_Name;
}

int muProcName::proc() {
    int ret = 0;
    switch (m_unk664) {
    case Step_Name:
        ret = selectNameMain();
        break;
    case Step_Confirm: {
        int answer = confirmMain();
        if (answer == 2) {
            break;
        }
        if (answer == 0) {
            m_nameTask->deleteNameID(m_nameNo);
        }
        m_nameTask->m_step = muNameTask::Step_BackAvoidList;
        m_objs[Obj_Confirm]->setAnimIdx(&sConfirmAnims[3], false);
        m_unk664 = Step_ConfirmLeave;
        break;
    }
    case Step_ConfirmLeave:
        if (m_nameTask->m_resultKind == muNameTask::Result_ListShown) {
            m_nameTask->clearResult();
            m_group->Remove(m_objs[Obj_Confirm]->m_sceneModel);
            m_unk664 = Step_Name;
        }
        break;
    case Step_Done:
        ret = 3;
        break;
    }
    return ret;
}

int muProcName::selectNameMain() {
    int ret = 0;
    if (!m_shown && m_nameTask->m_step != muNameTask::Step_Appear) {
        m_shown = true;
        ret = 4;
    }
    switch (m_nameTask->m_resultKind) {
    case muNameTask::Result_Chosen:
        // The list makes room for the window, which asks whether to delete the name (No selected)
        m_nameTask->m_step = muNameTask::Step_AvoidList;
        m_nameNo = m_nameTask->m_resultNameNo;
        m_unk664 = Step_Confirm;
        m_yes = false;
        m_objs[Obj_Confirm]->setFrameMatCol(0.0f);
        m_group->Insert(m_group->sceneItemsCount, m_objs[Obj_Confirm]->m_sceneModel);
        m_objs[Obj_Confirm]->setAnimIdx(&sConfirmAnims[1], false);
        g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
        m_nameTask->clearResult();
        break;
    case muNameTask::Result_Cancelled:
        // Back to the Options screen
        m_result = 1;
        ret = 1;
        m_nameTask->clearResult();
        break;
    case muNameTask::Result_Closed:
        ret = 3;
        m_unk664 = Step_Done;
        m_nameTask->exit();
        m_nameTask = NULL;
        break;
    }
    if (m_nameTask != NULL && m_nameTask->m_step == muNameTask::Step_SelectName) {
        // HYPOTHESIS: the help text tells whether the cursor is on a name (0x29) or not (0x28)
        m_panel->setMsg(m_nameTask->getCurNameIndex() < 0 ? 0x28 : 0x29, 1, 0);
    }
    return ret;
}

void muProcName::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Name0]));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Name0 + 1]));
    m_msg->detachMuObject(m_objs[Obj_Confirm]);
    delete m_msg;
    if (m_nameTask != NULL) {
        m_nameTask->exit();
        m_nameTask = NULL;
    }
}

// The window: A answers what is selected, B is No; Left selects Yes, Right No.
int muProcName::confirmMain() {
    u32 pad = padUpdate();
    MuObject* confirm = m_objs[Obj_Confirm];
    if (pad & Pad_Cancel) {
        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        return 1;
    }
    if (pad & Pad_Ok) {
        if (!m_yes) {
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            return 1;
        }
        g_sndSystem->playSE(static_cast<SndID>(0x28), -1, 0, 0, -1);
        return 0;
    }
    if ((pad & Pad_Right) && m_yes) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        confirm->setFrameMatCol(0.0f);
        m_yes = false;
    }
    if ((pad & Pad_Left) && m_yes != true) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        confirm->setFrameMatCol(1.0f);
        m_yes = true;
    }
    return 2;
}

void muProcName::playSE_CursorMove() { }
