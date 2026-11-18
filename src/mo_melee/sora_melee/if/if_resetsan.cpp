#include <if/if_resetsan.h>
#include <gf/gf_model.h>
#include <ms/ms_message.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <ms/ms_resfont.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <if/if_mngr.h>
#include <gm/gm_global.h>
#include <it/item.h>
#include <snd/snd_doubutsugo_task.h>
#include <mt/mt_vector.h>
#include <types.h>

extern "C" void fn_8007B0A4(sndDoubutsugoTask* task, const void* script);

extern "C" int fn_8004D9E8(GameGlobal* global);
// itArchive::getPublicData / getResFileFromId / getData (unnamed)
extern "C" void* fn_27_2B2678(itArchive* archive, const char* name);
extern "C" void* fn_27_2B26E8(itArchive* archive, int fileKind, int index, int fileId);
extern "C" void* fn_27_2B2764(itArchive* archive, int fileKind, int type, int index, int fileId);
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, u32 capacity);

#include <mt/mt_prng.h>

extern mtRand lbl_805A0420;

namespace {

// The UTF-8 code (kana) of a character and the voice clip that says it.
struct VoiceLabel {
    u8 m_utf8[3];
    s16 m_label;
};

VoiceLabel s_voiceLabels[231] = {
    { 0xE3, 0x81, 0x82, 0 }, { 0xE3, 0x81, 0x84, 1 }, { 0xE3, 0x81, 0x86, 2 }, { 0xE3, 0x81, 0x88, 3 },
    { 0xE3, 0x81, 0x8A, 4 }, { 0xE3, 0x81, 0x8B, 5 }, { 0xE3, 0x81, 0x8D, 6 }, { 0xE3, 0x81, 0x8F, 7 },
    { 0xE3, 0x81, 0x91, 8 }, { 0xE3, 0x81, 0x93, 9 }, { 0xE3, 0x81, 0x95, 10 }, { 0xE3, 0x81, 0x97, 11 },
    { 0xE3, 0x81, 0x99, 12 }, { 0xE3, 0x81, 0x9B, 13 }, { 0xE3, 0x81, 0x9D, 14 }, { 0xE3, 0x81, 0x9F, 15 },
    { 0xE3, 0x81, 0xA1, 16 }, { 0xE3, 0x81, 0xA4, 17 }, { 0xE3, 0x81, 0xA6, 18 }, { 0xE3, 0x81, 0xA8, 19 },
    { 0xE3, 0x81, 0xAA, 20 }, { 0xE3, 0x81, 0xAB, 21 }, { 0xE3, 0x81, 0xAC, 22 }, { 0xE3, 0x81, 0xAD, 23 },
    { 0xE3, 0x81, 0xAE, 24 }, { 0xE3, 0x81, 0xAF, 25 }, { 0xE3, 0x81, 0xB2, 26 }, { 0xE3, 0x81, 0xB5, 27 },
    { 0xE3, 0x81, 0xB8, 28 }, { 0xE3, 0x81, 0xBB, 29 }, { 0xE3, 0x81, 0xBE, 30 }, { 0xE3, 0x81, 0xBF, 31 },
    { 0xE3, 0x82, 0x80, 32 }, { 0xE3, 0x82, 0x81, 33 }, { 0xE3, 0x82, 0x82, 34 }, { 0xE3, 0x82, 0x84, 35 },
    { 0xE3, 0x82, 0x86, 36 }, { 0xE3, 0x82, 0x88, 37 }, { 0xE3, 0x82, 0x89, 38 }, { 0xE3, 0x82, 0x8A, 39 },
    { 0xE3, 0x82, 0x8B, 40 }, { 0xE3, 0x82, 0x8C, 41 }, { 0xE3, 0x82, 0x8D, 42 }, { 0xE3, 0x82, 0x8F, 43 },
    { 0xE3, 0x82, 0x92, 4 }, { 0xE3, 0x82, 0x93, 44 }, { 0xE3, 0x81, 0x8C, 45 }, { 0xE3, 0x81, 0x8E, 46 },
    { 0xE3, 0x81, 0x90, 47 }, { 0xE3, 0x81, 0x92, 48 }, { 0xE3, 0x81, 0x94, 49 }, { 0xE3, 0x81, 0x96, 50 },
    { 0xE3, 0x81, 0x98, 52 }, { 0xE3, 0x81, 0x9A, 52 }, { 0xE3, 0x81, 0x9C, 53 }, { 0xE3, 0x81, 0x9E, 54 },
    { 0xE3, 0x81, 0xA0, 55 }, { 0xE3, 0x81, 0xA2, 52 }, { 0xE3, 0x81, 0xA5, 52 }, { 0xE3, 0x81, 0xA7, 56 },
    { 0xE3, 0x81, 0xA9, 57 }, { 0xE3, 0x81, 0xB0, 58 }, { 0xE3, 0x81, 0xB3, 59 }, { 0xE3, 0x81, 0xB6, 60 },
    { 0xE3, 0x81, 0xB9, 61 }, { 0xE3, 0x81, 0xBC, 62 }, { 0xE3, 0x81, 0xB1, 63 }, { 0xE3, 0x81, 0xB4, 64 },
    { 0xE3, 0x81, 0xB7, 65 }, { 0xE3, 0x81, 0xBA, 66 }, { 0xE3, 0x81, 0xBD, 67 }, { 0xE3, 0x81, 0x81, 0 },
    { 0xE3, 0x81, 0x83, 1 }, { 0xE3, 0x81, 0x85, 2 }, { 0xE3, 0x81, 0x87, 3 }, { 0xE3, 0x81, 0x89, 4 },
    { 0xE3, 0x81, 0xA3, 17 }, { 0xE3, 0x82, 0x83, 35 }, { 0xE3, 0x82, 0x85, 36 }, { 0xE3, 0x82, 0x87, 37 },
    { 0xE3, 0x82, 0xA2, 0 }, { 0xE3, 0x82, 0xA4, 1 }, { 0xE3, 0x82, 0xA6, 2 }, { 0xE3, 0x82, 0xA8, 3 },
    { 0xE3, 0x82, 0xAA, 4 }, { 0xE3, 0x82, 0xAB, 5 }, { 0xE3, 0x82, 0xAD, 6 }, { 0xE3, 0x82, 0xAF, 7 },
    { 0xE3, 0x82, 0xB1, 8 }, { 0xE3, 0x82, 0xB3, 9 }, { 0xE3, 0x82, 0xB5, 10 }, { 0xE3, 0x82, 0xB7, 11 },
    { 0xE3, 0x82, 0xB9, 12 }, { 0xE3, 0x82, 0xBB, 13 }, { 0xE3, 0x82, 0xBD, 14 }, { 0xE3, 0x82, 0xBF, 15 },
    { 0xE3, 0x83, 0x81, 16 }, { 0xE3, 0x83, 0x84, 17 }, { 0xE3, 0x83, 0x86, 18 }, { 0xE3, 0x83, 0x88, 19 },
    { 0xE3, 0x83, 0x8A, 20 }, { 0xE3, 0x83, 0x8B, 21 }, { 0xE3, 0x83, 0x8C, 22 }, { 0xE3, 0x83, 0x8D, 23 },
    { 0xE3, 0x83, 0x8E, 24 }, { 0xE3, 0x83, 0x8F, 25 }, { 0xE3, 0x83, 0x92, 26 }, { 0xE3, 0x83, 0x95, 27 },
    { 0xE3, 0x83, 0x98, 28 }, { 0xE3, 0x83, 0x9B, 29 }, { 0xE3, 0x83, 0x9E, 30 }, { 0xE3, 0x83, 0x9F, 31 },
    { 0xE3, 0x83, 0xA0, 32 }, { 0xE3, 0x83, 0xA1, 33 }, { 0xE3, 0x83, 0xA2, 34 }, { 0xE3, 0x83, 0xA4, 35 },
    { 0xE3, 0x83, 0xA6, 36 }, { 0xE3, 0x83, 0xA8, 37 }, { 0xE3, 0x83, 0xA9, 38 }, { 0xE3, 0x83, 0xAA, 39 },
    { 0xE3, 0x83, 0xAB, 40 }, { 0xE3, 0x83, 0xAC, 41 }, { 0xE3, 0x83, 0xAD, 42 }, { 0xE3, 0x83, 0xAF, 43 },
    { 0xE3, 0x83, 0xB2, 4 }, { 0xE3, 0x83, 0xB3, 44 }, { 0xE3, 0x82, 0xAC, 45 }, { 0xE3, 0x82, 0xAE, 46 },
    { 0xE3, 0x82, 0xB0, 47 }, { 0xE3, 0x82, 0xB2, 48 }, { 0xE3, 0x82, 0xB4, 49 }, { 0xE3, 0x82, 0xB6, 50 },
    { 0xE3, 0x82, 0xB8, 52 }, { 0xE3, 0x82, 0xBA, 52 }, { 0xE3, 0x82, 0xBC, 53 }, { 0xE3, 0x82, 0xBE, 54 },
    { 0xE3, 0x83, 0x80, 55 }, { 0xE3, 0x83, 0x82, 52 }, { 0xE3, 0x83, 0x85, 52 }, { 0xE3, 0x83, 0x87, 56 },
    { 0xE3, 0x83, 0x89, 57 }, { 0xE3, 0x83, 0x90, 58 }, { 0xE3, 0x83, 0x93, 59 }, { 0xE3, 0x83, 0x96, 60 },
    { 0xE3, 0x83, 0x99, 61 }, { 0xE3, 0x83, 0x9C, 62 }, { 0xE3, 0x83, 0x91, 63 }, { 0xE3, 0x83, 0x94, 64 },
    { 0xE3, 0x83, 0x97, 65 }, { 0xE3, 0x83, 0x9A, 66 }, { 0xE3, 0x83, 0x9D, 67 }, { 0xE3, 0x82, 0xA1, 0 },
    { 0xE3, 0x82, 0xA3, 1 }, { 0xE3, 0x82, 0xA5, 2 }, { 0xE3, 0x82, 0xA7, 3 }, { 0xE3, 0x82, 0xA9, 4 },
    { 0xE3, 0x83, 0x83, 17 }, { 0xE3, 0x83, 0xA3, 35 }, { 0xE3, 0x83, 0xA5, 36 }, { 0xE3, 0x83, 0xA7, 37 },
    { 0xE3, 0x83, 0xB4, 60 }, { 0xEF, 0xBC, 0x90, 68 }, { 0xEF, 0xBC, 0x91, 69 }, { 0xEF, 0xBC, 0x92, 70 },
    { 0xEF, 0xBC, 0x93, 71 }, { 0xEF, 0xBC, 0x94, 72 }, { 0xEF, 0xBC, 0x95, 73 }, { 0xEF, 0xBC, 0x96, 74 },
    { 0xEF, 0xBC, 0x97, 75 }, { 0xEF, 0xBC, 0x98, 76 }, { 0xEF, 0xBC, 0x99, 77 }, { 0xEF, 0xBC, 0xA1, 78 },
    { 0xEF, 0xBC, 0xA2, 79 }, { 0xEF, 0xBC, 0xA3, 80 }, { 0xEF, 0xBC, 0xA4, 81 }, { 0xEF, 0xBC, 0xA5, 82 },
    { 0xEF, 0xBC, 0xA6, 83 }, { 0xEF, 0xBC, 0xA7, 84 }, { 0xEF, 0xBC, 0xA8, 85 }, { 0xEF, 0xBC, 0xA9, 86 },
    { 0xEF, 0xBC, 0xAA, 87 }, { 0xEF, 0xBC, 0xAB, 88 }, { 0xEF, 0xBC, 0xAC, 89 }, { 0xEF, 0xBC, 0xAD, 90 },
    { 0xEF, 0xBC, 0xAE, 91 }, { 0xEF, 0xBC, 0xAF, 92 }, { 0xEF, 0xBC, 0xB0, 93 }, { 0xEF, 0xBC, 0xB1, 94 },
    { 0xEF, 0xBC, 0xB2, 95 }, { 0xEF, 0xBC, 0xB3, 96 }, { 0xEF, 0xBC, 0xB4, 97 }, { 0xEF, 0xBC, 0xB5, 98 },
    { 0xEF, 0xBC, 0xB6, 99 }, { 0xEF, 0xBC, 0xB7, 100 }, { 0xEF, 0xBC, 0xB8, 101 }, { 0xEF, 0xBC, 0xB9, 102 },
    { 0xEF, 0xBC, 0xBA, 103 }, { 0xEF, 0xBD, 0x81, 78 }, { 0xEF, 0xBD, 0x82, 79 }, { 0xEF, 0xBD, 0x83, 80 },
    { 0xEF, 0xBD, 0x84, 81 }, { 0xEF, 0xBD, 0x85, 82 }, { 0xEF, 0xBD, 0x86, 83 }, { 0xEF, 0xBD, 0x87, 84 },
    { 0xEF, 0xBD, 0x88, 85 }, { 0xEF, 0xBD, 0x89, 86 }, { 0xEF, 0xBD, 0x8A, 87 }, { 0xEF, 0xBD, 0x8B, 88 },
    { 0xEF, 0xBD, 0x8C, 89 }, { 0xEF, 0xBD, 0x8D, 90 }, { 0xEF, 0xBD, 0x8E, 91 }, { 0xEF, 0xBD, 0x8F, 92 },
    { 0xEF, 0xBD, 0x90, 93 }, { 0xEF, 0xBD, 0x91, 94 }, { 0xEF, 0xBD, 0x92, 95 }, { 0xEF, 0xBD, 0x93, 96 },
    { 0xEF, 0xBD, 0x94, 97 }, { 0xEF, 0xBD, 0x95, 98 }, { 0xEF, 0xBD, 0x96, 99 }, { 0xEF, 0xBD, 0x97, 100 },
    { 0xEF, 0xBD, 0x98, 101 }, { 0xEF, 0xBD, 0x99, 102 }, { 0xEF, 0xBD, 0x9A, 103 }, { 0xE3, 0x83, 0xBB, 201 },
    { 0xE3, 0x83, 0xBC, 190 }, { 0xEF, 0xBC, 0x8D, 190 }, { 0xE3, 0x80, 0x80, 203 }, { 0xE3, 0x80, 0x81, 204 },
    { 0xE3, 0x80, 0x82, 205 }, { 0xEF, 0xBC, 0x9F, 206 }, { 0xEF, 0xBC, 0x81, 207 },
};

} // namespace

