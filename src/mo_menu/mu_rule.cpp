#include <gf/gf_heap_manager.h>
#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <types.h>

#include <mu/mu_rule.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size, int capacity,
                                                                                                    void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* scnMdl, void* object, const char* name);

muRuleTask::muRuleTask(nw4r::g3d::ScnGroup* group, gfArchive* archive, muPanelCtrlTask* panel, int mode, int priority)
    : gfTask("muRuleTask", Category_Menu, 15, 8, true) {
    m_curProc = Proc_Rule1;
    m_prevProc = Proc_None;
    m_group = group;
    m_timer = 0;
    m_end = false;
    m_panel = panel;
    m_startIsCancel = true;
    unk2C_b1 = false;

    muObjProcessTask* objProc = muObjProcessTask::create(Heaps::MenuInstance);
    m_objProc = objProc;
    objProc->m_objs = m_objs;
    objProc->m_count = 0x82;
    for (int i = 0; i < 0x82; i++) {
        m_objs[i] = NULL;
    }

    m_resFile = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);

    // The icons that show the choice of each item of the first screen (the number of the icon is in the name).
    m_objs[0] = MuObject::create(&m_resFile, "MenMainIcon0121_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[1] = MuObject::create(&m_resFile, "MenMainIcon0122_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[7] = MuObject::create(&m_resFile, "MenMainIcon0123_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[2] = MuObject::create(&m_resFile, "MenMainIcon0124_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[3] = MuObject::create(&m_resFile, "MenMainIcon0125_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[4] = MuObject::create(&m_resFile, "MenMainIcon0126_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[5] = MuObject::create(&m_resFile, "MenMainIcon0127_TopN", priority, NULL, Heaps::MenuInstance);
    m_objs[6] = MuObject::create(&m_resFile, "MenMainIcon0128_TopN", priority, NULL, Heaps::MenuInstance);

    // The digits of the numbers of the first screen (two numbers of two digits), the colon and the infinity mark.
    for (u32 i = 8; i <= 15; i++) {
        m_objs[i] = MuObject::create(&m_resFile, "MenMainIcon01Num_TopN", priority + 2, NULL, Heaps::MenuInstance);
    }
    m_objs[0x10] = MuObject::create(&m_resFile, "MenMainIcon01Colon_TopN", priority + 2, NULL, Heaps::MenuInstance);
    m_objs[0x11] = MuObject::create(&m_resFile, "MenMainIcon01Infini_TopN", priority + 3, NULL, Heaps::MenuInstance);
    m_objs[0x12] = MuObject::create(&m_resFile, "MenMainIcon0131_TopN", priority + 4, NULL, Heaps::MenuInstance);
    m_objs[0x13] = MuObject::create(&m_resFile, "MenMainIcon0132_TopN", priority + 4, NULL, Heaps::MenuInstance);
    m_objs[0x14] = MuObject::create(&m_resFile, "MenMainIcon0133_TopN", priority + 4, NULL, Heaps::MenuInstance);
    m_objs[0x15] = MuObject::create(&m_resFile, "MenMainIcon0134_TopN", priority + 4, NULL, Heaps::MenuInstance);
    m_objs[0x16] = MuObject::create(&m_resFile, "MenMainIcon0136_TopN", priority + 4, NULL, Heaps::MenuInstance);
    m_objs[0x17] = MuObject::create(&m_resFile, "MenMainIcon0135_TopN", priority + 4, NULL, Heaps::MenuInstance);
    for (u32 i = 0x18; i <= 0x1B; i++) {
        m_objs[i] = MuObject::create(&m_resFile, "MenMainIcon01Num_TopN", priority + 6, NULL, Heaps::MenuInstance);
    }
    m_objs[0x1C] = MuObject::create(&m_resFile, "MenMainIcon01Colon_TopN", priority + 6, NULL, Heaps::MenuInstance);
    m_objs[0x1D] = MuObject::create(&m_resFile, "MenMainIcon01Infini_TopN", priority + 6, NULL, Heaps::MenuInstance);

    // The switches (the buttons of the screens of the items and of the stages).
    m_objs[0x1E] = MuObject::create(&m_resFile, "MenMainSwitch0001_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x1E]->changeNodeAnimN("MenMainSwitch0001_TopN__0");
    m_objs[0x1F] = MuObject::create(&m_resFile, "MenMainSwitch0002_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x1F]->changeNodeAnimN("MenMainSwitch0002_TopN__0");
    m_objs[0x1F]->changeClrAnimN("MenMainSwitch0002_TopN__0");
    for (int i = 0; i < 0x31; i++) {
        m_objs[0x20 + i] = MuObject::create(&m_resFile, "MenMainSwitch0003_TopN", priority + 8, NULL, Heaps::MenuInstance);
        m_objs[0x20 + i]->changeClrAnimN("MenMainSwitch0003_TopN__0");
        m_objs[0x20 + i]->changeTexPatAnimN("MenMainSwitch0003_TopN__0");
    }
    m_objs[0x51] = MuObject::create(&m_resFile, "MenMainSwitch0004_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x51]->changeClrAnimN("MenMainSwitch0004_TopN__0");
    m_objs[0x52] = MuObject::create(&m_resFile, "MenMainSwitch0005_TopN", priority + 9, NULL, Heaps::MenuInstance);
    m_objs[0x52]->changeNodeAnimN("MenMainSwitch0005_TopN__0");
    m_objs[0x52]->changeTexPatAnimN("MenMainSwitch0005_TopN__0");
    m_objs[0x53] = MuObject::create(&m_resFile, "MenMainSwitch0006_TopN", priority + 9, NULL, Heaps::MenuInstance);
    m_objs[0x53]->changeNodeAnimN("MenMainSwitch0006_TopN__0");
    m_objs[0x53]->changeClrAnimN("MenMainSwitch0006_TopN__0");
    m_objs[0x53]->changeVisAnimN("MenMainSwitch0006_TopN__0");
    m_objs[0x54] = MuObject::create(&m_resFile, "MenMainSwitch0101_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x54]->changeNodeAnimN("MenMainSwitch0101_TopN__0");
    m_objs[0x54]->changeTexPatAnimN("MenMainSwitch0101_TopN__0");
    m_objs[0x55] = MuObject::create(&m_resFile, "MenMainSwitch0102_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x55]->changeClrAnimN("MenMainSwitch0102_TopN__0");
    m_objs[0x56] = MuObject::create(&m_resFile, "MenMainSwitch0103_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x56]->changeClrAnimN("MenMainSwitch0103_TopN__0");
    for (int i = 0; i < 0x1F; i++) {
        m_objs[0x57 + i] = MuObject::create(&m_resFile, "MenMainSwitch0104_TopN", priority + 8, NULL, Heaps::MenuInstance);
        m_objs[0x57 + i]->changeClrAnimN("MenMainSwitch0104_TopN__0");
        m_objs[0x57 + i]->changeTexPatAnimN("MenMainSwitch0104_TopN__0");
    }
    for (int i = 0; i < 10; i++) {
        m_objs[0x76 + i] = MuObject::create(&m_resFile, "MenMainSwitch0105_TopN", priority + 8, NULL, Heaps::MenuInstance);
        m_objs[0x76 + i]->changeClrAnimN("MenMainSwitch0105_TopN__0");
        m_objs[0x76 + i]->changeTexPatAnimN("MenMainSwitch0105_TopN__0");
    }
    m_objs[0x80] = MuObject::create(&m_resFile, "MenMainSwitch0106_TopN", priority + 8, NULL, Heaps::MenuInstance);
    m_objs[0x80]->changeClrAnimN("MenMainSwitch0106_TopN__0");
    m_objs[0x81] = MuObject::create(&m_resFile, "MenMainSwitch0107_TopN", priority + 9, NULL, Heaps::MenuInstance);
    m_objs[0x81]->changeNodeAnimN("MenMainSwitch0107_TopN__0");
    m_objs[0x81]->changeClrAnimN("MenMainSwitch0107_TopN__0");

    // Nothing moves until a screen starts its animations.
    for (u32 i = 0; i < 0x82; i++) {
        m_objs[i]->m_modelAnim->setUpdateRate(0.0f);
    }

    // The models that the objects are put in: the base model of each one is an object, and it gets room for the others.
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    u32 size;
    char name[32];
    int capacities[6] = { 5, 2, 2, 5, 0x35, 0x2D };
    int bases[6] = { 1, 3, 7, 0x12, 0x1E, 0x54 };
    for (u32 i = 0; i < 6; i++) {
        m_mdls[i] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(allocator, &size, capacities[i], m_objs[bases[i]]->m_sceneModel);
    }
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[1], m_objs[14]->m_sceneModel, "num2N");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[1], m_objs[15]->m_sceneModel, "num1N");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[2], m_objs[12]->m_sceneModel, "num2N");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[2], m_objs[13]->m_sceneModel, "num1N");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[4], m_objs[0x1F]->m_sceneModel, "pos00");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[4], m_objs[0x51]->m_sceneModel, "pos01");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[4], m_objs[0x52]->m_sceneModel, "pos60");
    for (int i = 0; i < 0x31; i++) {
        sprintf(name, "pos%02d", i + 2);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[4], m_objs[0x20 + i]->m_sceneModel, name);
    }

    // The screens. The second one is not the same in the three modes that have it.
    m_procs[Proc_Rule1] = new (Heaps::MenuInstance) muProcRule1(m_objs, m_mdls, mode, priority);
    if (mode == 1) {
        m_procs[Proc_Rule2] = new (Heaps::MenuInstance) muProcRule2(m_objs, m_mdls, mode, priority);
    } else if (mode == 2) {
        m_procs[Proc_Rule2] = new (Heaps::MenuInstance) muProcRule2(m_objs, m_mdls, mode, priority);
    } else {
        m_procs[Proc_Rule2] = new (Heaps::MenuInstance) muProcRule2(m_objs, m_mdls, mode, priority);
    }
    m_procs[Proc_ItemSwitch] = new (Heaps::MenuInstance) muProcItemSwitch(m_objs, m_mdls, archive->getData(Data_Type_Misc, 0, 0xFFFE));
    m_procs[Proc_StageSwitch] = new (Heaps::MenuInstance) muProcStageSwitch(m_objs, m_mdls, archive->getData(Data_Type_Misc, 1, 0xFFFE));

    for (int i = 0; i < Proc_Count; i++) {
        m_procs[i]->setPanelTask(panel);
    }
    m_msgData = panel->getMsgData();
    panel->setMsgData(archive->getData(Data_Type_Misc, 0, 0xFFFE));

    m_mode = mode;
    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 6:
        m_procs[Proc_Rule1]->init(m_group, 6);
        break;
    case 3:
        m_curProc = Proc_ItemSwitch;
        m_procs[Proc_ItemSwitch]->init(m_group, 0);
        break;
    case 4:
        m_curProc = Proc_StageSwitch;
        m_procs[Proc_StageSwitch]->init(m_group, 1);
        *(reinterpret_cast<u8*>(m_procs[Proc_StageSwitch]) + 0x762) = 1;
        break;
    case 5:
        m_curProc = Proc_StageSwitch;
        m_procs[Proc_StageSwitch]->init(m_group, 1);
        break;
    }
    unk2C_b1 = false;
}

