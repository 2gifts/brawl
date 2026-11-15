#include <wn/wn_damage_effector_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <it/item.h>
#include <types.h>

// HYPOTHESIS: map name soDamageEffectorImpl::reqCommonEffectParam (static); it stays unnamed in sora_melee.
extern "C" void fn_27_60CDC(float power, float reaction, float lr, float param0, float param1, float param2, float param3, float param4,
                            soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);

// TODO: reqShake, reqUniqEffect, reqCommonEffect, reqQuake and the static initializer.

// The damage effector shared by the items (declared here: include/wn/wn_damage_effector_impl.h has the base class).
class itDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~itDamageEffectorImpl();
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    // HYPOTHESIS: the module map names this slot reqEffectScreen.
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
    virtual void reqStop();
};

static inline BaseItem* getItem(soModuleAccesser* moduleAccesser) {
    return dynamic_cast<BaseItem*>(&moduleAccesser->getStageObject());
}

// HYPOTHESIS: only the item kinds 0x31 and 0x4F (read at +0x8C0 of the item) show the common damage effect, with unit
// parameters.
void itDamageEffectorImpl::reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
    BaseItem* item = getItem(moduleAccesser);
    int kind = *reinterpret_cast<int*>(reinterpret_cast<u8*>(item) + 0x8C0);
    if (kind == 0x31 || kind == 0x4F) {
        fn_27_60CDC(power, reaction, lr, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, moduleAccesser, attackData, collisionLog);
    }
}

// A strong hit in the air (level 3) shakes the camera by the reaction.
void itDamageEffectorImpl::reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level) {
    if (level == 3 && moduleAccesser->getSituationModule().getKind() == 2) {
        soCameraModule::QuakeKind kind;
        if (frameReaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0x7F4, 0)) {
            kind = soCameraModule::Quake_XL;
        } else {
            kind = (soCameraModule::QuakeKind)(frameReaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0x7F3, 0));
        }
        moduleAccesser->getCameraModule().reqQuake(kind, moduleAccesser);
    }
}

void itDamageEffectorImpl::reqStop() { }

void itDamageEffectorImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) { }

itDamageEffectorImpl::~itDamageEffectorImpl() { }
