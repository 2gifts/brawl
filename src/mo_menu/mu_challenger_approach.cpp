#include <gf/gf_pad_status.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_challenger_approach.h>

// main.dol functions that have no names yet.
// Creates the camera task that draws the menu models of the name, with the camera of the given file (the task keeps the
// scene group at +0x48 that the models are inserted into).
extern "C" gfTask* fn_800AFB94(const char* name, int heap, const char* cameraFile);
// sndSystem::stopSE(handle, fade frames) (main.dol, no name yet)
extern "C" void fn_80075F1C(sndSystem* system, s32 handle, int fade);
// nw4r::g3d::ResFile::Release()
extern "C" void Release__Q34nw4r3g3d7ResFileFv(nw4r::g3d::ResFile* resFile);

muChallengerApproachTask* muChallengerApproachTask::create() {
    return new (Heaps::MenuInstance) muChallengerApproachTask();
}

muChallengerApproachTask::muChallengerApproachTask() : gfTask("ChallengerApproach", Category_Menu, 0xF, 8, true) {
    m_resFileBase = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileChallenger = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    memset(m_nodeNames, 0, sizeof(m_nodeNames));
    sprintf(m_nodeNames[0], "MenChallenger0001_TopN");
    sprintf(m_nodeNames[1], "MenChallenger0002_TopN");
    sprintf(m_nodeNames[2], "MenChallenger0003_TopN");
    sprintf(m_nodeNames[3], "MenChallenger0004_TopN");
    sprintf(m_nodeNames[4], "MenChallenger0005_TopN");
    sprintf(m_nodeNames[5], "MenChallenger0006_TopN");
    sprintf(m_nodeNames[6], "MenChallenger0007_TopN");
    sprintf(m_nodeNames[7], "MenChallenger0008_TopN");
    sprintf(m_nodeNames[8], "MenChallenger0009_TopN");
    sprintf(m_nodeNames[9], "MenChallenger0010_TopN");
    sprintf(m_nodeNames[10], "MenChallenger0011_TopN");
    sprintf(m_nodeNames[11], "MenChallenger0012_TopN");
    sprintf(m_nodeNames[12], "MenChallenger0013_TopN");
    sprintf(m_nodeNames[13], "MenChallenger0014_TopN");
    m_cameraTask = NULL;
    m_objBase = NULL;
    m_objChallenger = NULL;
    m_step = Step_Idle;
    m_frame = 0;
}

muChallengerApproachTask::~muChallengerApproachTask() { }

