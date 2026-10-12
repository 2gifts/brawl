#include <st_metalgear/st_metalgear.h>
#include <types.h>
#include <if/if_smash_appear.h>

extern "C" IfFoxSmashAppearTask* fn_800F609C(IfFoxSmashAppearTask* task, gfArchive* archive);

IfFoxSmashAppearTask* IfFoxSmashAppearTask::create(gfArchive* archive) {
    // HYPOTHESIS: heap 0xF is Heaps::StageInstance by value; the heap's intent is unverified
    IfFoxSmashAppearTask* task = static_cast<IfFoxSmashAppearTask*>(
            operator new(0x428, Heaps::StageInstance));
    if (task != nullptr) {
        task = fn_800F609C(task, archive);
    }
    return task;
}


extern "C" IfSnakeSmashAppearTask* fn_800F6FB4(IfSnakeSmashAppearTask* task, gfArchive* archive);

IfSnakeSmashAppearTask* IfSnakeSmashAppearTask::create(gfArchive* archive) {
    IfSnakeSmashAppearTask* task = static_cast<IfSnakeSmashAppearTask*>(
            operator new(0x430, Heaps::StageInstance));
    if (task != nullptr) {
        task = fn_800F6FB4(task, archive);
    }
    return task;
}
