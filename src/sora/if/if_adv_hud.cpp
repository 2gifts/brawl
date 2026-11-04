#include <gf/gf_heap_manager.h>
#include <if/if_adv_gauge.h>
#include <if/if_stgedit_mngr.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0338[0x4] __attribute__((aligned(4))) = { 0 };
nw4r::g3d::ScnMdlExpand* lbl_805A033C = NULL;
unsigned char lbl_805A0340[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0344[0x4] __attribute__((aligned(4))) = { 0 };

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
