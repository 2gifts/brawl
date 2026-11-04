#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <ms/ms_message.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <types.h>

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
