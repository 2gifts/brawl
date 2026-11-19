#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <sr/sr_common.h>
#include <types.h>

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
