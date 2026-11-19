#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <if/if_mngr.h>
#include <if/if_smash_appear.h>
#include <memory.h>
#include <ms/ms_resfont.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <ut/ut_relocate.h>

// nw4r::g3d::ScnGroup::Construct(allocator, size, capacity)
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, u32 capacity);
// HYPOTHESIS: a GameGlobal query that selects the full-size message layout (map name missing).
extern "C" int fn_8004D9E8(GameGlobal* global);
// MuMsg::endPrint is not in the BrawlHeaders MuMsg.
// HYPOTHESIS: the original passes a message index (0) to endPrint.
extern "C" void endPrint__5MuMsgFv(MuMsg* msg, u32 msgIndex);
// IfSmashAppearTask::initMsg, destroyModel, processDefault and setScript (map names; initMsg needs the private
// Message of MuMsg, the others are not exact yet)
extern "C" void fn_800F5084(IfSmashAppearTask* task, void* msgData, IfSmashAppearTask::MsgData* data);
extern "C" void fn_800F5284(IfSmashAppearTask* task);
extern "C" void fn_800F5344(IfSmashAppearTask* task);

// The model name of a ResMdl, read as mu_object.cpp does (ResMdlData is opaque in the headers).
static inline const char* ResMdl_GetName(nw4r::g3d::ResMdl& mdl) {
    u8* data = (u8*)mdl.ptr();
    u32 off = *(u32*)(data + 0x3c);
    return off != 0 ? (const char*)(data + off) : NULL;
}

// HYPOTHESIS: an inline helper (no copy is emitted) that resolves a public symbol of a relocatable archive file.
static inline void* getArchivePublicAddress(gfArchive* archive, int fileIndex, const char* name) {
    if (archive == NULL) {
        return NULL;
    }
    int size = 0;
    void* data = archive->getData(Data_Type_Misc, fileIndex, &size, 0xFFFE);
    if (data != NULL) {
        utRelocate relocate((u8*)data, size);
        return relocate.getPublicAddress(name);
    }
    return NULL;
}

IfSmashAppearTask::IfSmashAppearTask(const char* name, gfArchive* archive, bool delayPrint, s32 drawPriority)
    : gfTask(name, Category_Info, 0xE, 6, true), m_archive(archive) {
    m_resFileCount = 0;
    m_state = 0;
    m_scnGroup = NULL;
    m_drawPriority = drawPriority;
    m_isInGameGroup = false;
    m_isRemoved = false;
    m_delayPrint = delayPrint;
    m_msgIndex = -1;
    m_face = -1;
    unkF8 = 0;
    unkFC = 0;
    unk100 = 1;
    for (int i = 0; i < 9; i++) {
        m_objects[i].m_object = NULL;
        m_objects[i].m_isInGroup = false;
    }
    unkE4 = NULL;
    m_msg = NULL;
    memset(m_steps, sizeof(m_steps), 0);
    m_stepCount = 0;
    unk408 = -1;
    unk410 = 0.0f;
    unk40C = -1.0f;
    m_sndGroupId = -1;
    m_sndGroup = -1;
    m_seHandle = -1;
    unk420 = 0.0f;
    m_fontType = 4;
}

// MATCH-ONLY: inline, so the subclass destructors expand the base destructor as the original does.
inline IfSmashAppearTask::~IfSmashAppearTask() {
    if (m_scnGroup != NULL) {
        fn_800F5284(this);
        m_scnGroup->Destroy();
        m_isInGameGroup = false;
        if (m_sndGroup >= 0) {
            g_sndSystem->freeGroup(m_sndGroup, 0);
            m_sndGroup = -1;
            m_sndGroupId = -1;
            m_seHandle = -1;
            unk420 = 0.0f;
        }
    }
}

