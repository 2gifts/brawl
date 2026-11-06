#include <gf/gf_heap_manager.h>
#include <if/if_adv_gauge.h>
#include <if/if_stgedit_mngr.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <gm/gm_global.h>
#include <gf/gf_3d_scene.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
MuObject* lbl_805A0338 = NULL;
nw4r::g3d::ScnMdlExpand* lbl_805A033C = NULL;
MuObject* lbl_805A0340 = NULL;
nw4r::g3d::ScnMdlExpand* lbl_805A0344 = NULL;

// HYPOTHESIS: table layouts are shared with the adventure enemy HUD.
struct IfAdvZakoModelInfo {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

struct IfAdvZakoScnMdlInfo {
    u8 m_start;
    u8 m_end;
    u8 m_objectNo;
    u8 m_capacity;
};


static const IfAdvZakoModelInfo lbl_804093B0[7] = {
    { "InfAdvPos_TopN", 0, 0, 1 },
    { "InfAdvScore0000_TopN", 1, 1, 2 },
    { "InfAdvScore0001_TopN", 2, 11, 3 },
    { "InfAdvCrowd0000_TopN", 12, 12, 4 },
    { "InfAdvCrowd0001_TopN", 13, 63, 5 },
    { "InfAdvPsta0000_TopN", 64, 64, 6 },
    { "InfAdvStock0000_TopN", 65, 65, 7 },
};

static const IfAdvZakoScnMdlInfo lbl_804093E8[3] = {
    { 0, 0, 0, 8 },
    { 1, 1, 1, 9 },
    { 2, 2, 12, 50 },
};

void IfAdvGauge::createModel(nw4r::g3d::ResFile* resFile) {
    MuObject* object;
    createObjResFile(lbl_804093B0, 7, resFile);
    createExpMdl(lbl_804093E8, 3);
    int objectNos[5] = { 0, 1, 12, 64, 65 };
    int* nos = objectNos;
    for (int index = 0; index < 5; index++, nos++) {
        object = unk0[*nos];
        object->enableSkipMatrix(true);
        object->enableSkipMaterial(true);
    }
}

static const IfAdvZakoModelInfo lbl_80409408[3] = {
    { "InfPos_TopN", 0x54, 0x54, 9 },
    { "InfTime_TopN", 0x4b, 0x4b, 8 },
    { "InfTimenum_TopN", 0x4c, 0x52, 8 },
};

static const IfAdvZakoScnMdlInfo lbl_805A2878[2] = {
    { 6, 6, 0x54, 4 },
    { 5, 5, 0x4b, 8 },
};

static const IfAdvZakoModelInfo lbl_805A2880[1] = {
    { "InfAdvStock0001_TopN", 0x43, 0x4a, 7 },
};

static const IfAdvZakoScnMdlInfo lbl_805A2888[2] = {
    { 4, 4, 0x4a, 1 },
    { 3, 3, 0x41, 10 },
};

void IfAdvGauge::createModel2(nw4r::g3d::ResFile* resFile) {
    createObjResFile(lbl_80409408, 3, resFile);
    createExpMdl(lbl_805A2878, 2);
}

void IfAdvGauge::createModelStRes(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* stResFile) {
    createObjResFile(lbl_805A2880, 1, resFile);
    unk238 = stResFile;
    createExpMdl(lbl_805A2888, 2);
}

void IfAdvGauge::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile) {
    int count;
    int index;
    int instance;
    const IfAdvZakoModelInfo* info = static_cast<const IfAdvZakoModelInfo*>(nodeInfo);
    MuObject* object;
    for (index = 0; index < nodeCount; index++, info++) {
        count = 1;
        if (info->m_start < info->m_end) {
            count = info->m_end - info->m_start + 1;
        }
        for (instance = 0; instance < count; instance++) {
            object = MuObject::create(resFile, info->m_name, info->m_priority, NULL, Heaps::InfoInstance);
            unk0[info->m_start + instance] = object;
            char animName[0x40];
            strcpy(animName, info->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void IfAdvGauge::createExpMdl(const void* nodeInfo, s32 nodeCount) {
    int count;
    int index;
    int instance;
    const IfAdvZakoScnMdlInfo* info = static_cast<const IfAdvZakoScnMdlInfo*>(nodeInfo);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
    for (index = 0; index < nodeCount; index++, info++) {
        count = 1;
        if (info->m_start < info->m_end) {
            count = info->m_end - info->m_start + 1;
        }
        for (instance = 0; instance < count; instance++) {
            unk154[info->m_start + instance] = nw4r::g3d::ScnMdlExpand::Construct(
                allocator, NULL, info->m_capacity, unk0[info->m_objectNo + instance]->m_sceneModel);
        }
    }
}

void IfAdvGauge::addPressStart(nw4r::g3d::ScnMdlExpand* model) {
    model->PushBack(unk0[0x40]->m_sceneModel, "pos02");
}

void IfStgeditMngr::freeResource() {
    for (s32 index = 0; index < 50; index++) {
        if (unk310[index]) {
            unk240[index]++;
            int length = unk34[index]->getNodeAnimLength();
            if (length == unk240[index]) {
                unk240[index] = 0;
                unk310[index] = 0;
                unk308 = index;
                unk34[unk308]->setFrameNode(0.0f);
                unk34[unk308]->setFrameMatCol(0.0f);
                char nodeName[6] = {};
                nw4r::g3d::ScnMdlExpand* model = unk15C;
                lbl_805A033C->Remove(model);
                lbl_805A033C->PushBack(model, "pos03");
                sprintf(nodeName, "pos%02d", index);
                model->Remove(unk34[index]->m_sceneModel);
                unk308 = index;
                unk30->setFrameNode(0.0f);
            }
            unk34[index]->setFrameNode(unk240[index]);
            unk34[index]->setFrameMatCol(unk240[index]);
        }
    }
}

void IfAdvGauge::removePressStart(nw4r::g3d::ScnMdlExpand* model) {
    model->Remove(unk0[0x40]->m_sceneModel);
}

void IfAdvGauge::drawStockSlotAnime(int count) {
    unk0[0x41]->setFrameNode(count);
}

void IfAdvGauge::setCrowd(int crowd) {
    unk308 = crowd;
    unk0[13 + crowd]->setFrameNode(0.0f);
    unk0[13 + unk308]->setFrameMatCol(0.0f);
}

void IfAdvGauge::draw2PEnt() {
    MuObject* object = unk0[0x40];
    if (++unk23C == 30) {
        unk23C = 0;
    }
    object->setFrameMatCol(unk23C);
}

void IfAdvGauge::drawStockBase(int show) {
    nw4r::g3d::ScnMdlExpand* model = unk154[3];
    if (show != 0) {
        lbl_805A033C->Remove(model);
        lbl_805A033C->PushBack(model, "pos07");
    } else {
        lbl_805A033C->Remove(model);
    }
}

void IfAdvGauge::createLocator(nw4r::g3d::ResFile* resFile) {
    char animName[0x40];
    const char* name = "InfAdvPos_TopN";
    lbl_805A0338 = MuObject::create(resFile, name, 1, NULL, Heaps::InfoInstance);
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0338->changeAnimN(animName);
    lbl_805A0338->m_modelAnim->setUpdateRate(0.0f);
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        lbl_805A0338->setFrameNode(0.0f);
    } else {
        lbl_805A0338->setFrameNode(1.0f);
    }
    lbl_805A033C = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x20, lbl_805A0338->m_sceneModel);
}

void IfAdvGauge::createLocator2(nw4r::g3d::ResFile* resFile) {
    char animName[0x40];
    const char* name = "InfPos_TopN";
    lbl_805A0340 = MuObject::create(resFile, name, 9, NULL, Heaps::InfoInstance);
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0340->changeAnimN(animName);
    lbl_805A0340->m_modelAnim->setUpdateRate(0.0f);
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        lbl_805A0340->setFrameNode(0.0f);
    } else {
        lbl_805A0340->setFrameNode(1.0f);
    }
    lbl_805A0344 = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x20, lbl_805A0340->m_sceneModel);
}

