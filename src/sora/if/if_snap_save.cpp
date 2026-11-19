#include <gf/gf_pad_status.h>
#include <gf/gf_thread.h>
#include <revolution/OS/OSThread.h>
#include <revolution/GX/GXTypes.h>
#include <string.h>
#include <gf/gf_application.h>
#include <gf/gf_capture_util.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_shutdown_manager.h>
#include <gm/gm_global.h>
#include <if/if_snap_save.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <revolution/OS/OSTime.h>
#include <snd/snd_system.h>
#include <types.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_homemenu.h>
#include <gf/gf_memory_pool.h>
#include <new>

extern const u8 lbl_80408CD0[0x20];

// HYPOTHESIS: still fn_ in other units; names from the map (mu_collection.o, nt_etc_file.o).
extern "C" muCollection* fn_800C35CC(void* mem, int kind, nw4r::g3d::ResFile resFile, nw4r::g3d::ResFile resFile2,
                                     void* msgData, nw4r::g3d::ScnGroup* scnGroup, int drawPriority,
                                     Heaps::HeapType heap, Heaps::HeapType heap2, int unk, int unk2);
extern "C" bool fn_800C39D8(muCollection* collection);
extern "C" nteFileDataPhoto* fn_80152970(void* mem, Heaps::HeapType heap);
extern "C" void fn_800C6E3C(muCollection* collection, int unk, int unk2);
extern "C" void fn_800C6E64(muCollection* collection, int drive, int unk, int unk2);
extern "C" void fn_80153488(nteSection* section);
extern "C" void fn_80153498(nteSection* section, int unk);
extern "C" void fn_80153610(nteSection* section, void* data, s64 time, Heaps::HeapType heap, int unk, int unk2);
extern "C" nw4r::g3d::ScnGroup* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
    MEMAllocator* allocator, u32* size, int capacity, nw4r::g3d::ScnMdl* scnMdl);
extern "C" bool PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(nw4r::g3d::ScnGroup* expand,
                                                                         nw4r::g3d::ScnObj* obj, const char* nodeName);

// HYPOTHESIS: IfSnapSaveTask::makeDataSizeMsg (0x800F227C has no map line of its own, still fn_) writes the
// free size text and returns its length.
extern "C" int fn_800F227C(IfSnapSaveTask* task, char* buf, u32 size, int unk);

// HYPOTHESIS: the replay file data keeps its kind in the first word.
struct IfSnapSaveFileData {
    s32 m_kind;
};

// HYPOTHESIS: views of the game records read by the save flow.
struct IfSnapSaveAppData {
    u8 unk0[0x10];
    u8 unk10; // 0x10 HYPOTHESIS: the SD card can be used
};
struct IfSnapSaveStageEditData {
    u8 unk0[0x249C];
    u8 unk249C;
    u8 unk249D[3];
    u32 unk24A0;
};

// HYPOTHESIS: the wide flag is the high bit of the byte at 0x28 of the menu data.
struct IfSnapSaveMenuBits {
    u8 m_unk00[0x28];
    bool m_isWide : 1;
};