void IfSmashAppearTask::createModel(ModelData* data, s32 count) {
    if (m_archive == NULL || data == NULL || count <= 0) {
        return;
    }
    ModelData* entry = data;
    for (int i = 0; i < count; entry++, i++) {
        s32 index;
        void* file = m_archive->getData(entry->m_nodeType, entry->m_fileIndex, 0xFFFE);
        if (file == NULL) {
            index = -1;
        } else {
            index = m_resFileCount;
            nw4r::g3d::ResFile* resFile = &m_resFiles[index];
            *resFile = file;
            nw4r::g3d::ResFile::Init(resFile);
            m_resFileCount++;
        }
        if (entry->m_object != -1) {
            nw4r::g3d::ResFile* texture = NULL;
            if (entry->m_textureFile >= 0) {
                texture = &m_resFiles[entry->m_textureFile];
            }
            m_objects[entry->m_object].m_object = MuObject::create(&m_resFiles[index], entry->m_nodeName,
                    entry->m_drawPriority + m_drawPriority, texture, Heaps::StageInstance);
            m_objects[entry->m_object].m_object->changeAnimN(entry->m_nodeName);
            m_objects[entry->m_object].m_object->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

void IfSmashAppearTask::setMessage(s32 msgIndex, s32 face, s32 openFrames, s32 closeFrames, s32 waitFrames) {
    if (m_msg == NULL) {
        return;
    }
    if (openFrames == 0) {
        openFrames = 3;
    }
    if (closeFrames == 0) {
        closeFrames = 10;
    }
    if (waitFrames == 0) {
        waitFrames = unk100;
    }
    unkFC = waitFrames;
    if (fn_8004D9E8(g_GameGlobal) == 0) {
        openFrames *= 0.5f;
        closeFrames *= 0.5f;
    }
    m_msgIndex = msgIndex;
    unkF8 = 0;
    if (m_face != face) {
        onChangeFace(face);
        m_face = face;
    }
    if (m_msgIndex >= 0) {
        if (m_delayPrint) {
            m_msg->setDelayPrint(0, openFrames, closeFrames, 0.0f, 0.0f);
        } else {
            int length;
            m_msg->getMsgData(msgIndex, &length);
        }
        m_msg->printIndex(0, msgIndex, NULL);
        endPrint__5MuMsgFv(m_msg, 0);
    } else {
        m_msg->printf(0, "");
        endPrint__5MuMsgFv(m_msg, 0);
    }
}

void IfSmashAppearTask::onChangeFace(s32 face) { }

void IfSmashAppearTask::start(s32 id) {
    if (m_state != 0) {
        stop(1);
    }
    m_state = 1;
    m_stateFrame = 0;
    if (!m_objects[0].m_isInGroup) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_objects[0].m_object->m_sceneModel);
        m_objects[0].m_isInGroup = true;
    }
    m_stepCount = 0;
    unk408 = -1;
    unk410 = 0.0f;
    unk40C = -1.0f;
}

void IfSmashAppearTask::dead() {
    if (m_seHandle >= 0) {
        g_sndSystem->stopSE(m_seHandle, 0);
        m_seHandle = -1;
    }
    if (m_state == 1) {
        stop(0);
    } else if (m_state >= 2 && m_state < 6) {
        onDead();
    }
}

void IfSmashAppearTask::stop(s32 immediate) {
    if (m_state == 0) {
        return;
    }
    setMessage(-1, -1, 0, 0, 0);
    if (m_state == 8 || m_state == 7 || m_state == 1 || immediate) {
        if (m_state != 0) {
            for (int i = 0; i < 9; i++) {
                if (m_objects[i].m_isInGroup) {
                    m_scnGroup->Remove(m_objects[i].m_object->m_sceneModel);
                    m_objects[i].m_isInGroup = false;
                }
            }
            if (m_state == 8) {
                m_state = 0;
                m_stateFrame = 0;
            }
        }
    } else if (m_state != 6) {
        m_state = 6;
        m_stateFrame = 0;
        onClose();
    }
    if (m_seHandle >= 0) {
        g_sndSystem->stopSE(m_seHandle, 0);
        m_seHandle = -1;
    }
    if (m_sndGroup >= 0) {
        g_sndSystem->freeGroup(m_sndGroup, 0);
        m_sndGroup = -1;
        m_sndGroupId = -1;
        m_seHandle = -1;
        unk420 = 0.0f;
    }
}

void IfSmashAppearTask::removeInfo() {
    if (m_scnGroup != NULL) {
        nw4r::g3d::ScnGroup* group = g_IfMngr->m_game2DGroup;
        if (group != NULL) {
            group->Remove(m_scnGroup);
        }
        m_isRemoved = true;
    }
}

float IfSmashAppearTask::getRate() {
    float rate = g_gfApplication->m_frameRate;
    if (rate < 60.0f) {
        rate = 60.0f;
    }
    return 60.0f / rate;
}

void IfSmashAppearTask::onOpen() {
    char name[0x40];
    sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), 1);
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(false);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
}

void IfSmashAppearTask::onClose() {
    char name[0x40];
    sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), 3);
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(false);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
}

