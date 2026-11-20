#include <snd/snd_system.h>
#include <revolution/OS/OSError.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_photo.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_task_scheduler.h>
#include <if/if_photo.h>
#include <if/if_snap_save.h>
#include <memory.h>
#include <mu/mu_adv_name.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// HYPOTHESIS: each photo state has initialization and update callbacks.
extern const struct {
    void (*unk0)(IfPhoto* photo);
    s32 (*unk4)(IfPhoto* photo);
} lbl_80408C08[4];

void IfPhoto::main() {
    s32 previousStep = m_step;
    if (lbl_80408C08[previousStep].unk4 != NULL) {
        m_step = lbl_80408C08[previousStep].unk4(this);
    }
    if (m_step != previousStep) {
        if (lbl_80408C08[m_step].unk0 != NULL) {
            lbl_80408C08[m_step].unk0(this);
        }
    }
}

// IfSnapSaveTask::getState (map name; still unnamed in if_snap_save.cpp)
extern "C" s32 fn_800F2894(IfSnapSaveTask* task);

// HYPOTHESIS: model records share the menu model table layout.
extern const muAdvNameModel lbl_805A2590[1];
extern const muAdvNameModel lbl_80408B98[5];
extern const muAdvNameModel lbl_80408BC0[5];

void IfPhoto::createModel(nw4r::g3d::ResFile* modelFile, nw4r::g3d::ResFile* resFile,
                          void* msgData, nw4r::g3d::ResFile* resFile2, u32 helpFlags) {
    init();
    m_resFile = *resFile;
    m_msgData = msgData;
    m_resFile2 = *resFile2;
    createModelByTbl(modelFile, lbl_805A2590, 1);
    m_helpFlags = helpFlags;
    if (helpFlags & 3) {
        createModelByTbl(modelFile, lbl_80408B98, 5);
        if (helpFlags & 2) {
            for (int index = 1; index < 6; index++) {
                m_objects[index]->setFrameVisible(1.0f);
            }
        }
    }
    if (helpFlags & 4) {
        createModelByTbl(modelFile, lbl_80408BC0, 5);
    }
    initMsg();
}