static s16 utf8charToVoiceLabel(const char* c) {
    if (c == NULL || *c == 0) {
        return 0xD3;
    }
    if (!(*c & 0x80)) {
        if (fn_8004D9E8(g_GameGlobal)) {
            if (*c >= '0' && *c <= '9') {
                return *c + 0x14;
            }
            if (*c >= 'A' && *c <= 'Z') {
                return *c + 0xD;
            }
            if (*c >= 'a' && *c <= 'z') {
                return *c - 0x13;
            }
        } else {
            if (*c >= '0' && *c <= '9') {
                return *c + 0x38;
            }
            if (*c >= 'A' && *c <= 'Z') {
                return *c + 0x31;
            }
            if (*c >= 'a' && *c <= 'z') {
                return *c + 0x11;
            }
        }
        if (*c == '.') {
            return 0xCC;
        }
        if (*c == ',') {
            return 0xCC;
        }
        if (*c == ' ') {
            return 0xCB;
        }
        if (*c == '?') {
            return 0xCE;
        }
        if (*c == '!') {
            return 0xCF;
        }
    } else if ((*c & 0xE0) != 0xC0 && (*c & 0xF0) == 0xE0) {
        const VoiceLabel* label = s_voiceLabels;
        for (u32 i = 0; i < 231; i++, label++) {
            if (label->m_utf8[0] == (u8)c[0] && label->m_utf8[1] == (u8)c[1] && label->m_utf8[2] == (u8)c[2]) {
                if (fn_8004D9E8(g_GameGlobal)) {
                    return label->m_label;
                }
                if (label->m_label >= 0x4E && label->m_label <= 0x67) {
                    return label->m_label + 0x24;
                }
                if (label->m_label >= 0x44 && label->m_label <= 0x4D) {
                    return label->m_label + 0x24;
                }
            }
        }
    }
    return s_voiceLabels[(int)(231.0f * lbl_805A0420.randf())].m_label;
}

