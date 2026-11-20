#include <sr/sr_common.h>
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

void muNoticeWndTask::setStep(int step) {
    m_step = step;
}

void muNoticeWndTask::deleteWnd() {
    unk44->Remove(unk40->m_sceneModel);
}

void muNoticeWndTask::dispSubMsg(const char* text) {
    printText(5, text, strlen(text), 1);
}

void muNoticeWndTask::setYesButtonLabel(void* msgData, int msgId) {
    if (unkFC == 1) {
        u32 size;
        int length;
        const char* text;
        if (msgData == NULL) {
            text = unk48->getMsgData(msgId, &length);
        } else {
            Message::getPrintIndexData(msgData, msgId, (char**)&text, &size);
            length = size;
        }
        printText(2, text, length, 2);
    }
}

void muNoticeWndTask::setNoButtonLabel(void* msgData, int msgId) {
    if (unkFC == 1) {
        u32 size;
        int length;
        const char* text;
        if (msgData == NULL) {
            text = unk48->getMsgData(msgId, &length);
        } else {
            Message::getPrintIndexData(msgData, msgId, (char**)&text, &size);
            length = size;
        }
        printText(3, text, length, 2);
    }
}

// TODO: clean up, give variables real names
namespace jj_800BFCE8 {
extern "C" {
void fn_800BFCE8(void *arg0) {
    *(int*)((char*)arg0 + 0x114) = 0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_800BFF34 {

struct Resource {
    char pad[0x3c];
    int nameOffset;

    const char* name() const {
        return nameOffset ? (const char*)this + nameOffset : 0;
    }
};

struct Anim {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void setFrame(float);
};

struct AnimSet {
    char pad[0x0c];
    Anim* node;
};

struct MuObject {
    char pad[8];
    Resource* resource;
    char pad0c[8];
    AnimSet* animations;
};

extern "C" {
extern char lbl_8059E0E8[6];
extern const float lbl_805A22B0;
extern const float lbl_805A22BC;
void changeNodeAnimN__8MuObjectFPCc(void* obj, const char* name);
void setFrameNode__8MuObjectFf(void* obj, float frame);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800BFF34(void* arg) {
    MuObject* obj = *(MuObject**)((char*)arg + 0x40);
    char buf[0x40];
    sprintf(buf, lbl_8059E0E8, obj->resource->name());
    changeNodeAnimN__8MuObjectFPCc(obj, buf);
    setFrameNode__8MuObjectFf(obj, lbl_805A22B0);
    obj->animations->node->setFrame(lbl_805A22BC);
}
}
}

// TODO: clean up, give variables real names
namespace jj_800BEEB8 {

extern "C" {
extern char lbl_805A22A8[8];
extern char lbl_8059E0D8;

void* __ct__15muNoticeWndTaskFv(void* self);
void fn_800B96E8(void* p, int v);
void createObjResFile__15muNoticeWndTaskFPCviPQ34nw4r3g3d7ResFileiQ25Heaps8HeapType(void* self, void* entries, int count, void* resFile, int prio, int heap);
void fn_800BF4BC(void* self, Heaps::HeapType a, int b, unsigned char c, unsigned char d);
void* fn_800B9640(void);
void* fn_800B9678(void);
unsigned long strlen(const char* s);
void printText__15muNoticeWndTaskFUlPCcii(void* self, int id, const char* str, int len, int flag);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_800BEEB8(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, Heaps::HeapType arg7,
                  int argSp8, unsigned char argSpC, unsigned char argSp10, unsigned char argSp14)
{
    char* s1;
    char* s2;
    unsigned long len1;
    unsigned long len2;
    char* self = (char*)operator new(0x12C, arg7);
    if (self != 0) {
        self = (char*)__ct__15muNoticeWndTaskFv(self);
    }
    *(int*)(self + 0xFC) = arg0;
    *(int*)(self + 0x4C) = arg1;
    *(int*)(self + 0x50) = arg2;
    *(int*)(self + 0x110) = arg6;
    *(int*)(self + 0x44) = arg4;
    fn_800B96E8(self + 0x54, argSp14);
    createObjResFile__15muNoticeWndTaskFPCviPQ34nw4r3g3d7ResFileiQ25Heaps8HeapType(self, lbl_805A22A8, 1, (void*)arg3, arg5, argSp8);
    fn_800BF4BC(self, arg7, argSp8, argSpC, argSp10);

    s1 = (char*)fn_800B9640();
    s2 = (char*)fn_800B9678();
    len1 = strlen(s1);
    len2 = strlen(s2);

    switch (*(int*)(self + 0xFC)) {
    case 1:
        printText__15muNoticeWndTaskFUlPCcii(self, 2, s1, (int)len1, 2);
        printText__15muNoticeWndTaskFUlPCcii(self, 3, s2, (int)len2, 2);
        break;
    case 2:
        printText__15muNoticeWndTaskFUlPCcii(self, 4, s1, (int)len1, 2);
        break;
    default:
        ((MuMsg*)*(void**)(self + 0x48))->printf(2, &lbl_8059E0D8);
        ((MuMsg*)*(void**)(self + 0x48))->printf(3, &lbl_8059E0D8);
        break;
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_800BF048 {

extern "C" {
extern char lbl_805A22A8[8];
extern char lbl_8059E0D8;

void* __ct__15muNoticeWndTaskFv(void* self);
void fn_800B96E8(void* p, int v);
void createObjResFile__15muNoticeWndTaskFPCviPQ34nw4r3g3d7ResFileiQ25Heaps8HeapType(void* self, void* entries, int count, void* resFile, int prio, int heap);
void fn_800BF4BC(void* self, Heaps::HeapType a, int b, unsigned char c, unsigned char d);
void* fn_800B9640(void);
void* fn_800B9678(void);
unsigned long strlen(const char* s);
void printText__15muNoticeWndTaskFUlPCcii(void* self, int id, const char* str, int len, int flag);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int heap, unsigned long size);
void* memcpy(void* d, const void* s, unsigned long n);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_800BF048(int arg0, void* arg1, unsigned long arg2, int arg3, int arg4, int arg5, int arg6,
                  Heaps::HeapType arg7, int argSp8, unsigned char argSpC, unsigned char argSp10,
                  unsigned char argSp14)
{
    char* s1;
    char* s2;
    unsigned long len1;
    unsigned long len2;
    char* self = (char*)operator new(0x12C, arg7);
    if (self != 0) {
        self = (char*)__ct__15muNoticeWndTaskFv(self);
    }
    *(int*)(self + 0xFC) = arg0;
    *(int*)(self + 0x4C) = 0;
    *(int*)(self + 0x50) = 0;
    if (arg1 != 0) {
        void* p = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(argSp8, arg2);
        *(void**)(self + 0x118) = p;
        memcpy(p, arg1, arg2);
        *(unsigned long*)(self + 0x11C) = arg2;
    }
    *(int*)(self + 0x110) = arg6;
    *(int*)(self + 0x44) = arg4;
    fn_800B96E8(self + 0x54, argSp14);
    createObjResFile__15muNoticeWndTaskFPCviPQ34nw4r3g3d7ResFileiQ25Heaps8HeapType(self, lbl_805A22A8, 1, (void*)arg3, arg5, argSp8);
    fn_800BF4BC(self, arg7, argSp8, argSpC, argSp10);

    s1 = (char*)fn_800B9640();
    s2 = (char*)fn_800B9678();
    len1 = strlen(s1);
    len2 = strlen(s2);

    switch (*(int*)(self + 0xFC)) {
    case 1:
        printText__15muNoticeWndTaskFUlPCcii(self, 2, s1, (int)len1, 2);
        printText__15muNoticeWndTaskFUlPCcii(self, 3, s2, (int)len2, 2);
        break;
    case 2:
        printText__15muNoticeWndTaskFUlPCcii(self, 4, s1, (int)len1, 2);
        break;
    default:
        ((MuMsg*)*(void**)(self + 0x48))->printf(2, &lbl_8059E0D8);
        ((MuMsg*)*(void**)(self + 0x48))->printf(3, &lbl_8059E0D8);
        break;
    }
    return self;
}
}
}

namespace jj_800BFCF4 {
extern "C" {
void fn_800B9F90(void* out, void* a, int flag);
int getButtonPushedControllerID__16muMenuControllerFUl(int arg0);
void setFrameMatCol__8MuObjectFf(void* obj, float frame);
void playSE__9sndSystemF5SndIDiiii(void* th, int sndID, int a1, int a2, int a3, int a4);
extern void* g_sndSystem;
extern const float lbl_805A22B0;
extern const float lbl_805A22BC;
}

struct PadStat {
    int w[16];
};

struct Self {
    char pad[0x40];
    void* f40;
    char pad44[0x10];
    int f54;
    char pad58[8];
    unsigned int f60;
    char pad64[0x98];
    int ffc;
    unsigned char byte100;
    char pad101[3];
    int f104;
    char pad108[8];
    int f110;
    int f114;
    char pad118[0xc];
    int f124;
};

// TODO: clean up, give variables real names
extern "C" int fn_800BFCF4(Self* self) {
    unsigned int flags60;
    PadStat status;
    PadStat result;
    fn_800B9F90(&result, &self->f54, 0);
    status = result;
    int cnt = self->f110;
    if (cnt < 0)
        return self->f104;
    int newv = self->f114 + 1;
    self->f114 = newv;
    if (newv < cnt)
        return self->f104;
    if (self->ffc == 1) {
        flags60 = self->f60;
        if ((flags60 & 1) && self->byte100 == 0) {
            float f = lbl_805A22B0;
            setFrameMatCol__8MuObjectFf(self->f40, f);
            self->byte100 = 1;
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
        }
        if ((flags60 & 2) && self->byte100 != 0) {
            float f = lbl_805A22BC;
            setFrameMatCol__8MuObjectFf(self->f40, f);
            self->byte100 = 0;
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
        }
    }
    unsigned int flags;
    int ffc = self->ffc;
    if (ffc == 3)
        return self->f104;
    flags = status.w[3];
    if ((flags & 0x1100) || (flags & 0x200)) {
        int sndid;
        unsigned char pad25;
        if (ffc == 1) {
            if (flags & 0x200) {
                pad25 = 0;
                sndid = 2;
            } else {
                pad25 = self->byte100;
                if (pad25 != 0) {
                    sndid = 0x13;
                } else {
                    sndid = 1;
                }
            }
        } else {
            pad25 = 0;
            sndid = ((flags >> 9) & 1) + 1;
        }
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, sndid, 0x10000, 0, 0, -1);
        self->byte100 = pad25;
        if (pad25 != 0) {
            int r = getButtonPushedControllerID__16muMenuControllerFUl(0x1300);
            self->f124 = r;
            if (r < 0)
                self->f124 = 0;
        }
        return 2;
    }
    return self->f104;
}
}

// TODO: clean up, give variables real names
namespace jj_800BF4BC {
struct Inner { char pad[0x10]; nw4r::g3d::ScnMdlSimple* model; };
struct Self { char pad[0x40]; Inner* inner; char pad2[4]; MuMsg* msg; void* data; };
extern "C" {
extern float lbl_805A22B4;
extern float lbl_805A22B8;
MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(unsigned long, HeapType, unsigned long);
void setFontType__5MuMsgFUlUl(MuMsg*, unsigned long, unsigned long);
void setBufField34__7MessageFii(void*, int, int);
void fn_800BF4BC(Self* self, HeapType heap, unsigned long heap2, int flag, int special) {
    Inner* p;
    MuMsg* msg;
    unsigned long type;
    unsigned long font;
    if (flag) { font = 8; type = 4; } else { font = 1; type = 0; }
    msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(font, heap, heap2);
    self->msg = msg;
    msg->allocMsgBuf(0x200, 6);
    msg->setMsgData(self->data);
    p = self->inner;
    msg->attachScnMdlSimple(0, p->model, 0, lbl_805A22B4);
    msg->getWScale(0, lbl_805A22B8);
    msg->setFontColor(0, 0, 0, 0, 255);
    setFontType__5MuMsgFUlUl(msg, 0, type);
    if (special) setBufField34__7MessageFii(*(void**)((char*)msg + 8), 0, 7);
    msg->attachScnMdlSimple(1, p->model, 1, lbl_805A22B4);
    msg->getWScale(1, lbl_805A22B8);
    msg->setFontColor(1, 0, 0, 0, 255);
    setFontType__5MuMsgFUlUl(msg, 1, type);
    if (special) setBufField34__7MessageFii(*(void**)((char*)msg + 8), 1, 7);
    msg->attachScnMdlSimple(2, p->model, 2, lbl_805A22B4);
    msg->setFontColor(2, 0, 0, 0, 255);
    setFontType__5MuMsgFUlUl(msg, 2, type);
    if (special) setBufField34__7MessageFii(*(void**)((char*)msg + 8), 2, 7);
    msg->attachScnMdlSimple(3, p->model, 3, lbl_805A22B4);
    msg->setFontColor(3, 0, 0, 0, 255);
    setFontType__5MuMsgFUlUl(msg, 3, type);
    if (special) setBufField34__7MessageFii(*(void**)((char*)msg + 8), 3, 7);
    msg->attachScnMdlSimple(4, p->model, 5, lbl_805A22B4);
    msg->setFontColor(4, 0, 0, 0, 255);
    setFontType__5MuMsgFUlUl(msg, 4, type);
    if (special) setBufField34__7MessageFii(*(void**)((char*)msg + 8), 4, 7);
    msg->attachScnMdlSimple(5, p->model, 4, lbl_805A22B4);
    msg->setFontColor(5, 0, 0, 0, 255);
    setFontType__5MuMsgFUlUl(msg, 5, type);
    if (special) setBufField34__7MessageFii(*(void**)((char*)msg + 8), 5, 7);
}
}
}

