#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <cm/cm_subject.h>
#include <gf/gf_camera.h>
#include <nw4r/ut/ut_LinkList.h>
#include <sr/sr_common.h>
#include <types.h>

extern const float lbl_805A1EA4;
extern const float lbl_805A1EBC;
extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

cmAIController* g_cmAIController;

// HYPOTHESIS: one table holds both default blocks (the constructor addresses the second from the first).
static struct {
    cmAIController::DefaultParam m_param;
    cmAIController::DefaultParam2 m_param2;
} s_defaultParam = {
    { -30.0f, 5.0f, -7.0f, 17.5f, -17.5f, 0.0f, 0.0682f, 60.0f, 120.0f, 0.05f, 0.1f, 120.0f, 900.0f,
      0.15f, 30.0f, 0.1f, 0.2f, 60.0f, 0.1f, 0.001f, 0.1f, 1.0f, 1.0f, 0.6f, 0.6f },
    { 0.02f, 1.0f, 0.14f, 1200.0f, -0.2f, 1.2f },
};

cmAIController* cmAIController::create() {
    if (!g_cmAIController) {
        g_cmAIController = new (Heaps::System) cmAIController();
    }
    return g_cmAIController;
}

cmAIController::cmAIController() {
    unk8 = 0;
    unk94_80 = false;
    unk94_40 = false;
    unk94_20 = false;
    unk94_10 = true;
    unk94_08 = true;
    unk94_04 = false;
    unk94_02 = false;
    unk94_01 = true;
    unk95_80 = true;
    m_devilOffset.m_x = 0.0f;
    m_devilOffset.m_y = 0.0f;
    unk17C = 0.0f;
    unk180 = 1.0f;
    unk184 = 0.0f;
    unk88 = 0;
    unk84 = 0;
    unk90 = 0;
    unk8C = 0;
    unkBC = s_defaultParam.m_param;
    unk120 = s_defaultParam.m_param2;
    unk138 = nullptr;
    unk13C = nullptr;
    unk140 = nullptr;
    m_aspect = gfCameraManager::getManager()->m_aspectRatio;
    m_param.clear();
    m_internalParam.clear();
    m_userRot.m_z = 0.0f;
    m_userRot.m_y = 0.0f;
    m_userRot.m_x = 0.0f;
    m_userPos.m_y = 0.0f;
    m_userPos.m_x = 0.0f;
    unk15C.m_left = -10.0f;
    unk15C.m_right = 10.0f;
    unk15C.m_up = 20.0f;
    unk15C.m_down = -20.0f;
    unk16C.m_left = -10.0f;
    unk16C.m_right = 10.0f;
    unk16C.m_up = 20.0f;
    unk16C.m_down = -20.0f;
    m_userDistMin = 0.0f;
    unk188.m_left = -3.4028235e38f;
    unk188.m_right = 3.4028235e38f;
    unk188.m_up = 3.4028235e38f;
    unk188.m_down = -3.4028235e38f;
}

cmAIController::~cmAIController() {
    g_cmAIController = NULL;
}

void cmAIController::reset() {
    unk88 = 0;
    unk8 = 0;
    unk84 = 0;
    unk90 = 0;
    unk8C = 0;
    unk17C = 0.0f;
    unk180 = 1.0f;
    unk184 = 0.0f;
    unk94_80 = false;
    unk94_10 = true;
    unk94_08 = true;
    unk94_40 = false;
    unk94_20 = false;
    unk94_04 = false;
    unk94_02 = false;
    unk94_01 = true;
    unk95_80 = true;
    m_devilOffset.m_x = 0.0f;
    m_devilOffset.m_y = 0.0f;
    m_userRot.m_z = 0.0f;
    m_userRot.m_y = 0.0f;
    m_userRot.m_x = 0.0f;
    m_userPos.m_y = 0.0f;
    m_userPos.m_x = 0.0f;
    m_userDistMin = 0.0f;
    m_param.clear();
    m_internalParam.clear();
    unk15C.m_left = -10.0f;
    unk15C.m_right = 10.0f;
    unk15C.m_up = 20.0f;
    unk15C.m_down = -20.0f;
    unk16C.m_left = -10.0f;
    unk16C.m_right = 10.0f;
    unk16C.m_up = 20.0f;
    unk16C.m_down = -20.0f;
    unk188.m_left = -3.4028235e38f;
    unk188.m_right = 3.4028235e38f;
    unk188.m_up = 3.4028235e38f;
    unk188.m_down = -3.4028235e38f;
    gfCamera* camera = &m_cameraManager->m_cameras[0];
    if (camera) {
        camera->init();
    }
    camera = gfCameraManager::getManager()->getCamera(1);
    if (camera) {
        camera->init();
        camera->m_aspect = 1.4848485f;
    }
}