void IfSmashAppearTask::onLoop() {
    char name[0x40];
    sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), 2);
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(true);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
}

int IfSmashAppearTask::onBeginMessage() {
    return true;
}

int IfSmashAppearTask::onEndMessage() {
    return true;
}

void IfSmashAppearTask::onDead() {
    stop(0);
}

// HYPOTHESIS: the start counters of the conversation scripts of each task (.sbss of this unit).
s32 lbl_805A02E8;
s32 lbl_805A02EC;
s32 lbl_805A02F0;

IfFoxSmashAppearTask::IfFoxSmashAppearTask(gfArchive* archive)
    : IfSmashAppearTask("IfFoxSmashAppearTask", archive, true, 0x84) {
    FontData::loadFoxFont(m_archive->getData(Data_Type_Misc, 0x77, 0xFFFE));
    m_fontType = 5;
    ModelData models[10] = {
        { Data_Type_Tex, 0x64, -1, -1, 0, NULL },
        { Data_Type_Model, 0x64, 0, 0, 1, "SmashApeal_Starfox" },
        { Data_Type_Model, 0x6E, 0, 1, 0, "SmashApeal_Starfox_Crystal" },
        { Data_Type_Model, 0x6F, 0, 2, 0, "SmashApeal_Starfox_Falco" },
        { Data_Type_Model, 0x70, 0, 3, 0, "SmashApeal_Starfox_Fox" },
        { Data_Type_Model, 0x71, 0, 4, 0, "SmashApeal_Starfox_Leon" },
        { Data_Type_Model, 0x72, 0, 5, 0, "SmashApeal_Starfox_Panther" },
        { Data_Type_Model, 0x73, 0, 6, 0, "SmashApeal_Starfox_Slippy" },
        { Data_Type_Model, 0x74, 0, 7, 0, "SmashApeal_Starfox_Wolf" },
        { Data_Type_Model, 0x75, 0, 8, 0, "SmashApeal_Starfox_Peppy" },
    };
    m_scnGroup = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::StageInstance), NULL, 9);
    unk2C_b1 = false;
    createModel(models, 10);
    nw4r::g3d::ScnGroup* group = g_IfMngr->m_game2DGroup;
    group->Insert(group->sceneItemsCount, m_scnGroup);
    m_isInGameGroup = true;
    float hScale = 1.5f;
    float wScale = 52.0f;
    float offset = 0.0f;
    int file = 0;
    if (fn_8004D9E8(g_GameGlobal) == 0) {
        file = 1;
        wScale *= 0.8f * 0.8f;
        offset += -1.5f;
    }
    void* msgData = m_archive->getData(Data_Type_Misc, file, 0xFFFE);
    MsgData msgs[2] = {
        { 0, "Tolk_Window", -14.0f, 0.0f, 28.0f, 0.0f, 3, 0.0f, 0.0f, 1.1f, 1, 1 },
        { -1 },
    };
    msgs[0].m_y = -3.0f + offset;
    msgs[0].m_h = 8.0f + offset;
    msgs[0].m_hScale = hScale;
    msgs[0].m_wScale = wScale;
    fn_800F5084(this, msgData, msgs);
}

IfFoxSmashAppearTask::~IfFoxSmashAppearTask() {
    if (m_seHandle >= 0) {
        g_sndSystem->stopSE(m_seHandle, 0);
        m_seHandle = -1;
    }
    FontData::removeFoxFont();
}

IfFoxSmashAppearTask* IfFoxSmashAppearTask::create(gfArchive* archive) {
    // HYPOTHESIS: heap 0xF is Heaps::StageInstance by value; the heap's intent is unverified
    IfFoxSmashAppearTask* task = new (Heaps::StageInstance) IfFoxSmashAppearTask(archive);
    return task;
}

void IfFoxSmashAppearTask::start(s32 id) {
    if (id < 0) {
        id = lbl_805A02E8++;
        if (lbl_805A02E8 >= 15) {
            lbl_805A02E8 = 0;
        }
    }
    IfSmashAppearTask::start(id);
    ScriptStep* script = (ScriptStep*)getArchivePublicAddress(m_archive, 0x76, "scripts");
    setScript(script, id);
}

void IfFoxSmashAppearTask::processDefault() {
    fn_800F5344(this);
    if (m_state == 2 && (int)m_objects[0].m_object->m_modelAnim->getFrame() == 0x2C) {
        g_sndSystem->playSE((SndID)0xEA, -1, 0, 0, -1);
    }
}

