#include <cm/cm_controller_photo.h>
#include <gf/gf_pad_status.h>
#include <mt/mt_vector.h>
#include <string.h>
#include <types.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_subject.h>

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

void cmPhotoController::update(float deltaFrame)
{
    switch (unkC) {
    case 0: updateShot(); break;
    case 1: updateMoveFig(); break;
    }
}

void cmPhotoController::setFigPosList(int figNo, const Vec3f* position)
{
    unk6C[figNo].unk0 = position->m_x;
    unk6C[figNo].unk4 = position->m_y;
    unk6C[figNo].unk8 = position->m_z;
    unk6C[figNo].unkC |= 0x80;
}

void cmPhotoController::removeFigPosList(int figNo)
{
    unk6C[figNo].unkC_0 = 0;
}

void cmPhotoController::updateControl(const gfPadStatus* pad, bool menu)
{
    unk14_t control = getControl(pad, menu);
    unk14_0 = control.unk0;
    __memcpy(&unk18, &control.unk4, sizeof(unk18));
    __memcpy(&unk20, &control.unkC, sizeof(unk20));
}

void cmPhotoController::updateLockFigNo(bool* seamless, int* figNo, const unk14_t* control)
{
    int action;
    if (control->unk0 & 0x10) action = 1;
    else if (control->unk0 & 4) action = 2;
    else {
        action = 0;
        if (control->unk0 & 8) action = 3;
    }
    switch (action) {
    case 0: break;
    case 1: *figNo = -1; break;
    case 2:
        if (*figNo < -1) *figNo = -1;
        do {
            ++*figNo;
            if (*figNo >= 4) {
                *figNo = -1;
                break;
            }
        } while (!unk6C[*figNo].unkC_0);
        break;
    case 3:
        if (*figNo < 0) *figNo = 4;
        do {
            --*figNo;
            if (*figNo < 0) {
                *figNo = -1;
                break;
            }
        } while (!unk6C[*figNo].unkC_0);
        break;
    }
}

void cmPhotoController::updateLockPlayerNo(bool* seamless, int* playerNo, const unk14_t* control)
{
    int action;
    if (control->unk0 & 0x10) action = 1;
    else if (control->unk0 & 4) action = 2;
    else {
        action = 0;
        if (control->unk0 & 8) action = 3;
    }
    if (*seamless && action) {
        *seamless = false;
        return;
    }
    switch (action) {
    case 0: break;
    case 1: *playerNo = -1; break;
    case 2: {
        cmSubject* subject;
        if (*playerNo < -1) *playerNo = -1;
        do {
            int player = ++*playerNo;
            if (player >= 7) {
                *playerNo = -1;
                break;
            }
            u32 disabled = CameraController::getInstance()->unk45;
            if (disabled & (1u << player)) {
                subject = 0;
            } else {
                subject = cmSubjectList::getSubjectByPlayerNo((u8)player);
                bool active = subject && subject->m_isLoupeTarget;
                switch (active) {
                case false: subject = 0; break;
                default: break;
                }
            }
        } while (!subject);
        break;
    }
    case 3: {
        cmSubject* subject;
        if (*playerNo < 0) *playerNo = 7;
        do {
            int player = --*playerNo;
            if (player < 0) {
                *playerNo = -1;
                break;
            }
            u32 disabled = CameraController::getInstance()->unk45;
            if (disabled & (1u << player)) {
                subject = 0;
            } else {
                subject = cmSubjectList::getSubjectByPlayerNo((u8)player);
                bool active = subject && subject->m_isLoupeTarget;
                switch (active) {
                case false: subject = 0; break;
                default: break;
                }
            }
        } while (!subject);
        break;
    }
    }
}
