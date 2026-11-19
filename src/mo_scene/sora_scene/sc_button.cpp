#include <ef/ef_screen.h>
#include <ft/ft_manager.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_button.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <sc/sc_button.h>
#include <sr/sr_common.h>
#include <revolution/os.h>
#include <types.h>

// HYPOTHESIS: the loader of the practice fighters (a task at scMelee+0x08); the scene tells it to pause or go on through
// two of its virtual functions. TODO: clean up - name the class and the two functions; the dummies only place the slots.
class scButtonLoader : public gfTask {
public:
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual void unk70();
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80();
    virtual void unk84();
    virtual void unk88();
    virtual void unk8C();
    virtual void pausePractice();       // 0x90
    virtual void startPractice();       // 0x94
};
// One fill of the screen (the handle is a byte, see efScreen::requestFill).
struct scButtonFill {
    u8 m_index;
};

// A static pair of the module that its initializer function fills in (the same shape as the one of scClearGetter).
// HYPOTHESIS: the names; the pairs are not read in this unit.
struct scButtonEntry {
    int m_id;
    int m_shown;
    scButtonEntry(int id, int shown) : m_id(id), m_shown(shown) { }
};
static scButtonEntry s_entryHidden(0xFF, 0);
static scButtonEntry s_entryShown(0xFF, 1);

scButton::scButton() : scMelee("scButton") {
    m_buttonConfigNo = 0;
    m_padNo = 0;
    m_work = NULL;
}

scButton* scButton::create() {
    return new (Heaps::GlobalMode) scButton();
}

scButton::~scButton() { }

void scButton::setModule() {
    setLoadModule("sora_menu_main.rel", false);
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_sel_stage.rel", false);
}

void scButton::start() {
    scMelee::start();
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_objProcess = muObjProcessTask::create(Heaps::MenuInstance);
    m_work->m_task = muButtonTask::create(this, Heaps::MenuInstance);
    // TODO: clean up - the fill color is built in a temporary first (b, g, r, a) and then copied; a plain initialized GXColor
    // gives a different instruction order.
    GXColor fillColor;
    fillColor.b = 0;
    fillColor.g = 0;
    fillColor.r = 0;
    m_work->m_isPractice = 0;
    m_work->m_unk9 = 1;
    fillColor.a = 0xFF;
    m_work->m_contNo = 0;
    GXColor color = fillColor;
    u32 handle = g_efScreen->requestFill(1.0f, 7, 0x80, &color);
    m_work->m_fill = handle >> 24;
}

int scButton::getEndNum() {
    return m_endNum;
}

s32 scButton::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_objProcess->exit();
    m_work->m_task->exit();
    gfHeapManager::free(m_work);
    m_work = NULL;
    g_gfSceneRoot->removeInfoCamResAnm();
    scMelee::exit();
    return 0;
}

// TODO: clean up - muObjProcessTask only declares its size here; the object list and its count are at +0x40 and +0x44.
void scButton::setMenuObjs(MuObject** objs, int num) {
    muObjProcessTask* objProcess = m_work->m_objProcess;
    *reinterpret_cast<MuObject***>(reinterpret_cast<u8*>(objProcess) + 0x40) = objs;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(objProcess) + 0x44) = num;
}

void scButton::addObj(nw4r::g3d::ScnObj* obj) {
    g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(8), reinterpret_cast<nw4r::g3d::ScnMdl*>(obj));
}

void scButton::removeObj(nw4r::g3d::ScnObj* obj) {
    g_gfSceneRoot->remove(reinterpret_cast<nw4r::g3d::ScnMdl*>(obj));
}

void scButton::removeAllObj() {
    g_gfSceneRoot->clear(static_cast<gfSceneRoot::LayerType>(8));
}

void scButton::setCamAnm(nw4r::g3d::ResAnmScn* anm) {
    g_gfSceneRoot->setInfoCamResAnm(anm);
}

void scButton::updateFrame() {
    gfSceneRoot* root = g_gfSceneRoot;
    root->unk44->UpdateFrame();
    root->layerUpdateFrame(8);
}

s32 scButton::process() {
    if (unkD8_b6 == 1 && !unkD8_b4) {
        if (m_work->m_unk9) {
            m_work->m_unk9 = 0;
            scButtonFill fill;
            fill.m_index = m_work->m_fill;
            g_efScreen->cancelFill(reinterpret_cast<efScreenHandle*>(&fill), 4.0f);
        }
        if (m_loaderPlayers[0] != NULL) {
            ftManager* manager = g_ftManager;
            int entryId = manager->getEntryId(0);
            if (entryId != -1) {
                u32 isPractice = m_work->m_task->m_work->m_step == 11;
                if (m_work->m_isPractice != isPractice) {
                    m_work->m_isPractice = isPractice;
                    if (isPractice) {
                        static_cast<scButtonLoader*>(m_loaderPlayers[0])->startPractice();
                        OSReport("scButton: practice on
");
                        manager->setContNo(entryId, m_work->m_contNo);
                    } else {
                        static_cast<scButtonLoader*>(m_loaderPlayers[0])->pausePractice();
                        OSReport("scButton: practice off
");
                    }
                }
            }
        }
    }
    return scMelee::process();
}

bool scButton::setContNo(int contNo) {
    m_work->m_contNo = contNo;
    if (unkD8_b6 == 0 || unkD8_b4 != 0) {
        return false;
    }
    ftManager* manager = g_ftManager;
    manager->setContNo(manager->getEntryId(0), contNo);
    return true;
}
