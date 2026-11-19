#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_ice_berg_wind.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnPopoIceBergWind::wnPopoIceBergWind(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergWindData* data) :
    wnWeaponBuilder<wnPopoIceBergWindModuleAccesserBuildConfig>(
        "wnPopoIceBergWind", articleId, info,
        wnModuleAccesserBuildData(0x99, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

bool wnPopoIceBergWind::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

#include <so/stageobject.h>
// TODO: clean up, give variables real names
namespace jj_2096C {
extern "C" {
extern const f32 lbl_105_rodata_128;
extern const f32 lbl_105_rodata_130;
Vec3f* __ct__5Vec3fFfff(Vec3f*, f32, f32, f32);
void activate__6WeaponFP14wnActivateDesc(void*, const void*);
void fn_105_20E30(void*, int, const Vec3f*);
struct IceClimberInitFlags { u8 first:1; u8 second:1; u8 third:1; u8 fourth:1; u8 fifth:1; u8 sixth:1; u8 seventh:1; u8 eighth:1; u8 ninth:1; u8 tenth:1; u8 reserved:6; };
struct IceClimberInit { int resource,team,teamOwner,teamAnother,index,owner,reserved0,reserved1; Vec3f position; f32 facing; int parameter,variant,situation,status,reserved2,motion,reserved3; IceClimberInitFlags flags;
 IceClimberInit(int arg1,int arg2,f32 farg0) {
 Vec3f zero;
 __ct__5Vec3fFfff(&zero,lbl_105_rodata_130,lbl_105_rodata_130,lbl_105_rodata_130);
 resource=arg1; team=0xffff; teamOwner=0xffff; teamAnother=0xffff; index=-1; owner=-1; reserved0=0; reserved1=0;
 __memcpy(&position,&zero,sizeof(Vec3f));
 facing=farg0; parameter=arg2; variant=0; situation=2; status=0x80; reserved2=0; motion=0x35f; reserved3=0;
 flags.first=1; flags.second=0; flags.third=0; flags.fourth=0; flags.fifth=1; flags.sixth=0; flags.seventh=0; flags.eighth=0; flags.ninth=0; flags.tenth=0;
 }
};
inline void changeIceMotion(StageObject* object) {
 soMotionChangeParam motion;
 motion.m_kind=0; motion.m_frame=lbl_105_rodata_130; motion.m_rate=lbl_105_rodata_128; motion._12=0; motion._13=0; motion._14=0; motion._15=0;
 object->m_moduleAccesser->getMotionModule().changeMotion(&motion);
}
void fn_105_2096C(void* arg0,int arg1,int arg2,f32 farg0,void* arg3,const Vec3f* arg4,const Vec3f* arg5,f32 farg1,f32 farg2) {
 StageObject* object=(StageObject*)arg0;
 IceClimberInit initialize(arg1,arg2,farg0);
 activate__6WeaponFP14wnActivateDesc(arg0,&initialize);
 object->m_moduleAccesser->getWorkManageModule().setFloat(farg1,0x11000001);
 object->m_moduleAccesser->getWorkManageModule().setFloat(farg2,0x11000002);
 object->updateNodeSRT();
 changeIceMotion(object);
 Vec3f a=*arg4; Vec3f b=*arg5;
 if(farg1>lbl_105_rodata_130 && lbl_105_rodata_128!=farg1){a.m_x/=farg1;a.m_y/=farg1;b.m_x/=farg1;b.m_y/=farg1;}
 object->m_moduleAccesser->m_enumerationStart->m_collisionAttackModule->setOffset(2,&a);
 object->m_moduleAccesser->m_enumerationStart->m_collisionAttackModule->setOffset(1,&b);
 Vec3f c=*(const Vec3f*)arg3;
 fn_105_20E30(arg0,0,&c);
}
}
}

