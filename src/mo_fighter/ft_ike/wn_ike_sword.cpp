#define SO_BUILDER_HELPERS_WEAK
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/ike/wn_ike_sword.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// The status unique process of the sword (slot 3). HYPOTHESIS: unknown status.
extern soStatusUniqProcess g_wnIkeSwordStatusUniqProcessUnk3;

wnIkeSword::wnIkeSword(s32 articleId, const wnArticleBuildInfo& info, const wnIkeSwordData* data) :
    wnWeaponBuilder<wnIkeSwordModuleAccesserBuildConfig>(
        "wnIkeSword", articleId, info,
        wnModuleAccesserBuildData(0x3A, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[4];
    uniqProcs[0] = 0;
    uniqProcs[1] = 0;
    uniqProcs[2] = 0;
    uniqProcs[3] = &g_wnIkeSwordStatusUniqProcessUnk3;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 4);
    Weapon::deactivate(false);
}

#include <so/stageobject.h>
// TODO: clean up, give variables real names
namespace jj_113E4 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__15soParamAccesserFv(void* p, short size) {
    if (p != 0 && size > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_12550 {
extern "C" void* __dt__18soGlowModuleSimpleFv(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_119_12550(void* object, int flag) {
    if (object != 0) {
        __dt__18soGlowModuleSimpleFv(object, -1);
        if ((short)flag > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_12BD8 {

struct IkeLaunchParam {
    int resource, team, teamOwner, teamAnother, index, owner;
    int reserved0, reserved1;
    Vec3f position;
    float facing;
    int parameter, variant, situation, status, reserved2, motion, reserved3;
    unsigned short flags;
};

extern "C" {
void activate__6WeaponFP14wnActivateDesc(void*, const IkeLaunchParam*);
void __ct__14wnActivateDescFUlUlbRC5Vec3ffiiiiiiUlUlibi(IkeLaunchParam* param, int a1, int a2, bool a3, int a4, int a5, int a6, int a7,
                  int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15);
}

extern "C" void activate__10wnIkeSwordFiiiP5Vec3ffibb(StageObject* object, int p4, int p5, int p6, int p7, int p8, int p9, int p10)
{
    if (p9) {
        IkeLaunchParam initialize;
        __ct__14wnActivateDescFUlUlbRC5Vec3ffiiiiiiUlUlibi(&initialize, p4, p5, true, p7, p6, 0, 0, 0, 2, -1, 0xffff, 0xffff, 0, 1, 0x80);
        initialize.owner = p10;
        activate__6WeaponFP14wnActivateDesc(object, &initialize);
        object->m_moduleAccesser->m_enumerationStart->m_statusModule->changeStatusForce(2, object->m_moduleAccesser);
        object->m_moduleAccesser->m_enumerationStart->m_workManageModule->onFlag(0x22000001);
    } else {
        IkeLaunchParam initialize;
        __ct__14wnActivateDescFUlUlbRC5Vec3ffiiiiiiUlUlibi(&initialize, p4, p5, false, p7, p6, 0, 0, 0, 2, -1, 0xffff, 0xffff, 0, 1, 0x80);
        initialize.owner = p10;
        activate__6WeaponFP14wnActivateDesc(object, &initialize);
        object->m_moduleAccesser->m_enumerationStart->m_linkModule->setModelConstraintPosOrt(3, 0, 0, 3, true);
        object->m_moduleAccesser->m_enumerationStart->m_workManageModule->offFlag(0x22000001);
        if (p8 == 0) {
            object->m_moduleAccesser->m_enumerationStart->m_statusModule->changeStatusForce(0, object->m_moduleAccesser);
        } else {
            object->m_moduleAccesser->m_enumerationStart->m_statusModule->changeStatusForce(1, object->m_moduleAccesser);
        }
    }
    object->m_moduleAccesser->m_enumerationStart->m_workManageModule->onFlag(0x12000000);
}
}

