#include <gf/gf_pad_status.h>
#include <snd/snd_system.h>
#include <gf/gf_pad_system.h>
#include <if/if_adv_pause.h>
#include <if/if_snap_save.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <string.h>
#include <types.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_task_scheduler.h>
#include <if/if_adv_radar.h>
#include <revolution/OS.h>

extern IfAdvRadar* lbl_805A0348[2];
char lbl_8045D268[] = "PauseExit\n";
char lbl_8059E640[] = "Camera";

IfAdvPause* IfAdvPause::create(int arg1, int arg2) {
    return new (Heaps::System) IfAdvPause(arg1, arg2);
}

void IfAdvPause::exitPause() {
    OSReport(lbl_8045D268);
    unk147 = 0;
    unk110->Remove(unkE8[0]->m_sceneModel);
    if (unk12C != NULL) {
        unk12C->exit();
        unk12C = NULL;
    }
    CameraController* camera = static_cast<CameraController*>(gfTaskScheduler::getInstance()->getTask(lbl_8059E640));
    camera->changeInput(static_cast<CameraController::Kind>(unk130));
    if (lbl_805A0348[0] != NULL) {
        lbl_805A0348[0]->hide();
    }
}

void IfAdvPause::destroyModel() {
    for (int i = 0; i < 7; i++) {
        if (unkE8[i] != NULL) {
            delete unkE8[i];
            unkE8[i] = NULL;
        }
    }
    if (unk110 != NULL) {
        unk110->Destroy();
        unk110 = NULL;
    }
}

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

extern const UnkModelNodeInfo lbl_805A2868[1];
extern const UnkModelNodeInfo lbl_80409378[5];
// nw4r::g3d::ScnGroup::Construct (the map name); other files call it by this stand-in name.
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, int capacity);

void IfAdvPause::createModel(nw4r::g3d::ResFile* resFile1, nw4r::g3d::ResFile* resFile2) {
    createObjResFile(lbl_805A2868, 1, resFile1);
    createObjResFile(lbl_80409378, 5, resFile2);
    unk110 = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 7);
}

IfAdvPause::~IfAdvPause() {
    for (int i = 0; i < 7; i++) {
        if (unkE8[i] != NULL) {
            delete unkE8[i];
            unkE8[i] = NULL;
        }
    }
    if (unk110 != NULL) {
        unk110->Destroy();
        unk110 = NULL;
    }
    if (unk12C != NULL) {
        unk12C->exit();
        unk12C = NULL;
    }
    if (unk138 != NULL) {
        unk138->exit();
        unk138 = NULL;
    }
}

// TODO: clean up, give variables real names
namespace jj_8010A36C {

extern "C" int fn_800BA3D8(void*);
extern "C" void fn_80016900(void*, int, int);
extern "C" void OSReport(const char*, ...);

extern void* g_gfApplication;
extern sndSystem* g_sndSystem;
extern char lbl_8045D250[];
extern char lbl_8045D2A4[];
extern float lbl_805A2858;

class VObj {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual float slot6();
};

struct Mid {
    char pad[0xC];
    VObj* v;
};

struct Self {
    char pad0[0x8];
    int entries[3];
    int count;
    int index;
    char pad1[0x10];
    int flags2c;
    u32 flags30;
    char pad2[0xB4];
    MuObject* obj;
    char pad3[0x54];
    int counter;
    u8 f144;
    char pad4[2];
    u8 f147;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_8010A36C(Self* self, int* out) {
    gfPadStatus status;
    u32 flags = self->flags30;
    int old;
    int idx;
    MuObject* obj;

    g_gfPadSystem->getSysPadStatus(fn_800BA3D8((char*)self + 0x20), &status);

    old = self->index;
    if ((flags & 1) && old > 0) {
        if (self->f144 != 0) {
            self->index = self->index - 1;
        } else if (self->index == 0 || self->index == 2) {
            self->index = self->index - 2;
        }
    }
    if (flags & 2) {
        idx = self->index;
        if (idx < self->count - 1) {
            if (self->f144 != 0) {
                self->index = idx + 1;
            } else if (idx == 0 || idx == 2) {
                self->index = self->index + 2;
            }
        }
    }
    if (old != self->index) {
        g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
    }
    obj = self->obj;
    self->counter = self->counter + 1;
    if (self->counter == 30) {
        self->counter = 0;
    }
    obj->changeClrAnimNIf(lbl_8045D250);
    obj->setFrameMatCol((f32)self->counter);
    obj->setFrameVisible((f32)self->index);
    if (((u32*)&status)[3] & 0x100) {
        *out = self->entries[self->index];
        return 1;
    }
    {
        VObj* v = ((Mid*)*(char**)((char*)obj + 0x14))->v;
        if (lbl_805A2858 == v->slot6()) {
            u32 f = self->flags2c;
            if ((f & 0x200) || (f & 0x1000)) {
                OSReport(lbl_8045D2A4);
                self->f147 = 1;
                fn_80016900(g_gfApplication, 0, 0);
                return 2;
            }
        }
    }
    return 0;
}
}

