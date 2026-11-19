#define IP_INPUT_BUTTON_RAW
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_dein.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

// The status unique processes of the article (wn_zelda_dein_status_uniq_process_*.cpp).
extern soStatusUniqProcess g_wnZeldaDeinStatusUniqProcessMove;
extern soStatusUniqProcess g_wnZeldaDeinStatusUniqProcessTame;

wnZeldaDein::wnZeldaDein(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinData* data) :
    wnWeaponBuilder<wnZeldaDeinModuleAccesserBuildConfig>(
        "ZeldaDein", articleId, info,
        wnModuleAccesserBuildData(0x29, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnZeldaDeinStatusUniqProcessMove;
    uniqProcs[1] = &g_wnZeldaDeinStatusUniqProcessTame;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}

// TODO: clean up, give variables real names
namespace jj_11DC4 {
extern "C" void __dt__17soArticleMediatorFv(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_104_11DC4(void* self, short flag) {
    if (self != 0) {
        __dt__17soArticleMediatorFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_12524 {
struct Vec { float x,y,z; Vec(); Vec(const Vec&); };
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual Vec slot4();
 virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
 virtual float slot9();
 virtual void slot10(); virtual void slot11(); virtual void slot12();
};
class Work {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual int slot4(int);
 virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
 virtual void slot9(); virtual void slot10(); virtual void slot11();
 virtual float slot12(int);
};
struct Modules { char pad[0xc]; Object_vptr_0* pos; char pad10[0x54]; Work* work; };
struct Sub { char pad[0x28]; void* p; };
struct Context { char pad[8]; Sub* sub; char padc[0xcc]; Modules* modules; };
extern "C" void activate__12wnZeldaDeinSFiiPC5Vec3fff(void*, void*, int, float, Vec*, float);
extern "C" int activeArticle__12wnZeldaDeinSFP16soModuleAccesser(void* self, Context* ctx) {
 void* p;
 Object_vptr_0* pos;
 Work* work;
 float f2;
 float f;
 Modules* modules = ctx->modules;
 pos = modules->pos;
 work = modules->work;
 p = ctx->sub->p;
 f = work->slot12(0x21000001);
 Vec v = pos->slot4();
 f2 = pos->slot9();
 activate__12wnZeldaDeinSFiiPC5Vec3fff(self, p, work->slot4(0x20000001), f2, &v, f);
 return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_12604 {
extern "C" {
void reflect__6WeaponFv(char* fighter);

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void call20(unsigned int arg);
};

void fn_104_12604(char* fighter)
{
    char* a = *(char**)(fighter + 0x60);
    char* b = *(char**)(a + 0xd8);
    VObj* obj = *(VObj**)(b + 0x64);
    obj->call20(0x22000000);
    reflect__6WeaponFv(fighter);
}
}
}

// TODO: clean up, give variables real names
namespace jj_12A2C {
extern "C" unsigned char g_ftRobotNullArticleStorage[];

extern "C" unsigned char* fn_104_12A2C(void* obj, int kind) {
    unsigned char buf[34];
    unsigned char* p = buf;

    switch (kind) {
        case 0: p[32] = 0; p[33] = 0; return g_ftRobotNullArticleStorage;
        case 1: p[30] = 0; p[31] = 0; return g_ftRobotNullArticleStorage;
        case 2: p[28] = 0; p[29] = 0; return g_ftRobotNullArticleStorage;
        case 3: p[26] = 0; p[27] = 0; return g_ftRobotNullArticleStorage;
        case 4: p[24] = 0; p[25] = 0; return g_ftRobotNullArticleStorage;
        case 5: p[22] = 0; p[23] = 0; return g_ftRobotNullArticleStorage;
        case 6: p[20] = 0; p[21] = 0; return g_ftRobotNullArticleStorage;
        case 7: p[18] = 0; p[19] = 0; return g_ftRobotNullArticleStorage;
        case 8: p[16] = 0; p[17] = 0; return g_ftRobotNullArticleStorage;
        case 9: p[14] = 0; p[15] = 0; return g_ftRobotNullArticleStorage;
        case 10: p[12] = 0; p[13] = 0; return g_ftRobotNullArticleStorage;
        case 11: p[10] = 0; p[11] = 0; return g_ftRobotNullArticleStorage;
        case 12: p[8] = 0; p[9] = 0; return g_ftRobotNullArticleStorage;
        case 13: p[6] = 0; p[7] = 0; return g_ftRobotNullArticleStorage;
        case 14: p[4] = 0; p[5] = 0; return g_ftRobotNullArticleStorage;
        case 15: p[2] = 0; p[3] = 0; return g_ftRobotNullArticleStorage;
        case 16: p[0] = 0; p[1] = 0; return g_ftRobotNullArticleStorage;
        default: return g_ftRobotNullArticleStorage;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_12BF8 {
extern "C" int fn_104_12BF8(void* r3, int r4, unsigned int id) {
    unsigned char buf[34];
    unsigned char* p = buf;
    switch (id) {
        case 0: p[32] = 0; p[33] = 0; return 0;
        case 1: p[30] = 0; p[31] = 0; return 0;
        case 2: p[28] = 0; p[29] = 0; return 0;
        case 3: p[26] = 0; p[27] = 0; return 0;
        case 4: p[24] = 0; p[25] = 0; return 0;
        case 5: p[22] = 0; p[23] = 0; return 0;
        case 6: p[20] = 0; p[21] = 0; return 0;
        case 7: p[18] = 0; p[19] = 0; return 0;
        case 8: p[16] = 0; p[17] = 0; return 0;
        case 9: p[14] = 0; p[15] = 0; return 0;
        case 10: p[12] = 0; p[13] = 0; return 0;
        case 11: p[10] = 0; p[11] = 0; return 0;
        case 12: p[8] = 0; p[9] = 0; return 0;
        case 13: p[6] = 0; p[7] = 0; return 0;
        case 14: p[4] = 0; p[5] = 0; return 0;
        case 15: p[2] = 0; p[3] = 0; return 0;
        case 16: p[0] = 0; p[1] = 0; return 0;
        default: return 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_12D7C {
namespace {
struct Two { bool a, b; };
}

extern "C" int fn_104_12D7C(void* r3, int r4, int id) {
    switch (id) {
    case 0: { Two s = {false, false}; return 0; }
    case 1: { Two s = {false, false}; return 0; }
    case 2: { Two s = {false, false}; return 0; }
    case 3: { Two s = {false, false}; return 0; }
    case 4: { Two s = {false, false}; return 0; }
    case 5: { Two s = {false, false}; return 0; }
    case 6: { Two s = {false, false}; return 0; }
    case 7: { Two s = {false, false}; return 0; }
    case 8: { Two s = {false, false}; return 0; }
    case 9: { Two s = {false, false}; return 0; }
    case 10: { Two s = {false, false}; return 0; }
    case 11: { Two s = {false, false}; return 0; }
    case 12: { Two s = {false, false}; return 0; }
    case 13: { Two s = {false, false}; return 0; }
    case 14: { Two s = {false, false}; return 0; }
    case 15: { Two s = {false, false}; return 0; }
    case 16: { Two s = {false, false}; return 0; }
    default: return 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_12F00 {
extern "C" int fn_104_12F00(void* obj, int kind) {
    unsigned char buf[34];
    unsigned char* p = buf;
    switch (kind) {
        case 0: p[32] = 0; p[33] = 0; return 0;
        case 1: p[30] = 0; p[31] = 0; return 0;
        case 2: p[28] = 0; p[29] = 0; return 0;
        case 3: p[26] = 0; p[27] = 0; return 0;
        case 4: p[24] = 0; p[25] = 0; return 0;
        case 5: p[22] = 0; p[23] = 0; return 0;
        case 6: p[20] = 0; p[21] = 0; return 0;
        case 7: p[18] = 0; p[19] = 0; return 0;
        case 8: p[16] = 0; p[17] = 0; return 0;
        case 9: p[14] = 0; p[15] = 0; return 0;
        case 10: p[12] = 0; p[13] = 0; return 0;
        case 11: p[10] = 0; p[11] = 0; return 0;
        case 12: p[8] = 0; p[9] = 0; return 0;
        case 13: p[6] = 0; p[7] = 0; return 0;
        case 14: p[4] = 0; p[5] = 0; return 0;
        case 15: p[2] = 0; p[3] = 0; return 0;
        case 16: p[0] = 0; p[1] = 0; return 0;
        default: return 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_13084 {
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual unsigned int slot6();
};
extern "C" int fn_104_13084(void*, void*, void* self) {
 unsigned char buf[40];
 unsigned char* p = buf;
 Object_vptr_0* obj = reinterpret_cast<Object_vptr_0*>(self);
 switch (obj->slot6()) {
 case 0: p[32]=0; p[33]=0; return 1;
 case 1: p[30]=0; p[31]=0; return 1;
 case 2: p[28]=0; p[29]=0; return 1;
 case 3: p[26]=0; p[27]=0; return 1;
 case 4: p[24]=0; p[25]=0; return 1;
 case 5: p[22]=0; p[23]=0; return 1;
 case 6: p[20]=0; p[21]=0; return 1;
 case 7: p[18]=0; p[19]=0; return 1;
 case 8: p[16]=0; p[17]=0; return 1;
 case 9: p[14]=0; p[15]=0; return 1;
 case 10: p[12]=0; p[13]=0; return 1;
 case 11: p[10]=0; p[11]=0; return 1;
 case 12: p[8]=0; p[9]=0; return 1;
 case 13: p[6]=0; p[7]=0; return 1;
 case 14: p[4]=0; p[5]=0; return 1;
 case 15: p[2]=0; p[3]=0; return 1;
 case 16: p[0]=0; p[1]=0; return 1;
 default: return 0;
 }
}
}

// TODO: clean up, give variables real names
namespace jj_1322C {
extern "C" void fn_104_1322C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_13230 {
extern "C" s32 fn_104_13230(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_13238 {
extern "C" void fn_104_13238(void* self, u8 value) {
    *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x9) = value;
}
}

// TODO: clean up, give variables real names
namespace jj_132A8 {
extern "C" void fn_104_13084(void*);

extern "C" void fn_104_132A8(void* self) {
    fn_104_13084(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_132B0 {
extern "C" void fn_104_11DC4(void*);

extern "C" void fn_104_132B0(void* self) {
    fn_104_11DC4(reinterpret_cast<u8*>(self) - 0x4);
}
}

