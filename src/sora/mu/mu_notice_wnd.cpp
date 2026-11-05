#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <ms/ms_message.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <types.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>

extern const char lbl_80456D30[12];
extern const char lbl_8059E0DC[4];
extern const char lbl_8059E0E0[6];
extern const u8 lbl_805A22C8[4][2];

muNoticeWndTask::muNoticeWndTask()
    : gfTask(lbl_80456D30, Category_Menu, 15, 8, true),
      unk44(NULL), unk48(NULL), unk4C(NULL), unk50(0),
      unkFC(0), m_selected(1), m_step(0), unk108(4), unk10C(0),
      unk110(0), unk114(0), unk118(NULL), unk11C(0), m_unk120(0),
      unk124(0), unk128(5) {
    unk2C_b1 = false;
    unk40 = NULL;
}

void muNoticeWndTask::createObjResFile(const void* models, int count,
                                     nw4r::g3d::ResFile* resFile, int priority, HeapType heap) {
    int copies;
    int modelIndex;
    const struct {
        const char* unk0[1];
        u8 unk4;
        u8 unk5;
        u8 unk6;
        u8 unk7;
    }* model = static_cast<const __typeof__(*model)*>(models);
    int copyIndex;
    for (modelIndex = 0; modelIndex < count; modelIndex++, model++) {
        copies = 1;
        if (model->unk4 < model->unk5) copies = model->unk5 - model->unk4;
        for (unk10C = 0; unk10C < 1; unk10C++) {
            if (resFile->GetResMdl(model->unk0[unk10C]).IsValid()) break;
        }
        MuObject* object;
        const char* modelName = model->unk0[unk10C];
        for (copyIndex = 0; copyIndex < copies; copyIndex++) {
            object = MuObject::create(resFile, modelName, model->unk6 + priority, NULL, heap);
            (&unk40)[model->unk4 + copyIndex] = object;
            char animName[0x40];
            strcpy(animName, modelName);
            strcat(animName, lbl_8059E0DC);
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) object->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

void muNoticeWndTask::mainStepAppearInit(muNoticeWndTask* task) {
    u32 messageIndex;
    const char* message;
    int messageLength;
    u8 selected = task->m_unk120;
    messageLength = task->unk11C;
    message = task->unk118;
    messageIndex = task->unk50;
    MuObject* object = task->unk40;
    object->setFrameVisible(lbl_805A22C8[task->unkFC][0]);
    u32 line = lbl_805A22C8[task->unkFC][1];
    if (message == NULL) {
        int length;
        const char* text = task->unk48->getMsgData(messageIndex, &length);
        task->printText(line, text, length, 0);
    } else {
        task->printText(line, message, messageLength, 0);
    }
    task->unk40->setFrameMatCol(selected ? 0.0f : 1.0f);
    task->m_selected = selected;
    char animName[0x40];
    const nw4r::g3d::ResMdlData& data = task->unk40->m_resMdl.ref();
    const void* modelData = &data;
    sprintf(animName, lbl_8059E0E0, data.m_toName ? static_cast<const char*>(modelData) + data.m_toName : NULL);
    object->changeNodeAnimN(animName);
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    task->unk44->Insert(task->unk44->sceneItemsCount, task->unk40->m_sceneModel);
    g_sndSystem->playSE(static_cast<SndID>(task->unk128), -1, 0, 0, -1);
}

extern const struct {
    void (*unk0)(muNoticeWndTask*);
    int (*unk4)(muNoticeWndTask*);
} lbl_80407C08[];

muNoticeWndTask::~muNoticeWndTask() {
    if (unk118 != NULL) gfHeapManager::free(unk118);
    if (unk48 != NULL) delete unk48;
    if (unk40 != NULL) delete unk40;
}

void muNoticeWndTask::printText(u32 index, const char* text, int length, int kind) {
    int count;
    u8 buffer[0x400];
    u8 color;
    if (g_GameGlobal->getLanguage() == 0) {
        color = kind == 2 ? 0 : 0x53;
    } else {
        color = kind == 2 ? 0 : 0x20;
    }
    count = Message::stripTags((const u8*)text, length, 0xFFFF, buffer);
    unk48->setFontColor(index, color, color, color, 0xFF);
    unk48->printData(index, buffer, count);
}

void muNoticeWndTask::setDoneButtonLabel(void* msgData, u32 index) {
    if (unkFC == 2) {
        u32 size;
        int length;
        const char* text;
        if (msgData == NULL) {
            text = unk48->getMsgData(index, &length);
        } else {
            Message::getPrintIndexData(msgData, index, (char**)&text, &size);
            length = size;
        }
        printText(4, text, length, 2);
    }
}

int muNoticeWndTask::mainStepAppearMain(muNoticeWndTask* task) {
    if (task->unk40->isNodeAnimFinished()) return 1;
    return task->m_step;
}

void muNoticeWndTask::processDefault() {
    fn_800B9984(&unk54);
    if (unk108 != m_step) {
        if (lbl_80407C08[m_step].unk0 != NULL) {
            lbl_80407C08[m_step].unk0(this);
        }
        unk108 = m_step;
    }
    if (lbl_80407C08[m_step].unk4 != NULL) {
        m_step = lbl_80407C08[m_step].unk4(this);
    }
}

int muNoticeWndTask::mainStepLeaveMain(muNoticeWndTask* task) {
    if (task->unk40->isNodeAnimFinished()) {
        task->unk44->Remove(task->unk40->m_sceneModel);
        return 3;
    }
    return task->m_step;
}
