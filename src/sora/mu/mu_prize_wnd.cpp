#include <gf/gf_pad_system.h>
#include <mu/mu_prize_wnd.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <gm/gm_common.h>

extern "C" int exchangeMuSelchkindToMuCharKind__6muMenuFiii(int selchKind);
extern "C" int getInfoMsgID__6muMenuFiii(int info);
extern const char lbl_8059E0FC[4];
extern const char lbl_8059E100[6];
extern const u8 lbl_804080E8[16];

extern struct {
    const char* unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
} lbl_804080C0[];

extern const struct {
    void (*unk0)(muPrizeWndTask*);
    int (*unk4)(muPrizeWndTask*);
} lbl_80407C28[];

extern const struct {
    int unk0;
    int unk4;
} lbl_80407C58[];

extern const char lbl_80456DD8[];
extern const char lbl_8059E0F8[4];

muPrizeWndTask::muPrizeWndTask()
    : gfTask(lbl_80456DD8, Category_Menu, 15, 8, true),
      unk58(NULL), unk5C(NULL), unk108(true), unk10C(5), unk110(6),
      unk114(0), unk118(0), unk21C(0), unk220(0), unk224(NULL), unk228(NULL),
      unk22C(0), unk230(0), unk2B4(false), unk2B8(NULL), unk2BC(NULL),
      unk2C0(NULL), unk344(NULL), unk348(NULL), unk3CC(-1), unk3D0(false) {
    unk2C_b1 = false;
    for (int i = 0; i < 5; i++) {
        unk40[i] = NULL;
    }
    unk54[0] = NULL;
    memset(unk11C, 0, sizeof(unk11C));
}

muPrizeWndTask7TexLoadFv::muPrizeWndTask7TexLoadFv() {}

muPrizeWndTask7TexLoadFv::~muPrizeWndTask7TexLoadFv() {}

