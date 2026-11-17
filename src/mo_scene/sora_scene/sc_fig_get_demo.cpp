#include <ad/ad_list.h>
#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_figure_get.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>

// The trophy get demo scene (what sc_fig_get_demo.h declares for its callers; here with the list of the original).
class scFigureGetDemo : public gfScene {
    muFigureGetTask* m_task;
    s32 m_0xC;
    adList<u32, Heaps::MenuInstance> m_list;

public:
    static scFigureGetDemo* create();
    scFigureGetDemo(const char* name) : gfScene(name), m_task(nullptr) { }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};

scFigureGetDemo* scFigureGetDemo::create() {
    return new (Heaps::System) scFigureGetDemo("scFigureGetDemo");
}

void scFigureGetDemo::setModule() {
    setLoadModule("sora_menu_fig_get_demo.rel", false);
}

s32 scFigureGetDemo::process() {
    if (m_task) {
        if (m_task->m_done != 0) {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
            g_gfApplication->unkD0.startKeepScreen();
        }
    }
    return 0;
}

s32 scFigureGetDemo::exit() {
    m_list.remove();
    m_task->exit();
    m_task = nullptr;
    g_sndSystem->stopBGM(0);
    return 0;
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::remove() {
    u32 num = count();
    reset();
    for (u32 i = 0; i < num; i++) {
        T* node = current();
        next();
        if (node) {
            delete node;
        }
    }
    m_start = nullptr;
    m_end = nullptr;
    m_current = nullptr;
    m_count = 0;
}

template <typename T, Heaps::HeapType H>
u32 adList<T, H>::count() const {
    return m_count;
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::reset() {
    m_current = m_start;
}

template <typename T, Heaps::HeapType H>
T* adList<T, H>::current() {
    return reinterpret_cast<T*>(m_current);
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::next() {
    m_current = m_current->m_next;
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::pushBack(T val) {
    if (m_start == nullptr) {
        adListNode<T>* node = new (H) adListNode<T>;
        m_current = node;
        m_end = node;
        m_start = node;
        node->m_val = val;
    } else {
        m_end->m_next = new (H) adListNode<T>;
        m_end->m_next->m_val = val;
        m_end->m_next->m_prev = m_end;
        m_end = m_end->m_next;
    }
    m_count++;
}

template <typename T, Heaps::HeapType H>
T* adList<T, H>::end() {
    return reinterpret_cast<T*>(m_end);
}

template <typename T, Heaps::HeapType H>
T adList<T, H>::get() const {
    return m_current->m_val;
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::set(T val) {
    m_current->m_val = val;
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::prev() {
    m_current = m_current->m_prev;
}

template <typename T, Heaps::HeapType H>
void adList<T, H>::eraseCurrentNode() {
    adListNode<T>* node = m_current;
    if (node->m_prev) {
        node->m_prev->m_next = node->m_next;
    } else {
        m_start = node->m_next;
    }
    node = m_current;
    if (node->m_next) {
        node->m_next->m_prev = node->m_prev;
    } else {
        m_end = node->m_prev;
    }
    node = m_current->m_next;
    m_current = node;
    m_count--;
    if (node) {
        delete node;
    }
}

template void adList<u32, Heaps::MenuInstance>::remove();
template u32 adList<u32, Heaps::MenuInstance>::count() const;
template void adList<u32, Heaps::MenuInstance>::reset();
template u32* adList<u32, Heaps::MenuInstance>::current();
template void adList<u32, Heaps::MenuInstance>::next();
template void adList<u32, Heaps::MenuInstance>::pushBack(u32);
template u32* adList<u32, Heaps::MenuInstance>::end();
template u32 adList<u32, Heaps::MenuInstance>::get() const;
template void adList<u32, Heaps::MenuInstance>::set(u32);
template void adList<u32, Heaps::MenuInstance>::prev();
template void adList<u32, Heaps::MenuInstance>::eraseCurrentNode();
