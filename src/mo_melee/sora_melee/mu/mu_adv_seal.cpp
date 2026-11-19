#include <ec/ec_mgr.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_pad_status.h>
#include <mt/mt_vector.h>
#include <mu/adv/mu_adv_seal_win_notice.h>
#include <mu/adv/mu_seal_figure_status.h>
#include <mu/mu_msg.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <snd/snd_system.h>
#include <mu/adv/mu_adv_seal.h>
#include <mu/adv/mu_adv_seal_tex_mgr.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <stdio.h>
#include <types.h>

// ecMgr::getMdl / getMdlAnim (main, unnamed): the scene model and the animation of an effect model
extern "C" nw4r::g3d::ScnMdlSimple* fn_800607C8(ecMgr* mgr, u32 handle);
extern "C" gfModelAnimation* fn_80060858(ecMgr* mgr, u32 handle);

namespace {

inline void setScnObjHidden(nw4r::g3d::ScnMdlSimple* obj, u32 hidden) {
    obj->SetScnObjOption(1, hidden);
    obj->SetScnObjOption(6, hidden);
    obj->SetScnObjOption(7, hidden);
}

} // namespace

void muAdvSealTask::getResourcePackData(const char** ppath, u32* psize) {
    *ppath = "/menu2/sc_adv_seal.pac";
    *psize = 0x344000;
}

namespace {

// The buttons of the menu actions for the current controller (written by set_pad_type).
u32 s_padButtons[4] = { 0x400, 0x10, 0x40, 0x20 };

} // namespace

static u8 s_isPadSideways;

void muAdvSealTask::set_pad_type(const gfPadStatus* pad) {
    u32* buttons = s_padButtons;
    if (pad->m_error != 0) {
        return;
    }
    s_isPadSideways = false;
    buttons[0] = 0x400;
    buttons[1] = 0x10;
    buttons[2] = 0x40;
    buttons[3] = 0x20;
    switch (pad->m_controllerType) {
    case 2:
        buttons[0] = 0x200000;
        buttons[1] = 0x40;
        buttons[2] = 0x200000;
        buttons[3] = 0x80000;
        break;
    case 3:
        buttons[0] = 0x80000;
        buttons[2] = 0x10000;
        buttons[3] = 0x20000;
        if (m_padKind != 1) {
            s_isPadSideways = true;
        }
        break;
    }
}

void muAdvSealWinList::list_sort(int kind, int order) {
    switch (kind) {
    case 0:
        m_sortKind = 6;
        break;
    case 1:
        m_sortKind = 1;
        break;
    case 2:
        m_sortKind = 0;
        break;
    case 3:
        m_sortKind = 5;
        break;
    }
    switch (order) {
    case 0:
        m_sortOrder = 4;
        break;
    case 1:
        m_sortOrder = 0;
        break;
    case 2:
        m_sortOrder = 1;
        break;
    case 3:
        m_sortOrder = 2;
        break;
    case 4:
        m_sortOrder = 3;
        break;
    }
    list_sort_kind(m_sortKind, m_sortOrder);
}

bool muAdvSealWinList::add_seal_num(u32 id, int num) {
    if (num != 0) {
        int i;
        for (i = 0; i < m_numItems; i++) {
            if (id == *m_items[i].m_data) {
                break;
            }
        }
        if (i < m_numItems) {
            int count = num + m_items[i].m_count;
            if (count >= 0) {
                m_items[i].m_count = count;
                m_flags |= 0xC;
                return true;
            }
        }
    }
    return false;
}

muAdvSealTexMgr::muAdvSealTexMgr() {
    for (int i = 0; i < 65; i++) {
        m_entries[i].m_index = i;
        m_entries[i].m_refCount = 0;
        m_entries[i].m_key = NULL;
        m_entries[i].m_prev = &m_entries[i] - 1;
        m_entries[i].m_next = &m_entries[i] + 1;
        m_entries[i].m_tex = NULL;
    }
    m_entries[0].m_prev = NULL;
    m_entries[64].m_next = NULL;
    m_head = &m_entries[0];
    m_tail = &m_entries[64];
    m_loading = NULL;
}

bool muAdvSealTexMgr::update(bool cancel) {
    if (!cancel && m_fileHandle.m_request == NULL && m_loading == NULL) {
        int i = 0;
        Entry* entry = m_entries;
        for (; i < 65; i++, entry++) {
            if (entry->m_key != NULL && entry->m_refCount != 0 && entry->m_tex == NULL) {
                m_loading = entry;
                break;
            }
        }
        if (m_loading == NULL) {
            entry = m_entries;
            for (int i = 0; i < 65; i++, entry++) {
                if (entry->m_key != NULL && entry->m_tex == NULL) {
                    m_loading = entry;
                    break;
                }
            }
        }
        if (m_loading != NULL) {
            char path[0x80];
            sprintf(path, "/toy/seal/%s", m_loading->m_key->m_name);
            m_fileHandle.readRequest(path, static_cast<Heaps::HeapType>(0x2B), 0, 0);
        }
    }
    if (m_fileHandle.m_request != NULL) {
        if (m_fileHandle.isReady()) {
            if (m_loading == NULL || cancel) {
                gfHeapManager::free(m_fileHandle.getBuffer());
            } else {
                m_loading->m_tex = m_fileHandle.getBuffer();
                nw4r::g3d::ResFile::Init(reinterpret_cast<nw4r::g3d::ResFile*>(&m_loading->m_tex));
                m_loading = NULL;
            }
            m_fileHandle.release();
        } else {
            return false;
        }
    }
    return true;
}