struct IfSnapSaveModelData {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

struct IfSnapSaveStepFunc {
    void (*m_init)(IfSnapSaveTask* task);
    s32 (*m_main)(IfSnapSaveTask* task);
};

struct IfSnapSaveCursorData {
    float m_frame;
    u8 m_play;
    s32 m_media;
};

struct IfSnapSaveErrorData {
    u8 m_objIndex;
    const char* m_nodeName;
};

struct IfSnapSaveSavingData {
    u8 m_cursorState;
    u32 m_lineIndex;
};

static const IfSnapSaveModelData s_modelTbl[5] = {
    { "InfPause0001_TopN", 0, 0, 0 },
    { "InfPause0100_TopN", 1, 1, 1 },
    { "InfPause0101_TopN", 2, 2, 3 },
    { "InfPause0102_TopN", 3, 5, 2 },
    { "InfCmnWait_TopN", 5, 5, 4 },
};

static const IfSnapSaveStepFunc s_stepTbl[12] = {
    { IfSnapSaveTask::photoStepNoneInit, NULL },
    { IfSnapSaveTask::photoStepShootInit, IfSnapSaveTask::photoStepShootMain },
    { IfSnapSaveTask::photoStepCaptureInit, IfSnapSaveTask::photoStepCaptureMain },
    { IfSnapSaveTask::photoStepFadeInit, IfSnapSaveTask::photoStepFadeMain },
    { IfSnapSaveTask::photoStepMediaScanWaitInit, IfSnapSaveTask::photoStepMediaScanWaitMain },
    { IfSnapSaveTask::photoStepSelectMediaInit, IfSnapSaveTask::photoStepSelectMediaMain },
    { IfSnapSaveTask::photoStepErrorInit, IfSnapSaveTask::photoStepErrorMain },
    { IfSnapSaveTask::photoStepConfirmStartInit, IfSnapSaveTask::photoStepConfirmStartMain },
    { IfSnapSaveTask::photoStepConfirmInit, IfSnapSaveTask::photoStepConfirmMain },
    { IfSnapSaveTask::photoStepSavingInit, IfSnapSaveTask::photoStepSavingMain },
    { IfSnapSaveTask::photoStepDoneInit, IfSnapSaveTask::photoStepDoneMain },
    { IfSnapSaveTask::photoStepWaitMediaInit, IfSnapSaveTask::photoStepWaitMediaMain },
};

static const IfSnapSaveCursorData s_cursorTbl[10] = {
    { 0.0f, 0, 1 },  { 0.0f, 1, 1 },  { 10.0f, 0, 1 }, { 10.0f, 1, 1 }, { 19.0f, 0, 1 },
    { 20.0f, 0, 0 }, { 20.0f, 1, 0 }, { 30.0f, 0, 0 }, { 30.0f, 1, 0 }, { 39.0f, 0, 0 },
};

static const IfSnapSaveErrorData s_errorTbl[2] = {
    { 4, "pos01" },
    { 3, "pos00" },
};

static const IfSnapSaveSavingData s_savingTbl[2] = {
    { 8, 0x55 },
    { 3, 0x54 },
};

IfSnapSaveTask* IfSnapSaveTask::create(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                                       nw4r::g3d::ScnGroup* scnGroup, int drawPriority, Heaps::HeapType heap,
                                       Heaps::HeapType heap2, Heaps::HeapType dataHeap) {
    IfSnapSaveTask* task = new (heap) IfSnapSaveTask();
    task->initProc(0, resFile, resFile2, msgData, scnGroup, drawPriority, heap, heap2, dataHeap, true);
    return task;
}

// HYPOTHESIS: IfSnapSaveTask::create for a replay (0x800F18C4, still fn_: IfMngr calls it by that name).
extern "C" IfSnapSaveTask* fn_800F18C4(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                                       nw4r::g3d::ScnGroup* scnGroup, int drawPriority, IfSnapSaveFileData* data,
                                       void* unkA4, Heaps::HeapType heap, Heaps::HeapType heap2,
                                       Heaps::HeapType dataHeap) {
    static const u8 kinds[4] = { 0xFF, 0, 1, 2 };
    IfSnapSaveTask* task = new (heap) IfSnapSaveTask();
    Heaps::HeapType sectionHeap = dataHeap;
    if (dataHeap == 0xFF) {
        sectionHeap = heap2;
    }
    fn_80153610(&task->m_section, data, OSGetTime(), sectionHeap, 0, 0);
    task->unkA4 = unkA4;
    task->initProc(kinds[data->m_kind], resFile, resFile2, msgData, scnGroup, drawPriority, heap, heap2, dataHeap,
                   false);
    return task;
}

IfSnapSaveTask::IfSnapSaveTask() : gfTask("IfSnapSave", Category_Info, 0xE, 6, true) {
    m_scnGroup = NULL;
    m_step = 0;
    unk68 = 2;
    unk6C = 0;
    m_captureBuffer = NULL;
    m_jpegBuffer = NULL;
    m_result = 0;
    fn_80153488(&m_section);
    unkA4 = NULL;
    unkA8 = 0;
    m_collection = NULL;
    m_scanEnable = false;
    m_scanWait = 0;
    m_scanState = 0;
    m_photoData = NULL;
    m_jpegWork = NULL;
    unkC8 = 0;
    m_kind = 0;
    m_dataHeap = (Heaps::HeapType)0xFF;
    unkD8 = 0;
    m_scanned = false;
    for (int i = 0; i < 6; i++) {
        m_objects[i] = NULL;
    }
    m_expand = NULL;
    memset(&m_stepWork, 0, sizeof(m_stepWork));
    for (int i = 0; i < 2; i++) {
        m_media[i].m_status = 0;
        m_media[i].m_free = 0;
    }
}

void IfSnapSaveTask::initProc(int kind, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                              nw4r::g3d::ScnGroup* scnGroup, int drawPriority, Heaps::HeapType heap,
                              Heaps::HeapType heap2, Heaps::HeapType dataHeap, bool capture) {
    static const u8 collectionKinds[4] = { 1, 2, 3, 0 };
    unk2C_b1 = false;
    m_kind = kind;
    createModel(resFile, drawPriority, heap, heap2);
    initMsg(msgData, heap, heap2);
    m_scnGroup = scnGroup;
    m_cursorState = 0;
    if (kind == 2) {
        IfSnapSaveStageEditData* stageEdit = reinterpret_cast<IfSnapSaveStageEditData*>(g_GameGlobal->m_stageEditData);
        unkD8 = stageEdit->unk249C;
        unk68 = stageEdit->unk24A0 != 0;
    }
    m_dataHeap = dataHeap;
    if (dataHeap == 0xFF) {
        m_dataHeap = heap2;
    }
    int collectionKind = collectionKinds[kind];
    void* mem = operator new(sizeof(muCollection), heap);
    if (mem != NULL) {
        mem = fn_800C35CC(mem, collectionKind, *resFile, *resFile2, msgData, scnGroup, drawPriority + 5, heap, heap, 1,
                          0xF0);
    }
    m_collection = static_cast<muCollection*>(mem);
    if (capture) {
        m_step = 1;
        m_captureBuffer = gfHeapManager::alloc(m_dataHeap, 0x12C000, 0x20);
        m_jpegBuffer = gfHeapManager::alloc(m_dataHeap, 0x19008);
        void* photo = operator new(0x14, heap);
        if (photo != NULL) {
            photo = new (photo) nteFileDataPhoto(m_dataHeap);
        }
        m_photoData = static_cast<nteFileDataPhoto*>(photo);
        m_jpegWork = new (heap) gfJpegEncodeWork();
    } else {
        m_step = 3;
        m_captureBuffer = NULL;
    }
    if (s_stepTbl[m_step].m_init != NULL) {
        s_stepTbl[m_step].m_init(this);
    }
}

IfSnapSaveTask::~IfSnapSaveTask() {
    if (m_jpegWork != NULL) {
        delete m_jpegWork;
    }
    if (m_photoData != NULL) {
        delete m_photoData;
    }
    if (m_collection != NULL) {
        m_collection->exit();
    }
    destroyModel();
    if (m_jpegBuffer != NULL) {
        gfHeapManager::free(m_jpegBuffer);
    }
    if (m_captureBuffer != NULL) {
        gfHeapManager::free(m_captureBuffer);
    }
    fn_80153498(&m_section, -1);
}

void IfSnapSaveTask::createModel(nw4r::g3d::ResFile* resFile, int drawPriority, Heaps::HeapType heap,
                                 Heaps::HeapType heap2) {
    int i;
    const IfSnapSaveModelData* data;
    for (i = 0, data = s_modelTbl; i < 5; i++, data++) {
        int count = data->m_start < data->m_end ? data->m_end - data->m_start : 1;
        for (int j = 0; j < count; j++) {
            m_objects[data->m_start + j] =
                MuObject::create(resFile, data->m_name, data->m_priority + drawPriority, NULL, heap2);
            m_objects[data->m_start + j]->changeAnimN(data->m_name);
            m_objects[data->m_start + j]->m_modelAnim->setUpdateRate(0.0f);
        }
    }
    m_expand = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
        gfHeapManager::getMEMAllocator(heap2), NULL, 2, m_objects[1]->m_sceneModel);
}