void cmAIController::setUserOffset(const Vec3f& rot, const Vec2f& pos) {
    m_userRot = rot;
    m_userPos = pos;
}

void cmAIController::updateCameraPosTarget() {
    gfCamera* camera = &m_cameraManager->m_cameras[0];
    m_param.m_pos = camera->m_targetPos;
    m_param.m_eye = camera->m_centerPos;
    m_internalParam.m_pos = m_param.m_pos;
    m_internalParam.m_eye = m_param.m_eye;
}

void cmAIController::resetDevilOffset() {
    m_devilOffset.m_y = 0.0f;
    m_devilOffset.m_x = 0.0f;
}

void cmAIController::accDevilOffset(Vec2f* offset) {
    m_devilOffset.m_x += offset->m_x;
    m_devilOffset.m_y += offset->m_y;
}

void cmAIController::setUserDistMin(float distance) {
    m_userDistMin = distance;
}

void cmAIController::setCameraParam(const float* param) {
    float value24;
    float value23;
    float value22;
    float value21;
    float value20;
    float value19;
    float value18;
    float value17;
    float value16;
    float value15;
    float value14;
    float value13;
    float value12;
    float value11;
    float value10;
    float value9;
    float value8;
    float value7;
    float value6;
    float value5;
    float value4;
    float value3;
    float value2;
    float value1;
    float value0;
    value0 = param[4];
    value1 = param[0];
    value2 = param[1];
    value3 = param[2];
    value4 = param[3];
    value5 = param[5];
    value6 = param[6];
    value7 = param[7];
    value8 = param[8];
    value9 = param[9];
    value10 = param[10];
    value11 = param[11];
    value12 = param[12];
    value13 = param[13];
    value14 = 38.0f;
    value15 = param[14];
    value16 = param[15];
    value17 = param[16];
    value18 = lbl_805A1EBC;
    value19 = param[17];
    value20 = param[18];
    value21 = param[19];
    value22 = param[20];
    value23 = param[21];
    value24 = param[22];
    unkBC.unk[0] = value0;
    unkBC.unk[1] = value1;
    unkBC.unk[2] = value2;
    unkBC.unk[3] = value3;
    unkBC.unk[4] = value4;
    unkBC.unk[5] = value5;
    unkBC.unk[6] = value6;
    unkBC.unk[7] = value7;
    unkBC.unk[8] = value8;
    unkBC.unk[9] = value9;
    unkBC.unk[10] = value10;
    unkBC.unk[11] = value11;
    unkBC.unk[12] = value12;
    unkBC.unk[13] = value13;
    unkBC.unk[14] = value14;
    unkBC.unk[15] = value15;
    unkBC.unk[16] = value16;
    unkBC.unk[17] = value17;
    unkBC.unk[18] = value18;
    unkBC.unk[19] = value19;
    unkBC.unk[20] = value20;
    unkBC.unk[21] = value21;
    unkBC.unk[22] = value22;
    unkBC.unk[23] = value23;
    unkBC.unk[24] = value24;
}

void cmAIController::normalUpdate(void* range, cmCameraParam* param, void* state, float aspect, float rate) {
    float speed = 1.0f;
    if (unk138) speed = unk138->m_speed;
    speed *= rate;
    getAtShiftNormal(param, range, speed);
    getEyeShiftNormal(param, state, speed);
    if (unk94_01) {
        CameraController* controller = CameraController::getInstance();
        Rect2D bounds;
        __memcpy(&bounds, &controller->unk158, sizeof(bounds));
        if (unk94_80) {
            float x = m_devilOffset.m_x;
            float y = m_devilOffset.m_y;
            bounds.m_left -= x;
            bounds.m_right -= x;
            bounds.m_up -= y;
            bounds.m_down -= y;
        }
        correctRangeEyeAt(&param->m_eye, &param->m_pos, &bounds, lbl_805A1EA4 * param->m_fovy, aspect, 1.0f, 1.0f);
    }
}


void cmAIController::draw() {
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
         it != lbl_8049DEDC.GetEndIter(); ++it) {
        if (it->m_isLoupeTarget) it->draw(1.0f);
    }
}
