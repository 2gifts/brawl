#include <gf/gf_scene.h>
#include <sr/sr_common.h>
#include <types.h>

// The sequence of a new challenger (sora_scene): the challenger approach scene, the fight and the result.
// HYPOTHESIS: the member names follow how start/setNext use them; setNext and setupChallenger are not written yet.
class sqChallenger : public gfSequence {
public:
    int m_step;           // 0x08
    int m_nextStep;       // 0x0C
    u8 m_memoryLayout;    // 0x10 the memory layout of the next scene
    u8 m_startLayout;     // 0x11 the memory layout the sequence was started with
    u8 m_unk12[2];

    sqChallenger(const char* name) : gfSequence(name), m_step(0) { }
    static sqChallenger* create();
    virtual void start();
    virtual void setNext();
    virtual void exit();
    void setupChallenger();
};
static_assert(sizeof(sqChallenger) == 0x14, "Class is wrong size!");

sqChallenger* sqChallenger::create() {
    return new (Heaps::GlobalMode) sqChallenger("sqChallenger");
}

void sqChallenger::start() {
    m_startLayout = gfSceneManager::getInstance()->unk31C;
    m_step = 0;
}

void sqChallenger::exit() { }