void IfSnapSaveTask::destroyModel() {
    MuMsg* msg = m_msg;
    if (m_expand != NULL) {
        m_expand->Destroy();
        m_expand = NULL;
    }
    msg->detachMuObject(m_objects[1]);
    msg->detachMuObject(m_objects[4]);
    msg->detachMuObject(m_objects[3]);
    delete msg;
    for (int i = 0; i < 6; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
            m_objects[i] = NULL;
        }
    }
}

void IfSnapSaveTask::updateMediaFreeState() {
    static const u8 drives[2] = { 0, 1 };
    for (int i = 0; i < 2; i++) {
        if (i == 0 && reinterpret_cast<IfSnapSaveAppData*>(g_GameGlobal->m_appData)->unk10 == 0) {
            m_media[i].m_status = 5;
            m_media[i].m_free = 0;
        } else {
            muCollectionDrive* drive = &m_collection->m_drives[drives[i]];
            switch (drive->m_state) {
            case 0:
            case 1:
                if (drive->m_state == 1 && drive->unk1 == 0) {
                    m_media[i].m_status = 4;
                    m_media[i].m_free = 0;
                } else {
                    m_media[i].m_status = 1;
                    m_media[i].m_free = drive->m_free;
                }
                break;
            case 2:
                m_media[i].m_status = 2;
                m_media[i].m_free = 0;
                break;
            case 3:
                m_media[i].m_status = 3;
                m_media[i].m_free = 0;
                break;
            }
        }
    }
}

void IfSnapSaveTask::mediaCursorStateInit(int state) {
    MuObject* obj = m_objects[1];
    float play = s_cursorTbl[state].m_play;
    float frame = s_cursorTbl[state].m_frame;
    s32 media = s_cursorTbl[state].m_media;
    m_cursorState = state;
    obj->setFrameNode(frame);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(play ? 1.0f : 0.0f);
    u32 blocks = (m_section.getSize() + 0x1FFFF) / 0x20000;
    bool enough = false;
    if (m_media[media].m_status == 1 && m_media[media].m_free >= blocks) {
        enough = true;
    }
    m_objects[1]->setFrameMatCol(enough ? 0.0f : 1.0f);
}

