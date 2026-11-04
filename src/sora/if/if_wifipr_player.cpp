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
