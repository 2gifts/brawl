// Local BrawlHeaders shadow: declares cmPhotoController's virtual destructor (emitted in cm_camera_controller.o), its control storage, pause state and clearControl.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>
#include <ut/ut_list.h>

struct gfPadStatus;

class cmPhotoController : public gfCameraController {
public:
    // HYPOTHESIS: unnamed snapshot type recovered from store and restore.
    typedef struct {
        Vec3f unk0, unkC;
        int unk18;
        Vec3f unk1C;
    } unk28_t;
    u8 unk8[0xC];
    // HYPOTHESIS: unnamed control result contains button flags and two vectors.
    typedef struct {
        u8 unk0;
        Vec2f unk4, unkC;
    } unk14_t;
    u8 unk14[0x14];
    union {
        u8 unk28[0x38];
        struct {
            struct {
                float unk0, unk4, unk8, unkC, unk10, unk14;
                int unk18;
                float unk1C, unk20, unk24;
            } unk28_0;
            u8 unk50;
            u8 unk51[0xF];
        };
    };
    s32 unk60;
    u8 unk64[0x50];
    int unkB4;

    void clearControl();
    unk14_t getControl(const gfPadStatus* pad, bool menu);
    void getTrackingControl(const gfPadStatus* pad, Vec2f* control);
    void getOrbitControl(const gfPadStatus* pad, Vec2f* control);
    void getOrbitControlWithMenu(const gfPadStatus* pad, Vec2f* control);
    void setSeamless(bool seamless);
    unk28_t store();
    void restore(unk28_t state);

    virtual ~cmPhotoController();
    void addCallBack(utListNode* cb);
    void removeCallBack(utListNode* cb);
    void setFreezeMode(void* p1);
};

class soPhotoController : public cmPhotoController {
public:
    bool setFreezeMode();
};
