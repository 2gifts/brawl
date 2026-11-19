#include <ft/ft_common_data_accesser.h>
#include <ft/ft_external_value_accesser.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// TODO: the position, status and hit-target queries (getHipPos, getCursorPos, the down-wait / smash-hold status
// tests that sora_enemy calls by their REL names, getHitTarget...).

soDamageModule* ftExternalValueAccesser::getDamageModule(Fighter* fighter) {
    return &soExternalValueAccesser::getModuleAccesser(fighter)->getDamageModule();
}

soCollisionHitModule* ftExternalValueAccesser::getsoCollisionHitModule(Fighter* fighter) {
    return &soExternalValueAccesser::getModuleAccesser(fighter)->getCollisionHitModule();
}

float ftExternalValueAccesser::getWeight(Fighter* fighter) {
    return soValueAccesser::getConstantFloat(soExternalValueAccesser::getModuleAccesser(fighter), 0xbe1, 0);
}

int ftExternalValueAccesser::getMetroidNodeId() {
    return 0xc9;
}

int ftExternalValueAccesser::getNodeIdFromHitNo(u8 hitNo) {
    return hitNo + 0xcb;
}

// The audience table hangs off the common fighter data (word 0x40).
u32* ftExternalValueAccesser::getAudienceData() {
    return *reinterpret_cast<u32**>(&g_ftCommonData.dataCommon->unk40[0]);
}

// TODO: clean up, give variables real names
namespace jj_14D19C {
struct Vec3 {
    float x, y, z;
};

struct Fighter;

struct ModelModule {
    char padding[8];
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual int getNodeId(int);
    virtual void slot34();
    virtual void slot35();
    virtual Vec3 getGlobalPosition(int, bool);
};

struct Modules {
    char padding[4];
    ModelModule* model;
};

struct Access {
    char padding[0xd8];
    Modules* modules;
};

extern "C" Access* getModuleAccesser__23soExternalValueAccesserFP11StageObject(Fighter*);

extern "C" Vec3 getHipPos__23ftExternalValueAccesserFP7Fighter(Fighter* fighter) {
    Access* access = getModuleAccesser__23soExternalValueAccesserFP11StageObject(fighter);
    int node = access->modules->model->getNodeId(0x12d);
    return access->modules->model->getGlobalPosition(node, false);
}
}

