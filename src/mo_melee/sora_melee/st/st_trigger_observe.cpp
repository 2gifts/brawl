#include <st/st_trigger_observe.h>
#include <gf/gf_area.h>
#include <math.h>
#include <st/st_area_manager.h>
#include <st/stage.h>
#include <types.h>
#include <cm/cm_camera_controller.h>
#include <gm/gm_global.h>

stObsTriggerCameraReactionCB::stObsTriggerCameraReactionCB(bool* flag, Rect2D* area, bool keep)
    : stObsTriggerCB(flag, keep) {
    __memcpy(&unk10, area, sizeof(unk10));
}

void stObsTriggerCameraReactionCB::userProc() {
    if (CameraController::getInstance() != nullptr && CameraController::getInstance()->isIntersect(&unk10)) {
        *unk8 = true;
    }
}

stObsTriggerPlayerIncludeAreaCB::stObsTriggerPlayerIncludeAreaCB(bool* flag, Rect2D* area, bool keep)
    : stObsTriggerCB(flag, keep) {
    __memcpy(&unk10, area, sizeof(unk10));
    unk20 = 0;
    unk24 = false;
    if (g_GameGlobal->m_advData->m_flags.m_unk0 == true) {
        unk24 = true;
    }
}

stObsTriggerCB::stObsTriggerCB(bool* flag, bool unkC) : unk8(flag), unkC(unkC) { }

void stObsTriggerCB::resetTrigger() {
    if (!unkC) {
        *unk8 = false;
    }
}

void stObsNumTriggerCB2::userProc() {
    bool allOn = true;
    bool anyOn = false;
    bool allOff = true;
    bool anyOff = false;
    for (u32 i = 0; i < m_referenceNum; i++) {
        if (*m_referenceFlags[i] == true) {
            anyOn = true;
            allOff = false;
        } else {
            allOn = false;
            anyOff = true;
        }
    }
    bool result;
    if (unk28) {
        result = !unk29 ? allOn : allOff;
    } else {
        result = !unk29 ? anyOn : anyOff;
    }
    if (result) {
        *unk8 = true;
    }
}

stObsTriggerArea::stObsTriggerArea(bool* flag, bool unkC) : stObsTriggerCB(flag, unkC) { }

stObsTriggerArea::~stObsTriggerArea() {
    if (m_area != nullptr) {
        stAreaManager::getInstance()->erase(m_area->m_id);
        delete m_area;
    }
    m_area = nullptr;
}

void stObsTriggerArea::userProc() { }

bool stObsTriggerArea::getAreaID(int* areaID) {
    if (m_area != nullptr) {
        *areaID = m_area->m_id;
        return true;
    }
    return false;
}

void stObsTriggerArea::setAreaSleep(bool isSleep) {
    if (isSleep) {
        m_area->m_isDrawFill = false;
    } else {
        m_area->m_isDrawFill = true;
    }
}

void stObsTriggerArea::setDefaultGimmickAreaData(stSimpleAreaData* simpleAreaData, stGimmickAreaData* gimmickAreaData) {
    bool useTwoBoundingPoints = simpleAreaData->m_useTwoBoundingPoints;
    // HYPOTHESIS: the default shape value is the gimmick group, overwritten by both shape branches.
    gimmickAreaData->m_shapeType = (gfArea::ShapeType)gfArea::Stage_Group_Gimmick_Normal;
    gimmickAreaData->m_group = gfArea::Stage_Group_Space_Normal;
    gimmickAreaData->m_checkTargetGroup = 0;
    gimmickAreaData->m_checkTargetLocalGroup = 0;
    gimmickAreaData->m_16 = 0;
    gimmickAreaData->m_18 = 0;
    gimmickAreaData->unk14 = 0;
    if (useTwoBoundingPoints == true) {
        float x1 = simpleAreaData->m_pos1.m_x;
        float y1 = simpleAreaData->m_pos1.m_y;
        float x2 = simpleAreaData->m_pos2.m_x;
        float y2 = simpleAreaData->m_pos2.m_y;
        gimmickAreaData->m_shapeType = gfArea::Shape_Rectangle;
        gimmickAreaData->m_offsetPos.m_x = (x1 + x2) / 2.0f;
        gimmickAreaData->m_offsetPos.m_y = (y1 + y2) / 2.0f;
        gimmickAreaData->m_range.m_y = fabs(y2 - y1);
        gimmickAreaData->m_range.m_x = fabs(x2 - x1);
    } else if (useTwoBoundingPoints == false) {
        float x = simpleAreaData->m_pos1.m_x;
        float y = simpleAreaData->m_pos1.m_y;
        float radius = simpleAreaData->m_pos2.m_y;
        gimmickAreaData->m_shapeType = gfArea::Shape_Circle;
        gimmickAreaData->m_offsetPos.m_x = x;
        gimmickAreaData->m_offsetPos.m_y = y;
        gimmickAreaData->m_range.m_x = radius;
        gimmickAreaData->m_range.m_y = radius;
    }
}