IfAdvGauge* IfAdvGauge::create() {
    return new (Heaps::InfoInstance) IfAdvGauge();
}

IfAdvGauge::~IfAdvGauge() {
    for (int index = 0; index < 7; index++) {
        if (unk154[index] != NULL) {
            unk154[index]->Destroy();
        }
    }
    for (int index = 0; index < 0x55; index++) {
        delete unk0[index];
    }
    g_gfSceneRoot->removeInfoCamResAnm();
}

void IfAdvGauge::destroyLocator() {
    if (lbl_805A033C != NULL) {
        lbl_805A033C->Destroy();
        lbl_805A033C = NULL;
    }
    if (lbl_805A0338 != NULL) {
        delete lbl_805A0338;
        lbl_805A0338 = NULL;
    }
    if (lbl_805A0344 != NULL) {
        lbl_805A0344->Destroy();
        lbl_805A0344 = NULL;
    }
    if (lbl_805A0340 != NULL) {
        delete lbl_805A0340;
        lbl_805A0340 = NULL;
    }
}

void IfAdvGauge::removeScore() {
    lbl_805A033C->Remove(unk154[1]);
    unk361 = 0;
}

void IfAdvGauge::removeCountTime() {
    lbl_805A033C->Remove(unk154[5]);
    unk362 = 0;
}

void IfAdvGauge::setCrowdDelAnimeNum(s32 num) {
    unk30C = num;
    unk310[num] = 1;
}

void IfAdvGauge::deleteStockSlotFirstIcon() {
    unk154[3]->Remove(unk0[0x43]->m_sceneModel);
}

void IfAdvGauge::drawCrowd() {
    int index;
    char nodeName[6] = {};
    nw4r::g3d::ScnMdlExpand* model = unk154[2];
    lbl_805A033C->Remove(model);
    lbl_805A033C->PushBack(model, "pos03");
    for (index = 0; index < 50; index++) {
        model->Remove(unk0[13 + index]->m_sceneModel);
    }
    for (index = 0; index < unk308; index++) {
        sprintf(nodeName, "pos%02d", index);
        model->PushBack(unk0[13 + index]->m_sceneModel, nodeName);
    }
    unk0[12]->setFrameNode(0.0f);
}