// TODO: clean up, give variables real names
namespace jj_14D218 {
struct Vec3 { float x, y, z; Vec3() {} Vec3(const Vec3& o) : x(o.x), y(o.y), z(o.z) {} };
struct Vec2 { float x, y; };
struct Fighter;
struct Access;

#define S(n) virtual void s##n();
struct Posture {
    S(0) S(1) S(2) S(3)
    virtual Vec3 getPos();
    S(5) S(6)
    virtual Vec3 getPos2();
    S(8) S(9) S(10) S(11) S(12) S(13) S(14) S(15) S(16) S(17) S(18) S(19) S(20) S(21)
    virtual float getScale();
};
struct Ground {
    char pad[8];
    S(0) S(1) S(2) S(3) S(4) S(5) S(6) S(7) S(8) S(9)
    S(10) S(11) S(12) S(13) S(14) S(15) S(16) S(17) S(18) S(19)
    S(20) S(21) S(22) S(23) S(24) S(25) S(26) S(27) S(28) S(29)
    S(30) S(31) S(32) S(33) S(34) S(35) S(36) S(37) S(38) S(39)
    S(40) S(41) S(42) S(43) S(44) S(45) S(46) S(47) S(48) S(49)
    S(50) S(51) S(52) S(53) S(54) S(55) S(56) S(57) S(58) S(59)
    S(60) S(61) S(62) S(63) S(64) S(65) S(66) S(67) S(68) S(69)
    S(70) S(71) S(72) S(73) S(74) S(75) S(76) S(77) S(78) S(79)
    S(80) S(81)
    virtual Vec2 getDownPos(int);
    S(83) S(84) S(85) S(86) S(87) S(88) S(89) S(90) S(91) S(92) S(93) S(94)
    S(95) S(96) S(97) S(98) S(99) S(100) S(101)
    virtual void getRange(float*, float*, float*, float*, int);
};
struct Kinetic {
    S(0) S(1) S(2) S(3) S(4) S(5) S(6) S(7) S(8) S(9)
    S(10) S(11) S(12) S(13) S(14) S(15) S(16) S(17) S(18) S(19)
    S(20) S(21) S(22) S(23) S(24) S(25) S(26) S(27) S(28) S(29)
    S(30) S(31) S(32) S(33) S(34) S(35) S(36) S(37) S(38) S(39)
    S(40) S(41) S(42) S(43) S(44) S(45) S(46) S(47) S(48) S(49)
    S(50) S(51) S(52) S(53) S(54) S(55) S(56) S(57) S(58) S(59)
    S(60) S(61) S(62) S(63) S(64) S(65) S(66) S(67) S(68) S(69)
    S(70) S(71) S(72) S(73) S(74) S(75) S(76) S(77) S(78) S(79)
    S(80) S(81) S(82) S(83) S(84) S(85) S(86) S(87)
    virtual unsigned int getSituation();
};
struct Work {
    S(0) S(1) S(2) S(3) S(4) S(5) S(6) S(7) S(8) S(9)
    S(10) S(11)
    virtual float getFloat(unsigned int);
    S(13) S(14) S(15) S(16)
    virtual unsigned int isFlag(unsigned int);
};
struct Modules {
    char p0[0xc];
    Posture* posture;
    Ground* ground;
    char p1[0x54 - 0x14];
    Kinetic* kinetic;
    char p2[0x64 - 0x58];
    Work* work;
};
struct Access {
    char padding[0xd8];
    Modules* modules;
};

extern "C" Access* getModuleAccesser__23soExternalValueAccesserFP11StageObject(Fighter*);
extern "C" float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(Access*, unsigned long, unsigned long);
extern const float lbl_27_rodata_18E8;

static inline Vec2 down(Ground* g) {
    Vec2 r = g->getDownPos(0);
    return r;
}

extern "C" Vec3 getCursorPos__23ftExternalValueAccesserFP7Fighter(Fighter* fighter) {
    Access* acc = getModuleAccesser__23soExternalValueAccesserFP11StageObject(fighter);
    Vec3 pos(acc->modules->posture->getPos());
    if (!acc->modules->work->isFlag(0x1200003c)) {
        float s = acc->modules->posture->getScale();
        float k = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(acc, 0xc08, 0);
        s = k * s;
        if (acc->modules->kinetic->getSituation() == 1) {
            pos = acc->modules->posture->getPos2();
            pos.y += lbl_27_rodata_18E8 * s;
        } else if (acc->modules->work->isFlag(0x12000038) == 1) {
            float a, b, c, d;
            acc->modules->ground->getRange(&a, &b, &c, &d, 0);
            pos.y += a + s;
        } else {
            Vec2 p = down(acc->modules->ground);
            pos.y = p.y + s;
        }
    }
    pos.y += acc->modules->work->getFloat(0x11000012);
    return pos;
}
}

// TODO: clean up, give variables real names
namespace jj_14D418 {
extern "C" void* getModuleAccesser__23soExternalValueAccesserFP11StageObject(void*);
extern "C" void getTouchLineWork__12grCollStatusFQ212grCollStatus7TouchId(void*, void*);

struct Module {
    char padding[8];
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void* get(int);
};

struct Modules {
    char padding[0x10];
    Module* module;
};

struct Access {
    char padding[0xd8];
    Modules* modules;
};

extern "C" void fn_27_14D418(void* self, void* arg) {
    Access* access =
        (Access*)getModuleAccesser__23soExternalValueAccesserFP11StageObject(self);
    Module* module = access->modules->module;
    getTouchLineWork__12grCollStatusFQ212grCollStatus7TouchId(module->get(0), arg);
}
}

// TODO: clean up, give variables real names
namespace jj_14D4E4 {
typedef unsigned int u32;

extern "C" u32 getModuleAccesser__23soExternalValueAccesserFP11StageObject(void* self);

struct VBase {
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
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual VBase* get();
};

extern "C" u32 fn_27_14D4E4(void* self) {
    char* obj = (char*)getModuleAccesser__23soExternalValueAccesserFP11StageObject(self);
    char* m = *(char**)(obj + 0xd8);
    VBase* sub = *(VBase**)(m + 0x50);
    char* r = (char*)sub->get();
    char* a = *(char**)r;
    char* b = *(char**)(a + 0x18);
    return *(u32*)(b + 0xc);
}
}

// TODO: clean up, give variables real names
namespace jj_14D530 {
extern "C" unsigned int fn_27_14D530(unsigned int x)
{
    unsigned int r3 = 1;
    unsigned int r4 = x - 77;
    r3 = r3 | ~r4;
    unsigned int r0 = (1 - r4) >> 1;
    return (r3 - r0) >> 31;
}
}

