#include <gf/gf_pad_system.h>
#include <cm/cm_controller_fig_disp1.h>
#include <gf/gf_pad_status.h>
#include <math.h>
#include <mt/mt_trig.h>
#include <nw4r/math/math_arithmetic.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
// TODO: clean up - names unknown; two angles set up by the static initializer
float lbl_805A0298 = mtConvDegToRad(-23.0f);
float lbl_805A029C = mtConvDegToRad(-10.0f);

cmFigDisp1Controller::cmFigDisp1Controller() : gfCameraController() {
    unk3C = 0;
    setCamParamByObjWidth(26.0f);
}

void cmFigDisp1Controller::setCamParamByObjWidth(float width) {
    float objWidth;
    float minAngle = 0.040142573f;
    unk8 = 0.0f;
    unkC = 265.0f;
    unk10 = 825.0f;
    float pitch = -unkC / unk10;
    objWidth = nw4r::math::FSelect(width - 26.0f, width, 26.0f);
    objWidth = nw4r::math::FSelect(objWidth - 500.0f, 500.0f, objWidth);
    unk14 = atan(pitch);
    unk18 = 0.0f;
    float angle = atan((objWidth / 3.5f) / (unk10 - objWidth / 2.0f));
    unk24 = angle;
    unk20 = -angle;
    angle = atan((objWidth / 8.0f) / (unk10 - objWidth / 2.0f));
    unk2C = unk14 + angle;
    unk28 = unk14 - angle;
    unk34 = 2.0f * (float)atan((objWidth / 2.0f) / unk10);
    if (unk34 < minAngle) {
        unk34 = minAngle;
    }
    unk30 = minAngle;
    unk38 = width;
    unk1C = minAngle + 0.8f * (unk34 - minAngle);
}

Vec2f cmFigDisp1Controller::getCameraRotControl(const gfPadStatus& status) {
    Vec2f rotation;
    if (status.m_controllerType == gfPadType::WIIMOTE) {
        rotation.m_y = 0.0f;
        rotation.m_x = 0.0f;
        if (status.m_buttonsCurrentFrame2.bits & 2) rotation.m_x = 0.4f;
        if (status.m_buttonsCurrentFrame2.bits & 1) rotation.m_x = -0.4f;
        if (status.m_buttonsCurrentFrame2.bits & 8) rotation.m_y = 0.4f;
        if (status.m_buttonsCurrentFrame2.bits & 4) rotation.m_y = -0.4f;
    } else {
        s8 magnitudeX = status.m_stickX < 0 ? -status.m_stickX : status.m_stickX;
        if (magnitudeX <= 30) rotation.m_x = 0.0f;
        else {
            rotation.m_x = (magnitudeX - 30) / 98.0f;
            if (status.m_stickX < 0) rotation.m_x = -rotation.m_x;
        }
        s8 magnitudeY = status.m_stickY < 0 ? -status.m_stickY : status.m_stickY;
        if (magnitudeY <= 30) rotation.m_y = 0.0f;
        else {
            rotation.m_y = (magnitudeY - 30) / 98.0f;
            if (status.m_stickY < 0) rotation.m_y = -rotation.m_y;
        }
    }
    return rotation;
}

// TODO: clean up, give variables real names
namespace jj_800A9AD4 {

struct StickResult {
    f32 x;
    f32 y;
};

struct PadOutput {
    u8 buttonMask;
    u8 pad[3];
    f32 x;
    f32 y;
};

struct PadMatchSub {
    u32 mask;
    u8 flags;
    u8 padding[3];
};

struct PadMatchEntry {
    u8 typeFlag;
    u8 resultMask;
    u8 pad[2];
    PadMatchSub sub[4];
};

extern const PadMatchEntry lbl_804079C0[3];
extern const f32 lbl_805A20FC;
extern const f32 lbl_805A212C;

extern "C" StickResult fn_800A9C7C(int, gfPadStatus*);

inline bool matchesPad(u32 source, const PadMatchSub& sub) {
    u32 matchedBits = source & sub.mask;
    if (((sub.flags >> 7) & 1) == 0) {
        if (matchedBits != 0) {
            return true;
        }
    } else if (matchedBits == sub.mask) {
        return true;
    }
    return false;
}

// TODO: clean up, give variables real names
inline u8 getPadMask(const gfPadStatus* pad) {
    u8 mask = 0;
    u32 current;
    u32 pressed;
    pressed = pad->m_buttonsPressedThisFrame.bits;
    current = pad->m_buttonsCurrentFrame2.bits;
    u32 controllerOffset = pad->m_controllerType * 8;
    const PadMatchEntry* entry;
    const PadMatchEntry* mapping;
    mapping = lbl_804079C0;
    entry = mapping;

    for (int j = 0; j < 3; j++, mapping++, entry++) {
        const PadMatchSub& sub =
            *(const PadMatchSub*)((const char*)mapping + controllerOffset + 4);
        u32 source = current;

        if (entry->typeFlag != 0) {
            source = pressed;
        }

        if (matchesPad(source, sub)) {
            mask |= entry->resultMask;
        }
    }

    return mask;
}

inline int isStickEmpty(PadOutput* self, f32 threshold) {
    int update = 0;
    int test = 0;

    if ((f32)(double)nw4r::math::FAbs(self->x) < threshold) {
        test = 1;
    }

    if (test != 0) {
        int secondTest = 0;
        if ((f32)(double)nw4r::math::FAbs(self->y) < threshold) {
            secondTest = 1;
        }
        if (secondTest != 0) {
            update = 1;
        }
    }

    return update;
}

// TODO: clean up, give variables real names
extern "C" void fn_800A9AD4(PadOutput* self, int arg1) {
    self->buttonMask = 0;
    f32 threshold = lbl_805A212C;
    self->y = lbl_805A20FC;
    self->x = lbl_805A20FC;

    for (int i = 0; i < 8; i++) {
        gfPadStatus pad;
        g_gfPadSystem->getSysPadStatus(i, &pad);

        if (pad.m_error == 0) {
            if (isStickEmpty(self, threshold)) {
                StickResult result = fn_800A9C7C(arg1, &pad);
                self->x = result.x;
                self->y = result.y;
            }

            self->buttonMask |= getPadMask(&pad);
        }
    }
}
}

