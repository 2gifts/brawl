#include <StaticAssert.h>
#include <gf/gf_heap_manager.h>
#include <if/if_minigame_homerun.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the expand group; its storage after ScnGroup is not modeled.
class ScnMdlExpand : public ScnGroup {
public:
    static ScnMdlExpand* Construct(MEMAllocator* allocator, u32* size, int capacity, ScnMdl* base);
    bool PushBack(ScnObj* object, const char* nodeName);
};
}}

// HYPOTHESIS: one model of the HUD: the name, the object slots it fills (start, end) and the draw priority.
struct IfMinigameHomerunModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};
static_assert(sizeof(IfMinigameHomerunModel) == 0x8, "Class is the wrong size!");

// HYPOTHESIS: one expand group: the slots it fills (start, end), the object it is built on and its capacity.
struct IfMinigameHomerunExpand {
    u16 m_start;
    u16 m_end;
    u16 m_objectNo;
    u8 m_capacity;
};
static_assert(sizeof(IfMinigameHomerunExpand) == 0x8, "Class is the wrong size!");

// HYPOTHESIS: one distance window: the object whose frame shows the digit count, the expand group of the digits and
// the first digit object.
struct IfMinigameHomerunWnd {
    u8 m_objectNo;
    u8 m_expandNo;
    u8 m_digitNo;
};

static const IfMinigameHomerunModel sModels[] = {
    {"InfHmrn0000_TopN", 0, 0, 0x49},
    {"InfHmrn0001_TopN", 1, 8, 0x4A},
    {"InfHmrn0000_TopN", 8, 8, 0x49},
    {"InfHmrn0001_TopN", 9, 0x10, 0x4A},
    {"InfHmrn0000_TopN", 0x10, 0x10, 0x49},
    {"InfHmrn0001_TopN", 0x11, 0x18, 0x4A},
    {"InfHmrn0002_TopN", 0x18, 0x18, 0x51},
    {"InfHmrn0010_TopN", 0x19, 0x19, 0x48},
    {"InfHmrn0020_TopN", 0x1A, 0x1A, 0x40},
    {"InfHmrn0021_TopN", 0x1B, 0x1F, 0x40},
    {"InfHmrn0022_TopN", 0x1F, 0x3B, 0x41},
};

static const IfMinigameHomerunExpand sExpands[] = {
    {0, 0, 0, 7},
    {1, 1, 8, 7},
    {2, 2, 0x10, 7},
    {3, 3, 0x1A, 4},
    {4, 8, 0x1B, 7},
};

static const IfMinigameHomerunWnd sWnds[] = {
    {0, 0, 1},
    {8, 1, 9},
    {0x10, 2, 0x11},
};

static const IfMinigameHomerun::StepFunc sStepFuncs[] = {
    {IfMinigameHomerun::initStepGame, IfMinigameHomerun::mainStepGame},
    {IfMinigameHomerun::initStepResultAnim, IfMinigameHomerun::mainStepResultAnim},
    {IfMinigameHomerun::initStepVersusResultAnim, IfMinigameHomerun::mainStepVersusResultAnim},
    {NULL, NULL},
};

IfMinigameHomerun::IfMinigameHomerun() {
    init();
}

void IfMinigameHomerun::init() {
    m_scnGroup = NULL;
    for (int i = 0; i < 59; i++) {
        m_objects[i] = NULL;
    }
    for (int i = 0; i < 8; i++) {
        m_expands[i] = NULL;
    }
    m_step = 0;
    unk114 = false;
    unk115 = false;
    m_versusDistances[0] = 0.0f;
    m_versusDistances[1] = 0.0f;
    m_versusDistances[2] = 0.0f;
}

IfMinigameHomerun::~IfMinigameHomerun() {
    for (int i = 0; i < 8; i++) {
        if (m_expands[i] != NULL) {
            m_expands[i]->Destroy();
            m_expands[i] = NULL;
        }
    }
    for (int i = 0; i < 59; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
            m_objects[i] = NULL;
        }
    }
}

