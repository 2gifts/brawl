#define SO_AREA_ENVIROMENT_ELEMENT_CHECKER_IMPL_CT
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/so_module_accesser.h>
#include <st/st_area_manager.h>
#include <types.h>

// stAreaManager::getWater (the map name); src/mo_melee/sora_melee/ft/ft_status_uniq_process_swim.cpp calls it by this stand-in name.
extern "C" int fn_27_22B3B8(stAreaManager* manager, u32 areaId, stWaterData* water);

void soAreaEnviromentElementCheckerImpl::check(soModuleAccesser* accesser) {
    int count = accesser->getAreaModule().getAreaContactCount(-1);
    for (int index = 0; index < count; index++) {
        soAreaContactLog* contact = accesser->getAreaModule().getAreaContactLog(-1, index);
        switch (contact->unkD) {
        case gfArea::Stage_Group_Space_Wind:
        case gfArea::Stage_Group_Space_Movement_Area: {
            stMovementData movement;
            if (stAreaManager::getInstance()->getMovementArea(contact->m_areaId, &movement) == true &&
                movement.m_speed > 0.0f && movement.m_gimmickKind != 13) {
                Vec2f rotatedSpeed;
                Vec2f speed(movement.m_speed, 0.0f);
                speed.rot(0.01745329238474369f * movement.m_vector, &rotatedSpeed);
                soKineticEnergy::OutsideType outsideType;
                if (contact->unkD == gfArea::Stage_Group_Space_Wind) {
                    outsideType = soKineticEnergy::Outside_Wind;
                } else {
                    outsideType = soKineticEnergy::Outside_Movement_Ground;
                    m_onMovementGroundArea = 1;
                }
                Vec3f outsideSpeed(rotatedSpeed, 0.0f);
                accesser->getKineticModule().addSpeedOutside(outsideType, &outsideSpeed);
            }
            break;
        }
        case gfArea::Stage_Group_Space_Water:
            m_waterAreaId = contact->m_areaId;
            m_waterAreaIndex = contact->m_area;
            break;
        }
    }
}

float soAreaEnviromentElementCheckerImpl::getWaterSurfaceY() {
    stWaterData water;
    if (fn_27_22B3B8(stAreaManager::getInstance(), m_waterAreaId, &water) == 1u) {
        return water.m_swimHeight;
    }
    return 0.0f;
}

soAreaEnviromentElementCheckerImpl::soAreaEnviromentElementCheckerImpl() : m_waterAreaId(0), m_waterAreaIndex(-1), m_onMovementGroundArea(0) {}

soAreaEnviromentElementCheckerImpl::~soAreaEnviromentElementCheckerImpl() {}

void soAreaEnviromentElementCheckerImpl::unable() {
    soEnable::disable();
    clear();
}

void soAreaEnviromentElementCheckerImpl::clearWaterInfo() {
    m_waterAreaId = 0;
    m_waterAreaIndex = -1;
}

void soAreaEnviromentElementCheckerImpl::clear() {
    clearWaterInfo();
    m_onMovementGroundArea = 0;
}

bool soAreaEnviromentElementCheckerImpl::isWater() {
    return m_waterAreaId != 0;
}

bool soAreaEnviromentElementCheckerImpl::isWaterAreaIndex(int index) {
    if (m_waterAreaIndex < 0) {
        return false;
    }
    return m_waterAreaIndex == index;
}

u32 soAreaEnviromentElementCheckerImpl::getOnMovementGroundArea() {
    return m_onMovementGroundArea;
}

int soAreaEnviromentElementCheckerImpl::getWaterAreaId() {
    return m_waterAreaId;
}

bool soAreaEnviromentElementCheckerImpl::isEnable() {
    return soEnable::isEnable();
}

void soAreaEnviromentElementCheckerImpl::enable() {
    soEnable::enable();
}
