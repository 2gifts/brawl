#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/ganon/wn_ganon_beast.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnGanonBeastStatusUniqProcessStart::wnGanonBeastStatusUniqProcessStart() { }

wnGanonBeastStatusUniqProcessStart::~wnGanonBeastStatusUniqProcessStart() { }

wnGanonBeastStatusUniqProcessAttack::~wnGanonBeastStatusUniqProcessAttack() { }

void wnGanonBeastStatusUniqProcessStart::initStatus(soModuleAccesser* moduleAccesser) {
    soCameraModule* camera = moduleAccesser->m_enumerationStart->m_cameraModule;
    float lr = moduleAccesser->m_enumerationStart->m_postureModule->getLr();
    Vec3f offset;
    offset.m_x = lr * soValueAccesser::getConstantFloat(moduleAccesser, 4000, 0);
    offset.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 4001, 0);
    offset.m_z = 0.0f;
    camera->addCameraRangeOffset(&offset, 0);
}

void wnGanonBeastStatusUniqProcessAttack::execStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* posture = moduleAccesser->m_enumerationStart->m_postureModule;
    Vec3f current = posture->getPos();
    Vec3f pos;
    pos = current;
    float lr = posture->getLr();
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 4002, 0);
    pos.m_x += speed * lr;
    posture->setPos(&pos);
}