void IfFoxSmashAppearTask::onClose() {
    char name[0x40];
    sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), 3);
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(false);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
    g_sndSystem->playSE((SndID)0xEA, -1, 0, 0, -1);
}

void IfFoxSmashAppearTask::onChangeFace(s32 face) {
    for (int i = 0; i < 9; i++) {
        if (i != 0 && m_objects[i].m_isInGroup) {
            m_scnGroup->Remove(m_objects[i].m_object->m_sceneModel);
            m_objects[i].m_isInGroup = false;
        }
    }
    if (face < 0) {
        return;
    }
    s32 index = face + 1;
    if (!m_objects[index].m_isInGroup) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_objects[index].m_object->m_sceneModel);
        m_objects[index].m_isInGroup = true;
    }
    char name[0x40];
    sprintf(name, "%s", ResMdl_GetName(m_objects[index].m_object->m_resMdl));
    m_objects[index].m_object->changeAnimN(name);
    m_objects[index].m_object->m_modelAnim->setLoop(true);
    m_objects[index].m_object->m_modelAnim->setUpdateRate(getRate());
}

IfDxFoxSmashAppearTask::IfDxFoxSmashAppearTask(gfArchive* archive)
    : IfSmashAppearTask("IfDxFoxSmashAppearTask", archive, true, 0x84) {
    m_action = -1;
    FontData::loadFoxFont(m_archive->getData(Data_Type_Misc, 0x77, 0xFFFE));
    m_fontType = 5;
    ModelData models[2] = {
        { Data_Type_Tex, 0, -1, -1, 0, NULL },
        { Data_Type_Model, 0xC8, 0, 0, 1, "StgDxCorneriaCall" },
    };
    m_scnGroup = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::StageInstance), NULL, 9);
    unk2C_b1 = false;
    createModel(models, 2);
    nw4r::g3d::ScnGroup* group = g_IfMngr->m_game2DGroup;
    group->Insert(group->sceneItemsCount, m_scnGroup);
    m_isInGameGroup = true;
    float hScale = 2.3f;
    float wScale = 70.0f;
    int file = 0;
    if (fn_8004D9E8(g_GameGlobal) == 0) {
        file = 1;
        hScale = 2.1f * 0.9f;
        wScale *= 0.9f;
    }
    void* msgData = m_archive->getData(Data_Type_Misc, file, 0xFFFE);
    MsgData msgs[2] = {
        { 0, "Messege", -16.0f, -2.5f, 32.5f, 8.5f, 3, 0.0f, 0.0f, 1.0f, 1, 1 },
        { -1 },
    };
    msgs[0].m_hScale = hScale;
    msgs[0].m_wScale = wScale;
    fn_800F5084(this, msgData, msgs);
    m_talker = -1;
}

IfDxFoxSmashAppearTask::~IfDxFoxSmashAppearTask() {
    FontData::removeFoxFont();
}

IfDxFoxSmashAppearTask* IfDxFoxSmashAppearTask::create(gfArchive* archive) {
    // HYPOTHESIS: heap 0xF is Heaps::StageInstance by value, as for the other tasks
    IfDxFoxSmashAppearTask* task = new (Heaps::StageInstance) IfDxFoxSmashAppearTask(archive);
    return task;
}

void IfDxFoxSmashAppearTask::start(s32 id) {
    IfSmashAppearTask::start(id);
    m_action = -1;
    if (id < 0) {
        id = lbl_805A02EC++;
        if (lbl_805A02EC >= 10) {
            lbl_805A02EC = 0;
        }
    }
    ScriptStep* script = (ScriptStep*)getArchivePublicAddress(m_archive, 0x76, "scripts");
    setScript(script, id);
    m_talker = -1;
    for (int i = 0; i < m_stepCount; i++) {
        if (m_talker < 0) {
            m_talker = m_steps[i].m_face;
        }
    }
}

void IfDxFoxSmashAppearTask::onOpen() {
    setMessage(-1, -1, 0, 0, 0);
    changeAction(0, -1, m_talker, false);
}

void IfDxFoxSmashAppearTask::onClose() {
    setMessage(-1, -1, 0, 0, 0);
    if (m_action != 2) {
        changeAction(0, 2, m_talker, false);
    }
}

void IfDxFoxSmashAppearTask::onLoop() { }

