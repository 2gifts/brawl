// Local BrawlHeaders shadow: adds control storage, pause state, and clearControl.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <ut/ut_list.h>

class cmPhotoController : public gfCameraController {
public:
    u8 unk8[0xC];
    u8 unk14[0x14];
    u8 unk28[0x38];
    s32 unk60;

    void clearControl();
    void addCallBack(utListNode* cb);
    void removeCallBack(utListNode* cb);
    void setFreezeMode(void* p1);
};

class soPhotoController : public cmPhotoController {
public:
    bool setFreezeMode();
};