void IfMinigameHomerun::createModel(nw4r::g3d::ResFile* resFile) {
    createObjResFile(sModels, 11, resFile, 0);
    MEMAllocator* allocator;
    const IfMinigameHomerunExpand* entry = sExpands;
    int num;
    int j;
    int i;
    allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
    for (i = 0; i < 5; i++, entry++) {
        num = entry->m_end - entry->m_start;
        if (num <= 0) {
            num = 1;
        }
        for (j = 0; j < num; j++) {
            m_expands[entry->m_start + j] = nw4r::g3d::ScnMdlExpand::Construct(
                allocator, NULL, entry->m_capacity, m_objects[entry->m_objectNo + j]->m_sceneModel);
        }
    }
}

void IfMinigameHomerun::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* mdlSrc, s32 prio) {
    s32 i;
    s32 j;
    const IfMinigameHomerunModel* p = static_cast<const IfMinigameHomerunModel*>(nodeInfo);
    char nameBuf[0x40];
    for (i = 0; i < nodeCount; i++, p++) {
        s32 nCopies = (p->m_start < p->m_end) ? p->m_end - p->m_start : 1;
        for (j = 0; j < nCopies; j++) {
            MuObject* obj = MuObject::create(mdlSrc, p->m_name, p->m_priority + prio + j, 0, Heaps::InfoInstance);
            m_objects[p->m_start + j] = obj;
            strcpy(nameBuf, p->m_name);
            strcat(nameBuf, "__0");
            obj->changeAnimN(nameBuf);
            if (obj->m_modelAnim) {
                obj->m_modelAnim->setUpdateRate(0);
            }
        }
    }
}

void IfMinigameHomerun::startMelee(nw4r::g3d::ScnGroup* scnGroup) {
    m_scnGroup = scnGroup;
    scnGroup->Insert(scnGroup->sceneItemsCount, m_expands[3]);
    m_step = 0;
    // MATCH-ONLY: the original tests the table entry and calls through a separate pointer to the table.
    const StepFunc* func = sStepFuncs;
    if (sStepFuncs[0].m_init != NULL) {
        func->m_init(this);
    }
}

void IfMinigameHomerun::setDistance(float distance) {
    setDistanceWnd(distance, 0);
}

void IfMinigameHomerun::setDistanceWnd(float distance, int wnd) {
    int objectNo = sWnds[wnd].m_objectNo;
    int digits = setDistansModel(m_expands[sWnds[wnd].m_expandNo], sWnds[wnd].m_digitNo, distance);
    float frame;
    if (wnd != 2) {
        if (digits < 6) {
            frame = 0.0f;
        } else if (digits == 6) {
            frame = 1.0f;
        } else {
            frame = 2.0f;
        }
    } else {
        frame = 3.0f;
    }
    m_objects[objectNo]->setFrameVisible(frame);
}

void IfMinigameHomerun::dispHistory(int index, float distance, int color) {
    nw4r::g3d::ScnMdlExpand* group = m_expands[3];
    nw4r::g3d::ScnMdlExpand* expand = m_expands[index + 4];
    setDistansModel(expand, index * 7 + 0x1F, distance);
    m_objects[index + 0x1B]->setFrameMatCol(color);
    group->Remove(expand);
    char name[8];
    sprintf(name, "pos%02d", index);
    group->PushBack(expand, name);
}

int IfMinigameHomerun::setDistansModel(nw4r::g3d::ScnMdlExpand* expand, int objectNo, float distance) {
    u32 value = 10.0f * distance;
    value = value > 0 ? value : 0;
    value = value < 9999999 ? value : 9999999;
    int i;
    for (i = 0; i < 7; i++) {
        expand->Remove(m_objects[objectNo + i]->m_sceneModel);
    }
    char name[8];
    for (i = 0; i < 7 && (i < 2 || value != 0); i++) {
        MuObject* obj = m_objects[objectNo + i];
        obj->setFrameTex(value % 10);
        sprintf(name, "pos%02d", i);
        expand->PushBack(obj->m_sceneModel, name);
        value /= 10;
    }
    return i;
}

void IfMinigameHomerun::dispResult(bool arg) {
    unk114 = arg;
    m_step = 1;
    if (sStepFuncs[1].m_init != NULL) {
        sStepFuncs[1].m_init(this);
    }
}

