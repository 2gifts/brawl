#include <cm/cm_controller_photo.h>
#include <gf/gf_pad_status.h>
#include <mt/mt_vector.h>
#include <string.h>
#include <types.h>

void cmPhotoController::setSeamless(bool seamless)
{
    int result = 1;
    int active = seamless || unk60 > 0;
    if (active == 0) {
        if (unkB4 == 0) result = 0;
    }
    unk50 = result;
}

cmPhotoController::unk28_t cmPhotoController::store()
{
    unk28_t state;
    memset(&state, 0, sizeof(state));
    state.unk0.m_x = unk28_0.unk0;
    state.unk0.m_y = unk28_0.unk4;
    state.unk0.m_z = unk28_0.unk8;
    state.unkC.m_x = unk28_0.unkC;
    state.unkC.m_y = unk28_0.unk10;
    state.unkC.m_z = unk28_0.unk14;
    state.unk18 = unk28_0.unk18;
    state.unk1C.m_x = unk28_0.unk1C;
    state.unk1C.m_y = unk28_0.unk20;
    state.unk1C.m_z = unk28_0.unk24;
    return state;
}

void cmPhotoController::restore(unk28_t state)
{
    unk28_0.unk0 = state.unk0.m_x;
    unk28_0.unk4 = state.unk0.m_y;
    unk28_0.unk8 = state.unk0.m_z;
    unk28_0.unkC = state.unkC.m_x;
    unk28_0.unk10 = state.unkC.m_y;
    unk28_0.unk14 = state.unkC.m_z;
    unk28_0.unk18 = state.unk18;
    unk28_0.unk1C = state.unk1C.m_x;
    unk28_0.unk20 = state.unk1C.m_y;
    unk28_0.unk24 = state.unk1C.m_z;
}

extern const struct {
    u8 unk0;
    u8 unk1;
    u32 unk4[4];
} lbl_804078F8[5], lbl_8040795C[5];

cmPhotoController::unk14_t cmPhotoController::getControl(const gfPadStatus* pad, bool menu)
{
    unk14_t control;
    control.unk0 = 0;
    control.unk4.m_x = control.unk4.m_y = 0.0f;
    control.unkC.m_x = control.unkC.m_y = 0.0f;
    if (unk60 > 0) return control;
    const __typeof__(lbl_804078F8[0])* mapping = lbl_804078F8;
    if (menu) mapping = lbl_8040795C;
    for (int count = 0; count < 5; ++count, ++mapping) {
        u32 buttons = mapping->unk4[pad->m_controllerType];
        if (mapping->unk0) {
            if ((pad->m_buttonsCurrentFrame2.bits & buttons) == buttons &&
                (pad->m_buttonsPressedThisFrame.bits & buttons)) {
                control.unk0 |= mapping->unk1;
            }
        } else {
            if (buttons && (pad->m_buttonsCurrentFrame2.bits & buttons) == buttons) {
                control.unk0 |= mapping->unk1;
            }
        }
    }
    if (!menu) {
        getTrackingControl(pad, &control.unk4);
        getOrbitControl(pad, &control.unkC);
    } else {
        getOrbitControlWithMenu(pad, &control.unkC);
    }
    return control;
}
