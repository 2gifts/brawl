#include <cm/cm_controller_adventure.h>
#include <types.h>

float lbl_80454DD8[8] = { 0.05f, 0.05f, 0.2f, 0.3f, 0.5f, 1.0f, 0.0f, 0.0f };

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0290[0x8] __attribute__((aligned(4))) = { 0 };

void cmAdventureController::setTargetWeightTable(float* table) {
    float f0, f1, f2, f3, f4, f5;
    f5 = table[0];
    f4 = table[1];
    f3 = table[2];
    f2 = table[3];
    f1 = table[4];
    f0 = table[5];
    lbl_80454DD8[0] = f5;
    lbl_80454DD8[1] = f4;
    lbl_80454DD8[2] = f3;
    lbl_80454DD8[3] = f2;
    lbl_80454DD8[4] = f1;
    lbl_80454DD8[5] = f0;
}

void cmAdventureController::resetTarget() {
    searchSubject();
    Vec3f target1 = getTargetPos();
    _5C = target1;
    Vec3f target2 = getTargetPos();
    _50 = target2;
    Vec3f target3 = getTargetPos();
    _2C = target3;
    Vec3f zero(0.0f, 0.0f, 0.0f);
    _80 = zero;
}