int IfDxFoxSmashAppearTask::onBeginMessage() {
    if (m_stateFrame == 0) {
        changeAction(0, 0, m_talker, false);
    } else if (m_stateFrame == 7) {
        g_sndSystem->playSE((SndID)0xE9, -1, 0, 0, -1);
    }
    return m_objects[0].m_object->isNodeAnimFinished();
}

int IfDxFoxSmashAppearTask::onEndMessage() {
    if (m_stateFrame == 0) {
        setMessage(-1, -1, 0, 0, 0);
        changeAction(0, 2, m_talker, false);
    }
    return m_objects[0].m_object->isNodeAnimFinished();
}

void IfDxFoxSmashAppearTask::changeAction(s32 object, s32 action, s32 talker, bool loop) {
    const char* actions[3] = { "open", "talk", "close" };
    if (action < -1 || action >= 3) {
        return;
    }
    const char* talkers[4] = { "beppy", "falco", "fox", "slippy" };
    if (talker < 0 || talker >= sizeof(talkers) / sizeof(talkers[0])) {
        return;
    }
    if (action != -1) {
        char name[0x40];
        sprintf(name, "%s__%s_%s", ResMdl_GetName(m_objects[object].m_object->m_resMdl), actions[action],
                talkers[talker]);
        m_objects[object].m_object->changeAnimN(name);
    } else {
        m_objects[object].m_object->m_modelAnim->unbindNodeAnim(m_objects[object].m_object->m_sceneModel);
    }
    m_objects[object].m_object->m_modelAnim->setLoop(loop);
    m_objects[object].m_object->m_modelAnim->setUpdateRate(getRate());
    m_action = action;
}

void IfDxFoxSmashAppearTask::onChangeFace(s32 face) {
    if (face >= 0) {
        changeAction(0, 1, face, true);
        m_talker = face;
    }
}

IfSnakeSmashAppearTask::IfSnakeSmashAppearTask(gfArchive* archive)
    : IfSmashAppearTask("IfSnakeSmashAppearTask", archive, false, 0x84) {
    FontData::loadResetsanFont(m_archive->getData(Data_Type_Misc, 0x3EE, 0xFFFE));
    m_fontType = 6;
    ModelData models[8] = {
        { Data_Type_Tex, 0, -1, -1, 0, NULL },
        { Data_Type_Model, 0x3E8, 0, 0, 0, "SmashApeal_Metalgear" },
        { Data_Type_Model, 0x3E9, 0, 1, 1, "SmashApeal_Metalgear_Kyanbel" },
        { Data_Type_Model, 0x3EA, 0, 2, 1, "SmashApeal_Metalgear_Meilin" },
        { Data_Type_Model, 0x3EB, 0, 3, 1, "SmashApeal_Metalgear_Otacon" },
        { Data_Type_Model, 0x3EC, 0, 4, 1, "SmashApeal_Metalgear_Slippy" },
        { Data_Type_Model, 0x3ED, 0, 5, 1, "SmashApeal_Metalgear_Snake" },
        { Data_Type_Model, 0x3F2, 0, 6, 1, "SmashApeal_Metalgear_Black" },
    };
    m_scnGroup = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::StageInstance), NULL, 9);
    unk2C_b1 = false;
    createModel(models, 8);
    nw4r::g3d::ScnGroup* group = g_IfMngr->m_game2DGroup;
    group->Insert(group->sceneItemsCount, m_scnGroup);
    m_isInGameGroup = true;
    float hScale = 1.4f;
    float wScale = 48.0f;
    int file = 0x3F0;
    if (fn_8004D9E8(g_GameGlobal) == 0) {
        file = 0x3F1;
        hScale = 1.25f;
    }
    void* msgData = m_archive->getData(Data_Type_Misc, file, 0xFFFE);
    MsgData msgs[2] = {
        { 0, "textN0", -16.0f, -5.0f, 32.0f, 11.0f, 3, 0.0f, 0.0f, 0.85f, 1, 1 },
        { -1 },
    };
    msgs[0].m_hScale = hScale;
    msgs[0].m_wScale = wScale;
    fn_800F5084(this, msgData, msgs);
    m_talker = -1;
}

IfSnakeSmashAppearTask::~IfSnakeSmashAppearTask() {
    FontData::removeResetsanFont();
}

IfSnakeSmashAppearTask* IfSnakeSmashAppearTask::create(gfArchive* archive) {
    IfSnakeSmashAppearTask* task = new (Heaps::StageInstance) IfSnakeSmashAppearTask(archive);
    return task;
}