void IfSnapSaveTask::mediaCursorStateMain() {
    float frame = m_objects[1]->m_modelAnim->m_anmObjChrRes->GetFrame();
    switch (m_cursorState) {
    case 1:
        if (frame >= 9.0f) {
            mediaCursorStateInit(2);
        }
        break;
    case 3:
        if (frame >= 19.0f) {
            mediaCursorStateInit(4);
        }
        break;
    case 6:
        if (frame >= 29.0f) {
            mediaCursorStateInit(7);
        }
        break;
    case 8:
        if (frame >= 39.0f) {
            mediaCursorStateInit(9);
        }
        break;
    case 9:
        break;
    }
}

void IfSnapSaveTask::processDefault() {
    s32 step = m_step;
    scanMediaMain();
    if (s_stepTbl[m_step].m_main != NULL) {
        m_step = s_stepTbl[m_step].m_main(this);
    }
    if (m_step != step && s_stepTbl[m_step].m_init != NULL) {
        s_stepTbl[m_step].m_init(this);
    }
    mediaCursorStateMain();
}

void IfSnapSaveTask::scanMediaMain() {
    switch (m_scanState) {
    case 0:
        if (m_scanEnable && m_collection->m_busy == 0) {
            if (!m_scanned) {
                m_collection->scanAll(0, 0);
                m_scanned = true;
            } else {
                m_collection->scan(1, 0, 0);
            }
            m_scanState = 1;
        }
        break;
    case 1:
        if (m_collection->m_busy == 0) {
            updateMediaFreeState();
            m_scanWait = 60;
            m_scanState = 2;
        }
        break;
    case 2:
        if (--m_scanWait <= 0) {
            m_scanState = 0;
        }
        break;
    }
}

void IfSnapSaveTask::printMediaStatus(u32 msgIndex, u32 lineIndex, s32 status, u32 freeSize) {
    MuMsg* msg = m_msg;
    if (status == 1) {
        char buf[0x100];
        int length = fn_800F227C(this, buf, freeSize, 1);
        msg->printData(msgIndex, buf, length);
    } else {
        switch (status) {
        case 2:
            lineIndex = 0x4E;
            break;
        case 3:
            lineIndex = 0x51;
            break;
        case 4:
            lineIndex = 0x53;
            break;
        case 5:
            lineIndex = 0x30;
            break;
        }
        msg->printIndex(msgIndex, lineIndex);
    }
}

// HYPOTHESIS: IfSnapSaveTask::getState (0x800F2894, still fn_: IfMngr calls it by that name).
extern "C" s32 fn_800F2894(IfSnapSaveTask* task) {
    if (task->m_step != 0) {
        return 0;
    }
    if (fn_800C39D8(task->m_collection)) {
        return task->m_result;
    }
    return 0;
}

void IfSnapSaveTask::photoStepNoneInit(IfSnapSaveTask* task) { }

