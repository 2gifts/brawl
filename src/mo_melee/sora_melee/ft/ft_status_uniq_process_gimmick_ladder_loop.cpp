#include <types.h>
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/stageobject.h>

void ftStatusUniqProcessGimmickLadder::initLadder(soModuleAccesser* moduleAccesser, soGimmickLadderEventArgs* info) {
    correctX(moduleAccesser, info->m_upperNodePos.m_x, 0);
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f rotation = moduleAccesser->getPostureModule().getRot(0);
        rotation.m_y = 180.0f;
        moduleAccesser->getPostureModule().setRot(&rotation, 0);
    }
    moduleAccesser->getStageObject().updateNodeSRT();
}

void ftStatusUniqProcessGimmickLadder::exitLadder(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        float lr = moduleAccesser->getPostureModule().getLr();
        float speedX = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD5, 0);
        speedX *= lr;
        Vec2f speed(speedX, 0.0f);
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(2));
        energy.m_speed = speed;
    }
}

#include <mt/mt_vector.h>
#include <nw4r/math/math_arithmetic.h>
// TODO: clean up, give variables real names
namespace jj_177608 {

struct Vec2 {
    float x;
    float y;
};

struct Cmd {
    int id;
    float a;
    float b;
    char c0;
    char c1;
    char c2;
    char c3;
};

class ObjX {
public:
    virtual void p0();
    virtual void p1();
    virtual void p2();
    virtual void p3();
    virtual Vec3f getPos();
    virtual void p5();
    virtual void p6();
    virtual void p7();
    virtual void p8();
    virtual void p9();
    virtual void p10();
    virtual void p11();
    virtual void p12();
    virtual void p13();
    virtual void p14();
    virtual void p15();
    virtual void p16();
    virtual void p17();
    virtual void p18();
    virtual void p19();
    virtual void p20();
    virtual void p21();
    virtual float getF();
};

class ObjA {
public:
    virtual void q0();
    virtual void q1();
    virtual void q2();
    virtual void q3();
    virtual void q4();
    virtual void q5();
    virtual void q6();
    virtual void setF(float);
    virtual void q8();
    virtual void q9();
    virtual void q10();
    virtual void q11();
    virtual void q12();
    virtual void q13();
    virtual void q14();
    virtual void q15();
    virtual void q16();
    virtual void q17();
    virtual void q18();
    virtual void q19();
    virtual void q20();
    virtual int getI();
    virtual void q22();
    virtual void q23();
    virtual void q24();
    virtual void q25();
    virtual void q26();
    virtual void q27();
    virtual void q28();
    virtual void q29();
    virtual void callP(void*);
};

struct Pad8 {
    int a;
    int b;
};

class ObjW : public Pad8 {
public:
    virtual void r0();
    virtual void r1();
    virtual void r2();
    virtual void r3();
    virtual void r4();
    virtual void r5();
    virtual void r6();
    virtual void r7();
    virtual void r8();
    virtual void r9();
    virtual void r10();
    virtual void r11();
    virtual void r12();
    virtual void r13();
    virtual void r14();
    virtual void r15();
    virtual void r16();
    virtual void r17();
    virtual void r18();
    virtual void r19();
    virtual void r20();
    virtual void r21();
    virtual void r22();
    virtual void r23();
    virtual void callW(int, int);
};

struct Data {
    char pad0[8];
    ObjA* a8;
    ObjX* xc;
    ObjW* w10;
};

struct Accesser {
    char pad[0xd8];
    Data* d8;
};

struct Arg {
    char pad0[8];
    float x;
    float y;
    char pad1[8];
    float f18;
    char pad2[4];
    unsigned char b20;
};

extern "C" {
void adjustLadderMovement__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserff(void* self, void* acc, float x, float y);
void getStickYRate__6ftUtilFP16soModuleAccesserfP5Vec2f(void* acc, Vec2* out);
float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(void* acc, unsigned long id, unsigned long idx);
extern const float lbl_27_rodata_2308[];
extern const float lbl_27_rodata_22F0;
}

extern "C" void execLadder__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserP24soGimmickLadderEventArgs(void* self, Accesser* acc, Arg* arg) {
    const float* k = lbl_27_rodata_2308;
    Vec3f sp20 = acc->d8->xc->getPos();
    Vec2 sp8;
    float f30 = k[2];
    int r30 = 0x17f;

    adjustLadderMovement__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserff(self, acc, arg->x, arg->y);
    getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(acc, 0xd16, 0);
    getStickYRate__6ftUtilFP16soModuleAccesserfP5Vec2f(acc, &sp8);
    if (k[3] != sp8.x) {
        if (sp8.y > k[3]) {
            int r29 = 0;
            if (arg->b20 != 0) {
                float f31 = getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(acc, 0xbe5, 0);
                if (arg->y - (sp20.m_y + f31 * acc->d8->xc->getF()) < k[4]) {
                    r29 = 1;
                }
            }
            if (r29 == 0) {
                f30 = sp8.x;
                r30 = 0x180;
            }
        } else {
            r30 = 0x181;
            f30 = sp8.x;
        }
    }
    if (arg->f18 + lbl_27_rodata_22F0 < sp20.m_y) {
        acc->d8->w10->callW(0, 0);
    } else {
        acc->d8->w10->callW(1, 0);
    }
    acc->d8->a8->setF(f30);
    if (r30 != acc->d8->a8->getI()) {
        Cmd c;
        c.id = r30;
        c.a = k[3];
        c.b = k[2];
        c.c0 = 0;
        c.c1 = 0;
        c.c2 = 0;
        c.c3 = 0;
        acc->d8->a8->callP(&c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_177834 {


extern f32 lbl_27_rodata_2310;
extern f32 lbl_27_rodata_2314;

class InitialValue {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual u64 getValue(s16*);
};

struct Vec3 {
    f32 x, y, z;
};

struct FloatBase {
    u32 pad[2];
};

class FloatAccess : public FloatBase {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16();
    virtual void s17();
    virtual void s18();
    virtual void s19();
    virtual void s20();
    virtual void s21();
    virtual void s22();
    virtual void s23();
    virtual void s24();
    virtual void s25();
    virtual void s26();
    virtual void s27();
    virtual void s28();
    virtual void s29();
    virtual void s30();
    virtual void s31();
    virtual void s32();
    virtual void s33();
    virtual void s34();
    virtual void s35();
    virtual Vec3 getFloat(u32, u32);
};

class Posture {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual Vec3 getPosition();
};

class ResultHandler {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void dispatch(s32, void*);
};

extern "C" void fixPosLadder__32ftStatusUniqProcessGimmickLadderFP16soModuleAccesserP24soGimmickLadderEventArgs(void*, u8* self, u8* config) {
    s32 result = -1;
    s16 one = 1;

    struct Pair {
        u32 x;
        u32 y;
    };

    Pair copy;
    u64 initialValue =
        (*(InitialValue**)(*(u8**)(self + 0xD8) + 0x7C))->getValue(&one);
    copy = *(Pair*)&initialValue;

    s32 constant = soValueAccesser::getConstantInt(
        (soModuleAccesser*)self, 0x59E2, 0);

    if (constant != 0) {
        FloatAccess* access =
            *(FloatAccess**)(*(u8**)(self + 0xD8) + 4);
        const Vec3& value = access->getFloat(constant, 0);

        if (value.y >= *(f32*)(config + 0xC) &&
            *(f32*)((u8*)&copy + 4) > lbl_27_rodata_2314) {
            if (config[0x20] == 0)
                result = 0x95;
        } else if (value.y <= *(f32*)(config + 0x18)) {
            result = 0xE;
        }
    }

    if (result == -1) {
        constant = soValueAccesser::getConstantInt(
            (soModuleAccesser*)self, 0x59E3, 0);

        if (constant != 0) {
            FloatAccess* access =
                *(FloatAccess**)(*(u8**)(self + 0xD8) + 4);
            const Vec3& value = access->getFloat(constant, 0);

            if (value.y >= *(f32*)(config + 0xC) &&
                *(f32*)((u8*)&copy + 4) > lbl_27_rodata_2314) {
                if (config[0x20] == 0)
                    result = 0x95;
            } else if (value.y <= *(f32*)(config + 0x18)) {
                result = 0xE;
            }
        }
    }

    if (result == -1) {
        const Vec3& position =
            (*(Posture**)(*(u8**)(self + 0xD8) + 0xC))->getPosition();
        if ((f32)(double)nw4r::math::FAbs(position.x - *(f32*)(config + 8)) >
            lbl_27_rodata_2310) {
            result = 0xE;
        }
    }

    if (result != -1) {
        (*(ResultHandler**)(*(u8**)(self + 0xD8) + 0x70))
            ->dispatch(result, self);
    }
}
}