void IfSnakeSmashAppearTask::start(s32 id) {
    if (id < 0) {
        id = lbl_805A02F0++;
        if (lbl_805A02F0 >= 0x29) {
            lbl_805A02F0 = 0;
        }
    }
    IfSmashAppearTask::start(id);
    m_talker = -1;
    m_isDead = false;
    ScriptStep* script = (ScriptStep*)getArchivePublicAddress(m_archive, 0x3EF, "scripts");
    setScript(script, id);
    for (int i = 0; i < m_stepCount; i++) {
        if (m_talker < 0) {
            s32 face = m_steps[i].m_face;
            if (face != 4 && face != 5) {
                m_talker = face;
            }
        }
    }
    g_sndSystem->playSE((SndID)0x18F5, -1, 0, 0, 0x25);
}

void IfSnakeSmashAppearTask::stop(s32 immediate) {
    IfSmashAppearTask::stop(immediate);
}

void IfSnakeSmashAppearTask::processDefault() {
    fn_800F5344(this);
    if (m_state == 2) {
        int frame = m_objects[0].m_object->m_modelAnim->getFrame();
        if (frame == 0x66) {
            g_sndSystem->playSE((SndID)0x18F6, -1, 0, 0, 0x25);
        } else if (frame == 0x91) {
            g_sndSystem->playSE((SndID)0x18F7, -1, 0, 0, 0x25);
        }
    } else if (m_state == 6) {
        if ((int)m_objects[0].m_object->m_modelAnim->getFrame() == 0x28) {
            g_sndSystem->playSE((SndID)0x18F8, -1, 0, 0, 0x25);
        }
    }
}

void IfSnakeSmashAppearTask::onOpen() {
    s32 base;
    switch (m_talker) {
        case 0:
        case 3:
            base = 0;
            break;
        case 1:
            base = 3;
            break;
        case 2:
            base = 6;
            break;
        default:
            base = 0;
            break;
    }
    char name[0x40];
    if (base + 1 == 0) {
        sprintf(name, "%s", ResMdl_GetName(m_objects[0].m_object->m_resMdl));
    } else {
        sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), base + 1);
    }
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(false);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
}

void IfSnakeSmashAppearTask::onClose() {
    s32 base;
    switch (m_talker) {
        case 0:
        case 3:
            base = 0;
            break;
        case 1:
            base = 3;
            break;
        case 2:
            base = 6;
            break;
        default:
            base = 0;
            break;
    }
    char name[0x40];
    if (base + 3 == 0) {
        sprintf(name, "%s", ResMdl_GetName(m_objects[0].m_object->m_resMdl));
    } else {
        sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), base + 3);
    }
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(false);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
}

void IfSnakeSmashAppearTask::onLoop() {
    s32 base;
    switch (m_talker) {
        case 0:
        case 3:
            base = 0;
            break;
        case 1:
            base = 3;
            break;
        case 2:
            base = 6;
            break;
        default:
            base = 0;
            break;
    }
    char name[0x40];
    if (base + 2 == 0) {
        sprintf(name, "%s", ResMdl_GetName(m_objects[0].m_object->m_resMdl));
    } else {
        sprintf(name, "%s_%02d", ResMdl_GetName(m_objects[0].m_object->m_resMdl), base + 2);
    }
    m_objects[0].m_object->changeAnimN(name);
    m_objects[0].m_object->m_modelAnim->setLoop(true);
    m_objects[0].m_object->m_modelAnim->setUpdateRate(getRate());
}

void IfSnakeSmashAppearTask::onDead() {
    m_isDead = true;
    m_face = -1;
    ScriptStep* script = (ScriptStep*)getArchivePublicAddress(m_archive, 0x3EF, "scripts");
    s32 id = -1;
    switch (m_talker) {
        case 0:
            id = 0xD;
            break;
        case 1:
            id = 0x26;
            break;
        case 2:
            id = 0x1A;
            break;
        case 3:
            id = 0x28;
            break;
    }
    if (id >= 0) {
        setScript(script, id);
        m_state = 3;
        m_stateFrame = 0;
        onLoop();
    }
}

