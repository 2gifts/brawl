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

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

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

muRecorder::~muRecorder() {
}

void muRecorder::check() {
    m_request = Request_Check;
}

// The locals of a subroutine of processDefault: they live on the stack of the script (see muScriptState).
struct muRecorderFrame3 {
    int unk0;
    int m_slot;
    int unk8;
};

struct muRecorderFrame4 {
    int unk0;
    int m_slot;
    int m_result;
    int unkC;
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

static inline void appendText(muRecorderText& text, muRecorderSlice slice) {
    memcpy(text.m_data + text.m_size, slice.m_data, slice.m_size);
    text.m_size += slice.m_size;
}

static inline void appendText(muRecorderText& text, const char* data, int size) {
    memcpy(text.m_data + text.m_size, data, size);
    text.m_size += size;
}

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
        FrameType local = (frame);                  \
        *reinterpret_cast<FrameType*>(m_script.m_sp) = local; \
        m_script.m_sp += sizeof(FrameType) / 4;     \
        *m_script.m_sp++ = (ret);                   \
        m_script.m_state = (callee);                \
    } while (0)

void muRecorder::processDefault() {
    muRecorderText text;
    nw4r::g3d::ScnGroup* scnGroup = static_cast<nw4r::g3d::ScnGroup*>(m_scnRoot);

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
            MU_CALL(muRecorderFrame3, muRecorderFrame3(), State_WaitSysRec, State_CheckDone);
        } else {
            MU_POP();
        }
        break;

    case State_CheckDone:
        m_script.m_sp -= 3;
        if (m_sysRecorder->m_result != 0) {
            MU_POP();
            break;
        }
        switch (m_sysRecorder->m_checkResult) {
        case 1: {
            text.m_size = 0;
            muRecorderSlice slice;
            if (m_kind <= 1 || m_kind >= 5) {
                Message::getPrintIndexData(m_msgData, 0x27, &slice.m_data, &slice.m_size);
            } else {
                Message::getPrintIndexData(m_msgData, 0x28, &slice.m_data, &slice.m_size);
            }
            appendText(text, slice);
            Message::getPrintIndexData(m_msgData, 0x2C, &slice.m_data, &slice.m_size);
            appendText(text, slice);
            m_wnd = fn_800BF048(1, text.m_data, text.m_size, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2,
                                m_wndUnk0, 1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_RenewStart);
            break;
        }
        case 2:
            goto error;
        default:
            m_flag7 = true;
            MU_POP();
            break;
        }
        break;

    case State_RenewStart:
        if (m_wnd->m_selected == 0) goto error;
        m_wnd = fn_800BEEB8(3, m_msgData, 0x26, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0, 1,
                            m_wndUnk2);
        if (m_obj.get()) {
            scnGroup->Insert(scnGroup->Size(), m_obj->m_sceneModel);
            m_obj->setFrameNode(0.0f);
            m_obj->setFrameTexSrt(0.0f);
            Vec3f trans(0.0f, -7.0f, 0.0f);
            m_obj->setTrans(&trans);
        }
        if (m_sysRecorder->requestRenew(m_heap2, m_recKind)) {
            m_sysRecorderUsed = true;
            MU_CALL(muRecorderFrame3, muRecorderFrame3(), State_WaitSysRec, State_RenewWait);
        } else {
            MU_POP();
        }
        break;

    case State_RenewWait:
        m_script.m_sp -= 3;
        if (m_wnd) {
            m_wnd->setStep(2);
            MU_PUSH_RET(State_WaitWnd, State_RenewDone);
            break;
        }
        // fall through
    case State_RenewDone:
        if (m_obj.get()) m_obj->m_scnMdl->DetachFromParent();
        {
            u8 flags = g_GameGlobal->getEnableSaveLoad();
            u8 bit = 1 << m_recKind;
            if (m_sysRecorder->m_result == 0) {
                flags |= bit;
            } else {
                flags &= ~bit;
            }
            g_GameGlobal->setEnableSaveLoad(flags);
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
            int blocks = 0;
            for (int i = 0; m_sysRecorder->m_require[i].m_kind != 6; i++) {
                if (m_recKind == m_sysRecorder->m_require[i].m_kind) {
                    blocks = m_sysRecorder->m_require[i].m_unk8;
                    break;
                }
            }
            muRecorderText message;
            message.m_size = 0;
            muRecorderSlice slice1;
            muRecorderSlice slice2;
            Message::getPrintIndexData(m_msgData, 0x2F, &slice1.m_data, &slice1.m_size);
            muRecorderSlice tail = slice1;
            Message::getPrintIndexData(m_msgData, 0x2E, &slice2.m_data, &slice2.m_size);
            appendText(message, slice2);
            char number[0x100];
            number[0] = 0;
            sprintf(number + strlen(number), "%d", blocks);
            appendText(message, number, strlen(number));
            appendText(message, tail);
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
                static_cast<u8>(reinterpret_cast<muRecorderFrame3*>(m_script.m_sp - 4)[-1].m_slot))) {
            m_sysRecorderUsed = true;
            MU_CALL(muRecorderFrame3, muRecorderFrame3(), State_WaitSysRec, State_SaveAdvDone);
        } else {
            MU_POP();
        }
        break;
    }

    case State_SaveAdvDone:
        m_script.m_sp -= 3;
        MU_POP();
        break;

    case State_LoadAdv: {
        muRecorderFrame4* frame = reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5);
        frame->m_result = 0;
        m_advData.reset(0);
        if (!gfSysRecorder2::isEnable(1)) {
            MU_POP();
            break;
        }
        if (m_sysRecorder->loadAdventure(m_heap2, static_cast<u8>(frame->m_slot))) {
            m_sysRecorderUsed = true;
            MU_CALL(muRecorderFrame3, muRecorderFrame3(), State_WaitSysRec, State_LoadAdvDone);
        } else {
            MU_POP();
        }
        break;
    }

    case State_LoadAdvDone: {
        m_script.m_sp -= 3;
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
            m_failed = !m_flag7;
        } else if (m_request == Request_SaveAdv) {
            {
                muRecorderFrame3 frame = {-1, static_cast<int>(m_slot), 0};
                MU_CALL(muRecorderFrame3, frame, State_SaveAdv, State_SaveAdvRet);
            }
            break;
        case State_SaveAdvRet:
            m_failed = 0;
            m_script.m_sp -= 3;
        } else if (m_request == Request_LoadAdv) {
            {
                muRecorderFrame4 frame = {-1, static_cast<int>(m_slot), 0, 0};
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