void IfMinigameHomerun::dispVersusResult(float distance0, float distance1, float distance2) {
    m_versusDistances[0] = distance0;
    m_versusDistances[1] = distance1;
    m_versusDistances[2] = distance2;
    m_step = 2;
    if (sStepFuncs[2].m_init != NULL) {
        sStepFuncs[2].m_init(this);
    }
}

void IfMinigameHomerun::main() {
    MuObject* obj;
    int step = m_step;
    if (sStepFuncs[step].m_main != NULL) {
        int next = sStepFuncs[step].m_main(this);
        if (next != step) {
            m_step = next;
            if (sStepFuncs[next].m_init != NULL) {
                sStepFuncs[next].m_init(this);
            }
        }
    }
    if (unk115) {
        obj = m_objects[25];
        if (obj->isNodeAnimFinished()) {
            m_scnGroup->Remove(obj->m_sceneModel);
            unk115 = false;
        }
    }
}

void IfMinigameHomerun::dispStart(int arg1, int arg2) {
    MuObject* obj = m_objects[25];
    m_scnGroup->Remove(obj->m_sceneModel);
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    obj->setFrameVisible(arg1 + (arg2 - 1) * 2);
    obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    obj->setFrameNode(0.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    obj->setFrameMatCol(0.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    unk115 = true;
}

void IfMinigameHomerun::initStepGame(IfMinigameHomerun* homerun) {
    MuObject* obj = homerun->m_objects[0];
    homerun->m_scnGroup->Remove(homerun->m_expands[0]);
    homerun->m_scnGroup->Remove(homerun->m_expands[1]);
    homerun->m_scnGroup->Remove(homerun->m_expands[2]);
    homerun->m_scnGroup->Insert(homerun->m_scnGroup->sceneItemsCount, homerun->m_expands[0]);
    obj->changeAnimN(0, 1, false, false);
    homerun->m_scnGroup->Remove(homerun->m_objects[24]->m_sceneModel);
    homerun->setDistanceWnd(0.0f, 0);
}

int IfMinigameHomerun::mainStepGame(IfMinigameHomerun* homerun) {
    return homerun->m_step;
}

void IfMinigameHomerun::initStepResultAnim(IfMinigameHomerun* homerun) {
    MuObject* obj = homerun->m_objects[24];
    homerun->m_scnGroup->Remove(obj->m_sceneModel);
    if (homerun->unk114) {
        homerun->m_scnGroup->Insert(homerun->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
        obj->setFrame(0.0f);
        obj->m_modelAnim->setUpdateRate(1.0f);
    }
    obj = homerun->m_objects[0];
    obj->setFrameNode(0.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
}

int IfMinigameHomerun::mainStepResultAnim(IfMinigameHomerun* homerun) {
    if (homerun->m_objects[0]->isNodeAnimFinished()) {
        return 3;
    }
    return homerun->m_step;
}

void IfMinigameHomerun::initStepVersusResultAnim(IfMinigameHomerun* homerun) {
    homerun->setDistanceWnd(homerun->m_versusDistances[0], 0);
    homerun->m_objects[0]->changeAnimN(1, 1, true, false);
    homerun->m_scnGroup->Remove(homerun->m_expands[0]);
    homerun->m_scnGroup->Insert(homerun->m_scnGroup->sceneItemsCount, homerun->m_expands[0]);
    homerun->setDistanceWnd(homerun->m_versusDistances[1], 1);
    homerun->m_objects[8]->changeAnimN(2, 1, true, false);
    homerun->m_scnGroup->Remove(homerun->m_expands[1]);
    homerun->m_scnGroup->Insert(homerun->m_scnGroup->sceneItemsCount, homerun->m_expands[1]);
    homerun->setDistanceWnd(homerun->m_versusDistances[2], 2);
    homerun->m_objects[16]->changeAnimN(3, 1, true, false);
    homerun->m_scnGroup->Remove(homerun->m_expands[2]);
    homerun->m_scnGroup->Insert(homerun->m_scnGroup->sceneItemsCount, homerun->m_expands[2]);
    homerun->m_scnGroup->Remove(homerun->m_objects[24]->m_sceneModel);
}

int IfMinigameHomerun::mainStepVersusResultAnim(IfMinigameHomerun* homerun) {
    if (homerun->m_objects[0]->isNodeAnimFinished()) {
        return 3;
    }
    return homerun->m_step;
}
