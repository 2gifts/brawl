// Local BrawlHeaders shadow: declares cmPhotoController's virtual destructor, emitted in cm_camera_controller.o.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <ut/ut_list.h>

class cmPhotoController : public gfCameraController {
public:
    virtual ~cmPhotoController();
    void addCallBack(utListNode* cb);
    void removeCallBack(utListNode* cb);
    void setFreezeMode(void* p1);
};

class soPhotoController : public cmPhotoController {
public:
    bool setFreezeMode();
};