int muAdvSealTexMgr::regist(const Key* key) {
    if (key != NULL) {
        u32 i;
        Entry* entry = m_entries;
        for (i = 0; i < 65; i++, entry++) {
            if (entry->m_key != NULL && entry->m_key->m_id == key->m_id) {
                break;
            }
        }
        if (i < 65) {
            if (entry->m_refCount == 0) {
                if (entry->m_prev == NULL) {
                    m_head = entry->m_next;
                } else {
                    entry->m_prev->m_next = entry->m_next;
                }
                if (entry->m_next == NULL) {
                    m_tail = entry->m_prev;
                } else {
                    entry->m_next->m_prev = entry->m_prev;
                }
            }
            entry->m_refCount++;
            return i;
        }
        Entry* free = m_head;
        if (free != NULL) {
            m_head = free->m_next;
            if (m_head == NULL) {
                m_tail = NULL;
            } else {
                m_head->m_prev = NULL;
            }
            if (m_loading != NULL && m_loading == free) {
                m_loading = NULL;
            }
            if (free->m_tex != NULL) {
                gfHeapManager::free(free->m_tex);
                free->m_tex = NULL;
            }
            free->m_refCount = 1;
            free->m_key = key;
            return free->m_index;
        }
    }
    return -1;
}

void muAdvSealTexMgr::un_regist(int* handle) {
    if (*handle < 0) {
        return;
    }
    Entry* entry = &m_entries[*handle];
    if (entry->m_refCount == 0) {
        for (Entry* e = m_head; e != NULL; e = e->m_next) { }
    }
    entry->m_refCount--;
    if (entry->m_refCount == 0) {
        if (m_head == NULL) {
            m_tail = entry;
            m_head = entry;
            entry->m_next = NULL;
            entry->m_prev = NULL;
        } else {
            m_tail->m_next = entry;
            entry->m_prev = m_tail;
            entry->m_next = NULL;
            m_tail = entry;
        }
        if (m_loading != NULL && m_loading == entry) {
            m_loading = NULL;
        }
    }
    *handle = -1;
}

void muSealFigureWork::calcEff() {
    clearEff();
    Entry* entry = m_entries;
    for (int i = 0; i < 64; i++, entry++) {
        if (!(m_slotFlags[i] & 0x80) && entry->m_seal != NULL) {
            applyEff(entry->m_seal);
        }
    }
}

void muAdvSealWinNotice::init(int priority, MuMsg* msg, u32 msgBase) {
    ecMgr* ec = g_ecMgr;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_effect = ec->setEffect(static_cast<EfID>(0x1360001), &pos);
    ec->setMdlPriority(m_effect, priority);
    ec->setMdlEnableUpdate(m_effect, false, 0x12);
    nw4r::g3d::ScnMdlSimple* obj = fn_800607C8(ec, m_effect);
    if (obj != NULL) {
        setScnObjHidden(obj, 1);
    }
    m_msg = msg;
    m_msgBase = msgBase;
    nw4r::g3d::ScnMdlSimple* mdl = fn_800607C8(ec, m_effect);
    m_msg->attachScnMdlSimple(m_msgBase, mdl, 1, 0.0625f);
    m_msg->attachScnMdlSimple(m_msgBase + 1, mdl, 2, 0.0625f);
    m_msg->attachScnMdlSimple(m_msgBase + 2, mdl, 3, 0.0625f);
    m_msg->setAlignMode(m_msgBase, static_cast<MuMsg::AlignMode>(0));
    m_msg->setAlignMode(m_msgBase + 1, static_cast<MuMsg::AlignMode>(0));
    m_msg->setAlignMode(m_msgBase + 2, static_cast<MuMsg::AlignMode>(0));
    m_msg->attachScnMdlSimple(m_msgBase + 3, mdl, 0, 0.0625f);
    m_msg->setAlignMode(m_msgBase + 3, static_cast<MuMsg::AlignMode>(0));
    m_state = 0;
    m_select = 0;
    m_timer = 0;
    m_cancelMask = 0x100;
}

