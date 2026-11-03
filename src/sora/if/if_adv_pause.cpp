#include <gf/gf_pad_system.h>
#include <if/if_adv_pause.h>
#include <if/if_snap_save.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <string.h>
#include <types.h>

// muMenuController::getControllerID (the map name); the adventure menus call it by this stand-in name.
extern "C" int fn_800BA3D8(muMenuController* controller);
// IfSnapSaveTask::getState (the map name); if_mngr.cpp calls it by this stand-in name.
extern "C" int fn_800F2894(gfTask* task);

IfAdvPause::IfAdvPause(int arg1, int arg2)
    : unk0(0), unk4(0), unk14(0), unk18(0), unk1C(0),
      unk108(NULL), unk128(0), unk12C(NULL), unk138(0), unk13C(NULL),
      unk140(0), unk144(0), unk145(1) {
    for (int i = 0; i < 3; i++) {
        unk8[i] = 0;
    }
    fn_800B96E8(&unk20, 0);
    unkC8 = 0;
    unk0 = arg2;
    int count = 0;
    unk8[count++] = 0;
    if (arg1) {
        unk8[count++] = 1;
    }
    unk8[count++] = 2;
    unk14 = count;
    for (int i = 0; i < 7; i++) {
        unkE8[i] = NULL;
    }
    unk110 = NULL;
    unk118 = 0;
    unk11C = 0;
    unk120 = 0;
    unk124 = 0;
    unk114 = 0;
    unk130 = 3;
    unk134 = 0;
}

void IfAdvPause::photoStepNoneInit() {
    nw4r::g3d::ScnGroup* group = unk110;
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove((u32)(group->sceneItemsCount - 1));
        }
    }
}

void IfAdvPause::photoStepSnapSaveInit() {
    unk12C = IfSnapSaveTask::create(&unk104, &unk10C, unk108, unk110, 1,
        Heaps::InfoInstance, Heaps::MenuInstance, Heaps::Network);
    drawCameraGuide(0);
}

s32 IfAdvPause::photoStepSnapSaveMain() {
    if (fn_800F2894(unk12C) != 0) {
        unk12C->exit();
        unk12C = NULL;
        return 1;
    }
    return unk114;
}

void IfAdvPause::drawCameraGuide(int visible) {
    MuObject* object;
    for (int i = 2; i < 7; i++) {
        unk110->Remove(unkE8[i]->m_sceneModel);
    }
    if (visible) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(fn_800BA3D8(&unk20), &status);
        int index;
        switch (status.m_controllerType) {
        case 0: index = 2; break;
        case 1: index = 3; break;
        case 3: index = 4; break;
        case 2: index = 5; break;
        default: index = 6; break;
        }
        object = unkE8[index];
        unk110->Insert(unk110->sceneItemsCount, object->m_sceneModel);
        int ready = false;
        if (unk118 == 0 && unk11C >= 1) {
            ready = true;
        }
        if (ready) {
            goto available;
        }
        {
            int ready2 = false;
            if (unk120 == 0 && unk124 >= 1) {
                ready2 = true;
            }
            if (!ready2) {
                goto unavailable;
            }
        }
    available:
        object->setFrameVisible(0.0f);
        return;
    unavailable:
        object->setFrameVisible(1.0f);
    }
}

// HYPOTHESIS: shared model table layout follows IfStgedit's UnkModelNodeInfo.
struct UnkModelNodeInfo {
    const char* m_name;
    u8 unk4;
    u8 unk5;
    u8 unk6;
};

void IfAdvPause::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile) {
    int num;
    int i;
    int j;
    const UnkModelNodeInfo* info = static_cast<const UnkModelNodeInfo*>(nodeInfo);
    MuObject* object;
    for (i = 0; i < nodeCount; i++, info++) {
        num = 1;
        if (info->unk4 < info->unk5) {
            num = info->unk5 - info->unk4 + 1;
        }
        for (j = 0; j < num; j++) {
            object = MuObject::create(resFile, info->m_name, info->unk6, NULL, Heaps::InfoInstance);
            unkE8[info->unk4 + j] = object;
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
