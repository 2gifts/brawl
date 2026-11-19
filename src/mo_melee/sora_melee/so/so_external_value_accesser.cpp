#define SO_EXTERNAL_VALUE_ACCESSER_IMPL
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/team/so_team_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/so_slow.h>
#include <so/stageobject.h>
#include <OS/OSError.h>
#include <so/area/so_area_module_impl.h>
#include <so/damage/so_damage_module_impl.h>

soModuleAccesser* soExternalValueAccesser::getModuleAccesser(StageObject* stageObject) {
    return stageObject->getModuleAccesser();
}

Vec3f soExternalValueAccesser::getPos(StageObject* stageObject) {
    soModuleAccesser* moduleAccesser = stageObject->getModuleAccesser();
    if (moduleAccesser->getSituationModule().getKind() == Situation_Outfield) {
        OSReport("Warning!! situation\x82\xAA" "OUTFIELD" "\x82\xCC\x8E\x9E\x82\xC9\x8D\xC0\x95\x57\x82\xAA\x8E\x51\x8F\xC6\x82\xB3\x82\xEA\x82\xDC\x82\xB5\x82\xBD [%s] \n", stageObject->m_taskName);
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    return stageObject->getModuleAccesser()->getPostureModule().getPos();
}

Vec3f soExternalValueAccesser::getPrevRoughPos(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getPostureModule().getPrevRoughPos();
}

Vec2f soExternalValueAccesser::getRhombusCenterPos(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getGroundModule().getCenterPos(0);
}

float soExternalValueAccesser::getLr(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getPostureModule().getLr();
}

float soExternalValueAccesser::getScl(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getPostureModule().getScale();
}

s8 soExternalValueAccesser::getClipOutStatus(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getCameraModule().getClipOutStatus(false);
}

int soExternalValueAccesser::getTeamNo(StageObject* stageObject) {
    soModuleAccesser* moduleAccesser = getModuleAccesser(stageObject);
    return moduleAccesser->getTeamModule().getTeam()->getNo();
}

int soExternalValueAccesser::getTeam2ndNo(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getTeamModule().getTeam()->get2nd();
}

u32 soExternalValueAccesser::getTeamOwnerId(StageObject* stageObject) {
    soModuleAccesser* moduleAccesser = getModuleAccesser(stageObject);
    u32 id = moduleAccesser->getTeamModule().getTeamOwnerId();
    if (id == -1) {
        return stageObject->m_taskId;
    }
    return id;
}

int soExternalValueAccesser::getStatusKind(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getStatusModule().getStatusKind();
}

int soExternalValueAccesser::getMotionKind(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getMotionModule().getKind();
}

// The other units call this one and getAreaIndexFromId through plain stand-in declarations (extern "C" fn_27_8CC2C / fn_27_8CC68).
extern "C" int fn_27_8CC2C(StageObject* stageObject, int touchMask) {
    return stageObject->getModuleAccesser()->getGroundModule().isTouch(touchMask, 0);
}

SituationKind soExternalValueAccesser::getSituationKind(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getSituationModule().getKind();
}

extern "C" int fn_27_8CC68(StageObject* stageObject, int areaId) {
    return stageObject->getModuleAccesser()->getAreaModule().getAreaIndexFromId(areaId);
}

bool soExternalValueAccesser::isTreadPassive(StageObject* stageObject) {
    return stageObject->isTreadPassive();
}

float soExternalValueAccesser::getSlowRate(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->getSlowModule().getRate();
}

float soExternalValueAccesser::getConstantFloat(StageObject* stageObject, u32 paramId) {
    return soValueAccesser::getConstantFloat(stageObject->getModuleAccesser(), paramId, 0);
}

void* soExternalValueAccesser::getConstantIndefinite(StageObject* stageObject, u32 paramId, u32 paramIndex) {
    return soValueAccesser::getConstantIndefinite(stageObject->getModuleAccesser(), paramId, paramIndex);
}

int soExternalValueAccesser::getWorkInt(StageObject* stageObject, u32 index) {
    return stageObject->getModuleAccesser()->getWorkManageModule().getInt(index);
}

float soExternalValueAccesser::getWorkFloat(StageObject* stageObject, u32 index) {
    return stageObject->getModuleAccesser()->getWorkManageModule().getFloat(index);
}

bool soExternalValueAccesser::getWorkFlag(StageObject* stageObject, u32 index) {
    return stageObject->getModuleAccesser()->getWorkManageModule().isFlag(index);
}

void soExternalValueAccesser::getAttackerInfo(StageObject* stageObject, soDamageAttackerInfo* attackerInfo) {
    stageObject->getModuleAccesser()->getDamageModule().getAttackerInfo(attackerInfo);
}

soModelModule* soExternalValueAccesser::getModelModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getModelModule();
}

soCollisionAttackModule* soExternalValueAccesser::getCollisionAttackModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getCollisionAttackModule();
}

soCollisionHitModule* soExternalValueAccesser::getCollisionHitModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getCollisionHitModule();
}

soCollisionShieldModule* soExternalValueAccesser::getCollisionShieldModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getCollisionShieldModule();
}

soCollisionShieldModule* soExternalValueAccesser::getCollisionReflectorModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getCollisionReflectorModule();
}

soCollisionSearchModule* soExternalValueAccesser::getCollisionSearchModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getCollisionSearchModule();
}

soLinkModule* soExternalValueAccesser::getLinkModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getLinkModule();
}

void* soExternalValueAccesser::getCatchModule(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->m_enumerationStart->m_catchModule;
}

void* soExternalValueAccesser::getCaptureModule(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->m_enumerationStart->m_captureModule;
}

soGroundModule* soExternalValueAccesser::getGroundModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getGroundModule();
}

void* soExternalValueAccesser::getGlowModule(StageObject* stageObject) {
    return stageObject->getModuleAccesser()->m_enumerationStart->m_glowModule;
}

soCameraModule* soExternalValueAccesser::getCameraModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getCameraModule();
}

soDamageModule* soExternalValueAccesser::getDamageModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getDamageModule();
}

soStopModule* soExternalValueAccesser::getStopModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getStopModule();
}

soKineticModule* soExternalValueAccesser::getKineticModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getKineticModule();
}

soControllerModule* soExternalValueAccesser::getControllerModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getControllerModule();
}

soItemManageModule* soExternalValueAccesser::getItemManageModule(StageObject* stageObject) {
    return &stageObject->getModuleAccesser()->getItemManageModule();
}

Input* soExternalValueAccesser::getInput(StageObject* stageObject) {
    return stageObject->getInput();
}

bool soExternalValueAccesser::isIncludeRect(StageObject* stageObject, float* rect, Vec3f* out, float width, float height) {
    soModuleAccesser* moduleAccesser = stageObject->getModuleAccesser();
    if (moduleAccesser->getSituationModule().getKind() != Situation_Outfield) {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        if (rect[1] >= pos.m_x - width && rect[0] <= pos.m_x + width && rect[2] >= pos.m_y && rect[3] <= pos.m_y + height) {
            if (out) {
                *out = pos;
            }
            return true;
        }
        return false;
    }
    return false;
}