// TODO: clean up, give variables real names
namespace jj_14D550 {
extern "C" int fn_27_14D550(int c) {
    if (c == 0x2a) {
        goto yes;
    }
    if (c == 0x2b) {
        goto yes;
    }
    if (c == 0x2d) {
        goto yes;
    }
    if (c == 0x2e) {
        goto yes;
    }
    if ((unsigned int)(c - 0x30) > 1) {
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_14D594 {
extern "C" {
void* getParamCommon__20ftCommonDataAccesserCFv(const void* self);
extern unsigned char g_ftCommonDataAccesser[];

void* setFinal__7ftEntryFb() {
    return *(void**)((unsigned char*)getParamCommon__20ftCommonDataAccesserCFv(g_ftCommonDataAccesser) + 0x234);
}
}
}

// TODO: clean up, give variables real names
namespace jj_14D610 {
#define SLOT(n) virtual void unused##n();

struct StageObject {
    char padding[0x3C];
    SLOT(0) SLOT(1) SLOT(2) SLOT(3) SLOT(4) SLOT(5) SLOT(6) SLOT(7) SLOT(8) SLOT(9)
    SLOT(10) SLOT(11) SLOT(12) SLOT(13) SLOT(14) SLOT(15) SLOT(16) SLOT(17) SLOT(18) SLOT(19)
    SLOT(20) SLOT(21) SLOT(22) SLOT(23) SLOT(24) SLOT(25) SLOT(26) SLOT(27) SLOT(28) SLOT(29)
    SLOT(30) SLOT(31) SLOT(32) SLOT(33) SLOT(34) SLOT(35) SLOT(36) SLOT(37) SLOT(38) SLOT(39)
    SLOT(40) SLOT(41) SLOT(42) SLOT(43) SLOT(44) SLOT(45) SLOT(46) SLOT(47) SLOT(48) SLOT(49)
    SLOT(50) SLOT(51) SLOT(52) SLOT(53) SLOT(54) SLOT(55) SLOT(56) SLOT(57) SLOT(58) SLOT(59)
    SLOT(60) SLOT(61) SLOT(62) SLOT(63) SLOT(64) SLOT(65) SLOT(66) SLOT(67) SLOT(68) SLOT(69)
    SLOT(70) SLOT(71) SLOT(72) SLOT(73) SLOT(74) SLOT(75) SLOT(76) SLOT(77) SLOT(78) SLOT(79)
    SLOT(80) SLOT(81) SLOT(82) SLOT(83) SLOT(84) SLOT(85) SLOT(86) SLOT(87) SLOT(88) SLOT(89)
    SLOT(90) SLOT(91) SLOT(92) SLOT(93) SLOT(94) SLOT(95) SLOT(96) SLOT(97) SLOT(98) SLOT(99)
    SLOT(100) SLOT(101) SLOT(102) SLOT(103) SLOT(104) SLOT(105) SLOT(106) SLOT(107) SLOT(108) SLOT(109)
    SLOT(110) SLOT(111) SLOT(112) SLOT(113) SLOT(114) SLOT(115) SLOT(116) SLOT(117) SLOT(118) SLOT(119)
    SLOT(120) SLOT(121) SLOT(122) SLOT(123) SLOT(124) SLOT(125) SLOT(126) SLOT(127) SLOT(128) SLOT(129)
    SLOT(130) SLOT(131) SLOT(132) SLOT(133) SLOT(134) SLOT(135) SLOT(136) SLOT(137) SLOT(138) SLOT(139)
    SLOT(140) SLOT(141) SLOT(142) SLOT(143) SLOT(144) SLOT(145) SLOT(146) SLOT(147) SLOT(148) SLOT(149)
    SLOT(150) SLOT(151) SLOT(152) SLOT(153) SLOT(154) SLOT(155) SLOT(156) SLOT(157) SLOT(158) SLOT(159)
    SLOT(160) SLOT(161) SLOT(162) SLOT(163) SLOT(164) SLOT(165) SLOT(166) SLOT(167) SLOT(168) SLOT(169)
    SLOT(170) SLOT(171) SLOT(172) SLOT(173) SLOT(174) SLOT(175) SLOT(176) SLOT(177) SLOT(178) SLOT(179)
    SLOT(180) SLOT(181) SLOT(182) SLOT(183) SLOT(184) SLOT(185)
    virtual int getKind();
};

struct WorkModule {
    SLOT(0) SLOT(1) SLOT(2) SLOT(3) SLOT(4) SLOT(5) SLOT(6) SLOT(7) SLOT(8) SLOT(9)
    SLOT(10) SLOT(11) SLOT(12) SLOT(13) SLOT(14) SLOT(15) SLOT(16)
    virtual bool isFlag(unsigned int);
};

#undef SLOT

struct ModuleTable {
    char padding[0x64];
    WorkModule* work;
};

struct Access {
    char padding[0xD8];
    ModuleTable* modules;
};

extern "C" Access* getModuleAccesser__23soExternalValueAccesserFP11StageObject(StageObject*);

extern "C" bool fn_27_14D610(StageObject* self) {
    if (self->getKind() == 5) {
        Access* access = getModuleAccesser__23soExternalValueAccesserFP11StageObject(self);
        return access->modules->work->isFlag(0x1200001F);
    }
    return false;
}
}

// TODO: clean up, give variables real names
namespace jj_14D680 {
typedef int s32;

struct Object {
    unsigned char padding[0x3C];

    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual s32 getKind();
};

struct Array {
    s32* data;
    s32 count;
};

struct CommonData {
    unsigned char padding[0x48];
    Array* array;
};

extern "C" {
extern unsigned char g_ftCommonDataAccesser;
CommonData* getParamEtc__20ftCommonDataAccesserCF6ftKind(void*, s32);

s32 getHitTarget__23ftExternalValueAccesserFP7Fighteri(Object* object, s32 index) {
    CommonData* common =
        getParamEtc__20ftCommonDataAccesserCF6ftKind(&g_ftCommonDataAccesser, object->getKind());

    if (index < 0 || index >= common->array->count) {
        return 0;
    }

    return common->array->data[index];
}
}
}

// TODO: clean up, give variables real names
namespace jj_14D6F4 {
typedef int s32;

struct Object {
    unsigned char padding[0x3C];

    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual s32 getKind();
};

struct Array {
    s32* data;
    s32 count;
};

struct CommonData {
    unsigned char padding[0x48];
    Array* array;
};

extern "C" {
extern unsigned char g_ftCommonDataAccesser;
CommonData* getParamEtc__20ftCommonDataAccesserCF6ftKind(void*, s32);

s32 fn_27_14D6F4(Object* object, s32 index) {
    CommonData* common =
        getParamEtc__20ftCommonDataAccesserCF6ftKind(&g_ftCommonDataAccesser, object->getKind());
    return common->array->data[index];
}
}
}

// TODO: clean up, give variables real names
namespace jj_14D74C {
#define V(n) virtual void slot##n();
class Object_vptr_0 {
public:
V(0) V(1) V(2) V(3) V(4)
virtual int slot5();
V(6) V(7) V(8) V(9) V(10) V(11) V(12) V(13) V(14) V(15)
virtual unsigned int slot16();
V(17) V(18) V(19) V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29) V(30) V(31) V(32) V(33)
virtual int slot34(int);
};
struct Object_vptr_3c_base { char pad[0x3c]; };
class Object_vptr_3c : public Object_vptr_3c_base {
public:
V(0) V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9) V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19) V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29) V(30) V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39)
virtual unsigned int slot40();
};
#undef V
struct Modules {
char pad[0x20];
Object_vptr_0* m20;
char pad2[0x1c];
Object_vptr_0* m40;
char pad3[0x2c];
Object_vptr_0* m70;
};
struct Access { char pad[0xd8]; Modules* modules; };
extern "C" unsigned int* getFtStatusData__20ftCommonDataAccesserCF6ftKindi(void*, unsigned int, unsigned int);
extern "C" char g_ftCommonDataAccesser[];
extern "C" int fn_27_14D74C(Object_vptr_3c* obj) {
    unsigned int motion = ((Access*)soExternalValueAccesser::getModuleAccesser((StageObject*)obj))->modules->m70->slot16();
    unsigned int kind = obj->slot40();
    if ((*getFtStatusData__20ftCommonDataAccesserCF6ftKindi(g_ftCommonDataAccesser, kind, motion) >> 27) & 1)
        return 0;
    if (((Access*)soExternalValueAccesser::getModuleAccesser((StageObject*)obj))->modules->m40->slot5())
        return 0;
    return ((Access*)soExternalValueAccesser::getModuleAccesser((StageObject*)obj))->modules->m20->slot34(0) == 0;
}
}

