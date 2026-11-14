#include <ec/ec_mgr.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
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
    m_timer = 0;
    m_select = select;
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
        nw4r::g3d::ScnMdlSimple* obj = fn_800607C8(ec, m_effect);
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
