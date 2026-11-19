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

// TODO: clean up, give variables real names
namespace jj_2B2F3C {
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, long);
extern "C" {
extern unsigned int __RTTI__8BaseItem[];
extern unsigned int __RTTI__11StageObject[];
extern float lbl_27_rodata_5EA0;
}
class Class_1250 {
public:
    virtual void __dt__18soMotionModuleImplFv();
    virtual void fn_27_14068();
    virtual void fn_27_140C0();
    virtual void fn_27_1412C();
    virtual void fn_27_14134();
    virtual void fn_27_14370();
    virtual void fn_27_14A28(int, void*, unsigned int, float);
};
extern "C" void fn_27_2B2F3C(void*, char* obj, char* arg, char* event) {
    int kind = *(int*)((char*)__dynamic_cast(*(void**)(obj + 8), 0x3c, __RTTI__8BaseItem, __RTTI__11StageObject, 1) + 0x8c0);
    if (kind == 0x31 || kind == 0x4f) {
        switch (*(unsigned int*)(event + 0x30) & 0x1f) {
        case 5:
            (*(Class_1250**)(*(char**)(obj + 0xd8) + 0x6c))->fn_27_14A28(4, obj, (unsigned int)(arg + 8), lbl_27_rodata_5EA0);
            break;
        case 3:
        case 20:
            (*(Class_1250**)(*(char**)(obj + 0xd8) + 0x6c))->fn_27_14A28(4, obj, (unsigned int)(arg + 12), lbl_27_rodata_5EA0);
            break;
        default:
            (*(Class_1250**)(*(char**)(obj + 0xd8) + 0x6c))->fn_27_14A28(4, obj, 7, lbl_27_rodata_5EA0);
            break;
        }
    }
}
}

