#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Local view of the article manage module (module slot 0x84). Two vtable words precede the first declared slot here.
// HYPOTHESIS: slot order follows soGenerateArticleManageModule in the header after those leading slots.
class ftSheikArticleManageView {
public:
    virtual void reserved();
    virtual void activate();
    virtual void deactivate();
    virtual void generate(int articleId, soModuleAccesser* moduleAccesser, void* logAttackInfo);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, int situationKind);
    virtual void entry(void* article);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
};

// Local view of the visibility module; its vtable has two leading words before the first declared slot.
class ftSheikVisibilityModuleView {
public:
    virtual void activate();
    virtual void setData(void*);
    virtual void set(u32, int);
    virtual void setStatusDefault(u32, u8, bool);
    virtual void setDefaultAll();
    virtual void update();
    virtual void hideAll();
    virtual void setWhole(u8);
};

class ftSheikStatusUniqProcessSpecialSLoop : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialSLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftSheikStatusUniqProcessSpecialSLoop::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: extendParam[1] of the Sheik data points to an int table; entry 1 seeds the work int 0x20000004.
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    int* table = static_cast<int*>(data->extendParam[1]);
    moduleAccesser->getWorkManageModule().setInt(table[1], 0x20000004);
}

// HYPOTHESIS: not reconstructed yet (per-frame loop and kinetic/effect updates); left as a stub.
void ftSheikStatusUniqProcessSpecialSLoop::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    soControllerModule* controller = &moduleAccesser->getControllerModule();
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    soMotionModule* motion = &moduleAccesser->getMotionModule();
    float stickX = controller->getStickX();
    float stickY = controller->getStickY();
    float response = soValueAccesser::getConstantFloat(moduleAccesser, 0xcb2, 0);
    float radians = atan2(stickY, stickX * posture->getLr());
    if (radians < 0.0f) radians += 6.2831855f;
    float angle = 57.29578f * radians;
    if (angle < 0.0f) angle = 0.0f;
    else if (angle > 359.0f) angle = 359.0f;
    float previousAngle = work->getFloat(0x21000004);
    float difference = angle - previousAngle;
    if (difference > 180.0f) difference -= 360.0f;
    else if (difference < -180.0f) difference += 360.0f;
    angle = previousAngle + difference * response;
    if (angle > 360.0f) angle -= 360.0f;
    else if (angle < 0.0f) angle += 360.0f;
    work->setFloat(angle, 0x21000004);
    float verticalMagnitudeSquared = stickY * stickY;
    float horizontalMagnitudeSquared = stickX * stickX;
    float magnitude = mtSqrtf(horizontalMagnitudeSquared + verticalMagnitudeSquared);
    if (magnitude > 1.0f) magnitude = 1.0f;
    float previousMagnitude = work->getFloat(0x21000005);
    magnitude = previousMagnitude + response * (magnitude - previousMagnitude);
    work->setFloat(magnitude, 0x21000005);
    float frame = 4.0f + angle * 0.0556f;
    motion->setWeightRate((1.0f - previousMagnitude) - motion->getPrevWeight());
    motion->set2ndRate(frame - motion->get2ndFrame());
}

// HYPOTHESIS: not reconstructed yet (vector math and article/effect updates); left as a stub.
void ftSheikStatusUniqProcessSpecialSLoop::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    soCollisionAttackModule* attack = &moduleAccesser->getCollisionAttackModule();
    soLinkModule* link = &moduleAccesser->getLinkModule();
    void* parameters = g_ftCommonDataAccesser.getData(Fighter_Sheik)->extendParam[1];
    int index;
    bool moving;
    Vec3f positions[4];
    int nodes[4] = {3, 5, 7, 9};
    Vec3f fighterPosition = moduleAccesser->getPostureModule().getPos();
    float facing = moduleAccesser->getPostureModule().getLr();
    if (link->isLink(6)) {
        moving = false;
        float distances[4];
        Vec3f previous;
        Vec3f attackDisplacement;
        Vec3f offset;
        for (index = 0; index < 4; ++index) {
            Vec3f nodePosition = link->getParentModelNodeGlobalPosition(6, nodes[index], true);
            positions[index] = nodePosition;
            positions[index].m_z = 0.0f;
            previous.m_x = work->getFloat(0x21000006 + index);
            previous.m_y = work->getFloat(0x2100000a + index);
            previous.m_z = 0.0f;
            {
                Vec3f displacement;
                positions[index].sub(&previous, &displacement);
                float squared = displacement.m_z * displacement.m_z +
                    (displacement.m_x * displacement.m_x + displacement.m_y * displacement.m_y);
                double absolute = __fabs(squared);
                float magnitude = absolute;
                distances[index] = magnitude <= 1.17549435e-38f ? 0.0f : squared * rsqrtf(squared);
            }
            work->setFloat(positions[index].m_x, 0x21000006 + index);
            work->setFloat(positions[index].m_y, 0x2100000a + index);
            if (distances[index] > static_cast<float*>(parameters)[6]) moving = true;
        }
        float tipDistance = distances[0];
        if (work->getInt(0x20000004) > 0) {
            work->decInt(0x20000004);
            if (work->getInt(0x20000004) <= 0) {
                for (index = 0; index < 4; ++index) {
                    if (attack->isAttack(index, false)) attack->clear(index);
                }
                work->onFlag(0x22000016);
            }
        }
        if (moving) {
            if (work->getInt(0x20000004) <= 0) {
                work->setInt(static_cast<int*>(parameters)[1], 0x20000004);
                if (work->isFlag(0x22000016)) {
                    for (index = 0; index < 4; ++index) {
                        if (!attack->isAttack(index, false)) attack->set(index, 0);
                    }
                    work->offFlag(0x22000016);
                }
            }
        } else {
            for (index = 0; index < 4; ++index) {
                bool cleared = false;
                if (attack->isAttack(index, false)) {
                    attack->clear(index);
                    cleared = true;
                }
                if (cleared) {
                    work->onFlag(0x22000016);
                    work->setInt(0, 0x20000004);
                }
            }
        }
        for (int index = 0; index < 4; ++index) {
            if (attack->isAttack(index, false)) {
                positions[index].sub(&fighterPosition, &attackDisplacement);
                offset.set(0.0f, attackDisplacement.m_y, 0.0f);
                offset.m_z = attackDisplacement.m_x * facing;
                if (0.0f != moduleAccesser->getPostureModule().getScale()) {
                    *reinterpret_cast<nw4r::math::VEC3*>(&offset) /= moduleAccesser->getPostureModule().getScale();
                }
                attack->setOffset(index, &offset);
            }
        }
        if (work->getInt(0x20000005) > 0) {
            work->decInt(0x20000005);
        } else if (tipDistance > 1.0f) {
            work->setInt(10, 0x20000005);
            if (tipDistance > 8.0f) moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1036), false);
            else if (tipDistance > 3.0f) moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1035), false);
            else moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1034), false);
        }
    }
}

void ftSheikStatusUniqProcessSpecialSLoop::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11b && nextStatus != 0x81) {
        reinterpret_cast<ftSheikArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(2, 0);
    }
}

ftSheikStatusUniqProcessSpecialSLoop g_ftSheikStatusUniqProcessSpecialSLoop;
