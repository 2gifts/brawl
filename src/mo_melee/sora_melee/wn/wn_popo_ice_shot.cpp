#define WN_PARAM_ACCESSER_INLINE_DTOR
#define FT_MODULE_BUILDER
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define SO_NULLABLE_INLINE_CTOR
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_ice_shot.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_extend_param_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// MATCH-ONLY: the original keeps these five module builders out of line.
typedef wnPopoIceShotModuleAccesserBuildConfig IceShotBC;
// MATCH-ONLY: instantiate the helpers the out-of-line builders call inline before inlining is switched off.
static void warmInlineHelpers() {
    ftKineticEnergyManager manager(false);
    soNullable nullable(false);
}
#pragma dont_inline on
template soInsideEventManageModuleBuilder<IceShotBC::InsideEventManageModuleBuildConfig, wnInsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
template soDamageModuleBuilder<IceShotBC::DamageModuleBuildConfig>::soDamageModuleBuilder(soModuleAccesser*, soEventObserverRegistrationDesc*);
template soCollisionHitModuleBuilder<IceShotBC::CollisionHitModuleBuildConfig>::soCollisionHitModuleBuilder(soModuleAccesser*, int, u8, soEventObserverRegistrationDesc*);
template soKineticModuleBuilder<IceShotBC::KineticModuleBuildConfig>::soKineticModuleBuilder(soModuleAccesser*);
template soSlopeModuleBuilder<IceShotBC::SlopeModuleBuildConfig>::soSlopeModuleBuilder(soModuleAccesser*, float);
template soEventManageModuleImpl* soInsideEventManageModuleBuilder<IceShotBC::InsideEventManageModuleBuildConfig, wnInsideEventManageModuleTypes>::getModule();
template soCollisionHitModule* soCollisionHitModuleBuilder<IceShotBC::CollisionHitModuleBuildConfig>::getModule();
template soDamageModuleImpl* soDamageModuleBuilder<IceShotBC::DamageModuleBuildConfig>::getModule();
template soKineticModule* soKineticModuleBuilder<IceShotBC::KineticModuleBuildConfig>::getModule();
template soSlopeModuleImpl* soSlopeModuleBuilder<IceShotBC::SlopeModuleBuildConfig>::getModule();
template soModuleAccesser* soModuleAccesserBuilder<IceShotBC>::getModuleAccesser();
#pragma dont_inline off

wnPopoIceShot::wnPopoIceShot(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceShotData* data) :
    wnWeaponBuilder<wnPopoIceShotModuleAccesserBuildConfig>(
        "PopoIceShot", articleId, info,
        wnModuleAccesserBuildData(0x6d, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(ftGetManageId(m_moduleAccesser), -1),
    soCollisionAttackEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param), m_paramAccesser(data->m_param) {
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}

void wnPopoIceShot::activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, float lr, int unk40) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = unk40;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc1, 0), 0x10000006);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000003);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(0xd), true);
}

void wnPopoIceShot::shoot() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0) +
                                                     soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0),
                                                 0x10000005);
    setGroundShapeSafePosWithAncestor();
    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
    if (m_moduleAccesser->getCollisionAttackModule().isAttack(0, false) == true) {
        m_moduleAccesser->getWorkManageModule().setFloat(m_moduleAccesser->getCollisionAttackModule().getPower(0, false), 0x11000001);
    }
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(0xd), false);
}

