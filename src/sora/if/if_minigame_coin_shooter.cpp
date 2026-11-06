#include <types.h>
#include <stdarg.h>
#include <if/if_minigame_coin_shooter.h>
#include <if/if_figure_loader.h>
#include <if/if_reward_list_task.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049E560[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049E570[0xc] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
IfCoinShooter* lbl_805A0310 = NULL;
unsigned char lbl_805A0314[0x1] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" void fn_80101810(...) {}


void IfCoinShooter::destroy() {
    if (lbl_805A0310) {
        lbl_805A0310->exit();
    }
    lbl_805A0310 = NULL;
}

void IfCoinShooter::create() {
    lbl_805A0310 = new (Heaps::MenuInstance) IfCoinShooter;
}

void IfCoinShooterLabel::setPos(const Vec3f* position) {
    if (unk0) {
        Vec3f converted = Vec3f::IfCommon_exchangeStageObjPosToInfoPos(position, 0);
        unk0->model()->setTrans(&converted);
    }
}

void IfCoinShooterLabel::reset() {
    IfCoinShooterLabelImpl* impl = unk0;
    if (impl && impl->unk10_2) {
        impl->detach();
        impl->unk10 = 2;
    }
    if (unk0 && this) {
        unk0->unkC = NULL;
        unk0 = NULL;
    }
}

IfCoinShooterLabel::~IfCoinShooterLabel() {
    reset();
}


// HYPOTHESIS: the loader checks are defined by the stand-in of a list task function.
extern "C" bool fn_800FA740(IfRewardListTask* task);

// HYPOTHESIS: start raises a flag in the unrecovered members.
void IfCoinShooter::start() {
    unk1AC[9] = 1;
}

void IfCoinShooterLabelImpl::update() {
}

IfCoinShooterLabel::IfCoinShooterLabel() {
    unk0 = NULL;
}
