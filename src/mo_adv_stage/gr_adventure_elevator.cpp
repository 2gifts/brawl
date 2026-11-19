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

#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <math.h>
#include <nw4r/ut/ut_algorithm.h>
// TODO: clean up, give variables real names
namespace jj_34F90 {
typedef struct { float x, y, z; } Vec3;
inline void swap(Vec3 &a, Vec3 &b) {
    Vec3 t = a;
    a = b;
    b = t;
}
extern "C" void fn_40_34F90(Vec3 **a, Vec3 **b, Vec3 **c)
{
    Vec3 *bp;
    Vec3 *cp;
    Vec3 *ap;
    ap = *a;
    cp = *c;
    int first = !(cp->y < ap->y);
    bp = *b;
    int second = !(bp->y < cp->y);
    if (first != 0) {
        if (second != 0) return;
    }
    if (first == 0 && second == 0) {
        swap(**a, **b);
        return;
    }
    int lt = bp->y < ap->y;
    if (lt != 0) {
        swap(**a, **b);
    }
    if (first != 0) {
        swap(**b, **c);
    } else {
        swap(**a, **c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_354A0 {

extern "C" {
extern const float lbl_40_rodata_6A0;
extern const float lbl_40_rodata_6BC;
extern float g_stGameFrame;
}

struct MovementParams {
    char pad[0x28];
    float speed;
};

struct MovingGimmick : grYakumono {
    MovementParams* params;
    char pad154[4];
    u8 state;
    char pad159[3];
    float frame;
    Vec3f* positions;
    char pad164[4];
    u32 currentIndex;
    u32 targetIndex;
    float traveled;
    float remaining;
    float threshold;
    float acceleration;
    float speed;
    float duration;
};

static inline float magnitude(float value) {
    return (float)(double)nw4r::ut::Abs(value);
}

extern "C" void moveFloor__19grAdventureElevatorFv(MovingGimmick* self) {
    float oldSpeed = self->speed;
    double threshold = fabs(self->threshold);
    double traveled = fabs(self->traveled);
    if ((float)traveled < (float)threshold) {
        self->speed = oldSpeed + self->acceleration;
    } else {
        float remaining = fabs(self->remaining);
        if (remaining < (float)threshold) {
            self->speed = oldSpeed - self->acceleration;
        } else if (self->remaining > lbl_40_rodata_6A0) {
            self->speed = self->params->speed;
        } else {
            self->speed = -self->params->speed;
        }
    }

    Vec3f pos;
    pos = self->grGimmick::getPos();
    pos.m_y += self->speed;
    self->traveled += self->speed;
    self->remaining -= self->speed;

    if (self->duration - self->frame <= lbl_40_rodata_6BC) {
        float delta = g_stGameFrame;
        if (self->duration - self->frame > lbl_40_rodata_6BC - delta) {
            self->startGimmickSE(3);
        }
    }

    if ((oldSpeed < lbl_40_rodata_6A0 && self->speed >= lbl_40_rodata_6A0) ||
        (oldSpeed > lbl_40_rodata_6A0 && self->speed <= lbl_40_rodata_6A0)) {
        goto reached;
    }
    {
        double step = fabs(self->speed);
        double distance = fabs(self->remaining);
        if (!((float)distance < (float)step)) {
            goto finished;
        }
    }
reached:
    {
        pos.m_y = self->positions[self->targetIndex].m_y;
        self->state = 2;
        self->frame = lbl_40_rodata_6A0;
        self->currentIndex = self->targetIndex;
        self->stopGimmickSE(0);
        self->startGimmickSE(1);
        self->grYakumono::enableArea();
    }
finished:
    self->grGimmick::setPos(&pos);
}
}