void wnPopoIceShot::processUpdate() {
    Weapon::processUpdate();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        if (m_moduleAccesser->getWorkManageModule().getInt(0x10000005) >= 0) {
            m_moduleAccesser->getWorkManageModule().decInt(0x10000005);
        }
        if (m_moduleAccesser->getWorkManageModule().getInt(0x10000005) < soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc4, 0)) {
            soPostureModule& posture = m_moduleAccesser->getPostureModule();
            posture.setScale(posture.getBaseScale() * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa5, 0));
            if (m_moduleAccesser->getCollisionAttackModule().isAttack(0, false) == true) {
                float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
                power = power * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa6, 0);
                m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
                m_moduleAccesser->getCollisionAttackModule().setPower(0, power, false);
                soModuleAccesser* moduleAccesser = m_moduleAccesser;
                if (moduleAccesser->getWorkManageModule().getInt(0x10000005) <
                    soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0) - soValueAccesser::getConstantInt(moduleAccesser, 0x5dc3, 0)) {
                    m_moduleAccesser->getCollisionAttackModule().setReactionEffect(0, 0, false);
                    m_moduleAccesser->getCollisionAttackModule().setReactionFix(0, 0, false);
                    m_moduleAccesser->getCollisionAttackModule().setReactionAdd(0, 0, false);
                }
            }
        }
    }
}

bool wnPopoIceShot::notifyEventCollisionAttackCheck(u32 flags) {
    bool result = false;
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1 &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000005) == false &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000006) == true) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        result = true;
    }
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000005);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000006);
    return result;
}

void wnPopoIceShot::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

void wnPopoIceShot::updatePosture(bool update) {
    StageObject::updatePosture(update);
}

template class wnExtendParamAccesser<3999, 11, 23999, 7, 0, 0>;

// Registers the Popo ice shot article class (kind 109) in the class info table during static initialization.
wnClassInfoImpl<109, wnPopoIceShot, wnPopoIceShotData> g_wnPopoIceShotClassInfo;