bool IfSmashAppearTask::setScript(ScriptStep* script, s32 id) {
    if (script == NULL) {
        return false;
    }
    m_stepCount = 0;
    unk408 = -1;
    unk410 = 0.0f;
    unk40C = -1.0f;
    u32 soundGroup = -1;
    int conversation = 0;
    for (;; script++) {
        if (script->m_msgIndex == -2) {
            break;
        }
        if (script->m_msgIndex == -1) {
            if (id == conversation) {
                if (soundGroup != 0xFFFFFFFF && m_sndGroupId == 0xFFFFFFFF) {
                    m_sndGroupId = soundGroup;
                    unk420 = 0.0f;
                    if (m_sndGroup < 0) {
                        bool loading = !g_sndSystem->isFinishLoad1(soundGroup);
                        if (!loading && !g_sndSystem->isSoundGroupLoaded((SndGroupID)m_sndGroupId)) {
                            m_sndGroup = g_sndSystem->loadSoundGroup((SndGroupID)m_sndGroupId, 1, true);
                            if (m_sndGroup >= 0) {
                                unk420 = 60.0f;
                            }
                        }
                    }
                }
                return true;
            }
            conversation++;
        } else if (conversation == id) {
            if (script->m_msgIndex == -3) {
                soundGroup = script->m_se;
            } else if (script->m_msgIndex == -4) {
                unk40C = 60.0f + script->m_face;
            } else {
                if (m_stepCount >= sizeof(m_steps) / sizeof(m_steps[0])) {
                    return false;
                }
                m_steps[m_stepCount] = *script;
                m_stepCount++;
            }
        }
    }
    return false;
}

// TODO: clean up, give variables real names
namespace jj_800F7880 {

struct Inner {
    char pad[0x3C];
    int off;
};

struct ObjectHandler {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void bind(void*, void*);
    virtual void slot12();
    virtual void unbind(void*);
    char pad[0xE0];
    void* fieldE4;
};

struct TaskBase {
    char pad[0x3C];
};

struct Entry {
    MuObject* mu;
    bool active;
};

class gfTask : public TaskBase {
public:
    virtual void fn_800F7454();
    virtual void processBegin();
    virtual void processAnim();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
    virtual void fn_800F7168();
    virtual void fn_800F7280();
    virtual void fn_800F5A58();
    virtual void fn_800F7450();
    virtual void fn_800F5C60();
    virtual void fn_800F7CAC();
    virtual float fn_800F5CBC();