void IfPhoto::createModelByTbl(nw4r::g3d::ResFile* modelFile, const void* table, int count) {
    int copies;
    int index;
    int copy;
    const muAdvNameModel* entry = static_cast<const muAdvNameModel*>(table);
    for (index = 0; index < count; index++, entry++) {
        copies = 1;
        if (entry->m_start < entry->m_end) {
            copies = entry->m_end - entry->m_start;
        }
        for (copy = 0; copy < copies; copy++) {
            MuObject* object = MuObject::create(modelFile, entry->m_name, entry->m_priority,
                                               NULL, Heaps::MenuInstance);
            m_objects[entry->m_start + copy] = object;
            object->changeAnimN(entry->m_name);
            m_objects[entry->m_start + copy]->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

IfPhoto::IfPhoto() {
    init();
}

void IfPhoto::init() {
    m_scnGroup = NULL;
    for (int i = 0; i < 11; i++) {
        m_objects[i] = NULL;
    }
    m_msg = NULL;
    m_step = 0;
    unk38 = 0;
    unk3C = 0;
    unk40 = 0;
    unk44 = 0;
    m_padNo = 0;
    unk4C = 0;
    m_cameraInput = 2;
    m_resFile = nw4r::g3d::ResFile();
    m_msgData = NULL;
    m_snapSaveTask = NULL;
    m_state = 0;
    unk60 = 0;
    unk64 = 1;
    m_resFile2 = nw4r::g3d::ResFile();
    m_helpFlags = 0;
}

void IfPhoto::startMelee(nw4r::g3d::ScnGroup* scnGroup) {
    m_scnGroup = scnGroup;
}

CameraController* IfPhoto::getCameraController() {
    return static_cast<CameraController*>(gfTaskScheduler::getInstance()->getTask("Camera"));
}

gfPadStatus IfPhoto::getPadStatus() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(m_padNo, &status);
    return status;
}

void IfPhoto::removeAllObj() {
    nw4r::g3d::ScnGroup* group = m_scnGroup;
    while (group->sceneItemsCount != 0) {
        // nw4r ScnGroup::PopBack
        if (group->sceneItemsCount != 0) {
            group->Remove((u32)(group->sceneItemsCount - 1));
        }
    }
}

bool IfPhoto::haveSnapHelp() {
    return m_helpFlags & 1;
}

bool IfPhoto::haveRetryHelp() {
    return (m_helpFlags >> 1) & 1;
}

bool IfPhoto::haveResetHelp() {
    return (m_helpFlags >> 2) & 1;
}

bool IfPhoto::getSaveEnable(IfSnapSaveMedia* media, u32 size) {
    bool enable = false;
    if (media->m_status == 0 && media->m_free >= size) {
        enable = true;
    }
    return enable;
}

void IfPhoto::photoStepSnapSaveInit() {
    m_snapSaveTask = IfSnapSaveTask::create(&m_resFile, &m_resFile2, m_msgData, m_scnGroup, 0xC4,
            Heaps::MenuInstance, Heaps::MenuInstance, Heaps::Network);
}

s32 IfPhoto::photoStepSnapSaveMain() {
    if (fn_800F2894(m_snapSaveTask) != 0) {
        m_snapSaveTask->exit();
        m_snapSaveTask = NULL;
        return 1;
    }
    return m_step;
}

void IfPhoto::exitPause() {
    if (m_snapSaveTask) {
        m_snapSaveTask->exit();
        m_snapSaveTask = NULL;
    }
    CameraController* camera = getCameraController();
    cmPhotoController* photo = dynamic_cast<cmPhotoController*>(camera->getCameraController(CameraController::Photo));
    if (photo->unk60 <= 0) {
        camera->changeInput(static_cast<CameraController::Kind>(m_cameraInput));
    }
}

void cmPhotoController::clearControl() {
    memset(unk14, 0, sizeof(unk14));
}

void IfPhoto::getMediaState(IfSnapSaveMedia* media, int kind) {
    // HYPOTHESIS: the nonzero media kind leaves the free-size value unset.
    int status;
    u32 size;
    if (kind == 0) {
        size = 0x1000000;
        status = 0;
    } else {
        status = 1;
    }
    switch (status) {
    case 0:
        size >>= 17;
        status = 0;
        break;
    }
    media->m_status = status;
    media->m_free = size;
}

// TODO: clean up, give variables real names
extern "C" void fn_800F1010(void) {}

extern "C" u32 fn_800F01B8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x40);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_800F1750(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x48);
}

// TODO: clean up, give variables real names
namespace jj_800F1774 {
// fn_800F1774: returns the global sound system pointer/handle (sda21-relative load).

extern "C" int g_sndSystem;

// TODO: clean up, give variables real names
extern "C" int fn_800F1774(void) {
    return g_sndSystem;
}
}

namespace jj_800F1758 {
// TODO: clean up, give variables real names
extern "C" int fn_800F1758(const unsigned int *p)
{
    return (p[3] >> 12) & 1;
}
}