#include <new>
#include <so/collision/so_collision_hit_module_impl.h>
#include <nw4r/math/math_types.h>
#include <so/so_enable.h>
// TODO: clean up, give variables real names
namespace jj_349E18 {
class soCollisionAttackModule;
#pragma use_simple_stdpnew on

extern "C" {
extern char lbl_27_data_39430[], lbl_27_data_46F8[], lbl_27_data_1E0A0[];
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void __ct__18soCollisionHitPartFv(void*);
void __dt__18soCollisionHitPartFv(void*);
void __ct__16soCollisionGroupFv(void*);
void __dt__16soCollisionGroupFv(void*);
void __ct__19soCollisionHitGroupFv(void*);
void __dt__19soCollisionHitGroupFv(void*);
int resize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacelbblll(void*, int, int, int, int, int, int);
void postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(void*, int, int);
}
struct Bits { unsigned int a:4, b:2, c:1, rest:25; };
struct Obj {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual int v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(const soCollisionHitPart&);
    virtual void v11(); virtual void v12(); virtual int v13(); virtual int v14();
    virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18();
    virtual void v19(); virtual void v20(); virtual void v21(); virtual void v22();
    virtual void v23(); virtual void v24(); virtual void v25(); virtual void v26();
    virtual int v27(); virtual int v28();
    void* calc; unsigned int flags; char parts[0x68];
    void* vt2; void* calc2; Bits bits2; char groups[0x78];
    void* vt3; void* calc3; Bits bits3; char hitgroups[0x38];
    char impl[0x68];
};
extern "C" Obj* fn_27_349E18(Obj* self, soModuleAccesser* accesser, int id, gfTask::Category category, soEventObserverRegistrationDesc* desc) {
    {
        soCollisionHitPart part((soCollision::Category)0, 0x3ff);
        *(void**)self = lbl_27_data_39430;
        self->calc = lbl_27_data_39430 + 0x48;
        self->flags &= 0x01ffffff;
        __construct_array(self->parts, __ct__18soCollisionHitPartFv, __dt__18soCollisionHitPartFv, 0x68, 1);
        int d, c, b, a;
        a = self->v28();
        b = self->v27();
        c = self->v13();
        d = self->v14();
        int e = self->v4();
        int count = resize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacelbblll(&self->calc, 1, e, d, c, b, a);
        for (int i = 0; i < count; i++) self->v10(part);
    }
    self->vt2 = lbl_27_data_46F8;
    self->calc2 = lbl_27_data_46F8 + 0x48;
    self->bits2.a = 0; self->bits2.b = 1; self->bits2.c = 0;
    __construct_array(self->groups, __ct__16soCollisionGroupFv, __dt__16soCollisionGroupFv, 0x78, 1);
    postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(&self->calc2, 1, 1);
    self->vt3 = lbl_27_data_1E0A0;
    self->calc3 = lbl_27_data_1E0A0 + 0x48;
    self->bits3.a = 0; self->bits3.b = 1; self->bits3.c = 0;
    __construct_array(self->hitgroups, __ct__19soCollisionHitGroupFv, __dt__19soCollisionHitGroupFv, 0x38, 1);
    postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(&self->calc3, 1, 1);
    new (self->impl) soCollisionHitModuleImpl(accesser, id, category,
        (soArray<soCollisionHitPart>*)self, (soArray<soCollisionGroup>*)&self->vt2,
        (soArray<soCollisionHitGroup>*)&self->vt3, desc, true);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_34BCA0 {
#pragma fp_contract off
class Object_vptr_0 {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2();
virtual void slot3();
virtual int slot4(unsigned);
virtual void slot5();
virtual void slot6();
virtual void slot7();
virtual void slot8(unsigned);
virtual void slot9();
virtual void slot10();
virtual void slot11();
virtual float slot12(unsigned);
virtual void slot13(unsigned, float);
virtual unsigned slot14(int,int);
virtual void slot15();
virtual int slot16();
virtual void slot17(int,int,int);
virtual void slot18();
virtual void slot19();
virtual void slot20();
virtual float slot21();
virtual void slot22();
virtual void slot23(float);
virtual void slot24();
virtual void slot25();
virtual void slot26();
virtual void slot27();
virtual void slot28();
virtual void slot29(int,int,int);
virtual void slot30();
virtual void slot31(int,int,int);
virtual void slot32();
virtual void slot33(int,int,int);
};
struct Modules {
char pad0[0xc]; Object_vptr_0* posture;
char pad10[0xc]; Object_vptr_0* motion;
char pad20[0x44]; Object_vptr_0* work;
char pad68[8]; Object_vptr_0* status;
};
struct Access { char pad[0xd8]; Modules* modules; };
struct Self { char pad[0x60]; Access* access; };
extern "C" void processUpdate__6WeaponFv(void*);
extern "C" void fn_27_34BCA0(Self* self) {
int max;
Object_vptr_0* work;
Object_vptr_0* work2;
Access* access2;
Object_vptr_0* posture;
processUpdate__6WeaponFv(self);
if (self->access->modules->status->slot16() == 1) {
if (self->access->modules->work->slot4(0x10000005) >= 0)
self->access->modules->work->slot8(0x10000005);
work = self->access->modules->work;
max = soValueAccesser::getConstantInt((soModuleAccesser*)self->access,0x5dc4,0);
if (work->slot4(0x10000005) < max) {
posture = self->access->modules->posture;
float scale = soValueAccesser::getConstantFloat((soModuleAccesser*)self->access,0xfa5,0);
float v = posture->slot21();
posture->slot23(v * scale);
if (self->access->modules->motion->slot14(0,0) == 1) {
float value = self->access->modules->work->slot12(0x11000001);
value *= soValueAccesser::getConstantFloat((soModuleAccesser*)self->access,0xfa6,0);
self->access->modules->work->slot13(0x11000001,value);
self->access->modules->motion->slot17(0,(int)value,0);
access2 = self->access;
work2 = access2->modules->work;
max = soValueAccesser::getConstantInt((soModuleAccesser*)access2,0x5dc4,0);
max -= soValueAccesser::getConstantInt((soModuleAccesser*)access2,0x5dc3,0);
if (work2->slot4(0x10000005) < max) {
self->access->modules->motion->slot29(0,0,0);
self->access->modules->motion->slot31(0,0,0);
self->access->modules->motion->slot33(0,0,0);
}
}
}
}
}
}

// TODO: clean up, give variables real names
namespace jj_34BF64 {
#pragma scheduling on
using namespace nw4r::math;
extern "C" float lbl_27_rodata_78DC;
class Object_vptr_0 {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2();
virtual void slot3(int,void*);
virtual int slot4(int);
virtual void slot5();
virtual void slot6();
virtual void slot7();
virtual void slot8();
virtual void slot9();
virtual void slot10(int,int);
virtual void slot11();
virtual void slot12();
virtual _VEC2 slot13(short*);
virtual void slot14();
virtual void slot15();
virtual void slot16();
virtual void slot17();
virtual void slot18(int,void*);
};
class Posture {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2();
virtual void slot3();
virtual VEC3 slot4();
};
struct Data {
char pad[0xc]; Posture* c;
char pad1[0x54]; Object_vptr_0* obj64;
char pad2[8]; Object_vptr_0* obj70;
char pad3[8]; Object_vptr_0* obj7C;
};
struct Mod { char pad[0xd8]; Data* data; };
struct Fighter { char pad[0x60]; Mod* mod; };
struct Input { char pad[4]; float y,z; char pad2[0x74]; VEC3 pos; };
inline void subtractPosition(const VEC3* pos,const VEC3* target,VEC3* out) { VEC3Sub(out,target,pos); }
inline float directionSign(float v) { return v < lbl_27_rodata_78DC ? -1 : 1; }
extern "C" void notifyEventOnDamage__13wnPopoIceShotFP8soDamagebP16soModuleAccesser(Fighter* self,Input in) {
int value=(int)in.y;
self->mod->data->obj64->slot10(value,0x10000006);
if(self->mod->data->obj64->slot4(0x10000006)<0) {
self->mod->data->obj70->slot3(2,self->mod);
return;
}
if(in.z>lbl_27_rodata_78DC) {
VEC3 diff;
VEC3 pos=self->mod->data->c->slot4();
short flag=-1;
subtractPosition(&pos,&in.pos,&diff);
VEC2 delta;
delta.x=diff.x; delta.y=diff.y;
_VEC2 vec=self->mod->data->obj7C->slot13(&flag);
float a=vec.x<lbl_27_rodata_78DC ? -1 : 1;
float b=directionSign(delta.x);
bool match=b==a;
if(match) self->mod->data->obj64->slot18(0x12000004,self->mod);
}
}
}

// TODO: clean up, give variables real names
namespace jj_34CA68 {
extern "C" void fn_27_34CF20(void*, void*);
extern "C" void checkClearSpeed__23soKineticTransactHelperFP15soKineticEnergy(void*);

extern "C" void fn_27_34CA68(void* object, void* arg) {
    fn_27_34CF20(arg, static_cast<char*>(object) + 4);
    checkClearSpeed__23soKineticTransactHelperFP15soKineticEnergy(static_cast<char*>(object) + 0x14);
}
}

// TODO: clean up, give variables real names
namespace jj_34CAA4 {
struct Data {
    unsigned char pad[5];
    unsigned char active : 1;
    unsigned char rest : 7;
    unsigned char state;
};
extern "C" void fn_27_34D3FC(Data*, int);
extern "C" void fn_27_34CAA4(void* self, int arg) {
    Data* data = (Data*)((char*)self + 0x14);
    if ((bool)data->active == true && data->state == 0)
        fn_27_34D3FC(data, arg);
}
}

// TODO: clean up, give variables real names
namespace jj_34CB38 {

namespace {
class SoElement {
public:
    virtual void slot0();
    virtual void slot1(void*);
    virtual void slot2();
    virtual void slot3();
    unsigned char reserved;
    soEnable enabled;
    unsigned char suspension;
};

class SoContainer {
public:
    virtual void slot0();
    virtual SoElement*** slot1(int);
    virtual void slot2();
    virtual int slot3();
};
}

extern "C" void fn_27_34CB38(void*, SoContainer* container, void* arg) {
    for (int i = 0; i < container->slot3(); ++i) {
        SoElement* element = **container->slot1(i);
        unsigned int enabled = element->enabled.isEnable();
        unsigned int suspension;

        if (enabled && (suspension = element->suspension) != 1) {
            if (enabled == true && suspension == 0) {
                if (enabled == true && suspension == 0) {
                    element->slot1(arg);
                }
            }
        }
    }
}
}

