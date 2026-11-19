#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <if/if_wifipr_player.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: the one HUD model this screen builds. start/end select the object slots (one model when start >= end) and the draw priority is added to the caller's priority.
struct IfWifiprPlayerModel {
    const char* m_suffix;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

IfWifiprPlayerModel lbl_805A2720 = {"State0000_TopN", 0, 0, 2};

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
MuObject* lbl_805A0308;
nw4r::g3d::ScnMdlExpand* lbl_805A030C;

void IfWifiprPlayer::createPlayerLocator(nw4r::g3d::ResFile* res) {
    const char* name = "InfPos_TopN";
    lbl_805A0308 = MuObject::create(res, name, 1, NULL, Heaps::InfoInstance);
    char animName[0x40];
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0308->changeAnimN(animName);
    lbl_805A0308->m_modelAnim->setUpdateRate(0.0f);
    float frame;
    if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        frame = 14.0f;
    } else {
        frame = 4.0f;
    }
    lbl_805A0308->setFrame(frame);
    lbl_805A030C = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 4,
                                                      lbl_805A0308->m_sceneModel);
}

void IfWifiprPlayer::destroyPlayerLocator() {
    if (lbl_805A030C != NULL) {
        lbl_805A030C->Destroy();
        lbl_805A030C = NULL;
    }
    if (lbl_805A0308 != NULL) {
        delete lbl_805A0308;
        lbl_805A0308 = NULL;
    }
}

void IfWifiprPlayer::startMelee(nw4r::g3d::ScnGroup* group) {
    group->Insert(group->sceneItemsCount, lbl_805A030C);
}

IfWifiprPlayer::IfWifiprPlayer(int playerNo, int mode) : m_heap(Heaps::InfoInstance) {
    m_object = NULL;
    m_state = 0;
    unk10 = 0;
    m_scnGroup = NULL;
    m_playerNo = playerNo;
    m_mode = mode;
    m_heap = mode == 0 ? Heaps::InfoInstance : Heaps::MenuInstance;
}

IfWifiprPlayer::~IfWifiprPlayer() {
    if (m_object != NULL) {
        delete m_object;
        m_object = NULL;
    }
}

static const char* s_names[] = {"WifiInf", "MenSelchr"};

static const IfWifiprPlayer::DispStateFunc s_dispState[] = {
    {IfWifiprPlayer::wifiDispStateNoneInit, NULL},
    {IfWifiprPlayer::wifiDispStateAcceptInit, IfWifiprPlayer::wifiDispStateAcceptMain},
    {IfWifiprPlayer::wifiDispStateSelCharInit, IfWifiprPlayer::wifiDispStateSelCharMain},
    {IfWifiprPlayer::wifiDispStateSelStageInit, IfWifiprPlayer::wifiDispStateSelStageMain},
    {IfWifiprPlayer::wifiDispStatePracticeInit, IfWifiprPlayer::wifiDispStatePracticeMain},
    {IfWifiprPlayer::wifiDispStateEscapeInit, IfWifiprPlayer::wifiDispStateEscapeMain},
    {IfWifiprPlayer::wifiDispStateMeleeInit, NULL},
};