// The state machine of the approach: appear, wait for a button, leave.
void muChallengerApproachTask::processDefault() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee)[0x9F] - 1, &pad);

    switch (m_step) {
    case Step_Start:
        m_step = Step_Wait;
        break;
    case Step_Wait: {
        // the animation of the challenger has the name of its panel and the suffix "__0"
        char animName[0x40];
        memset(animName, 0, sizeof(animName));
        strcat(animName, m_nodeNames[m_challengerNo]);
        strcat(animName, "__0");
        m_objBase->changeNodeAnimNIf("MenChallenger0000_TopN__0");
        m_objBase->changeVisAnimNIf("MenChallenger0000_TopN__0");
        m_objBase->changeClrAnimN("MenChallenger0000_TopN__0");
        m_objChallenger->changeNodeAnimNIf(animName);
        m_objChallenger->changeVisAnimNIf(animName);
        m_objChallenger->changeClrAnimN(animName);
        m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x1EEB), -1, 0, 0, -1);
        m_step = Step_AppearPlay;
        break;
    }
    case Step_AppearPlay:
        {
            // all six animations (node, visibility, color of both panels) have to be finished
            u32 finished;
            if (!m_objBase->isNodeAnimFinished()) {
                finished = 0;
            } else if (!m_objBase->isVisAnimFinished()) {
                finished = 0;
            } else if (!m_objBase->isClrAnimFinished()) {
                finished = 0;
            } else if (!m_objChallenger->isNodeAnimFinished()) {
                finished = 0;
            } else if (!m_objChallenger->isVisAnimFinished()) {
                finished = 0;
            } else {
                u32 clr = m_objChallenger->isClrAnimFinished();
                finished = ((-clr) | clr) >> 31;
            }
            if (finished == 1) {
                m_step = Step_AppearWait;
            }
        }
        break;
    case Step_AppearWait:
        m_step = Step_AppearEnd;
        break;
    case Step_AppearEnd:
        m_step = Step_WaitButton;
        break;
    case Step_WaitButton: {
        // A, B, Start (or the button that is bit 20 of the status) lets the challenger go on
        u32 pressed = pad.m_buttonsPressedThisFrame.bits;
        bool isPressed;
        if ((pressed & 0x100) != 0) {
            isPressed = true;
        } else if ((pressed & 0x200) != 0) {
            isPressed = true;
        } else if ((pressed & 0x1000) != 0) {
            isPressed = true;
        } else {
            isPressed = (pressed >> 20) & 1;
        }
        if (isPressed == true) {
            g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
            m_step = Step_Pressed;
        }
        break;
    }
    case Step_Pressed:
        m_step = Step_ExitStart;
        break;
    case Step_ExitStart: {
        // the animation that takes the panels away has the suffix "__1"
        char animName[0x40];
        memset(animName, 0, sizeof(animName));
        strcat(animName, m_nodeNames[m_challengerNo]);
        strcat(animName, "__1");
        m_objBase->changeNodeAnimNIf("MenChallenger0000_TopN__1");
        m_objBase->changeVisAnimNIf("MenChallenger0000_TopN__1");
        m_objBase->changeClrAnimN("MenChallenger0000_TopN__1");
        m_objChallenger->changeNodeAnimNIf(animName);
        m_objChallenger->changeVisAnimNIf(animName);
        m_objChallenger->changeClrAnimN(animName);
        m_step = Step_ExitPlay;
        break;
    }
    case Step_ExitPlay:
        {
            // all six animations (node, visibility, color of both panels) have to be finished
            u32 finished;
            if (!m_objBase->isNodeAnimFinished()) {
                finished = 0;
            } else if (!m_objBase->isVisAnimFinished()) {
                finished = 0;
            } else if (!m_objBase->isClrAnimFinished()) {
                finished = 0;
            } else if (!m_objChallenger->isNodeAnimFinished()) {
                finished = 0;
            } else if (!m_objChallenger->isVisAnimFinished()) {
                finished = 0;
            } else {
                u32 clr = m_objChallenger->isClrAnimFinished();
                finished = ((-clr) | clr) >> 31;
            }
            if (finished == 1) {
                m_step = Step_ExitEnd;
            }
        }
        break;
    case Step_ExitEnd:
        fn_80075F1C(g_sndSystem, m_seHandle, 0);
        m_step = Step_Done;
        break;
    case Step_Done:
    case 12:
        break;
    }
    m_frame++;
}

// Starts the approach of a challenger (1 - 14 is the number of the panel + 1).
void muChallengerApproachTask::initialize(int challengerNo) {
    m_challengerNo = challengerNo;
    unk2C_b1 = false;
    m_seHandle = 0;
}

void muChallengerApproachTask::release() {
    m_cameraTask->exit();
    m_cameraTask = NULL;
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileBase);
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileChallenger);
    delete m_objBase;
    m_objBase = NULL;
    delete m_objChallenger;
    m_objChallenger = NULL;
}

// Loads the panels from the menu archive and hangs them in the scene of a camera task.
void muChallengerApproachTask::createData(gfArchive* archive) {
    m_resFileBase = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 0, 0xFFFE));
    m_resFileChallenger = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 1, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFileBase);
    nw4r::g3d::ResFile::Init(&m_resFileChallenger);
    m_cameraTask = fn_800AFB94("ChallengerTask", 0x10, "/menu/defaultcamera/CharacterSelect.brres");
    m_objBase = MuObject::create(&m_resFileBase, "MenChallenger0000_TopN", 1, NULL, Heaps::MenuInstance);
    m_objChallenger = MuObject::create(&m_resFileChallenger, m_nodeNames[m_challengerNo], 1, NULL, Heaps::MenuInstance);
    nw4r::g3d::ScnGroup* group = *reinterpret_cast<nw4r::g3d::ScnGroup**>(reinterpret_cast<u8*>(m_cameraTask) + 0x48);
    group->Insert(reinterpret_cast<int*>(group)[0x39], reinterpret_cast<nw4r::g3d::ScnObj*>(m_objBase->m_scnMdl));
    group = *reinterpret_cast<nw4r::g3d::ScnGroup**>(reinterpret_cast<u8*>(m_cameraTask) + 0x48);
    group->Insert(reinterpret_cast<int*>(group)[0x39], reinterpret_cast<nw4r::g3d::ScnObj*>(m_objChallenger->m_scnMdl));
}