void IfSnapSaveTask::photoStepShootInit(IfSnapSaveTask* task) {
    for (int i = 0; i < 6; i++) {
        if (task->m_objects[i] != NULL) {
            task->m_scnGroup->Remove(task->m_objects[i]->m_sceneModel);
        }
    }
    if (task->m_expand != NULL) {
        task->m_scnGroup->Remove(task->m_expand);
    }
    MuObject* obj = task->m_objects[0];
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    obj->setFrameNode(0.0f);
    obj->setFrameMatCol(0.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    task->unkC8 = OSGetTime();
}

s32 IfSnapSaveTask::photoStepShootMain(IfSnapSaveTask* task) {
    MuObject* obj = task->m_objects[0];
    if (obj->m_modelAnim->m_anmObjChrRes->GetFrame() < 7.0f) {
        return task->m_step;
    }
    obj->setFrameNode(7.0f);
    obj->setFrameMatCol(7.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    return 2;
}

void IfSnapSaveTask::photoStepCaptureInit(IfSnapSaveTask* task) {
    gfApplication* app = g_gfApplication;
    task->m_stepWork.m_count = 0;
    app->unkED |= 2;
    app->unk10C = task->m_captureBuffer;
    task->unkC0 = 0;
    memset(task->m_jpegBuffer, 0, 0x19008);
}

void IfSnapSaveTask::photoStepFadeInit(IfSnapSaveTask* task) {
    MuObject* obj = task->m_objects[0];
    task->m_scnGroup->Remove(obj->m_sceneModel);
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    obj->setFrameNode(8.0f);
    obj->setFrameMatCol(8.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    MuObject* photoObj = task->m_objects[1];
    float frame;
    if (task->m_kind != 1) {
        photoObj->changeMaterialTex("PausePhotoMat", task->unkA4, 0xA0, 0x78);
        frame = static_cast<float>(reinterpret_cast<IfSnapSaveMenuBits*>(g_GameGlobal->getGlobalRecordMenuDatap())->m_isWide);
    } else {
        frame = 2.0f;
    }
    photoObj->setFrameVisible(frame);
    photoObj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
}

void IfSnapSaveTask::photoStepMediaScanWaitInit(IfSnapSaveTask* task) {
    task->m_scanEnable = true;
}

s32 IfSnapSaveTask::photoStepMediaScanWaitMain(IfSnapSaveTask* task) {
    // MATCH-ONLY: the early exit keeps the native per-drive branches.
    bool scanned;
    for (int i = 0; i < 2; i++) {
        if (task->m_media[i].m_status == 0) {
            scanned = false;
            goto done;
        }
    }
    scanned = true;
done:
    if (!scanned) {
        return task->m_step;
    }
    if (task->unkD8) {
        task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, task->m_expand);
        return 7;
    }
    return 5;
}

void IfSnapSaveTask::photoStepErrorInit(IfSnapSaveTask* task) {
    nw4r::g3d::ScnGroup* expand = task->m_expand;
    const IfSnapSaveErrorData* data = &s_errorTbl[task->unk68];
    IfSnapSaveMedia* media = &task->m_media[task->unk68];
    MuObject* obj = task->m_objects[data->m_objIndex];
    expand->Remove(obj->m_sceneModel);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(expand, obj->m_sceneModel, data->m_nodeName);
    obj->setFrameVisible(1.0f);
    u32 lineIndex;
    switch (media->m_status) {
    case 1:
        lineIndex = 0x50;
        if (task->unk68 == 0) {
            lineIndex = 0x4F;
        }
        break;
    case 2:
        lineIndex = 0x4E;
        break;
    case 3:
        lineIndex = 0x51;
        break;
    case 4:
        lineIndex = 0x52;
        break;
    case 5:
        lineIndex = 0x30;
        break;
    }
    u32 msgIndex = 5;
    if (task->unk68 == 0) {
        msgIndex = 4;
    }
    task->m_msg->printIndex(msgIndex, lineIndex);
    task->m_stepWork.m_count = 0;
}

void IfSnapSaveTask::photoStepConfirmStartInit(IfSnapSaveTask* task) {
    task->mediaCursorStateInit(task->unk68 == 0 ? 6 : 1);
    int msgIndex = 2;
    if (task->unk68 == 0) {
        msgIndex = 1;
    }
    task->m_msg->printf(msgIndex, "");
    nw4r::g3d::ScnGroup* expand = task->m_expand;
    expand->Remove(task->m_objects[3]->m_sceneModel);
    expand->Remove(task->m_objects[4]->m_sceneModel);
}

s32 IfSnapSaveTask::photoStepConfirmStartMain(IfSnapSaveTask* task) {
    // MATCH-ONLY: a loop-shaped test reproduces the native block order.
    while (task->m_cursorState == 6 || task->m_cursorState == 1) {
        return task->m_step;
    }
    return 8;
}

void IfSnapSaveTask::photoStepConfirmInit(IfSnapSaveTask* task) {
    task->mediaCursorStateInit(task->unk68 == 0 ? 7 : 2);
    MuObject* obj = task->m_objects[2];
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    task->m_stepWork.m_flag = true;
    obj->setFrameNode(0.0f);
}

void IfSnapSaveTask::photoStepSavingInit(IfSnapSaveTask* task) {
    MuObject* obj;
    const IfSnapSaveSavingData* data = &s_savingTbl[task->unk68];
    task->mediaCursorStateInit(data->m_cursorState);
    obj = task->m_objects[5];
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    Vec3f pos(0.0f, -3.0f, 0.0f);
    obj->setTrans(&pos);
    obj->setFrame(0.0f);
    obj->m_modelAnim->setUpdateRate(1.0f);
    task->m_msg->printIndex(3, data->m_lineIndex);
    task->unk94 = 0x78;
    task->unk98 = 0;
    task->m_scanEnable = false;
    g_gfShutdownManager.unkD = false;
}

void IfSnapSaveTask::photoStepDoneInit(IfSnapSaveTask* task) {
    task->m_stepWork.m_count = 0;
    g_sndSystem->playSE((SndID)0x27, -1, 0, 0, -1);
    task->m_msg->printIndex(3, 0x4C);
}

void IfSnapSaveTask::photoStepWaitMediaInit(IfSnapSaveTask* task) {
    task->m_scanEnable = false;
}

s32 IfSnapSaveTask::photoStepWaitMediaMain(IfSnapSaveTask* task) {
    if (task->m_collection->m_busy == 0) {
        return 0;
    }
    return task->m_step;
}

s32 IfSnapSaveTask::photoStepDoneMain(IfSnapSaveTask* task) {
    if (++task->m_stepWork.m_count < 15) {
        return task->m_step;
    }
    gfPadStatus pad;
    pad = g_gfPadSystem->getSysPadStatus(0xF0);
    if ((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) || task->m_stepWork.m_count >= 60) {
        g_gfHomeMenu->setBan(0, 5);
        g_gfShutdownManager.unkD = true;
        task->m_msg->printf(3, "");
        task->m_result = 1;
        return 11;
    }
    return task->m_step;
}

s32 IfSnapSaveTask::photoStepErrorMain(IfSnapSaveTask* task) {
    if (++task->m_stepWork.m_count < 15) {
        return task->m_step;
    }
    gfPadStatus pad;
    pad = g_gfPadSystem->getSysPadStatus(0xF0);
    if ((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) || task->m_stepWork.m_count >= 420) {
        g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        u32 messageIndex = 5;
        if (task->unk68 == 0) {
            messageIndex = 4;
        }
        task->m_msg->printf(messageIndex, "");
        return 5;
    }
    return task->m_step;
}

void IfSnapSaveTask::initMsg(void* msgData, Heaps::HeapType heap, Heaps::HeapType heap2) {
    const u8* messageData;
    int messageIndex;
    u32 fontSetting;
    u32 fontType;
    if (g_HeapInfos[0x2F].m_name != NULL || g_HeapInfos[0x31].m_name != NULL) {
        fontSetting = 1;
        fontType = 0;
    } else {
        fontSetting = 8;
        fontType = 4;
    }
    m_msg = MuMsg::create(fontSetting, heap, heap2);
    m_msg->allocMsgBuf(0x200, 10);
    m_msg->setMsgData(msgData);
    for (messageIndex = 0, messageData = lbl_80408CD0; messageIndex < 9; messageIndex++, messageData += 3) {
        m_msg->attachScnMdlSimple(messageIndex, m_objects[messageData[0]]->m_sceneModel, messageData[1], 0.0625f);
        m_msg->getMessage()->setBufField34(messageIndex, 7);
        switch (messageData[2]) {
        case 1:
            m_msg->setFontColor(messageIndex, 255, 255, 255, 255);
            m_msg->setEdgeEnable(messageIndex, true);
            m_msg->setEdge(messageIndex, 0, 0, 0, 3.0f);
            break;
        case 2:
            m_msg->setFontColor(messageIndex, 128, 128, 128, 255);
            break;
        }
        m_msg->setFontType(messageIndex, fontType);
    }
    m_msg->getWScale(4, 48.0f);
    m_msg->getWScale(5, 48.0f);
    m_msg->getWScale(3, 48.0f);
    m_msg->printIndex(6, 0x47);
    m_msg->printIndex(7, 0x48);
    m_msg->printIndex(8, 0x49);
}

s32 IfSnapSaveTask::photoStepFadeMain(IfSnapSaveTask* task) {
    MuObject* object = task->m_objects[0];
    gfPadStatus pad;
    pad = g_gfPadSystem->getSysPadStatus(0xF0);
    if (object->m_modelAnim->m_anmObjChrRes->GetFrame() >= 10.0f || pad.m_buttonsPressedThisFrame.bits != 0) {
        object->setFrameNode(10.0f);
        object->setFrameMatCol(10.0f);
        object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        if (task->m_kind != 2) {
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        }
        bool scanned;
        for (int mediaIndex = 0; mediaIndex < 2; mediaIndex++) {
            if (task->m_media[mediaIndex].m_status == 0) {
                scanned = false;
                goto scanComplete;
            }
        }
        scanned = true;
    scanComplete:
        if (scanned) {
            if (task->unkD8) {
                task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, task->m_expand);
                return 7;
            }
            return 5;
        }
        return 4;
    }
    return task->m_step;
}

// TODO: clean up, give variables real names
namespace jj_800F27C8 {

struct Result { unsigned int width, height, size; };
extern "C" int fn_80260730(void*, void*, unsigned int, unsigned char, unsigned char, unsigned int, Result*);
extern "C" int fn_802608BC(void*, void*, unsigned int, void*, unsigned int, unsigned char, unsigned char, unsigned int);

extern "C" void* fn_800F27C8(void* self, void* data, unsigned int size) {
    unsigned char work[0x1c08];
    Result result;
    void* output;
    void* copy = gfHeapManager::alloc(*(Heaps::HeapType*)((char*)self + 0xd4), size, 0x20);
    memcpy(copy, data, size);
    fn_80260730(work, copy, size, 6, 4, 0, &result);
    output = gfHeapManager::alloc(*(Heaps::HeapType*)((char*)self + 0xd4), result.size, 0x20);
    fn_802608BC(work, copy, size, output, result.size, 6, 4, 0);
    gfHeapManager::free(copy);
    return output;
}
}

namespace jj_800F28F0 {
extern "C" { void setFrameNode__8MuObjectFf(void*, float); void setFrameMatCol__8MuObjectFf(void*, float); extern float lbl_805A25C0; extern float lbl_805A25D4; }
class PClass { public: virtual void m01(); virtual void m02(); virtual void m03(); virtual void m04(); virtual void m05(); virtual void m06(); virtual void m07(); virtual void m08(); virtual void m09(); virtual void m10(); virtual void m11(); virtual void m12(u32,u32); virtual void m13(); virtual void m14(u32); unsigned char pad[0xE0]; u32 unk_e4; };
struct Listener { virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3(); virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7(); virtual void setFrame(float); };
struct Resource { unsigned char pad[0x0C]; Listener* listener; void* l1; void* l1b; Listener* l2; };
struct Item { unsigned char pad[0x10]; u32 unk10; Resource* resource; };
struct Self { unsigned char pad[0x40]; PClass* p; Item* items[6]; void* extra; unsigned char pad2[0x68]; unsigned long long time; };
extern "C" void fn_800F28F0(Self* self) {
    Item* first;
    int i = 0;
    unsigned char* cursor = (unsigned char*)self;
    while (i < 6) {
        Item* item = *(Item**)(cursor + 0x44);
        if (item) self->p->m14(item->unk10);
        ++i;
        cursor += 4;
    }
    if (self->extra) self->p->m14((u32)self->extra);
    first = self->items[0];
    self->p->m12(self->p->unk_e4, first->unk10);
    setFrameNode__8MuObjectFf(first, lbl_805A25C0);
    setFrameMatCol__8MuObjectFf(first, lbl_805A25C0);
    first->resource->listener->setFrame(lbl_805A25D4);
    first->resource->l2->setFrame(lbl_805A25D4);
    self->time = OSGetTime();
}
}

// TODO: clean up, give variables real names
namespace jj_800F1AB0 {
struct FnObj {
    u8 pad00[0x2C];
    u8 unk2C;
    u8 pad2D[0x13];
    s32 unk40;
    u8 pad44[0x1C];
    s32 unk60;
    u8 pad64[4];
    s32 unk68;
    u8 pad6C[4];
    s32 unk70;
    u8 pad74[0x10];
    void* unk84;
    void* unk88;
    u8 pad8C[0x20];
    void* unkAC;
    u8 padB0[0xC];
    void* unkBC;
    u8 padC0[4];
    void* unkC4;
    u8 padC8[8];
    s32 unkD0;
    s32 unkD4;
    u8 unkD8;
};

struct GfThreadObj {
    void* vt;
    u8 pad4[0x340 - 4];
    s32 unk340;
};

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(u32 size, int heap);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUll(void* heap, int type, s32 align);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(void* heap, int type);
void* fn_800C35CC(void* self, u32 mode, s32* a, s32* b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i);
void fn_800F1DD4(void* self, void* file, s32 base, s32 c, s32 heap);
void fn_800F1ECC(void* self, s32 data, s32 arg2, s32 arg3);
void* fn_80152970(void* self, void* heap);
void createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(void* self, void* runnable, u32 a, u32 b, int heap);
extern u8 __vt__8gfThread[];
extern u8 __vt__16gfJpegEncodeWork[];
extern void* g_GameGlobal;
extern u8 lbl_805A25BC[4];
extern u8 lbl_80408C70[];

void fn_800F1AB0(FnObj* self, s32 a1, s32* a2, s32* a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 s9, s32 s10, u8 s11) {
    s32 sp1C;
    s32 sp18;
    u8 mode;
    void* obj;
    GfThreadObj* thread;
    void (*fn)(void*);

    self->unk2C = (u8)(self->unk2C & 0xFFFFFFFD);
    self->unkD0 = a1;
    fn_800F1DD4(self, a2, a6, a7, s9);
    fn_800F1ECC(self, a4, a7, s9);
    self->unk40 = a5;
    self->unk70 = 0;
    if (a1 == 2) {
        u8* p = *(u8**)((u8*)g_GameGlobal + 0x40);
        self->unkD8 = p[0x249C];
        self->unk68 = (*(s32*)(p + 0x24A0)) != 0;
    }
    self->unkD4 = s10;
    if (s10 == 0xFF) {
        self->unkD4 = s9;
    }
    mode = lbl_805A25BC[a1];
    obj = __nw__FUlQ25Heaps8HeapType(0x394, a7);
    if (obj != NULL) {
        sp1C = *a3;
        sp18 = *a2;
        obj = fn_800C35CC(obj, mode, &sp18, &sp1C, a4, a5, a6 + 5, a7, a7, 1, 0xF0);
    }
    self->unkAC = obj;
    if (s11 != 0) {
        self->unk60 = 1;
        self->unk84 = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUll((void*)self->unkD4, 0x12C000, 0x20);
        self->unk88 = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl((void*)self->unkD4, 0x19008);
        obj = __nw__FUlQ25Heaps8HeapType(0x14, a7);
        if (obj != NULL) {
            obj = fn_80152970(obj, (void*)self->unkD4);
        }
        self->unkBC = obj;
        thread = (GfThreadObj*)__nw__FUlQ25Heaps8HeapType(0x360, a7);
        if (thread != NULL) {
            thread->vt = __vt__8gfThread;
            createThread__8gfThreadFP10gfRunnableUlUlQ25Heaps8HeapType(thread, thread, 0x11, 0x2000, 0x3A);
            thread->vt = __vt__16gfJpegEncodeWork;
            thread->unk340 &= 0xFFFFFF;
        }
        self->unkC4 = thread;
    } else {
        self->unk60 = 3;
        self->unk84 = NULL;
    }
    fn = *(void (**)(void*))(lbl_80408C70 + (self->unk60 << 3));
    if (fn != NULL) {
        fn(self);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_800F2AEC {
extern "C" void __dt__8gfThreadFv(void*, int);
extern "C" void __dl__FPv(void*);
struct ThreadEx {
    char pad[0x340];
    union {
        struct { u32 m340a : 8; u32 m340b : 8; u32 m340c : 16; };
        struct { u16 m340; u16 m342; };
    };
    u16 m344;
    char pad346[2];
    u32 m348;
    u32 m34c;
    void* m350;
    char pad354[4];
    void* m358;
};
struct Self {
    char pad00[0x60];
    int result;
    int counter;
    char pad68[0x1C];
    void* buffer84;
    unsigned char* buffer88;
    char pad8c[0x10];
    char object9c[8];
    void* objecta4;
    char pada8[0x14];
    void* objectbc;
    int state;
    gfThread* thread;
    int c8;
    int cc;
    char padd0[4];
    int d4;
};
struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(const Vec2& other) { x = other.x; y = other.y; }
};
struct PadCopy {
    int words[6];
    Vec2 vectors[3];
    unsigned char b0, b1, b2, b3, b4, b5, b6, b7, b8;
    int last;
};
static inline gfPadStatus getRaw() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    return status;
}
static inline int getDirection() {
    PadCopy b(*(PadCopy*)&getRaw());
    if ((b.words[1] & 8) || ((signed char)b.b1 > 30)) return 1;
    if ((b.words[1] & 4) || ((signed char)b.b1 < -30)) return 2;
    if ((b.words[1] & 2) || ((signed char)b.b0 > 30)) return 3;
    if ((b.words[1] & 1) || ((signed char)b.b0 < -30)) return 4;
    return 0;
}
extern "C" void fn_80152A60(void*, const void*, unsigned int);
extern "C" void fn_80153610(void*, void*, int, int, int, int, int);
extern "C" void* fn_800F27C8(void*, const void*, unsigned int);
extern "C" int fn_800F2AEC(Self* self) {
    switch (self->state) {
    case 0:
        if (++self->counter == 2) {
            ThreadEx* t = (ThreadEx*)self->thread;
            t->m340b = self->d4;
            void* buffer = self->buffer84;
            char* data = (char*)self->buffer88 + 8;
            t = (ThreadEx*)self->thread;
            t->m342 = 0x280;
            t->m344 = 0x1E0;
            t->m348 = 6;
            t->m34c = 0x19000;
            t->m350 = buffer;
            t->m358 = data;
            OSResumeThread((OSThread*)((char*)self->thread + 8));
            self->state = 1;
        }
        break;
    case 1: {
        gfThread* t = self->thread;
        if (((s32)*(u32*)((char*)t + 0x340) >> 24) == 2) {
            self->state = 2;
            self->counter = 0;
            if (t != 0) {
                __dt__8gfThreadFv(t, 0);
                __dl__FPv(t);
            }
            self->thread = 0;
            gfHeapManager::free(self->buffer84);
            self->buffer84 =
                fn_800F27C8(self, (char*)self->buffer88 + 8, 0x19000);
            unsigned char* p = self->buffer88;
            p[0] = (((u8*)g_GameGlobal->getGlobalRecordMenuDatap())[0x28] >> 7) & 1;
            *(u32*)(p + 4) = 0x19000;
            fn_80152A60(self->objectbc, self->buffer88, 0x19008);
            fn_80153610(self->object9c, self->objectbc, self->c8, self->cc, self->d4, 0, 0);
            void* buffer = self->buffer84;
            fn_80153610(self->object9c, self->objectbc, self->c8, self->cc, self->d4, 0, 0);
            self->objecta4 = buffer;
        }
        break;
    }
    case 2: {
        PadCopy a(*(PadCopy*)&getRaw());
        int dir = getDirection();
        if (a.words[3] != 0 || dir != 0 || ++self->counter >= 0xB4)
            return 3;
        break;
    }
    }
    return self->result;
}
}

