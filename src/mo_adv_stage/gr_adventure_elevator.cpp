#include <sora_adv_stage/gr_adventure_elevator.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <st/stage.h>

grAdventureElevator* grAdventureElevator::create(int mdlIndex, const char* taskName) {
    grAdventureElevator* ground = new (Heaps::StageInstance) grAdventureElevator(taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

grAdventureElevator::~grAdventureElevator() {
    if (m_floors != NULL) {
        delete[] m_floors;
    }
}

void grAdventureElevator::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_state) {
    case 1:
        moveFloor();
        break;
    case 2:
        if (m_timer > 60.0f) {
            m_state = 0;
        }
        break;
    }
    m_timer += deltaFrame;
    grGimmick::updateCallback(0);
}

void grAdventureElevator::getNextFloorTime() {
    unk184 = 0.0f;
    float distanceTraveled;
    float speed = unk180;
    float traveled = unk170Value;
    float remaining = unk174;
    while (true) {
        distanceTraveled = __fabs(traveled);
        float threshold = __fabs(unk178);
        float previousSpeed = speed;
        if (distanceTraveled < threshold) {
            speed += unk17C;
        } else {
            float distanceRemaining = __fabs(remaining);
            if (distanceRemaining < threshold) {
                speed -= unk17C;
            } else if (remaining > 0.0f) {
                speed = m_data->unk28;
            } else {
                speed = -m_data->unk28;
            }
        }
        traveled += speed;
        remaining -= speed;
        if (previousSpeed < 0.0f && speed >= 0.0f) {
            return;
        }
        if (previousSpeed > 0.0f && speed <= 0.0f) {
            return;
        }
        float absoluteSpeed = __fabs(speed);
        float distanceRemaining = __fabs(remaining);
        if (distanceRemaining < absoluteSpeed) {
            return;
        }
        float deltaFrame = g_stGameFrame.m_frameDelta;
        unk184 += deltaFrame;
    }
}
