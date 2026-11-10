#include <mu/mu_collection.h>
#include <mu/mu_recorder.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_sys_recorder2.h>
#include <gm/gm_global.h>
#include <ms/ms_message.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
#include <string.h>

void muRecorder::saveAdv(u32 slot) {
    m_slot = slot;
    m_request = Request_SaveAdv;
}

void muRecorder::loadAdv(u32 slot) {
    m_slot = slot;
    m_request = Request_LoadAdv;
}

inline muRecorder::Data::~Data() {
    reset(0);
}

void muRecorder::Data::reset(void* data) {
    if (unk0) gfHeapManager::free(unk0);
    unk0 = data;
}

// The name of the model of the object (the node of the model's name string, NULL when it has none).
static inline const char* getResMdlName(MuObject* object) {
    const nw4r::g3d::ResMdlData* data = object->m_resMdl.ptr();
    return data->m_toName != 0 ? reinterpret_cast<const char*>(data) + data->m_toName : 0;
}

muRecorder::muRecorder(int kind, nw4r::g3d::ResFile modelRes, nw4r::g3d::ResFile msgRes, void* msgData, void* scnRoot,
                       int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2, u8 wndUnk0, u8 wndUnk2,
                       const char* name)
    : gfTask(name, gfTask::Category(8), 15, 8, true),
      muRecorderBase(heap, heap2, msgRes, msgData, drawPriority, wndUnk0, wndUnk2, scnRoot),
      m_modelRes(modelRes), m_sysRecorder(0), m_sysRecorderUsed(false), m_recKind(kind == 1 ? 1 : 3), m_kind(kind), m_request(0) {
    unk2C_b1 = false;
    if (!m_modelRes.IsValid()) {
        m_obj.reset(0);
    } else {
        m_obj.reset(MuObject::create(&m_modelRes, "InfCmnWait_TopN", drawPriority + 1, 0, m_heap));
        m_obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        m_obj->changeNodeAnimN(getResMdlName(m_obj.get()));
        m_obj->changeTexSrtAnimN(getResMdlName(m_obj.get()));
    }
}

muRecorder::~muRecorder() {}

void muRecorder::check() {
    m_request = Request_Check;
}

// The locals of a subroutine of processDefault: they live on the stack of the script (see muScriptState).
// HYPOTHESIS: the first and the last word are guards; the last one is set to the first one before the frame is pushed.
struct muRecorderFrame3 {
    int m_head;
    int m_slot;
    int m_tail;
};

struct muRecorderFrame4 {
    int m_head;
    int m_slot;
    int m_result;
    int m_tail;
};

// A text that is put together for a notice window.
struct muRecorderText {
    char m_data[0x200];
    int m_size;
};

struct muRecorderSlice {
    char* m_data;
    u32 m_size;
};

// The text of the message with the number id of the message data. BYTES: the result is returned by value (the stack
// holds the local of this function and the returned structure).
static inline muRecorderSlice getText(void* msgData, u32 id) {
    muRecorderSlice slice;
    Message::getPrintIndexData(msgData, id, &slice.m_data, &slice.m_size);
    return slice;
}

static inline void appendText(muRecorderText& text, muRecorderSlice slice) {
    int size = text.m_size;
    memcpy(text.m_data + size, slice.m_data, slice.m_size);
    text.m_size = size + slice.m_size;
}

static inline void appendText(muRecorderText& text, const char* data, int size) {
    int used = text.m_size;
    memcpy(text.m_data + used, data, size);
    text.m_size = used + size;
}

// The text of the message for a number of blocks: the first part, the number and the last part.
// BYTES: the head is a by-value parameter and the tail a reference (one copy of each, as on the stack of the original),
// and the arguments are evaluated from the right (the tail is fetched first).
static inline void appendBlocksText(muRecorderText& text, muRecorderSlice head, int blocks, const muRecorderSlice& tail) {
    appendText(text, head.m_data, head.m_size);
    muRecorderText number;
    number.m_data[0] = 0;
    sprintf(number.m_data + strlen(number.m_data), "%d", blocks);
    appendText(text, number.m_data, strlen(number.m_data));
    appendText(text, tail.m_data, tail.m_size);
}

