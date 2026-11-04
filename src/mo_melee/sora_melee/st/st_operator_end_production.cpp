#include <cm/cm_camera_controller.h>
#include <st/operator/st_operator_end_production.h>
#include <types.h>

stOperatorEndProduction::stOperatorEndProduction() :
    stOperator("stOperatorEndProduction"), unk9C(0.0f) {
    unk2C_b1 = false;
    m_state = 3;
    unk48 = 0;
    unk4C = 0;
    unk98 = 1.0f;
    unk9C = 0.0f;
    unkA0 = 0.0f;
    unkA4 = false;
    unk80 = 0.0f;
    unk84 = 0.0f;
    unk88 = 0.0f;
    unk8C = 0.0f;
    unk90 = 0.0f;
    unk94 = 0.0f;
}

stOperatorEndProduction::~stOperatorEndProduction() { }

void stOperatorEndProduction::processBegin() {
    if (!isPause()) {
        if (!isExit()) {
            if (m_isStart) {
                unk9C -= 1.0f;
                CameraController::getInstance()->m_currentController->update(unkA0);
                unkA0 *= 0.928f;
                if (unk9C < 0.001f) {
                    unk9C = 0.0f;
                }
                if (unk9C == 0.0f) {
                    unkA4 = true;
                }
            }
        }
    }
}
