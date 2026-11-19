#include <wn/poke/wn_poke_zenigame_water.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/reflect/so_reflect_module.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

// Registers the Pokemon water gun article class (kind 95) in the class info table during static initialization.
wnClassInfoImpl<95, wnPokeZenigameWater, wnPokeZenigameWaterData> g_wnPokeZenigameWaterClassInfo;

// The water model follows the posture and points along its flight direction.
void wnPokeZenigameWater::updateNodeSRT() {
    float value = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = value;
    scale.m_y = value;
    scale.m_x = value;
    Vec3f rotation;
    Vec3f position;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    wnKineticEnergyNormal& normal =
        dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f speed = normal.getSpeed();
    rotation.m_z = 57.29578f * (float)atan2(speed.m_y, speed.m_x);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

// A hit on a target ends the water stream (status 1).
bool wnPokeZenigameWater::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// A reflector turns the water back: the horizontal speed by the reflector's speed factor, the vertical speed flipped.
bool wnPokeZenigameWater::reflect() {
    Vec3f speed;
    speed.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    speed.m_y = -1.0f;
    speed.m_z = 1.0f;
    Vec3f accel;
    accel.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    accel.m_y = 1.0f;
    accel.m_z = 1.0f;
    m_moduleAccesser->getKineticModule().mulSpeed(&speed, soKineticEnergy::AttributeFlag(1));
    m_moduleAccesser->getKineticModule().mulAccel(&accel, soKineticEnergy::AttributeFlag(1));
    return false;
}

#include <mt/mt_vector.h>
// TODO: clean up, give variables real names
namespace jj_373A80 {
extern "C" void* fn_27_373A80(void* self) {
    return reinterpret_cast<u8*>(self) + 0x428;
}
}

// TODO: clean up, give variables real names
namespace jj_3745E8 {
extern "C" void* fn_27_3745E8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x90;
}
}

// TODO: clean up, give variables real names
namespace jj_376050 {
struct Vec { float x,y,z; };
struct Posture {
 virtual void isNull(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual Vec slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
 virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
 virtual void slot12(); virtual void slot13(); virtual Vec slot14(int);
 virtual void slot15(); virtual void slot16(); virtual void slot17(); virtual void slot18();
 virtual void slot19(); virtual void slot20(); virtual void slot21(); virtual float slot22();
};
struct Energy {
 virtual void isNull(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual void slot4(); virtual void slot5(); virtual void* slot6(int);
};

struct Speed {
 virtual void slot0(); virtual void slot1(); virtual Vec2f slot2();
};
struct Pad { int a,b; };
struct Model : Pad {
 virtual void isNull(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
 virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
 virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
 virtual void slot16(); virtual void slot17(); virtual void slot18(); virtual void slot19();
 virtual void slot20(); virtual void slot21(); virtual void slot22(int, Vec*, Vec*, Vec*);
};
struct Modules { char pad0[4]; Model* model; char pad8[4]; Posture* posture; char pad10[0x6c]; Energy* energy; };
struct Actor { char pad[0xd8]; Modules* modules; };
struct Object { char pad[0x60]; Actor* actor; };
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, long);
extern "C" char __RTTI__21wnKineticEnergyNormal[], __RTTI__15soKineticEnergy[];
extern "C" float lbl_27_rodata_7BF4;
extern "C" void fn_27_376050(Object* object) {
 Vec scale;
 scale.x = scale.y = scale.z = object->actor->modules->posture->slot22();
 Vec position;
 Vec rotation;
 rotation = object->actor->modules->posture->slot4();
 position = object->actor->modules->posture->slot14(0);
 Speed* speed = (Speed*)__dynamic_cast(object->actor->modules->energy->slot6(0), 0, __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 1);
 Vec2f v;
 Vec2f::copy(v, speed->slot2());
 float x = v.m_x;
 float y = v.m_y;
 position.z = lbl_27_rodata_7BF4 * (float)atan2(y, x);
 object->actor->modules->model->slot22(0, &scale, &position, &rotation);
}
}