// BYTES: the position is set by a function that owns the vector (this puts it below the frames of the script).
static inline void setObjectTrans(std::auto_ptr<MuObject>& object, float x, float y, float z) {
    Vec3f trans(x, y, z);
    object->setTrans(&trans);
}

#define MU_WAIT_SYSREC(ret)                         \
    do {                                            \
        muRecorderFrame3 waitFrame = {-1, 0, 0};    \
        MU_CALL(muRecorderFrame3, waitFrame, State_WaitSysRec, ret); \
    } while (0)

#define MU_POP()                                    \
    do {                                            \
        m_script.m_sp--;                            \
        m_script.m_state = *m_script.m_sp;          \
    } while (0)

#define MU_PUSH_RET(callee, ret)                    \
    do {                                            \
        *m_script.m_sp++ = (ret);                   \
        m_script.m_state = (callee);                \
    } while (0)

#define MU_CALL(FrameType, frame, callee, ret)      \
    do {                                            \
        (frame).m_tail = (frame).m_head;            \
        *reinterpret_cast<FrameType*>(m_script.m_sp) = (frame); \
        m_script.m_sp += sizeof(FrameType) / 4;     \
        *m_script.m_sp++ = (ret);                   \
        m_script.m_state = (callee);                \
    } while (0)

void muRecorder::processDefault() {
    muRecorderText text;

    switch (m_script.m_state) {
    case State_Init:
        m_script.m_state = State_Begin;
        goto begin;

    case State_WaitWnd:
        if (m_wnd->m_step == 3) MU_POP();
        break;

    case State_WaitSysRec:
        if (m_sysRecorder->m_busy == 0) MU_POP();
        break;

    case State_Check:
        m_flag7 = false;
        m_flag6 = false;
        if (!gfSysRecorder2::isEnable(m_recKind)) goto error;
        if (m_sysRecorder->requestCheck(m_heap2, m_recKind)) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_CheckDone);
            break;
        case State_CheckDone:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        // BYTES: the pop of a result that is not 0 is placed in front of the cases of the switch.
        if (m_sysRecorder->m_result != 0) goto checkDonePop;
        switch (m_sysRecorder->m_checkResult) {
        checkDonePop:
            MU_POP();
            break;
        case 0:
        default:
            m_flag7 = true;
            MU_POP();
            break;
        case 1: {
            text.m_size = 0;
            switch (m_kind) {
            case 1:
            default: {
                appendText(text, getText(m_msgData, 0x27));
                break;
            }
            case 2:
            case 3:
            case 4: {
                appendText(text, getText(m_msgData, 0x28));
                break;
            }
            }
            appendText(text, getText(m_msgData, 0x2C));
            m_wnd = fn_800BF048(1, text.m_data, text.m_size, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2,
                                m_wndUnk0, 1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_RenewStart);
            break;
        }
        case 2:
            goto error;
        }
        break;

    case State_RenewStart:
        if (m_wnd->m_selected == 0) goto error;
        m_wnd = fn_800BEEB8(3, m_msgData, 0x26, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0, 1,
                            m_wndUnk2);
        if (m_obj.get()) {
            nw4r::g3d::ScnGroup* scnGroup = static_cast<nw4r::g3d::ScnGroup*>(m_scnRoot);
            scnGroup->Insert(scnGroup->Size(), m_obj->m_sceneModel);
            m_obj->setFrameNode(0.0f);
            m_obj->setFrameTexSrt(0.0f);
            setObjectTrans(m_obj, 0.0f, -7.0f, 0.0f);
        }
        if (m_sysRecorder->requestRenew(m_heap2, m_recKind)) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_RenewWait);
            break;
        case State_RenewWait:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        if (m_wnd) {
            m_wnd->setStep(2);
            MU_PUSH_RET(State_WaitWnd, State_RenewDone);
            break;
        case State_RenewDone:;
        }
        if (m_obj.get()) m_obj->m_scnMdl->DetachFromParent();
        {
            u8 flags = g_GameGlobal->getEnableSaveLoad();
            u8 bit = 1 << m_recKind;
            u8 enabled = flags & ~bit;
            if (m_sysRecorder->m_result == 0) enabled = flags | bit;
            g_GameGlobal->setEnableSaveLoad(enabled);
        }
        switch (m_sysRecorder->m_result) {
        case 3:
            MU_POP();
            break;
        case 0:
        default:
            m_flag7 = true;
            m_flag6 = true;
            MU_POP();
            break;
        case 1:
            m_wnd = fn_800BEEB8(0, m_msgData, 0x2D, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0,
                                1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_ErrorWnd);
            break;
        case 2: {
            gfSysRecorder2Require* require = m_sysRecorder->m_require;
            int blocks = 0;
            gfSysRecorder2Require* entry = require;
            int i = 0;
            while (entry->m_kind != 6) {
                if (m_recKind == entry->m_kind) {
                    blocks = require[i].m_unk8;
                    break;
                }
                entry++;
                i++;
            }
            muRecorderText message;
            message.m_size = 0;
            appendBlocksText(message, getText(m_msgData, 0x2E), blocks, getText(m_msgData, 0x2F));
            m_wnd = fn_800BF048(0, message.m_data, message.m_size, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap,
                                m_heap2, m_wndUnk0, 1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_ErrorWnd2);
            break;
        }
        }
        break;

    case State_ErrorWnd:
    case State_ErrorWnd2:
    error:
        m_wnd = fn_800BEEB8(0, m_msgData, 0x30, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0, 1,
                            m_wndUnk2);
        MU_PUSH_RET(State_WaitWnd, State_ErrorWndRet);
        break;

    case State_ErrorWndRet:
        MU_POP();
        break;

    case State_SaveAdv: {
        m_advData.reset(0);
        if (!gfSysRecorder2::isEnable(1)) {
            MU_POP();
            break;
        }
        void* buffer = m_sysRecorder->getBuffer(1);
        if (!buffer) {
            MU_POP();
            break;
        }
        g_GameGlobal->getAdventureSaveData(buffer);
        if (m_sysRecorder->saveAdventure(
                static_cast<u8>(reinterpret_cast<muRecorderFrame3*>(m_script.m_sp - 4)->m_slot))) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_SaveAdvDone);
            break;
        case State_SaveAdvDone:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        MU_POP();
        break;
    }

    case State_LoadAdv: {
        reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5)->m_result = 0;
        m_advData.reset(0);
        if (!gfSysRecorder2::isEnable(1)) {
            MU_POP();
            break;
        }
        if (m_sysRecorder->loadAdventure(m_heap2, static_cast<u8>(reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5)->m_slot))) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_LoadAdvDone);
            break;
        case State_LoadAdvDone:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        if (m_sysRecorder->m_result == 0) {
            m_advData.reset(m_sysRecorder->m_handle.getBuffer());
        } else {
            reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5)->m_result = 1;
        }
        MU_POP();
        break;
    }

    case State_Begin:
    begin:
        m_script.m_state = State_Idle;
        break;

    case State_Idle:
        if (m_request == 0) goto done;
        m_sysRecorder = gfSysRecorder2::createInstance(m_heap2);
        if (m_request == Request_Check) {
            MU_PUSH_RET(State_Check, State_CheckRet);
            break;
        case State_CheckRet:
            m_failed = m_flag7 ^ 1; // BYTES: xori of the extracted flag (the not operator compiles to cntlzw)
        } else if (m_request == Request_SaveAdv) {
            {
                muRecorderFrame3 frame = {-1, 0, 0};
                frame.m_slot = m_slot;
                MU_CALL(muRecorderFrame3, frame, State_SaveAdv, State_SaveAdvRet);
            }
            break;
        case State_SaveAdvRet:
            m_failed = 0;
            m_script.m_sp -= 3;
        } else if (m_request == Request_LoadAdv) {
            {
                muRecorderFrame4 frame = {-1, 0, 0, 0};
                frame.m_slot = m_slot;
                MU_CALL(muRecorderFrame4, frame, State_LoadAdv, State_LoadAdvRet);
            }
            break;
        case State_LoadAdvRet:
            m_script.m_sp -= 4;
            m_failed = reinterpret_cast<muRecorderFrame4*>(m_script.m_sp)->m_result;
        }
        if (m_sysRecorder) {
            m_sysRecorderUsed = false;
            gfSysRecorder2::dropInstance();
            m_sysRecorder = 0;
        }
    done:
        m_request = 0;
        goto begin;
    }
}