void muAdvSealWinNotice::start(int line0, int line1, int line2, int select, u32 cancelMask) {
    ecMgr* ec = g_ecMgr;
    nw4r::g3d::ScnMdlSimple* obj = fn_800607C8(ec, m_effect);
    if (obj != NULL) {
        setScnObjHidden(obj, 0);
    }
    ec->setMdlFrame(m_effect, line1 >= 0 ? 1.0f : 0.0f, 2);
    ec->setMdlFrame(m_effect, select == 1 ? 0.0f : 1.0f, 0x10);
    ec->setMdlAnmIdx(m_effect, 0, 1);
    m_lines[0] = line0;
    m_lines[1] = line1;
    m_lines[2] = line2;
    if (line1 >= 0) {
        m_msg->setFontColor(m_msgBase, 0, 0, 0, 0xFF);
        m_msg->printIndex(m_msgBase, m_lines[0], NULL);
        m_msg->printf(m_msgBase + 3, "");
    } else {
        m_msg->setFontColor(m_msgBase + 3, 0, 0, 0, 0xFF);
        m_msg->printIndex(m_msgBase + 3, m_lines[0], NULL);
        m_msg->printf(m_msgBase, "");
    }
    for (int i = 1; i < 3; i++) {
        m_msg->setFontColor(m_msgBase + i, 0, 0, 0, 0xFF);
        if (m_lines[i] < 0) {
            m_msg->printf(m_msgBase + i, "");
        } else {
            m_msg->printIndex(m_msgBase + i, m_lines[i], NULL);
        }
    }
    m_state = 1;
    m_select = select;
    m_timer = 0;
    m_cancelMask = cancelMask | 0x100;
    g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
}

void muAdvSealWinNotice::end() {
    g_ecMgr->setMdlAnmIdx(m_effect, 1, 1);
    m_state = 2;
    m_timer = 0;
}

void muAdvSealWinNotice::update() {
    ecMgr* ec = g_ecMgr;
    nw4r::g3d::ScnMdlSimple* obj;
    switch (m_state) {
    case 1:
        if (m_lines[1] >= 0) {
            int first;
            int second;
            if (m_select == 1) {
                first = 1;
                second = 2;
            } else {
                first = 2;
                second = 1;
            }
            m_msg->printIndex(first + m_msgBase, m_lines[first], NULL);
            m_msg->printIndex(second + m_msgBase, m_lines[second], NULL);
        }
        m_timer++;
        break;
    case 2:
        if (++m_timer < 60) {
            gfModelAnimation* anim = fn_80060858(ec, m_effect);
            if (anim != NULL) {
                float frame = anim->getFrame();
                if (anim->getFrame() < frame) {
                    break;
                }
            }
        }
        obj = fn_800607C8(ec, m_effect);
        if (obj != NULL) {
            setScnObjHidden(obj, 1);
        }
        m_state = 0;
        g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
        break;
    }
}