namespace {

const IfResetsanTask::ModelDesc s_modelDesc[] = {
    { "InfAssist405_TopN", 0, 0, 0 },
};

} // namespace

IfResetsanTask* IfResetsanTask::create(BaseItem* item, itArchive* archive, int priority) {
    void* resData = fn_27_2B26E8(archive, 1, 0, 0x14);
    void* msgData = fn_27_2B2764(archive, 0, 1, 1, 0x14);
    void* voiceData = fn_27_2B2764(archive, 1, 1, 0, 0x1E);
    int fontIndex = 0;
    if (!fn_8004D9E8(g_GameGlobal)) {
        fontIndex = 1;
    }
    void* fontData = fn_27_2B2764(archive, 1, 1, fontIndex, 0x14);
    void* pageFrame = fn_27_2B2678(archive, "PageFrame");
    IfResetsanTask* task = new (Heaps::ItemInstance) IfResetsanTask(item, reinterpret_cast<gfArchive*>(archive), resData, pageFrame);
    nw4r::g3d::ResFile::Init(&task->m_resFile);
    nw4r::g3d::ScnGroup* group = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::FighterTechqniq), NULL, 1);
    FontData::loadEndingFont(fontData);
    task->unk2C_b1 = false;
    task->m_group = NULL;
    for (int i = 0; i < 1; i++) {
        task->m_objects[i] = NULL;
    }
    for (int i = 0; i < 1; i++) {
        task->m_sceneObjects[i] = NULL;
    }
    task->createModel(&task->m_resFile, priority);
    task->m_group = group;
    group->Insert(group->sceneItemsCount, task->m_objects[0]->getSceneModel());
    task->initMsg(msgData);
    g_IfMngr->addGame2DObj(group);
    sndDoubutsugoTask* voiceTask = new (Heaps::FighterTechqniq) sndDoubutsugoTask(voiceData, Heaps::FighterTechqniq);
    voiceTask->init();
    task->m_voiceTask = voiceTask;
    return task;
}

