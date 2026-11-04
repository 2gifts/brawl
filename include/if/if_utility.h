// Local BrawlHeaders shadow: utility function and class declarations.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <ut/ut_list.h>

namespace utility {
class Loader;
class AutoFile;
}

// The list that every utility::Loader is on while it exists (utility::AutoList<utility::Loader>), and one more word.
// HYPOTHESIS: the list belongs to AutoList<Loader>; it has no name in the maps (defined in if_utility.cpp).
struct utLoaderList {
    utList m_list;
    u32 m_unkC;
};
extern utLoaderList lbl_8049E550;


namespace utility {

// A node of a list that knows the list it is on. The class names are from the RTTI of the menu module.
template <class T>
class List {
public:
    class Node : public utListNode {
    public:
        utList* m_list;             // 0x08 the list that the node is on

        Node() : m_list(NULL) {
            m_next = NULL;
            m_prev = NULL;
        }
        ~Node() {
            if (m_list != NULL) {
                m_list->removeExist(this);
                m_list = NULL;
            }
        }
    };
};

// A node that puts itself on the list of all the nodes of its kind when it is made.
template <class T>
class AutoList {
public:
    static utList& getList();

    class Node : public List<T>::Node {
    public:
        Node() {
            getList().addTail(this);
            this->m_list = &getList();
        }
        ~Node() {
            getList().removeExist(this);
            this->m_list = NULL;
        }
        // Takes the node off the list again (an object that is loaded by an outer loader).
        // HYPOTHESIS: the name; AutoResFile does it to its file.
        void unlink() {
            getList().removeExist(this);
            this->m_list = NULL;
        }
    };
};

// Something that loads in the background: Loader::isLoadFinish asks every loader that exists if it is done.
class Loader : public AutoList<Loader>::Node {
public:
    virtual bool loading() = 0;
    virtual ~Loader() { }
    static bool isLoadFinish();
};

template <>
inline utList& AutoList<Loader>::getList() {
    return lbl_8049E550.m_list;
}

// A file that is read in the background.
class AutoFile : public Loader {
public:
    gfFileIOHandle m_handle;        // 0x10
    bool m_isRequested;             // 0x14 a read was started (clear has something to drop)

    AutoFile() : m_isRequested(false) { }
    virtual bool loading() { return m_handle.isReady(); }
    virtual ~AutoFile() {
        clear();
        m_handle.release();
    }
    // Drops the file (cancels the read or frees the buffer).
    // HYPOTHESIS: the names of clear and request; request is a real member call (the callers compute this first).
    void clear();
    // Starts to read a file.
    void request(const char* path, Heaps::HeapType heap, int unk);
};

// A resource file (brres) that is read in the background and set up once it is there.
class AutoResFile : public nw4r::g3d::ResFile, public Loader {
public:
    AutoFile m_file;                // 0x14
    bool m_isInit;                  // 0x2C the resource file was set up

    AutoResFile() : m_isInit(false) { m_file.unlink(); }
    virtual bool loading();
    virtual ~AutoResFile() {
        m_file.clear();
        static_cast<nw4r::g3d::ResFile&>(*this) = nw4r::g3d::ResFile();
        m_isInit = false;
    }
    // Drops the file and starts to read another one.
    void request(const char* path, Heaps::HeapType heap, int unk) {
        m_file.clear();
        static_cast<nw4r::g3d::ResFile&>(*this) = nw4r::g3d::ResFile();
        m_isInit = false;
        m_file.request(path, heap, unk);
    }
};

class StandardTask : public gfTask {
public:
    // HYPOTHESIS: the fields of StandardTask are known only from its constructor (inline in StandardMenu's).
    u32 m_unk40 : 16;
    u8 m_unk42_7 : 1;               // set by the constructor
    u8 m_unk42_6 : 1;
    u8 m_unk42_5 : 1;               // set by muVCTrialTask
    u8 m_unk42_3 : 2;               // cleared by the constructor
    u8 m_unk42_0 : 3;
    Heaps::HeapType m_resHeap;      // 0x44 the heap of the files of the menu
    AutoResFile m_cameraRes;        // 0x48 the default camera
    char m_cameraPath[0x84];        // 0x7C

    virtual void processDefault();
    virtual ~StandardTask();

    virtual bool preLoad();
    virtual void loadResource() = 0;
    virtual bool checkLoadFinish() const;
    virtual void setupScene();
    virtual void createModel() = 0;
    virtual void update() = 0;
};

class StandardMenu : public StandardTask {
public:
    muMenuController m_controller;  // 0x100
    u32 m_input;                    // 0x1A8 the menu buttons of this frame (1: ?, 2: decide, 4: cancel, 8/0x10: page)

    // HYPOTHESIS: the parameters are the pad mask of the controller and the heap of the files.
    StandardMenu(const char* name, int pads, Heaps::HeapType resHeap);
    virtual void processDefault();
    virtual ~StandardMenu() { }
};

// A task that fades the screen out; it is done when the fade is.
class FadeOutTask : public gfTask {
public:
    // HYPOTHESIS: the names of create and isEnd.
    static FadeOutTask* create(Heaps::HeapType heap);
    virtual ~FadeOutTask();
    virtual bool isEnd();
};

class DepthScaler : public nw4r::g3d::IScnObjCallback {
public:
    float unk4;
    float unk8;
    DepthScaler(MuObject& object, float first, float second);
    virtual ~DepthScaler();
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing, nw4r::g3d::ScnObj*, u32, void*);
    virtual void ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing, nw4r::g3d::ScnObj*, u32, void*);
};

}