gfArea* stObsTriggerArea::getAreaPointer() {
    return m_area;
}

void stObsTriggerArea::setArea(gfArea* area) {
    m_area = area;
}

stGimmickAreaData* stObsTriggerArea::getAreaDataPointer() {
    return &m_gimmickAreaData;
}

stObsTriggerFrameCB::stObsTriggerFrameCB(bool* flag, float frame) : stObsTriggerCB(flag, false), unk10(frame), unk14(0.0f) { }

void stObsTriggerFrameCB::userProc() {
    if (unk14 >= unk10) {
        *unk8 = true;
    }
    float delta = g_stGameFrame.m_frameDelta;
    unk14 += delta;
}

stObsTriggerSquareWindCB::stObsTriggerSquareWindCB(bool* flag, grGimmickWindData2nd* windData)
    : stObsTriggerArea(flag, false) {
    windData->m_areaData.m_group = gfArea::Stage_Group_Space_Wind;
    createArea(&windData->m_areaData, Vec2f(windData->m_pos.m_x, windData->m_pos.m_y));
    stMovementAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_Wind_2nd;
    info.m_data.m_speed = windData->m_speed;
    info.m_data.m_vector = windData->m_vector;
    info.m_data._0xC = windData->m_60;
    info.m_data._0x10 = windData->m_64;
    info.m_data._0x14 = windData->m_68;
    info.m_data._0x1c = windData->m_72;
    getAreaID(&info.m_areaId);
    m_windIndex = stAreaManager::getInstance()->setWind(&info);
}

stObsTriggerSquareWindCB::~stObsTriggerSquareWindCB() { }

stObsTriggerSquareBeltConveyorCB::stObsTriggerSquareBeltConveyorCB(bool* flag, grGimmickPressureAreaData* beltData)
    : stObsTriggerArea(flag, false) {
    beltData->m_areaData.m_group = gfArea::Stage_Group_Space_Movement_Area;
    createArea(&beltData->m_areaData, Vec2f(beltData->m_pos.m_x, beltData->m_pos.m_y));
    stMovementAreaInfoData info;
    if (beltData->m_isPow == true) {
        info.m_data.m_gimmickKind = Gimmick::Area_Pow;
    } else {
        info.m_data.m_gimmickKind = Gimmick::Area_Yuka;
    }
    info.m_data.m_speed = beltData->m_speed;
    info.m_data._0xC = 0.0f;
    info.m_data._0x10 = 0.0f;
    info.m_data.m_vector = beltData->m_vector;
    info.m_data.m_taskId = beltData->m_taskId;
    info.m_data._0x1c = 0;
    getAreaID(&info.m_areaId);
    m_beltConveyorIndex = stAreaManager::getInstance()->setBelt(&info);
}

stObsTriggerSquareBeltConveyorCB::~stObsTriggerSquareBeltConveyorCB() { }