    char pad2[0x90 - 0x40];
    ObjectHandler* handler;
    char pad3[0x9C - 0x94];
    Entry entries[9];
    char pad4[0x428 - 0xE4];
    bool flag428;
    char pad5[3];
    int state;
};

extern "C" {
extern char lbl_8059E3E0[3];
extern char lbl_8059E3E8[8];
void setLoop__16gfModelAnimationFd(gfModelAnimation*, bool);
}

inline void unbindEntry(gfTask* self, Entry* e) {
    if (e->active) {
        self->handler->unbind(e->mu->m_sceneModel);
        e->active = false;
    }
}

inline void bindEntry(gfTask* self, Entry* e) {
    if (!e->active) {
        self->handler->bind(self->handler->fieldE4, e->mu->m_sceneModel);
        e->active = true;
    }
}

// TODO: clean up, give variables real names
inline void playAnim(gfTask* self, Entry* e, int sel) {
    char buf[0x40];
    int suffix = sel + 1;
    if (suffix == 0) {
        Inner* inner = *(Inner**)&e->mu->m_resMdl;
        char* name;
        if (inner->off != 0)
            name = (char*)inner + inner->off;
        else
            name = 0;
        sprintf(buf, lbl_8059E3E0, name);
    } else {
        Inner* inner = *(Inner**)&e->mu->m_resMdl;
        char* name;
        if (inner->off != 0)
            name = (char*)inner + inner->off;
        else
            name = 0;
        sprintf(buf, lbl_8059E3E8, name, suffix);
    }
    e->mu->changeAnimN(buf);
    setLoop__16gfModelAnimationFd(e->mu->m_modelAnim, true);
    float rate = self->fn_800F5CBC();
    e->mu->m_modelAnim->setUpdateRate(rate);
}

// TODO: clean up, give variables real names
extern "C" void fn_800F7880(gfTask* self, int index) {
    for (int i = 0; i < 9; i++) {
        if (i != 0)
            unbindEntry(self, &self->entries[i]);
    }

    if (index >= 0) {
        if (index != 4 && index != 5)
            self->state = index;

        if (self->flag428 || index == 5) {
            bindEntry(self, &self->entries[6]);
        } else {
            bindEntry(self, &self->entries[5]);
            playAnim(self, &self->entries[5], (int)((u32)((4 - index) | (index - 4)) >> 31));
        }

        if (self->state >= 0) {
            int n = self->state + 1;
            bindEntry(self, &self->entries[n]);
            playAnim(self, &self->entries[n], (int)((u32)((index - self->state) | (self->state - index)) >> 31));
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800F5084 {
typedef unsigned char u8;
typedef unsigned int u32;

struct Entry {
    int id;
    int unk4;
    float x, y, w, h;
    u8 color; char pad[3];
    float hs, ws1, ws2;
    int align;
    int unk2c;
};

struct MsgObj { char pad[8]; void* unk8; };

extern "C" {
MsgObj* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, u32, u32);
void allocMsgBuf__5MuMsgFUlUl(MsgObj*, u32, u32);
void setMsgData__5MuMsgFPv(MsgObj*, void*);
void attachMuObject__5MuMsgFUlP8MuObjectPCcfffff(MsgObj*, int, void*, int, float, float, float, float, float);
void setBufField34__7MessageFii(void*, int, int);
void setAlignMode__5MuMsgFUlQ25MuMsg9AlignMode(MsgObj*, int, int);
void setVerticalMode__5MuMsgFUlb(MsgObj*, int, int);
void setHScale__5MuMsgFUlf(MsgObj*, int, float);
void getHScale__5MuMsgFUlf(MsgObj*, int, float);
void setFontColor__5MuMsgFUlUcUcUcUc(MsgObj*, int, u8, u8, u8, u8);
void FUN_800b9448__5MuMsgFUlb(MsgObj*, int, int);
void FUN_800b9460__5MuMsgFUlUcUcUcf(MsgObj*, int, u8, u8, u8, float);
void setFontType__5MuMsgFUlUl(MsgObj*, int, int);
void getWScale__5MuMsgFUlf(MsgObj*, int, float);
void setFontWidthModeProportional__5MuMsgFUl(MsgObj*, int);
void setWScale__5MuMsgFUlf(MsgObj*, int, float);
extern float lbl_805A2618;
extern float lbl_805A261C;
}

struct Self {
    char pad[0x9c];
    struct { void* p; int q; } items[(0xe8 - 0x9c) / 8];
    char pad2[0xe8 - 0x9c - ((0xe8 - 0x9c) / 8) * 8];
    MsgObj* msg;
    char pad3[0x424 - 0xec];
    int unk424;
};

extern "C" void fn_800F5084(Self* self, void* data, Entry* e) {
    self->msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(8, 0xf, 0xf);
    allocMsgBuf__5MuMsgFUlUl(self->msg, 0x200, 1);
    setMsgData__5MuMsgFPv(self->msg, data);
    for (int i = 0; ; e++, i++) {
        if (e->id == -1) break;
        attachMuObject__5MuMsgFUlP8MuObjectPCcfffff(self->msg, i, self->items[e->id].p, e->unk4, e->x, e->y,
                    e->x + e->w, e->y + e->h, lbl_805A2618);
        setBufField34__7MessageFii(self->msg->unk8, i, 7);
        setAlignMode__5MuMsgFUlQ25MuMsg9AlignMode(self->msg, i, e->align);
        setVerticalMode__5MuMsgFUlb(self->msg, i, e->unk2c);
        setHScale__5MuMsgFUlf(self->msg, i, e->hs);
        getHScale__5MuMsgFUlf(self->msg, i, e->hs);
        switch (e->color) {
        case 1:
            setFontColor__5MuMsgFUlUcUcUcUc(self->msg, i, 0xff, 0xff, 0xff, 0xff);
            FUN_800b9448__5MuMsgFUlb(self->msg, i, 1);
            FUN_800b9460__5MuMsgFUlUcUcUcf(self->msg, i, 0, 0, 0, lbl_805A261C);
            break;
        case 2:
            setFontColor__5MuMsgFUlUcUcUcUc(self->msg, i, 0x80, 0x80, 0x80, 0xff);
            break;
        case 3:
            setFontColor__5MuMsgFUlUcUcUcUc(self->msg, i, 0xff, 0xff, 0xff, 0xff);
            break;
        }
        setFontType__5MuMsgFUlUl(self->msg, i, self->unk424);
        getWScale__5MuMsgFUlf(self->msg, i, e->ws1);
        setFontWidthModeProportional__5MuMsgFUl(self->msg, i);
        setWScale__5MuMsgFUlf(self->msg, i, e->ws2);
    }
}
}