// TODO: clean up, give variables real names
namespace jj_800EFC8C {
extern "C" {

void exit__6gfTaskFv(void*);
void detachMuObject__5MuMsgFP8MuObject(void*, void*);
void __dt__5MuMsgFv(void*, int);
void __dl__FPv(void*);
extern "C" char lbl_8045B824[];

struct Part {
    char padding[0x5C];
    virtual void method(int);
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_800EFC8C(void* arg0, int arg1) {
    if (arg0 != 0) {
        char* base = (char*)arg0;
        void* task = *(void**)(base + 0x5C);
        *(char**)(base + 0x74) = lbl_8045B824;
        if (task != 0) {
            exit__6gfTaskFv(task);
            *(void**)(base + 0x5C) = 0;
        }
        detachMuObject__5MuMsgFP8MuObject(*(void**)(base + 0x30), *(void**)(base + 0x4));
        int i = 0;
        Part** cur = (Part**)arg0;
        do {
            Part* p = cur[1];
            if (p != 0) {
                if (p != 0) {
                    p->method(1);
                }
                cur[1] = 0;
            }
            i++;
            cur++;
        } while (i < 11);
        if (*(void**)(base + 0x30) != 0) {
            __dt__5MuMsgFv(*(void**)(base + 0x30), 1);
            *(void**)(base + 0x30) = 0;
        }
        if (arg1 > 0) {
            __dl__FPv(arg0);
        }
    }
    return arg0;
}

}
}

namespace jj_800F00D4 {
extern "C" {
void* getInstance__15gfTaskSchedulerFv();
void* getTask__15gfTaskSchedulerFPCc(void*, const char*);
void* getCameraController__16CameraControllerFQ216CameraController4Kind(void*, int);
void changeInput__16CameraControllerFQ216CameraController4Kind(void*, int);
void* __dynamic_cast(void*, long, const void*, const void*, long);
void setSeamless__17cmPhotoControllerFb(void*, s32);
void fn_800A8DA8(void*, s32);
void fn_800A709C(void*, s32);
void fn_800F1014(void*);
extern "C" char lbl_8059E3A0;
extern "C" char lbl_8059DEC0;
extern "C" char __RTTI__18gfCameraController;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800F00D4(void* arg0, s8 arg1, s32 arg2, s32 arg3, s32 arg4) {
    char* self = (char*)arg0;
    char* task = (char*)getTask__15gfTaskSchedulerFPCc(getInstance__15gfTaskSchedulerFv(), &lbl_8059E3A0);
    *(s32*)(self + 0x70) = 1;
    *(s32*)(self + 0x50) = *(s32*)(task + 0x40);
    *(s8*)(self + 0x64) = 1;
    char* cam = (char*)__dynamic_cast(getCameraController__16CameraControllerFQ216CameraController4Kind(task, 7), 4, &lbl_8059DEC0, &__RTTI__18gfCameraController, 0);
    if (*(s32*)(cam + 0x60) <= 0) {
        changeInput__16CameraControllerFQ216CameraController4Kind(task, 7);
        setSeamless__17cmPhotoControllerFb(cam, arg4);
        fn_800A8DA8(cam, 0);
        if (arg3 >= 0) {
            *(s32*)(cam + 0x40) = arg3;
            fn_800A709C(cam, arg3);
        }
    }
    *(s8*)(self + 0x48) = arg1;
    *(s32*)(self + 0x4C) = arg2;
    fn_800F1014(arg0);
    *(s32*)(self + 0x34) = 1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_800F14C0 {

struct Raw { unsigned int w[16]; };
struct Self {
    char pad0[0x34];
    int result;
    int a[2];
    int b[2];
    char pad48[0x18];
    int delay;
    bool toggle;
    char pad65[0xb];
    int state;
};
extern "C" {
void* getCameraController__7IfPhotoFv();
void getPadStatus__7IfPhotoFv(Raw*, void*);
void getMediaState__7IfPhotoFP15IfSnapSaveMediai(void*, int*, int);
int haveSnapHelp__7IfPhotoFv(void*);
int fn_800F01B8(void*);
void* fn_800F1750(void*);
void* __dynamic_cast(void*, int, void*, void*, int);
void updateControl__17cmPhotoControllerFPC11gfPadStatusb(void*, Raw*, int);
int haveResetHelp__7IfPhotoFv(void*);
int fn_800F0A2C(void*);
int haveRetryHelp__7IfPhotoFv(void*);
int fn_800F0CD8(void*);
int fn_800F1758(Raw*);
int fn_800F0308(void*, int);
void fn_800F05CC(void*, bool);
int getSaveEnable__7IfPhotoFP15IfSnapSaveMediaUl(void*, int*, int);
void clearControl__17cmPhotoControllerFv(void*);
sndSystem* fn_800F1774();
void removeAllObj__7IfPhotoFv(void*);
extern "C" char lbl_8059DEC0, __RTTI__18gfCameraController;
extern "C" char lbl_8045B7E8[];
}
// TODO: clean up, give variables real names
inline Raw getPad(Self* self) {
    Raw r;
    getPadStatus__7IfPhotoFv(&r, self);
    return r;
}
// TODO: clean up, give variables real names
extern "C" int fn_800F14C0(Self* self) {
    void* obj = getCameraController__7IfPhotoFv();
    void* camera = 0;
    Raw pad = getPad(self);
    getMediaState__7IfPhotoFP15IfSnapSaveMediai(self, self->b, 1);
    if (haveSnapHelp__7IfPhotoFv(self) && fn_800F01B8(obj) == 7) {
        camera = __dynamic_cast(fn_800F1750(obj), 4, &lbl_8059DEC0, &__RTTI__18gfCameraController, 0);
        updateControl__17cmPhotoControllerFPC11gfPadStatusb(camera, &pad, 0);
    }
    if (--self->delay < 0) {
        if (haveResetHelp__7IfPhotoFv(self) && fn_800F0A2C(self)) {
            self->state = 3;
            return self->result;
        }
        if (haveRetryHelp__7IfPhotoFv(self) && fn_800F0CD8(self)) {
            self->state = 4;
            return self->result;
        }
        if (fn_800F1758(&pad)) {
            self->state = 2;
            return self->result;
        }
        if (haveSnapHelp__7IfPhotoFv(self)) {
            if (fn_800F0308(self, 1)) {
                self->toggle = !self->toggle;
                fn_800F05CC(self, self->toggle);
            }
            if ((getSaveEnable__7IfPhotoFP15IfSnapSaveMediaUl(self, self->a, 1) || getSaveEnable__7IfPhotoFP15IfSnapSaveMediaUl(self, self->b, 1)) && fn_800F0308(self, 0)) {
                if (camera) clearControl__17cmPhotoControllerFv(camera);
                fn_800F1774()->playSE((SndID)4, -1, 0, 0, -1);
                OSReport(lbl_8045B7E8);
                removeAllObj__7IfPhotoFv(self);
                return 2;
            }
        }
    }
    fn_800F05CC(self, self->toggle);
    return self->result;
}
}

namespace jj_800F05CC {


extern "C" unsigned char lbl_805A25AC[4];
extern "C" unsigned char lbl_805A25B0[4];

class Object_vptr_0 {
public:
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
    virtual void slot11(int, void*);
    virtual void slot12(int);

    char pad[0xe0];
    int count;
};

struct Item {
    char pad[0x10];
    void* value;
};

struct Self {
    Object_vptr_0* object;
    Item* items[11];
    char pad[0x18];
    unsigned char port;
    char pad2[0x23];
    int flags;
};

struct Vec2 {
    float x, y;

    Vec2() {}
    Vec2(const Vec2& other) {
        x = other.x;
        y = other.y;
    }
};

struct PadCopy {
    int words[6];
    Vec2 vectors[3];
    unsigned char b0, b1, b2, b3, b4, b5, b6, b7, b8;
    int last;
};

static inline gfPadStatus getSysPad(int port) {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(port, &status);
    return status;
}

// TODO: clean up, give variables real names
extern "C" void fn_800F05CC(Self* self, int arg) {
    Object_vptr_0* object = self->object;
    while (object->count != 0) {
        if (object->count != 0)
            object->slot12(object->count - 1);
    }

    if (arg) {
        self->object->slot11(self->object->count, self->items[0]->value);

        if ((self->flags & 1) || (self->flags & 2)) {
            gfPadStatus raw;
            g_gfPadSystem->getSysPadStatus(self->port, &raw);
            gfPadStatus copy = raw;
            PadCopy status(*(PadCopy*)&copy);
            int index;
            if (status.last >= 0 && status.last < 4)
                index = lbl_805A25AC[status.last];
            else
                index = 5;
            self->object->slot11(self->object->count, self->items[index]->value);
        }

        if (self->flags & 4) {
            gfPadStatus raw;
            g_gfPadSystem->getSysPadStatus(self->port, &raw);
            gfPadStatus copy = raw;
            PadCopy status(*(PadCopy*)&copy);
            int index;
            if (status.last >= 0 && status.last < 4)
                index = lbl_805A25B0[status.last];
            else
                index = 10;
            self->object->slot11(self->object->count, self->items[index]->value);
        }
    }
}
}