void stObsTriggerSquareBeltConveyorCB::setParam(grGimmickBeltConveyorData* beltData) {
    if (getAreaPointer() != nullptr && dynamic_cast<gfAreaRectangle*>(getAreaPointer()) != nullptr) {
        dynamic_cast<gfAreaRectangle*>(getAreaPointer())->setShape(
            Vec2f(beltData->m_areaData.m_offsetPos.m_x + beltData->m_pos.m_x,
                  beltData->m_areaData.m_offsetPos.m_y + beltData->m_pos.m_y),
            Vec2f(beltData->m_areaData.m_range.m_x, beltData->m_areaData.m_range.m_y));
    }
    stMovementAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_BeltConveyor;
    info.m_data.m_speed = beltData->m_speed;
    info.m_data._0xC = 0.0f;
    info.m_data._0x10 = 0.0f;
    if (beltData->m_isRight) {
        info.m_data.m_vector = 0.0f;
    }
    if (!beltData->m_isRight) {
        info.m_data.m_vector = 180.0f;
    }
    info.m_data._0x1c = 0;
    getAreaID(&info.m_areaId);
    stAreaManager::getInstance()->setBeltParam(&info, m_beltConveyorIndex);
}

void stObsTriggerSquareBeltConveyorCB::setParam(grGimmickPressureAreaData* beltData) {
    if (getAreaPointer() != nullptr && dynamic_cast<gfAreaRectangle*>(getAreaPointer()) != nullptr) {
        dynamic_cast<gfAreaRectangle*>(getAreaPointer())->setShape(
            Vec2f(beltData->m_areaData.m_offsetPos.m_x + beltData->m_pos.m_x,
                  beltData->m_areaData.m_offsetPos.m_y + beltData->m_pos.m_y),
            Vec2f(beltData->m_areaData.m_range.m_x, beltData->m_areaData.m_range.m_y));
    }
    stMovementAreaInfoData info;
    if (beltData->m_isPow == true) {
        info.m_data.m_gimmickKind = Gimmick::Area_Pow;
    } else {
        info.m_data.m_gimmickKind = Gimmick::Area_Yuka;
    }
    info.m_data.m_speed = beltData->m_speed;
    info.m_data._0xC = 0.0f;
    info.m_data._0x10 = 0.0f;
    info.m_data.m_vector = beltData->m_vector;
    info.m_data.m_taskId = beltData->m_taskId;
    info.m_data._0x1c = 0;
    getAreaID(&info.m_areaId);
    stAreaManager::getInstance()->setBeltParam(&info, m_beltConveyorIndex);
}

stObsTriggerTriBeltConveyorCB::~stObsTriggerTriBeltConveyorCB() { }

stObsTriggerSquareWaterCB::stObsTriggerSquareWaterCB(bool* flag, grGimmickWaterData* waterData)
    : stObsTriggerArea(flag, false) {
    waterData->m_areaData.m_group = gfArea::Stage_Group_Space_Water;
    createArea(&waterData->m_areaData, Vec2f(0.0f, 0.0f));
    stWaterAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_Water;
    info.m_data.m_swimHeight = waterData->m_swimHeight;
    info.m_data.m_0xc = waterData->m_canDrown != false;
    info.m_data.m_currentSpeed = waterData->m_speed;
    getAreaID(&info.m_areaId);
    m_waterIndex = stAreaManager::getInstance()->setWater(&info);
}

stObsTriggerSquareWaterCB::~stObsTriggerSquareWaterCB() { }

void stObsTriggerSquareWaterCB::setParam(grGimmickWaterData* waterData) {
    if (getAreaPointer() != nullptr && dynamic_cast<gfAreaRectangle*>(getAreaPointer()) != nullptr) {
        dynamic_cast<gfAreaRectangle*>(getAreaPointer())->setShape(
            Vec2f(waterData->m_areaData.m_offsetPos.m_x, waterData->m_areaData.m_offsetPos.m_y),
            Vec2f(waterData->m_areaData.m_range.m_x, waterData->m_areaData.m_range.m_y));
    }
    stWaterAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_Water;
    info.m_data.m_swimHeight = waterData->m_swimHeight;
    info.m_data.m_0xc = waterData->m_canDrown != false;
    info.m_data.m_currentSpeed = waterData->m_speed;
    getAreaID(&info.m_areaId);
    stAreaManager::getInstance()->setWaterParam(&info, m_waterIndex);
}

stObsTriggerSquareHitPointEffectCB::~stObsTriggerSquareHitPointEffectCB() { }

stObsTriggerCameraReactionCB::~stObsTriggerCameraReactionCB() { }

stObsTriggerPlayerIncludeAreaCB::~stObsTriggerPlayerIncludeAreaCB() { }