void muPrizeWndTask::createObjResFile(const void* models, int count,
                                     nw4r::g3d::ResFile* resFile, int priority) {
    int num;
    int i;
    int j;
    const __typeof__(lbl_804080C0[0])* model =
        static_cast<const __typeof__(lbl_804080C0[0])*>(models);
    for (i = 0; i < count; i++, model++) {
        num = 1;
        if (model->unk4 < model->unk5) {
            num = model->unk5 - model->unk4;
        }
        for (j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, model->unk0,
                model->unk6 + priority, NULL, Heaps::MenuInstance);
            unk40[model->unk4 + j] = object;
            char animName[0x40];
            strcpy(animName, model->unk0);
            strcat(animName, lbl_8059E0F8);
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muPrizeWndTask::processDefault() {
    fn_800B9984(&unk60);
    if (unk110 != unk10C) {
        if (lbl_80407C28[unk10C].unk0 != NULL) {
            lbl_80407C28[unk10C].unk0(this);
        }
        unk110 = unk10C;
    }
    if (lbl_80407C28[unk10C].unk4 != NULL) {
        unk10C = lbl_80407C28[unk10C].unk4(this);
    }
}

bool muPrizeWndTask::appendPrizeList(const muPrizeWndEntry* entry) {
    if (unk21C >= 16) {
        return false;
    }
    prizeList[unk21C++] = *entry;
    return true;
}

int muPrizeWndTask::mainStepAppearMain(muPrizeWndTask* task) {
    if (task->unk40[0]->isNodeAnimFinished()) {
        return 3;
    }
    return task->unk10C;
}

void muPrizeWndTask::mainStepSelectingInit(muPrizeWndTask* task) {
    task->unk118 = 0;
    task->unk3D0 = false;
}

int muPrizeWndTask::mainStepSelectingMain(muPrizeWndTask* task) {
    bool isYes = true;
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    if (task->unk114 < 0) {
        return task->unk10C;
    }
    if (++task->unk118 < task->unk114) {
        return task->unk10C;
    }
    muPrizeWndEntry* entry = &task->prizeList[task->unk220];
    if (entry->unk0 == 2 && entry->unkC != 0) {
        bool old = task->unk108;
        bool selected = old;
        if (task->unk60.m_pressed & 1) {
            selected = true;
        }
        if (task->unk60.m_pressed & 2) {
            selected = false;
        }
        if (selected != old) {
            task->unk40[0]->setFrameMatCol(selected ? 1.0f : 2.0f);
            task->unk108 = selected;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
        isYes = !task->unk108;
    }
    if ((pad.m_buttonsPressedThisFrame.bits & 0x100) || (pad.m_buttonsPressedThisFrame.bits & 0x1000) || task->unk3D0) {
        if (isYes || task->unk3D0) {
            if (task->unk3CC != g_sndSystem->getBGMId()) {
                g_sndSystem->stopBGM(0);
            }
            return 4;
        }
        g_sndSystem->stopBGM(0);
        g_sndSystem->playBGM(entry->unk8, 0, true);
    }
    return task->unk10C;
}

void muPrizeWndTask::mainStepLeaveInit(muPrizeWndTask* task) {
    MuObject* object = task->unk40[0];
    char animName[0x40];
    const nw4r::g3d::ResMdlData& data = object->m_resMdl.ref();
    sprintf(animName, "%s__1", data.m_toName != 0 ? reinterpret_cast<const char*>(&data) + data.m_toName : NULL);
    object->changeNodeAnimN(animName);
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
}

void muPrizeWndTask::setBgmID(int index, muPrizeWndEntry* entry) {
    entry->unk4 = lbl_80407C58[index].unk4;
}

void muPrizeWndTask::openWnd(int index) {
    nw4r::g3d::ScnMdlExpand* group = unk54[0];
    muPrizeWndEntry& entry = prizeList[index];
    MuObject* window = unk40[0];
    int frameIndex;
    window->setFrameVisible(0.0f);
    window->setFrameMatCol(0.0f);
    group->Remove(unk40[1]->m_sceneModel);
    group->Remove(unk40[2]->m_sceneModel);
    group->Remove(unk40[3]->m_sceneModel);
    group->Remove(unk40[4]->m_sceneModel);
    unk5C->printf(0, lbl_8059E0FC);
    unk5C->printf(1, lbl_8059E0FC);
    unk5C->printf(2, lbl_8059E0FC);
    unk5C->printf(3, lbl_8059E0FC);
    switch (entry.unk0) {
    case 0:
        setWndFigurePrize(index);
        break;
    case 1:
        setWndSealPrize(index);
        break;
    case 2:
        setWndBGMPrize(index);
        break;
    case 3: {
        muPrizeWndTask7TexLoadFv& texture = unk34C[index];
        int characterKind;
        MuObject* character = unk40[4];
        characterKind = entry.unk8;
        nw4r::g3d::ScnMdlExpand* characterGroup = unk54[0];
        character->changeMaterialTex("Character_dammy_Mat", 0, &texture.unk4);
        character->changeMaterialTex("Character_dammy_shadow_Mat", 0, &texture.unk4);
        characterGroup->PushBack(character->m_sceneModel, lbl_8059E100);
        int menuKind = exchangeMuSelchkindToMuCharKind__6muMenuFiii(characterKind);
        int message = getInfoMsgID__6muMenuFiii(static_cast<u8>(gmRetIfKindMenukind(menuKind)));
        unk5C->printIndex(0, message, NULL);
        unk40[0]->setFrameVisible(6.0f);
        break;
    }
    case 4: {
        frameIndex = entry.unkC_word;
        unk5C->printIndex(0, entry.unk8, NULL);
        unk40[0]->setFrameVisible(lbl_804080E8[frameIndex]);
        break;
    }
    }
    if (entry.unk4 > 0) {
        g_sndSystem->stopBGM(0);
        g_sndSystem->playBGM(entry.unk4, 0, true);
    }
}
