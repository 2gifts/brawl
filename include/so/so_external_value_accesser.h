#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/stageobject.h>
#include <types.h>

// NOTE: shadows the BrawlHeaders copy to add getConstantFloat/getKineticModule.
class soStopModule;
class soItemManageModule;
class soCollisionShieldModule;
class soCameraModule;
class soDamageAttackerInfo;
class soDamageModule;
class soModelModule;
class soCollisionSearchModule;

class soExternalValueAccesser {
public:
    static bool isIncludeRect(StageObject* stageObject, float* rect, Vec3f* out, float, float); // HYPOTHESIS: parameters
    static Input* getInput(StageObject* stageObject);
    static int getTeamNo(StageObject* stageObject);
    static Vec3f getPos(StageObject* stageObject);
    static float getLr(StageObject* stageObject);
    static float getScl(StageObject* stageObject);
    static Vec3f getPrevRoughPos(StageObject* stageObject);
#ifdef SO_EXTERNAL_VALUE_ACCESSER_IMPL
    static s8 getClipOutStatus(StageObject* stageObject); // BYTES: the defining unit tail calls the camera's signed byte unchanged
#else
    static u32 getClipOutStatus(StageObject* stageObject);
#endif
    static Vec2f getRhombusCenterPos(StageObject* stageObject);
    static SituationKind getSituationKind(StageObject* stageObject);
    static int getStatusKind(StageObject* stageObject);
    static int getMotionKind(StageObject* stageObject);
    static bool isTreadPassive(StageObject* stageObject);
    static soGroundModule* getGroundModule(StageObject* stageObject);
    static bool getWorkFlag(StageObject* stageObject, u32);
    static int getWorkInt(StageObject* stageObject, u32);
    static float getWorkFloat(StageObject* stageObject, u32);
    static soModuleAccesser* getModuleAccesser(StageObject* stageObject);
    static soCollisionHitModule* getCollisionHitModule(StageObject* stageObject);
    static soControllerModule* getControllerModule(StageObject* stageObject);
    static soCameraModule* getCameraModule(StageObject* stageObject);
    static u32 getTeamOwnerId(StageObject* stageObject);
    static float getConstantFloat(StageObject* stageObject, u32 paramId);
    static float getSlowRate(StageObject* stageObject);
    static soKineticModule* getKineticModule(StageObject* stageObject);
    static soCollisionAttackModule* getCollisionAttackModule(StageObject* stageObject);
    static soCollisionShieldModule* getCollisionShieldModule(StageObject* stageObject);
    static soLinkModule* getLinkModule(StageObject* stageObject);
    static soItemManageModule* getItemManageModule(StageObject* stageObject);
    static void* getGlowModule(StageObject* stageObject);
    static soStopModule* getStopModule(StageObject* stageObject);
    static int getTeam2ndNo(StageObject* stageObject);
    static void* getConstantIndefinite(StageObject* stageObject, u32 paramId, u32 paramIndex);
    static void getAttackerInfo(StageObject* stageObject, soDamageAttackerInfo* attackerInfo);
    static soModelModule* getModelModule(StageObject* stageObject);
    static soCollisionShieldModule* getCollisionReflectorModule(StageObject* stageObject);
    static soCollisionSearchModule* getCollisionSearchModule(StageObject* stageObject);
    static void* getCatchModule(StageObject* stageObject);
    static void* getCaptureModule(StageObject* stageObject);
    static soDamageModule* getDamageModule(StageObject* stageObject);
};
// TODO size assertion
