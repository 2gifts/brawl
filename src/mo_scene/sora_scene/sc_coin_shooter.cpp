#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <ph/ph_Interface.h>
#include <sr/sr_common.h>
#include <types.h>

// The minigame object of sora_minigame.rel (0x500 bytes; its constructor is not named there) and the object that the scene
// keeps for the minigame (it has a virtual destructor).
class ifCoinShooterMain : public gfTask {
public:
    ifCoinShooterMain();
    u8 m_unk40[0x500 - 0x40];
};
class ifCoinShooterObj {
public:
    ifCoinShooterObj();
    virtual ~ifCoinShooterObj();
    u8 m_unk04[0x28 - 0x04];
};

// The havok system of the game, the minigame (module sora_minigame) and the object of the scene.
extern phInterface* lbl_8049DEC8;
extern void* lbl_20_bss_8;
extern ifCoinShooterObj* lbl_1_bss_60;
// Leaves the minigame (sora_minigame.rel).
extern "C" void fn_20_9044(gfTask* task);

// The task of the coin shooter minigame of sora_minigame.rel. Only what the scene reads is declared.
class ifCoinShooterTask : public gfTask {
public:
    u8 m_unk40[4];
    u8 m_isEnd;                         // 0x44 the minigame is over
};

// The scene of the coin shooter minigame (module sora_scene): starts the havok system and the minigame of sora_minigame.rel
// and goes back when the minigame is over.
class scCoinShooter : public gfScene {
public:
    int m_unk08;
    ifCoinShooterTask* m_task;          // 0x0C

    scCoinShooter() : gfScene("scCoinShooter"), m_unk08(0), m_task(NULL) { }
    static scCoinShooter* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scCoinShooter) == 0x10, "Class is wrong size!");

scCoinShooter* scCoinShooter::create() {
    return new (Heaps::GlobalMode) scCoinShooter();
}

void scCoinShooter::setModule() {
    setLoadModule("sora_minigame.rel", false);
}

s32 scCoinShooter::process() {
    if (m_task->m_isEnd != 0) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
    }
    return 0;
}

void scCoinShooter::start() {
    if (lbl_8049DEC8 == NULL) {
        phInterface* physics = new (Heaps::Physics) phInterface();
        lbl_8049DEC8 = physics;
        static_cast<gfTask*>(physics)->init();
    }
    ifCoinShooterMain* game = new (Heaps::Physics) ifCoinShooterMain();
    lbl_20_bss_8 = game;
    m_task = reinterpret_cast<ifCoinShooterTask*>(game);
    lbl_1_bss_60 = new (Heaps::MenuInstance) ifCoinShooterObj();
}

s32 scCoinShooter::exit() {
    if (lbl_1_bss_60 != NULL) {
        delete lbl_1_bss_60;
    }
    fn_20_9044(m_task);
    m_task->exit();
    if (lbl_8049DEC8 != NULL) {
        lbl_8049DEC8->removeHavokSystem();
        lbl_8049DEC8->deleteInstance();
    }
    return 0;
}