void IfWifiprPlayer::setWifiStatePlayerColor(int color) {
    MuObject* object = m_object;
    object->setFrameMatCol(static_cast<float>(color));
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void IfWifiprPlayer::createModel(nw4r::g3d::ResFile* res, int priority) {
    m_object = NULL;
    IfWifiprPlayerModel* info = &lbl_805A2720;
    const char* name = s_names[m_mode];
    m_state = 0;
    unk10 = 0;
    m_scnGroup = NULL;
    u8 end = info->m_end;
    u8 start = info->m_start;
    int count = 1;
    if (start < end) {
        count = end - start;
    }
    char modelName[0x40];
    char animName[0x40];
    strcpy(modelName, name);
    strcat(modelName, info->m_suffix);
    strcpy(animName, modelName);
    strcat(animName, "__0");
    int i;
    for (i = 0; i < count; i++) {
        (&m_object)[info->m_start + i] = MuObject::create(res, modelName, info->m_priority + priority, NULL, m_heap);
        (&m_object)[info->m_start + i]->changeAnimN(animName);
    }
    setWifiStatePlayerColor(m_playerNo);
}

void IfWifiprPlayer::setWifiDispState(int state) {
    void (*init)(IfWifiprPlayer*) = s_dispState[state].m_init;
    if (init != NULL) {
        init(this);
    }
    m_state = state;
}

void IfWifiprPlayer::wifiDispStateNoneInit(IfWifiprPlayer* player) {
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
}

void IfWifiprPlayer::wifiDispStateAcceptInit(IfWifiprPlayer* player) {
    nw4r::g3d::ScnMdl* attached;
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
    attached = object->m_sceneModel;
    if (player->m_mode == 0) {
        char nodeName[8];
        sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
        lbl_805A030C->PushBack(attached, nodeName);
    } else {
        player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
    }
    object->changeAnimN(0, 3, true, false);
    object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    player->unk10 = 0;
}

int IfWifiprPlayer::wifiDispStateAcceptMain(IfWifiprPlayer* player) {
    if (player->unk10 == 0) {
        if (player->m_object->isNodeAnimFinished()) {
            nw4r::g3d::ScnMdl* attached;
            MuObject* object = player->m_object;
            nw4r::g3d::ScnMdl* model = object->m_sceneModel;
            if (player->m_mode == 0) {
                lbl_805A030C->Remove(model);
            } else {
                player->m_scnGroup->Remove(model);
            }
            attached = object->m_sceneModel;
            if (player->m_mode == 0) {
                char nodeName[8];
                sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
                lbl_805A030C->PushBack(attached, nodeName);
            } else {
                player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
            }
            object->changeAnimN(1, 3, true, false);
            object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
            object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            player->unk10 = 1;
        }
    }
    return player->m_state;
}

void IfWifiprPlayer::wifiDispStateSelCharInit(IfWifiprPlayer* player) {
    nw4r::g3d::ScnMdl* attached;
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
    attached = object->m_sceneModel;
    if (player->m_mode == 0) {
        char nodeName[8];
        sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
        lbl_805A030C->PushBack(attached, nodeName);
    } else {
        player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
    }
    object->changeAnimN(2, 3, true, false);
    object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    player->unk10 = 2;
}

void IfWifiprPlayer::wifiDispStateSelStageInit(IfWifiprPlayer* player) {
    nw4r::g3d::ScnMdl* attached;
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
    attached = object->m_sceneModel;
    if (player->m_mode == 0) {
        char nodeName[8];
        sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
        lbl_805A030C->PushBack(attached, nodeName);
    } else {
        player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
    }
    object->changeAnimN(5, 3, true, false);
    object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    player->unk10 = 5;
}

int IfWifiprPlayer::wifiDispStateSelStageMain(IfWifiprPlayer* player) {
    if (player->unk10 == 5) {
        if (player->m_object->isNodeAnimFinished()) {
            nw4r::g3d::ScnMdl* attached;
            MuObject* object = player->m_object;
            nw4r::g3d::ScnMdl* model = object->m_sceneModel;
            if (player->m_mode == 0) {
                lbl_805A030C->Remove(model);
            } else {
                player->m_scnGroup->Remove(model);
            }
            attached = object->m_sceneModel;
            if (player->m_mode == 0) {
                char nodeName[8];
                sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
                lbl_805A030C->PushBack(attached, nodeName);
            } else {
                player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
            }
            object->changeAnimN(6, 3, true, false);
            object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
            object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            player->unk10 = 6;
        }
    }
    return player->m_state;
}

void IfWifiprPlayer::wifiDispStatePracticeInit(IfWifiprPlayer* player) {
    nw4r::g3d::ScnMdl* attached;
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
    attached = object->m_sceneModel;
    if (player->m_mode == 0) {
        char nodeName[8];
        sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
        lbl_805A030C->PushBack(attached, nodeName);
    } else {
        player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
    }
    object->changeAnimN(7, 3, true, false);
    object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    player->unk10 = 7;
}

int IfWifiprPlayer::wifiDispStatePracticeMain(IfWifiprPlayer* player) {
    if (player->unk10 == 7) {
        if (player->m_object->isNodeAnimFinished()) {
            nw4r::g3d::ScnMdl* attached;
            MuObject* object = player->m_object;
            nw4r::g3d::ScnMdl* model = object->m_sceneModel;
            if (player->m_mode == 0) {
                lbl_805A030C->Remove(model);
            } else {
                player->m_scnGroup->Remove(model);
            }
            attached = object->m_sceneModel;
            if (player->m_mode == 0) {
                char nodeName[8];
                sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
                lbl_805A030C->PushBack(attached, nodeName);
            } else {
                player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
            }
            object->changeAnimN(8, 3, true, false);
            object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
            object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            player->unk10 = 8;
        }
    }
    return player->m_state;
}

void IfWifiprPlayer::wifiDispStateEscapeInit(IfWifiprPlayer* player) {
    nw4r::g3d::ScnMdl* attached;
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
    attached = object->m_sceneModel;
    if (player->m_mode == 0) {
        char nodeName[8];
        sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
        lbl_805A030C->PushBack(attached, nodeName);
    } else {
        player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
    }
    object->changeAnimN(9, 3, true, false);
    object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    player->unk10 = 9;
}

int IfWifiprPlayer::wifiDispStateEscapeMain(IfWifiprPlayer* player) {
    if (player->unk10 == 9) {
        if (player->m_object->isNodeAnimFinished()) {
            return 1;
        }
    }
    return player->m_state;
}

void IfWifiprPlayer::wifiDispStateMeleeInit(IfWifiprPlayer* player) {
    nw4r::g3d::ScnMdl* attached;
    MuObject* object = player->m_object;
    nw4r::g3d::ScnMdl* model = object->m_sceneModel;
    if (player->m_mode == 0) {
        lbl_805A030C->Remove(model);
    } else {
        player->m_scnGroup->Remove(model);
    }
    attached = object->m_sceneModel;
    if (player->m_mode == 0) {
        char nodeName[8];
        sprintf(nodeName, "pos%02d", player->m_playerNo + 1);
        lbl_805A030C->PushBack(attached, nodeName);
    } else {
        player->m_scnGroup->Insert(player->m_scnGroup->sceneItemsCount, attached);
    }
    object->changeAnimN(10, 3, true, false);
    object->setFrameTex(1.0f + static_cast<float>(static_cast<int>(player->m_playerNo)));
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    player->unk10 = 10;
}

void IfWifiprPlayer::main() {
    int oldState = m_state;
    int (*mainFunc)(IfWifiprPlayer*) = s_dispState[oldState].m_main;
    if (mainFunc != NULL) {
        m_state = mainFunc(this);
    }
    if (m_state != oldState) {
        void (*init)(IfWifiprPlayer*) = s_dispState[m_state].m_init;
        if (init != NULL) {
            init(this);
        }
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_800FD648(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x14) = value;
}

// TODO: clean up, give variables real names
namespace jj_800FD8E0 {
struct Entry {
    void (*f0)(void*);
    int (*f4)(void*);
};

extern Entry lbl_804090F0[];

struct Self {
    char pad[8];
    int state;
};

// TODO: clean up, give variables real names
extern "C" void fn_800FD8E0(Self* self) {
    int old = self->state;
    if (lbl_804090F0[old].f4 != 0) {
        self->state = lbl_804090F0[old].f4(self);
    }
    if (self->state != old) {
        if (lbl_804090F0[self->state].f0 != 0) {
            lbl_804090F0[self->state].f0(self);
        }
    }
}
}

namespace jj_800FD698 {
extern "C" char lbl_8045C768[];

struct Child {
    u8 pad[0x5C];
    virtual ~Child();
};

struct Obj {
    u8 pad0[0xC];
    Child* m_c[1];
    u8 pad10[0xC];
    void* m_1c;
};

// TODO: clean up, give variables real names
extern "C" Obj* fn_800FD698(Obj* p, s32 flags) {
    if (p != nullptr) {
        p->m_1c = lbl_8045C768;
        for (u32 i = 0; i < 1; i++) {
            if (p->m_c[i]) {
                delete p->m_c[i];
                p->m_c[i] = nullptr;
            }
        }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_800FDDD4 {

extern "C" {
int sprintf(char*, const char*, ...);
int fn_800B61C0(MuObject* obj);
void fn_800B5854(MuObject* obj, int a, int b, int c, int d);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
}

extern char lbl_8059E518[8];
extern nw4r::g3d::ScnMdl* lbl_805A030C;
extern float lbl_805A2728;
extern float lbl_805A2740;

class VCall {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8(float f);
    virtual void vf9();
    virtual void vf10(void* a, void* b);
    virtual void vf11(void* a, void* b);
    virtual void vf12();
    virtual void vf13(void* a);
    char pad4[0xE0];
    void* unkE4;
};

struct SelfObj {
    s32 unk0;
    u8 unk4;
    s32 unk8;
    MuObject* unkC;
    s32 unk10;
    VCall* unk14;
};

// TODO: clean up, give variables real names
extern "C" int fn_800FDDD4(SelfObj* self) {
    switch (self->unk10) {
    case 2: {
        char sp10[8];
        if (fn_800B61C0(self->unkC)) {
            MuObject* obj = self->unkC;
            void* r30 = obj->m_sceneModel;
            if (self->unk0 == 0) {
                ((VCall*)lbl_805A030C)->vf13(r30);
            } else {
                self->unk14->vf13(r30);
            }
            r30 = obj->m_sceneModel;
            if (self->unk0 == 0) {
                sprintf(sp10, lbl_8059E518, self->unk4 + 1);
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(lbl_805A030C, r30, sp10);
            } else {
                self->unk14->vf11(self->unk14->unkE4, r30);
            }
            fn_800B5854(obj, 3, 3, 1, 0);
            obj->setFrameTex(lbl_805A2740 + (float)(s32)self->unk4);
            ((VCall*)obj->m_modelAnim->m_anmObjTexPatRes)->vf8(lbl_805A2728);
            self->unk10 = 3;
        }
        break;
    }
    case 3: {
        char sp8[8];
        if (fn_800B61C0(self->unkC)) {
            MuObject* obj = self->unkC;
            void* r30 = obj->m_sceneModel;
            if (self->unk0 == 0) {
                ((VCall*)lbl_805A030C)->vf13(r30);
            } else {
                self->unk14->vf13(r30);
            }
            r30 = obj->m_sceneModel;
            if (self->unk0 == 0) {
                sprintf(sp8, lbl_8059E518, self->unk4 + 1);
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(lbl_805A030C, r30, sp8);
            } else {
                self->unk14->vf11(self->unk14->unkE4, r30);
            }
            fn_800B5854(obj, 4, 3, 1, 0);
            obj->setFrameTex(lbl_805A2740 + (float)(s32)self->unk4);
            ((VCall*)obj->m_modelAnim->m_anmObjTexPatRes)->vf8(lbl_805A2728);
            self->unk10 = 4;
        }
        break;
    }
    }
    return self->unk8;
}
}