IfResetsanTask::IfResetsanTask(BaseItem* item, gfArchive* archive, void* resData, void* pageFrame) :
    gfTask("IfResetsan", Category_Info, 0xE, 6, true),
    m_item(item), m_archive(archive), m_resFile(resData), m_state(0), m_textLen(0), m_nameKind1(-1), m_nameKind2(-1), m_nameStr1(NULL),
    m_nameStr2(NULL), m_nameWStr1(NULL), m_nameWStr2(NULL), m_voiceTask(NULL), m_pageFrame(pageFrame), unk194(0) {
    unk60 = -1;
    unk64 = 0;
    unk68 = 0.0f;
    unk6C = 0.0f;
}

IfResetsanTask::~IfResetsanTask() {
    if (m_voiceTask != NULL) {
        m_voiceTask->exit();
        m_voiceTask = NULL;
    }
    FontData::removeEndingFont();
    destroyModel();
    if (m_group != NULL) {
        m_group->Destroy();
    }
}

void IfResetsanTask::createModel(nw4r::g3d::ResFile* resFile, int priority) {
    const ModelDesc* desc = s_modelDesc;
    for (int j = 0; j < 1; j++, desc++) {
        int num = 1;
        if (desc->m_start < desc->m_end) {
            num = desc->m_end - desc->m_start;
        }
        for (int i = 0; i < num; i++) {
            m_objects[desc->m_start + i] = MuObject::create(resFile, desc->m_name, desc->m_priority + priority, NULL, Heaps::FighterTechqniq);
            m_objects[desc->m_start + i]->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

namespace {

const IfResetsanTask::MsgDesc s_msgDesc[] = {
    { 0, 0, 3, 0, 0 },
    { 0, 1, 3, 1, 1 },
};

} // namespace

void IfResetsanTask::initMsg(void* msgData) {
    m_msg = MuMsg::create(0, Heaps::ItemInstance, Heaps::FighterTechqniq);
    m_msg->allocMsgBuf(0x200, 2);
    m_msg->setMsgData(msgData);
    const MsgDesc* desc = s_msgDesc;
    for (int i = 0; i < 2; i++, desc++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objects[desc->m_objIdx]->m_sceneModel), desc->m_boneIdx, 0.0625f);
        m_msg->getMessage()->setBufField34(i, 7);
        m_msg->setAlignMode(i, static_cast<MuMsg::AlignMode>(desc->m_align));
        m_msg->setVerticalMode(i, desc->m_vertical);
        m_msg->setHScale(i, 1.3333334f);
        m_msg->getHScale(i, 1.3333334f);
        switch (desc->m_colorType) {
        case 1:
            m_msg->setFontColor(i, 0xFF, 0xFF, 0xFF, 0xFF);
            m_msg->setEdgeEnable(i, true);
            m_msg->setEdge(i, 0, 0, 0, 3.0f);
            break;
        case 2:
            m_msg->setFontColor(i, 0x80, 0x80, 0x80, 0xFF);
            break;
        case 3:
            m_msg->setFontColor(i, 0, 0, 0, 0xFF);
            break;
        }
        m_msg->setFontType(i, 3);
        m_msg->setHSpace(i, 4.0f);
        if (!fn_8004D9E8(g_GameGlobal)) {
            m_msg->setFontWidthModeProportional(i);
        }
    }
}

#pragma dont_inline on
void IfResetsanTask::destroyModel() {
    MuMsg* msg = m_msg;
    if (m_sceneObjects[0] != NULL) {
        m_sceneObjects[0]->Destroy();
        m_sceneObjects[0] = NULL;
    }
    msg->detachMuObject(m_objects[0]);
    delete msg;
    if (m_objects[0] != NULL) {
        delete m_objects[0];
        m_objects[0] = NULL;
    }
}
#pragma dont_inline reset

void IfResetsanTask::processDefault() {
    if (g_gfApplication->m_pauseState != 0 || g_gfApplication->unkEE_80) {
        return;
    }
    if (m_msg == NULL) {
        return;
    }
    if (m_state == 3) {
        if (m_objects[0]->isNodeAnimFinished()) {
            m_msg->printf(0, "");
            m_msg->endPrint();
            m_msg->printf(1, "");
            m_msg->endPrint();
            m_state = 4;
        }
    } else if (m_state == 1 || m_state == 2) {
        if (unk60 >= 0 && m_textLen != 0 && m_voiceTask != NULL) {
            char* tag;
            if (m_msg->getTag(1, &tag)) {
                s16 label = utf8charToVoiceLabel(tag);
                if (label != 0xD3) {
                    static_cast<sndDoubutsugoTask*>(m_voiceTask)->playWord(label);
                }
            }
        }
        if (unk60 >= 0 && !unk64) {
            unk194++;
            m_msg->isEndDelayPrint(1);
            u32 len = 0;
            if (unk60 >= 0) {
                len = static_cast<u32*>(m_pageFrame)[unk60];
            }
            bool done = unk194 >= len - unk6C;
            if (done) {
                unk68 -= 1.0f;
                if (unk68 < 0.0f) {
                    unk68 = 0.0f;
                    unk6C = 0.0f;
                    unk64 = true;
                    m_textLen = 0;
                    if (m_item != NULL) {
                        m_item->notifyEventInfoWindow(unk60);
                    }
                }
            }
        }
    }
}

void IfResetsanTask::setMessage(int msgId, float delay1, float delay3, float delay4, float delay2, float wait, float posY) {
    if (delay1 == 0.0f) {
        delay1 = 2.0f;
    }
    if (delay2 == 0.0f) {
        delay2 = 6.0f;
    }
    unk68 = wait;
    unk6C = wait;
    if (unk60 < 0) {
        m_state = 1;
        m_objects[0]->changeAnimN(0, 3, false, false);
        m_objects[0]->m_modelAnim->setUpdateRate(1.0f);
        m_msg->printIndex(0, 0, NULL);
        m_msg->endPrint();
    }
    unk60 = msgId;
    unk64 = false;
    unk194 = 0;
    if (!fn_8004D9E8(g_GameGlobal)) {
        delay1 *= 0.5f;
        delay3 *= 0.5f;
        delay2 *= 0.5f;
    }
    float frameRate = g_gfApplication->m_frameRate;
    if (!(frameRate <= 60.0f)) {
        float scale = 60.0f / frameRate;
        delay1 *= scale;
        delay3 *= scale;
        delay2 *= scale;
    }
    if (unk60 < 0) {
        m_state = 3;
        m_objects[0]->changeAnimN(1, 3, false, false);
        m_objects[0]->m_modelAnim->setUpdateRate(1.0f);
        return;
    }
    m_state = 2;
    if (m_voiceTask != NULL && m_archive != NULL) {
        static_cast<sndDoubutsugoTask*>(m_voiceTask)->playPageSE();
        int fileId;
        int voiceId = 0;
        if (msgId >= 4 && msgId <= 0x1B) {
            voiceId = msgId + 0x128;
        } else if (msgId >= 0x1C && msgId <= 0x3D) {
            voiceId = msgId - 0x1C;
        } else if (msgId >= 0x3E && msgId <= 0x5F) {
            voiceId = msgId + 0x26;
        } else if (msgId >= 0x63 && msgId <= 0x82) {
            voiceId = msgId + 0x65;
        } else if (msgId >= 0x85 && msgId <= 0xB9) {
            voiceId = msgId + 0x10A;
        } else if (msgId >= 1 && msgId <= 3) {
            voiceId = msgId + 0x1F3;
        }
        fileId = 0x32;
        if (!fn_8004D9E8(g_GameGlobal)) {
            fileId = 0x33;
        }
        void* script = fn_27_2B2764(reinterpret_cast<itArchive*>(m_archive), 1, 1, voiceId, fileId);
        if (script != NULL && !fn_8004D9E8(g_GameGlobal)) {
            fn_8007B0A4(static_cast<sndDoubutsugoTask*>(m_voiceTask), script);
        }
    }
    Vec3f trans;
    Vec3f* src = &m_objects[0]->m_trans;
    trans.m_x = src->m_x;
    trans.m_y = src->m_y;
    trans.m_z = src->m_z;
    trans.m_y = posY;
    m_objects[0]->setTrans(&trans);
    m_msg->setDelayPrint(1, delay1, delay2, delay3, delay4);
    char* text;
    switch (msgId) {
    case 0x4F:
        text = m_text;
        m_textLen = 0;
        m_textLen += Message::writeTagClear(text);
        appendText(0x60);
        appendName(text, m_nameKind1, m_nameStr1, m_nameWStr1);
        appendText(0x61);
        appendName(m_text, m_nameKind2, m_nameStr2, m_nameWStr2);
        appendText(0x62);
        text[m_textLen] = 0;
        m_msg->printData(1, m_text, m_textLen);
        break;
    case 0x6F:
        text = m_text;
        m_textLen = 0;
        m_textLen += Message::writeTagClear(text);
        appendText(0x83);
        appendName(text, m_nameKind1, m_nameStr1, m_nameWStr1);
        appendText(0x84);
        text[m_textLen] = 0;
        m_msg->printData(1, m_text, m_textLen);
        break;
    default:
        m_msg->printIndex(1, msgId, NULL);
        break;
    }
    m_msg->endPrint();
}
