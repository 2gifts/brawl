#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <ms/ms_message.h>
#include <mu/mu_msg.h>
#include <string.h>
#include <types.h>

class BaseItem;
class itArchive;
class gfArchive;
class MuMsg;
class MuObject;

// The speech window of Mr. Resetti (assist trophy): his lines with voice playback.
class IfResetsanTask : public gfTask {
public:
    // A model of the window: name, slot range and priority offset.
    struct ModelDesc {
        const char* m_name;
        u8 m_start;
        u8 m_end;
        u8 m_priority;
    };
    // A text line of the window: model, bone, color style, alignment and direction.
    struct MsgDesc {
        u8 m_objIdx;
        u8 m_boneIdx;
        u8 m_colorType;
        int m_align;
        int m_vertical;
    };

    BaseItem* m_item;               // 0x40
    gfArchive* m_archive;           // 0x44
    nw4r::g3d::ResFile m_resFile;   // 0x48
    int m_state;                    // 0x4C
    nw4r::g3d::ScnGroup* m_group;   // 0x50 the scene group of the window (registered with IfMngr)
    MuObject* m_objects[1];         // 0x54
    nw4r::g3d::G3dObj* m_sceneObjects[1]; // 0x58
    MuMsg* m_msg;                   // 0x5C
    int unk60;                      // 0x60
    u8 unk64;                       // 0x64
    float unk68;                    // 0x68
    float unk6C;                    // 0x6C
    char m_text[0x100];             // 0x70 the line being composed (tags, texts and names)
    int m_textLen;                  // 0x170
    int m_nameKind1;                // 0x174 HYPOTHESIS: -1 when the name is not set
    int m_nameKind2;                // 0x178
    const char* m_nameStr1;         // 0x17C a full-width name
    const char* m_nameStr2;         // 0x180
    const wchar_t* m_nameWStr1;     // 0x184 a UTF-16 name (wins when not empty)
    const wchar_t* m_nameWStr2;     // 0x188
    gfTask* m_voiceTask;            // 0x18C sndDoubutsugoTask
    void* m_pageFrame;              // 0x190
    u32 unk194;                     // 0x194

    static IfResetsanTask* create(BaseItem* item, itArchive* archive, int priority);
    IfResetsanTask(BaseItem* item, gfArchive* archive, void* resData, void* pageFrame);
    virtual ~IfResetsanTask();
    void createModel(nw4r::g3d::ResFile* resFile, int priority);
    void initMsg(void* msgData);
    void destroyModel();
    virtual void processDefault();
    void setMessage(int msgId, float delay1, float delay3, float delay4, float delay2, float wait, float posY);

    inline void appendText(u32 msgIndex) {
        int len;
        const char* text = m_msg->getMsgData(msgIndex, &len);
        memcpy(&m_text[m_textLen], text, len);
        m_textLen += len;
    }
    inline void appendName(char* text, int kind, const char* str, const wchar_t* wstr) {
        if (wstr != NULL && wstr[0] != 0) {
            char buf[0x18];
            Message::utf16to8(buf, wstr);
            int len = strlen(buf);
            memcpy(&text[m_textLen], buf, len);
            m_textLen += len;
        } else if (kind == -1 || str == NULL) {
            appendText(0);
        } else {
            m_textLen += strlen(Message::fullToHalf(&text[m_textLen], str));
        }
    }
};
static_assert(sizeof(IfResetsanTask) == 0x198, "Class is wrong size!");