muRuleTask::~muRuleTask() {
    for (u32 i = 0; i < 6; i++) {
        m_mdls[i]->Destroy();
        m_mdls[i] = NULL;
    }
    for (u32 i = 0; i < 0x82; i++) {
        if (m_objs[i] != NULL) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    for (u16 i = 0; i < Proc_Count; i++) {
        delete m_procs[i];
        m_procs[i] = NULL;
    }
    m_objProc->exit();
    m_objProc = NULL;
}

void muRuleTask::processAnim() {
    int result = 0;
    m_timer++;
    if (m_curProc != Proc_None) {
        result = m_procs[m_curProc]->proc();
        if (result != 0 && m_prevProc != Proc_None) {
            m_procs[m_prevProc]->exit();
            m_prevProc = Proc_None;
        }
    }
    if (m_prevProc != Proc_None) {
        if (m_procs[m_prevProc]->proc() == 3) {
            m_procs[m_prevProc]->exit();
            m_prevProc = Proc_None;
        }
    }
    switch (result) {
    case 1: {
        // The proc is done: the menu closes if it is the screen the menu started with, or the player goes back to it.
        int home = Proc_Rule1;
        switch (m_mode) {
        case 3:
            home = Proc_ItemSwitch;
            break;
        case 4:
        case 5:
            home = Proc_StageSwitch;
            break;
        }
        m_prevProc = m_curProc;
        if (m_curProc == home) {
            m_timer = 0;
            m_end = true;
            m_curProc = Proc_None;
            m_panel->setMsgData(m_msgData);
        } else {
            m_curProc = m_procs[m_prevProc]->m_result - 0x1E;
            m_procs[m_curProc]->init(m_group, m_prevProc);
        }
        if (m_prevProc != home) {
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        }
        break;
    }
    case 2:
        // The player picked an entry that opens another screen.
        m_prevProc = m_curProc;
        m_curProc = m_procs[m_prevProc]->m_result - 0x1E;
        m_procs[m_curProc]->init(m_group, m_prevProc);
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        break;
    }
}

bool muRuleTask::isProcessEnd() {
    if (m_end && m_prevProc == Proc_None) {
        return true;
    }
    return false;
}

void muRuleTask::removeObj() {
    if (m_curProc != Proc_None) {
        m_procs[m_curProc]->exit();
        m_curProc = Proc_None;
    }
    if (m_prevProc != Proc_None) {
        m_procs[m_prevProc]->exit();
        m_prevProc = Proc_None;
    }
    m_end = true;
}

void muRuleTask::setCtrlFlg(u8 mask) {
    for (int i = 0; i < Proc_Count; i++) {
        m_procs[i]->setCtrlFlg(mask);
    }
}

void muRuleTask::setStartButtonIsCancel(bool value) {
    m_startIsCancel = value;
    if (m_procs[Proc_Rule1] != NULL) {
        *(reinterpret_cast<u8*>(m_procs[Proc_Rule1]) + 0x6A9) = value;
    }
    if (m_procs[Proc_Rule2] != NULL) {
        *(reinterpret_cast<u8*>(m_procs[Proc_Rule2]) + 0x69B) = value;
    }
    if (m_procs[Proc_ItemSwitch] != NULL) {
        *(reinterpret_cast<u8*>(m_procs[Proc_ItemSwitch]) + 0x688) = value;
    }
    if (m_procs[Proc_StageSwitch] != NULL) {
        *(reinterpret_cast<u8*>(m_procs[Proc_StageSwitch]) + 0x761) = value;
    }
}
