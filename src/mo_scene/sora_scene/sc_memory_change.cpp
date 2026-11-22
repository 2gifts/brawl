#include <gf/gf_scene.h>
#include <sr/sr_common.h>
#include <st/loader/st_loader_manager.h>
#include <types.h>

// A word of the sora_scene data the memory change scene starts with.
// TODO: clean up - name it (its owner unit is not decompiled).
extern void* lbl_1_data_128;

// The scene that switches the memory layout between modes (module sora_scene).
// HYPOTHESIS: the member names; the constructor, process and the loader parts are not written yet.
class scMemoryChange : public gfScene {
public:
    void* m_unk8;   // 0x08
    int m_step;     // 0x0C
    u8 m_unk10[2];
    u8 m_unk12;     // 0x12
    u8 m_unk13[0x3C - 0x13];

    scMemoryChange();
    static scMemoryChange* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
};
static_assert(sizeof(scMemoryChange) == 0x3C, "Class is wrong size!");

scMemoryChange* scMemoryChange::create() {
    return new (Heaps::GlobalMode) scMemoryChange();
}

void scMemoryChange::start() {
    stLoaderManager::create();
    m_unk8 = lbl_1_data_128;
    m_step = 0;
    m_unk12 = 0;
}

s32 scMemoryChange::exit() {
    return 0;
}