int muAdvSealWinNotice::input(u32 held, u32 trigger) {
    ecMgr* ec = g_ecMgr;
    if (m_state != 1) {
        return -3;
    }
    if (trigger & m_cancelMask) {
        end();
        if (m_lines[1] >= 0) {
            return m_select;
        }
        return -2;
    }
    if (trigger & 0x200) {
        end();
        return -2;
    }
    if (m_lines[1] >= 0) {
        if (trigger & 1) {
            if (m_select != 1) {
                m_select = 1;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
            ec->setMdlFrame(m_effect, 0.0f, 0x10);
        } else if (trigger & 2) {
            if (m_select != -1) {
                m_select = -1;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
            ec->setMdlFrame(m_effect, 1.0f, 0x10);
        }
    }
    return 0;
}

#include <nw4r/g3d/g3d_obj.h>
#include <gf/gf_task.h>
#include <gf/gf_file_io_handle.h>
#include <mt/mt_prng.h>
#include <nw4r/ut/ut_algorithm.h>
#include <mt/mt_matrix.h>
// TODO: clean up, give variables real names
namespace jj_2C3FE4 {
typedef unsigned char u8;
typedef unsigned int u32;

struct gfArchive;

struct muAdvSealTaskParam {
    u8 mode;
};

struct muAdvSealTask {
    u8 pad0[0x44];
    u8 mode;
    u8 pad45[0xF];
    u32 state;
    u8 pad58[0x1AF24 - 0x58];
    void* scene;
    u8 tail[0x1B018 - 0x1AF28];
};

struct GameData {
    u8 pad0[0x24];
    u32 value;
};

struct GameGlobal {
    u8 pad0[0x1C];
    GameData* data;
};

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
muAdvSealTask* __ct__13muAdvSealTaskFP9gfArchive(muAdvSealTask*, gfArchive*);
void fn_800B96E8(void*, int);
void* getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(int);
void* fn_801AB6CC(void*, void*, int);
void setBGMVol__9sndSystemFbf(void*, bool, float);

extern muAdvSealTask* lbl_27_bss_7328;
extern GameGlobal* g_GameGlobal;
extern void* g_sndSystem;
extern float lbl_27_rodata_61C4;
}

extern "C" muAdvSealTask* create__13muAdvSealTaskFPC18muAdvSealTaskParamP9gfArchive(
    const muAdvSealTaskParam* param, gfArchive* archive)
{
    muAdvSealTask* task =
        (muAdvSealTask*)__nw__FUlQ25Heaps8HeapType(0x1B018, 0x2A);

    if (task)
        task = __ct__13muAdvSealTaskFP9gfArchive(task, archive);

    lbl_27_bss_7328 = task;
    task->mode = param->mode;

    if (param->mode == 1)
        fn_800B96E8((u8*)task + 0x58, 0xF0);
    else
        fn_800B96E8((u8*)task + 0x58, (u8)g_GameGlobal->data->value);

    switch (task->mode) {
    case 1:
        task->state = 0x135;
        task->scene = fn_801AB6CC(
            getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(0x2A),
            0, 6);
        break;

    default:
        setBGMVol__9sndSystemFbf(g_sndSystem, true, lbl_27_rodata_61C4);
        task->state = 0x134;
        break;
    }

    return task;
}
}

// TODO: clean up, give variables real names
namespace jj_2C447C {

extern "C" {
void setBGMVol__9sndSystemFbf(void*, bool, float);
void killEffectGroup__5ecMgrFUlUlbbi(void*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void fn_8005EB68(void*, int);
void remove__11gfSceneRootFPQ34nw4r3g3d6ScnMdl(void*, void*);
void removeInfoCamResAnm__11gfSceneRootFv(void*);
void __dt__5MuMsgFv(void*, int);
void fn_800AD87C(void*, int);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
extern char __vt__13muAdvSealTask[];
extern const float lbl_27_rodata_61C8;
extern void* g_ecMgr;
extern void* g_gfSceneRoot;
extern void* g_sndSystem;
}

struct ModelBase { char pad[0x5C]; };
class Model : public ModelBase {
public:
    virtual void destroy(int);
};

extern "C" void* __dt__13muAdvSealTaskFv(void* self, int deleting) {
    char* s = (char*)self;
    if (self != 0) {
        char* p;
        unsigned int counter;
        *(void**)(s + 0x3C) = __vt__13muAdvSealTask;
        if (*(unsigned char*)(s + 0x44) != 1) {
            setBGMVol__9sndSystemFbf(g_sndSystem, true, lbl_27_rodata_61C8);
        }
        killEffectGroup__5ecMgrFUlUlbbi(g_ecMgr, 0x04000000, 0xFF000000, 1, 1, 0);
        fn_8005EB68(g_ecMgr, *(int*)(s + 0x54));
        fn_8005EB68(g_ecMgr, 0x136);
        if (*(int*)(s + 0x66C) != 0) *(int*)(s + 0x66C) = 0;
        if (*(int*)(s + 0x670) != 0) *(int*)(s + 0x670) = 0;
        if (*(int*)(s + 0x674) != 0) *(int*)(s + 0x674) = 0;
        if (*(int*)(s + 0x678) != 0) *(int*)(s + 0x678) = 0;
        if (*(int*)(s + 0x67C) != 0) *(int*)(s + 0x67C) = 0;
        {
            char* q = s;
            unsigned int i = 0;
            void* root = g_gfSceneRoot;
            do {
                Model* slot = *(Model**)(q + 0x1AEE0);
                if (slot != 0) {
                    remove__11gfSceneRootFPQ34nw4r3g3d6ScnMdl(root, *(void**)((char*)slot + 0xC));
                    slot = *(Model**)(q + 0x1AEE0);
                    if (slot != 0) {
                        if (slot != 0) slot->destroy(1);
                        *(void**)(q + 0x1AEE0) = 0;
                    }
                }
                i++;
                q += 4;
            } while (i < 0xB);
        }
        (*(gfTask**)(s + 0x680))->exit();
        *(void**)(s + 0x680) = 0;
        (*(nw4r::g3d::G3dObj**)(s + 0x1AEDC))->Destroy();
        *(void**)(s + 0x1AEDC) = 0;
        if (*(int*)(s + 0x668) != 0) {
            removeInfoCamResAnm__11gfSceneRootFv(g_gfSceneRoot);
        }
        if (*(void**)(s + 0x684) != 0) {
            __dt__5MuMsgFv(*(void**)(s + 0x684), 1);
            *(void**)(s + 0x684) = 0;
        }
        if (*(void**)(s + 0x688) != 0) {
            __dt__5MuMsgFv(*(void**)(s + 0x688), 1);
            *(void**)(s + 0x688) = 0;
        }
        if (*(void**)(s + 0x68C) != 0) {
            __dt__5MuMsgFv(*(void**)(s + 0x68C), 1);
            *(void**)(s + 0x68C) = 0;
        }
        if (*(void**)(s + 0x690) != 0) {
            __dt__5MuMsgFv(*(void**)(s + 0x690), 1);
            *(void**)(s + 0x690) = 0;
        }
        if (*(gfTask**)(s + 0x1AF18) != 0) {
            (*(gfTask**)(s + 0x1AF18))->exit();
        }
        if (*(nw4r::g3d::G3dObj**)(s + 0x1AF24) != 0) {
            (*(nw4r::g3d::G3dObj**)(s + 0x1AF24))->Destroy();
        }
        if (*(void**)(s + 0x6A0) != 0) {
            fn_800AD87C(*(void**)(s + 0x6A0), 1);
            *(void**)(s + 0x6A0) = 0;
        }
        p = s + 0x144;
        if (p != 0) {
            counter = 0;
            do {
                if (*(void**)(p + 0x10) != 0) {
                    gfHeapManager::free(*(void**)(p + 0x10));
                    *(void**)(p + 0x10) = 0;
                }
                counter++;
                p += 0x14;
            } while (counter < 0x41);
            __dt__14gfFileIOHandleFv(s + 0x658, -1);
        }
        __dt__14gfFileIOHandleFv(s + 0x48, -1);
        __dt__6gfTaskFv(s, 0);
        if (deleting > 0) {
            __dl__FPv(s);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_2C4750 {
struct muMenu {
    char pad0[0x44];
    u8 unk44;
    u8 unk45;
    char pad46[6];
    void* unk4C;
    void* unk50;
    s32 unk54;
    char pad58[0x610];
    void* unk668;
    void* unk66C;
    void* unk670;
    void* unk674;
    void* unk678;
    void* unk67C;
    char pad680[4];
    void* unk684;
    void* unk688;
    char pad68C[4];
    void* unk690;
    void* unk694;
    void* unk698;
    void* unk69C;
    void* unk6A0;
    char pad6A4[0x1A878];
    void* unk1AF1C;
    void* unk1AF20;
    char pad1AF24[0xF0];
    void* unk1B014;
};

extern "C" {
void loadMenuSound__6muMenuFv(void);
int isLoadFinishMenuSound__6muMenuFv(void);
void* isLoadFinish__21tySealListDataManagerFv(void*);
void* getData__9gfArchiveFi11ARCNodeTypeiPiUs(void*, s32, s32, s32, s32*, u16);
void* getData__9gfArchiveFi11ARCNodeTypeiUs(void*, s32, s32, s32, u16);
void registResource__5ecMgrFPcPvii(void*, void*, void*, s32, s32);
void* fn_800B03F0(void*, s32);
void* getGlobalRecordMenuDatap__10GameGlobalFv(void*);
void setMsgData__5MuMsgFPv(void*, void*);
extern void* g_GameGlobal;
extern void* g_ecMgr;
extern char lbl_27_data_7B298[];
}

extern "C" s32 fn_27_2C4750(muMenu* arg0) {
    char* base = lbl_27_data_7B298;
    s32 sp8;
    s32 var_r6;
    void* var_r29;
    void* temp;
    void* temp_r3;

    while (arg0->unk45 != 0) {
        switch (arg0->unk45) {
        case 1:
            temp = isLoadFinish__21tySealListDataManagerFv(arg0->unk6A0);
            if (temp != NULL) {
                if (arg0->unk44 == 1) {
                    loadMenuSound__6muMenuFv();
                    arg0->unk45 += 1;
                } else {
                    arg0->unk45 += 2;
                }
            } else {
                return 0;
            }
            break;
        case 2:
            if (isLoadFinishMenuSound__6muMenuFv() != 0) {
                arg0->unk45 += 1;
            } else {
                return 0;
            }
            break;
        case 3:
            temp = getData__9gfArchiveFi11ARCNodeTypeiPiUs(arg0->unk1B014, 0, 1, 0x28, &sp8, 0xFFFE);
            arg0->unk50 = temp;
            registResource__5ecMgrFPcPvii(g_ecMgr, base + 0xB8, temp, sp8, 0x2B);
            if (arg0->unk54 == 0x134) {
                var_r29 = base + 0xC8;
                var_r6 = 0x14;
            } else {
                var_r6 = 0x46;
                var_r29 = base + 0xD0;
            }
            temp = getData__9gfArchiveFi11ARCNodeTypeiPiUs(arg0->unk1B014, 0, 1, var_r6, &sp8, 0xFFFE);
            arg0->unk4C = temp;
            registResource__5ecMgrFPcPvii(g_ecMgr, var_r29, temp, sp8, 0x2B);
            arg0->unk668 = fn_800B03F0(arg0->unk1B014, -((!((((u8*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal))[0x28] >> 7) & 1)) & 1) + 0xB);
            arg0->unk66C = fn_800B03F0(arg0->unk1B014, 0x3C);
            arg0->unk670 = fn_800B03F0(arg0->unk1B014, 0x1E);
            arg0->unk674 = fn_800B03F0(arg0->unk1B014, 0x32);
            arg0->unk678 = fn_800B03F0(arg0->unk1B014, 0x64);
            arg0->unk67C = fn_800B03F0(arg0->unk1B014, 0x8C);
            arg0->unk1AF1C = fn_800B03F0(arg0->unk1B014, 0x82);
            arg0->unk1AF20 = getData__9gfArchiveFi11ARCNodeTypeiUs(arg0->unk1B014, 0, 1, 0x5A, 0xFFFE);
            temp_r3 = getData__9gfArchiveFi11ARCNodeTypeiUs(arg0->unk1B014, 0, 1, 0x78, 0xFFFE);
            arg0->unk694 = temp_r3;
            setMsgData__5MuMsgFPv(arg0->unk684, temp_r3);
            temp_r3 = getData__9gfArchiveFi11ARCNodeTypeiUs(arg0->unk1B014, 0, 1, 0x6E, 0xFFFE);
            arg0->unk698 = temp_r3;
            setMsgData__5MuMsgFPv(arg0->unk688, temp_r3);
            temp_r3 = getData__9gfArchiveFi11ARCNodeTypeiUs(arg0->unk1B014, 0, 1, 0x50, 0xFFFE);
            arg0->unk69C = temp_r3;
            setMsgData__5MuMsgFPv(arg0->unk690, temp_r3);
            arg0->unk45 += 1;
            /* fallthrough */
        default:
            arg0->unk45 = 0;
            return 1;
        }
        return 0;
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_2CA964 {
extern const float lbl_27_rodata_6078[];

extern "C" void fn_27_2CA964(void* arg0, float* arg1, float* arg2, int arg3) {
    unsigned char* p = (unsigned char*)arg0;
    const float* data = lbl_27_rodata_6078;
    switch (arg3) {
    case 1:
        if (p[0x44] == 0) {
            *arg1 = data[0x1E0 / 4];
            *arg2 = data[0x1E4 / 4];
            return;
        }
        *arg1 = data[0x1E8 / 4];
        *arg2 = data[0x1E4 / 4];
        return;
    case 0:
        *arg1 = data[0x1EC / 4];
        *arg2 = data[0x1F0 / 4];
        return;
    default:
        *arg1 = data[0x1F4 / 4];
        *arg2 = data[0x1F8 / 4];
        if (*(int*)(p + 0x43A4) == 0) {
            float value = *(float*)(p + 0x4328);
            *arg2 = data[0x1F8 / 4] - (data[0x1FC / 4] * value);
        }
        return;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2CBB7C {
struct S {
    char pad0[0xc];
    s32 divisor;
    s32 unk3c6c;
    char pad1[0x20];
    s32 rem;
    s32 quot;
};

struct Obj {
    char pad0[0x10];
    s32 unk3c18;
    char pad1[0x3c3c - 0x3c1c];
    s32 unk3c3c;
    s32 unk3c40;
    char pad2[0x54 - 0x3c];
};

struct Self {
    char pad0[0x3c08];
    Obj obj;
    S s;
    char pad1[0x3cb0 - 0x3c98];
    s32 unk3cb0;
    s32 unk3cb4;
    char pad2[0x3cc0 - 0x3cb8];
    s32 unk3cc0;
    s32 unk3cc4;
    char pad3[0x3ccc - 0x3cc8];
    s32 unk3ccc;
    s32 unk3cd0;
};

extern "C" {
void init__13muAdvSealListFiiib(void*, int, int, int, int);
void fn_27_2C9714(int, int);
void flush__13muAdvSealListFv(void*);
void setCurIdx__13muAdvSealListFi(S*, int);
void setBarMode__13muAdvSealListFbb(void*, int, int);
}
extern void* lbl_27_bss_7328;

extern "C" void fn_27_2CBB7C(Self* self, int arg1) {
    self->unk3ccc = arg1;
    self->unk3cc4 = 0;
    self->unk3cc0 = 0;
    flush__13muAdvSealListFv(&self->obj);
    setBarMode__13muAdvSealListFbb(&self->obj, 0, 0);
    switch (arg1) {
    case 0: {
        int idx = self->obj.unk3c3c * self->obj.unk3c18 + self->obj.unk3c40;
        char* task = (char*)lbl_27_bss_7328;
        if (idx < *(int*)(task + 0x42dc)) {
            char* record = *(char**)(task + 0x2edc + idx * 4);
            fn_27_2C9714(*(int*)record, *(short*)(record + 4));
        } else {
            fn_27_2C9714(0, -1);
        }
        break;
    }
    case 1: {
        init__13muAdvSealListFiiib(&self->s, 3, 3, 1, 1);
        switch (self->unk3cb0) {
        case 1:
            setCurIdx__13muAdvSealListFi(&self->s, 1);
            break;
        case 0:
            setCurIdx__13muAdvSealListFi(&self->s, 2);
            break;
        case 5:
            setCurIdx__13muAdvSealListFi(&self->s, 3);
            break;
        default:
            setCurIdx__13muAdvSealListFi(&self->s, 0);
            break;
        }
        flush__13muAdvSealListFv(&self->s);
        int idx = self->s.quot + self->s.rem * self->s.unk3c6c;
        if (idx == 3) {
            fn_27_2C9714(0, 0x21);
        } else {
            fn_27_2C9714(0, idx + 0x18);
        }
        break;
    }
    case 2: {
        char* task = (char*)lbl_27_bss_7328;
        if (*(unsigned char*)(task + 0x44) == 0) {
            init__13muAdvSealListFiiib(&self->s, 4, 4, 1, 1);
            switch (self->unk3cb4) {
            case 0:
                setCurIdx__13muAdvSealListFi(&self->s, 1);
                break;
            case 1:
                setCurIdx__13muAdvSealListFi(&self->s, 2);
                break;
            case 2:
                setCurIdx__13muAdvSealListFi(&self->s, 3);
                break;
            case 3:
                setCurIdx__13muAdvSealListFi(&self->s, 4);
                break;
            default:
                setCurIdx__13muAdvSealListFi(&self->s, 0);
                break;
            }
            flush__13muAdvSealListFv(&self->s);
            fn_27_2C9714(0, self->s.quot + self->s.rem * self->s.unk3c6c + 0x1D);
        } else {
            init__13muAdvSealListFiiib(&self->s, 0xA, 0xA, 2, 1);
            setCurIdx__13muAdvSealListFi(&self->s, self->unk3cd0);
            flush__13muAdvSealListFv(&self->s);
            fn_27_2C9714(0, 0x23);
        }
        break;
    }
    case 3:
        fn_27_2C9714(0, 0x22);
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2CBEF0 {

extern "C" {
void fn_800ADD1C(void*, int, int);
void clearNarrow__21tySealListDataManagerFv(void*);
void fn_800AE200(void*, int, int);
int isPutChara__21tySealListDataManagerFiUl(void*, int, int);
void init__13muAdvSealListFiiib(void*, int, int, int, int);
void fn_27_2C9714(int, short);
void fn_27_2CBB7C(void*, int);
extern muAdvSealTask* lbl_27_bss_7328;
}

extern "C" void list_sort_kind__16muAdvSealWinListFii(u32* arg0, int arg1, int arg2) {
    char* writePtr;
    char* task;
    int bound;
    int i;
    int stored;
    int headMinusOne;
    unsigned int* scanPtr;
    char* self;
    int offset;
    int* target;
    int index;
    char* manager;
    int value;
    int entryCount;
    int scanCount;
    int k;
    char* record;

    self = (char*)arg0;
    task = (char*)lbl_27_bss_7328;

    *(int*)(self + 0x3cb4) = arg2;
    *(int*)(self + 0x3cb0) = arg1;
    fn_800ADD1C(*(void**)(task + 0x6a0), arg1, 0);

    if (*(int*)(self + 0x3cb4) == 4) {
        clearNarrow__21tySealListDataManagerFv(*(void**)(task + 0x6a0));
    } else {
        fn_800AE200(*(void**)(task + 0x6a0), *(int*)(self + 0x3cb4), 0);
    }

    stored = 0;
    headMinusOne = -1;
    if (*(unsigned char*)(task + 0x44) == 0) {
        entryCount = *(int*)(task + 0x448c);
        if (entryCount >= 0) {
            headMinusOne = *(int*)(task + entryCount * 0x70c + 0x4b9c) - 1;
        }
    }

    if (*(int*)(self + 0x3cb4) == 4) {
        bound = *(int*)(*(int*)(task + 0x6a0) + 0x2ef8);
    } else {
        bound = *(int*)(*(int*)(task + 0x6a0) + 0xc);
    }

    writePtr = self;
    i = 0;
    offset = 0;

    while (i < bound) {
        scanPtr = (unsigned int*)self;
        if (*(int*)(self + 0x3cb4) == 4) {
            index = i;
        } else {
            index = *(int*)((char*)*(void**)(task + 0x6a0) + 0x1784 + offset);
        }

        manager = *(char**)(task + 0x6a0);
        value = *(int*)(manager + index * 4 + 0x7e4);
        target = (int*)(*(int*)manager + value * 0x64);

        if (headMinusOne < 0 || isPutChara__21tySealListDataManagerFiUl(manager, headMinusOne, *target) != 0) {
            scanCount = *(int*)(self + 0x2800);
            k = 0;
            while (k < scanCount) {
                if (*scanPtr == (unsigned int)target) {
                    break;
                }
                ++k;
                scanPtr += 2;
            }

            if (k < scanCount) {
                *(int*)(writePtr + 0x2804) = (int)scanPtr;
                writePtr += 4;
                ++stored;
            }
        }

        ++i;
        offset += 4;
    }

    *(int*)(self + 0x3c04) = stored;
    init__13muAdvSealListFiiib(self + 0x3c08, stored, 9, 1, 1);

    if (*(int*)(self + 0x3ccc) == 0) {
        value = *(int*)(self + 0x3c40) +
                *(int*)(self + 0x3c3c) * *(int*)(self + 0x3c18);
        if (value < *(int*)((char*)lbl_27_bss_7328 + 0x42dc)) {
            record = *(char**)((char*)lbl_27_bss_7328 + value * 4 + 0x2edc);
            fn_27_2C9714(*(int*)record, *(short*)(record + 4));
        } else {
            fn_27_2C9714(0, -1);
        }
    }

    if (*(int*)(self + 0x3ccc) == 0) {
        fn_27_2CBB7C(self, 0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2D0C84 {
extern "C" {
extern const float lbl_27_rodata_62C4;
extern const float lbl_27_rodata_62C8;
}
struct Record { char pad0[0x18]; int count; char pad1[0x6C4]; signed char indices[44]; };
struct Object { char pad0[0x10]; int current; char pad1[0x168E4]; signed char values[64]; int count; bool ready; };
extern "C" void fn_27_2D0C84(Object* self) {
    self->count = 0;
    for (int i = 0; i < 64;) {
        self->values[i++] = 0;
    }
    for (int j = 0; j < ((Record*)self)[self->current].count; j++) {
        if (((Record*)self)[self->current].indices[j] >= 0) {
            self->values[((Record*)self)[self->current].indices[j]] =
                (signed char)(lbl_27_rodata_62C4 - ((float)j + lbl_27_rodata_62C8 * randf()));
            self->count++;
        }
    }
    self->ready = true;
}
}

// TODO: clean up, give variables real names
namespace jj_2D5104 {


struct Point {
    float x, y;
};
struct Obj {
    char pad[0x52];
    short h;
};
struct Entry {
    Point pos;
    Obj* obj;
};
struct Mgr {
    int unk0;
    Entry entries[64];
    char pad[0x648 - 0x304];
    s8 orderX[64];
    s8 orderY[64];
};

extern "C" {
extern const float lbl_27_rodata_61E0;
extern const float lbl_27_rodata_626C;

void fn_27_2D5104(Mgr* self) {
    int j;
    int pos;
    int pos2;
    int i;
    Entry* e = self->entries;
    int count = 0;
    for (i = 0; i < 64; i++, e++) {
        if (e->obj != 0) {
            pos = 0;
            Point coords = { e->pos.x, e->pos.y };
            coords.y += (float)e->obj->h / lbl_27_rodata_626C;

            while (pos < count) {
                Point prev = self->entries[self->orderX[pos]].pos;
                prev.y += (float)self->entries[self->orderX[pos]].obj->h / lbl_27_rodata_626C;

                if (nw4r::ut::Abs(coords.x - prev.x) < lbl_27_rodata_61E0) {
                    if (coords.y > prev.y)
                        break;
                } else if (coords.x < prev.x) {
                    break;
                }
                pos++;
            }

            for (j = count; j > pos; j--) {
                self->orderX[j] = self->orderX[j - 1];
            }
            self->orderX[pos] = i;

            pos2 = 0;
            while (pos2 < count) {
                Point prev = self->entries[self->orderY[pos2]].pos;
                prev.y += (float)self->entries[self->orderY[pos2]].obj->h / lbl_27_rodata_626C;

                if (nw4r::ut::Abs(coords.y - prev.y) < lbl_27_rodata_61E0) {
                    if (coords.x < prev.x)
                        break;
                } else if (coords.y > prev.y) {
                    break;
                }
                pos2++;
            }

            for (j = count; j > pos2; j--) {
                self->orderY[j] = self->orderY[j - 1];
            }
            count++;
            self->orderY[pos2] = i;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_2D56B4 {

extern char lbl_27_data_7B3F0[];
extern const float lbl_27_rodata_61C8;
extern const float lbl_27_rodata_61E8;
extern "C" void fn_800613B8(Matrix*, nw4r::g3d::ScnMdlSimple*, unsigned short, const Vec3f*, const Vec3f*, const Vec3f*);
extern "C" void ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(void*, int, Matrix*);

extern "C" void fn_27_2D56B4(nw4r::g3d::ScnMdlSimple* self, void* arg1) {
    Matrix matrix(true);
    Vec3f scale;
    Vec3f rotation;
    Vec3f position;
    unsigned short index = 0;
    nw4r::g3d::ResMdl model = self->m_resMdl;
    nw4r::g3d::ResNode node = model.GetResNode(lbl_27_data_7B3F0);
    if (node.IsValid()) {
        unsigned int value;
        if (node.IsValid()) value = node.ptr()->m_nodeIndex;
        else value = 0;
        index = (unsigned short)value;
    }
    position.m_x = lbl_27_rodata_61E8;
    position.m_y = lbl_27_rodata_61E8;
    position.m_z = lbl_27_rodata_61E8;
    rotation.m_x = lbl_27_rodata_61E8;
    rotation.m_y = lbl_27_rodata_61E8;
    rotation.m_z = lbl_27_rodata_61E8;
    scale.m_x = lbl_27_rodata_61C8;
    scale.m_y = lbl_27_rodata_61C8;
    scale.m_z = lbl_27_rodata_61C8;
    fn_800613B8(&matrix, self, index, &scale, &rotation, &position);
    ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(arg1, 0, &matrix);
}
}

// TODO: clean up, give variables real names
namespace jj_2D5D3C {

extern "C" {
extern muAdvSealTask* lbl_27_bss_7328;
}

struct FnObj27 {
    s32 m_0;
    s32 m_4;
    u32 m_8;
    u32 m_c;
};

class SealObj {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6(int, int);
};

struct SealMgr {
    char pad[0xc];
    SealObj* obj;
};

extern "C" void end__22muAdvSealWinNoticeSealFv(FnObj27* self) {
    SealObj* o;
    SealObj* o2;
    unsigned int off;

    g_ecMgr->setMdlAnmIdx(self->m_c, 1, 1);
    self->m_0 = 2;
    self->m_4 = 0;
    char* task;
    task = (char*)lbl_27_bss_7328;
    if (*(u8*)(task + 0x6a6) != 0 && *(s32*)(task + 0x4488) >= 2) {
        o = (**(SealMgr***)(task + 0x1AF10))->obj;
        if (o != NULL) {
            o->s6(1, 0);
            o->s6(6, 0);
            o->s6(7, 0);
        }
    int i;
        i = 0;
        off = 0;
        do {
            o2 = (*(SealMgr**)(*(char**)(task + 0x1AF0C) + off))->obj;
            if (o2 != NULL) {
                o2->s6(1, 0);
                o2->s6(6, 0);
                o2->s6(7, 0);
            }
            i += (1);
            off = off + (4);
        } while (i < 8);
    }
}
}

