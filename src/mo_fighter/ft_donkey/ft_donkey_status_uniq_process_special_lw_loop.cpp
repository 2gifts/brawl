#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>

class ftDonkeyStatusUniqProcessSpecialLwLoop : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialLwLoop() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessSpecialLwLoop::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    const float* params = static_cast<const float*>(g_ftCommonDataAccesser.getData(Fighter_Donkey)->extendParam[3]);
    Vec3f position = posture.getPos();
    float facing = posture.getLr();
    if (work.isFlag(0x22000011)) {
        work.offFlag(0x22000011);
        if (ground.isTouch(8, 0)) {
            for (int index = 0; index < 4; ++index) {
                if (attack.isAttack(index, false)) {
                    float facingOffset = params[1] * facing;
                    float offset = params[0] * index - 1.5f * params[0];
                    offset += facingOffset;
                    offset *= posture.getScale();
                    Vec3f hitPosition = position;
                    Vec3f direction;
                    direction.m_x = 0.0f;
                    direction.m_y = -100.0f;
                    direction.m_z = 0.0f;
                    hitPosition.m_x += offset;
                    if (!ground.getDownCorrectPos(&hitPosition, &direction, 0)) {
                        hitPosition.m_x = position.m_x;
                        hitPosition.m_y = position.m_y;
                        hitPosition.m_z = position.m_z;
                    }
                    hitPosition.m_y += 1.0f;
                    Vec3fSub(&hitPosition, &hitPosition, &position);
                    Vec3f hitOffset;
                    hitOffset.m_x = hitPosition.m_z;
                    hitOffset.m_y = hitPosition.m_y;
                    hitOffset.m_z = hitPosition.m_x * facing;
                    attack.setOffset(index, &hitOffset);
                }
            }
        }
    }
}

ftDonkeyStatusUniqProcessSpecialLwLoop g_ftDonkeyStatusUniqProcessSpecialLwLoop;
