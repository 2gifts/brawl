#include <ft/ft_value_accesser.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern int g_soValueVariation;

ftValueAccesser::ftValueAccesser() : soValueAccesser(0) { }

ftValueAccesser::~ftValueAccesser() { }

void* ftValueAccesser::getConstantIndefiniteCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if ((s32)id <= 42999)
        return soValueAccesser::getConstantIndefiniteCore(moduleAccesser, id, index);
    if ((s32)id < 43025)
        return moduleAccesser->getParamCustomizeModule().getParamIndefinite(g_soValueVariation, id);
    return g_ftCommonDataAccesser.getParamCommonIndefinite(g_soValueVariation, id);
}

bool ftValueAccesser::isControllerValueCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if (soValueAccesser::isControllerValueCore(moduleAccesser, id, index) == true)
        return true;
    if ((s32)id > 999 && (s32)id < 1037) {
        if ((s32)id >= 1010 && (s32)id <= 1028)
            return true;
    } else if ((s32)id > 20999 && (s32)id < 21030) {
        if ((s32)id >= 21001 && (s32)id <= 21021)
            return true;
    }
    return false;
}

#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
// TODO: clean up, give variables real names
namespace jj_149E5C {
enum ftKind {
    ftKind_None = -1
};

class ftExtendParamAccesser {
public:
    static ftExtendParamAccesser* getAccesser(ftKind kind);
};

class Fighter {
    unsigned char padding[0x3C];

public:
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual ftKind getKind();
};

struct ModuleAccessor {
    unsigned char padding[8];
    Fighter* fighter;
};

extern "C" ftExtendParamAccesser* fn_27_149E5C(void*, ModuleAccessor accessor) {
    return ftExtendParamAccesser::getAccesser(accessor.fighter->getKind());
}
}

// TODO: clean up, give variables real names
namespace jj_14A8B8 {
class Obj {
public:
    virtual int s0();
    virtual int s1();
    virtual int s2();
    virtual int s3();
    virtual int s4(int a);
    virtual int s5();
    virtual int s6();
    virtual int s7();
    virtual int s8();
    virtual float s9();
    virtual int s10();
    virtual int s11();
    virtual int s12();
    virtual int s13();
    virtual int s14();
    virtual int s15();
    virtual int s16(int a);
    virtual int s17();
    virtual int s18();
    virtual int s19();
    virtual int s20();
    virtual int s21();
    virtual int s22();
    virtual int s23();
    virtual int s24();
    virtual int s25();
    virtual int s26();
    virtual int s27();
    virtual int s28();
    virtual int s29();
    virtual int s30();
    virtual int s31();
    virtual int s32();
    virtual int s33();
    virtual int s34();
    virtual int s35();
    virtual int s36();
    virtual int s37();
    virtual int s38();
    virtual int s39(int a);
    virtual int s40(int a);
};

class IpHuman {
public:
    unsigned int isWinPoseIndicate() const;
    int getWinPose() const;
};

struct OwnerRec {
    int f0;
    void* f4;
};

class ftManager {
public:
    OwnerRec* getOwner(int id);
};

extern ftManager* g_ftManager;
extern char lbl_27_data_2BDE0[];
extern char lbl_27_data_350[];
extern float lbl_27_rodata_18B8;

extern "C" {
int getVariableIntCore__15soValueAccesserFP16soModuleAccesserUlUl(void);
int getItemWaitMotion__6ftUtilFP16soModuleAccesser(void* self);
int getItemSquatWaitMotion__6ftUtilFP16soModuleAccesser(void* self);
unsigned int isOperationCpu__7ftOwnerFv(void);
int getTeam__7ftOwnerFv(void);
void* __dynamic_cast(void*, long, const void*, const void*, long);
}
int randi(long);

static inline void* subOf(void* self) { return *(void**)((char*)self + 0xd8); }
static inline Obj* objAt(void* p, int off) { return *(Obj**)((char*)p + off); }

extern "C" int fn_27_14A8B8(void* a, void* self, int id) {
    if (id <= 0x5207) {
        return getVariableIntCore__15soValueAccesserFP16soModuleAccesserUlUl();
    }
    switch (id) {
    case 0x5209:
        return (unsigned char)objAt(subOf(self), 0x5c)->s23();
    case 0x520B:
        return (unsigned char)objAt(subOf(self), 0x5c)->s27();
    case 0x520D:
        return (unsigned char)objAt(subOf(self), 0x5c)->s29();
    case 0x520F:
        return (signed char)objAt(subOf(self), 0x5c)->s24();
    case 0x520A:
        return (unsigned char)objAt(subOf(self), 0x5c)->s25();
    case 0x520C:
        return (unsigned char)objAt(subOf(self), 0x5c)->s28();
    case 0x520E:
        return (unsigned char)objAt(subOf(self), 0x5c)->s31();
    case 0x5210:
        return (signed char)objAt(subOf(self), 0x5c)->s26();
    case 0x5208:
        return objAt(subOf(self), 0x8c)->s5();
    case 0x5211:
        return (unsigned char)objAt(subOf(self), 0x5c)->s39(0);
    case 0x5212:
        return (unsigned char)objAt(subOf(self), 0x5c)->s40(0);
    case 0x5213:
        return (unsigned char)objAt(subOf(self), 0x5c)->s39(1);
    case 0x5214:
        return (unsigned char)objAt(subOf(self), 0x5c)->s40(1);
    case 0x5215:
        return (unsigned char)objAt(subOf(self), 0x5c)->s39(2);
    case 0x5216:
        return (unsigned char)objAt(subOf(self), 0x5c)->s40(2);
    case 0x5217:
        return (unsigned char)objAt(subOf(self), 0x5c)->s39(3);
    case 0x5218:
        return (unsigned char)objAt(subOf(self), 0x5c)->s40(3);
    case 0x5219:
        return (unsigned char)objAt(subOf(self), 0x5c)->s39(4);
    case 0x521A:
        return (unsigned char)objAt(subOf(self), 0x5c)->s40(4);
    case 0x521E:
        switch (objAt(subOf(self), 0xa8)->s16(0)) {
        case 0x4:  return 0x121;
        case 0x21: return 0x128;
        case 0x3a: return 0x133;
        case 0x2e: return 0x13a;
        case 0x1f: return 0x12c;
        default:   return 0x121;
        }
    case 0x521F:
        switch (objAt(subOf(self), 0xa8)->s16(0)) {
        case 0x4:  return 0x122;
        case 0x21: return 0x129;
        case 0x3a: return 0x134;
        case 0x2e: return 0x13b;
        case 0x1f: return 0x12d;
        default:   return 0x122;
        }
    case 0x5220:
        switch (objAt(subOf(self), 0xa8)->s16(0)) {
        case 0x4:  return 0x124;
        case 0x21: return 0x12a;
        case 0x3a: return 0x136;
        case 0x2e: return 0x13d;
        case 0x1f: return 0x12f;
        default:   return 0x124;
        }
    case 0x5221:
        switch (objAt(subOf(self), 0xa8)->s16(0)) {
        case 0x4:  return 0x127;
        case 0x21: return 0x12b;
        case 0x3a: return 0x139;
        case 0x2e: return 0x140;
        case 0x1f: return 0x132;
        default:   return 0x127;
        }
    case 0x521B: {
        Obj* c = objAt(subOf(self), 0x0c);
        if (lbl_27_rodata_18B8 == c->s9()) {
            int r = (signed char)objAt(subOf(self), 0x5c)->s24();
            if (r > 0) {
                return (unsigned char)objAt(subOf(self), 0x5c)->s23();
            }
            goto fallback_521b;
        }
        int r = (signed char)objAt(subOf(self), 0x5c)->s24();
        if (r < 0) {
            return (unsigned char)objAt(subOf(self), 0x5c)->s23();
        }
    fallback_521b:
        return 0xfe;
    }
    case 0x521C: {
        Obj* c = objAt(subOf(self), 0x0c);
        if (lbl_27_rodata_18B8 == c->s9()) {
            int r = (signed char)objAt(subOf(self), 0x5c)->s24();
            if (r < 0) {
                return (unsigned char)objAt(subOf(self), 0x5c)->s23();
            }
            goto fallback_521c;
        }
        int r = (signed char)objAt(subOf(self), 0x5c)->s24();
        if (r > 0) {
            return (unsigned char)objAt(subOf(self), 0x5c)->s23();
        }
    fallback_521c:
        return 0xfe;
    }
    case 0x521D: {
        Obj* c = objAt(subOf(self), 0x0c);
        if (lbl_27_rodata_18B8 == c->s9()) {
            int r = (signed char)objAt(subOf(self), 0x5c)->s30();
            if (r < 0) {
                return (unsigned char)objAt(subOf(self), 0x5c)->s29();
            }
            goto fallback_521d;
        }
        int r = (signed char)objAt(subOf(self), 0x5c)->s30();
        if (r > 0) {
            return (unsigned char)objAt(subOf(self), 0x5c)->s29();
        }
    fallback_521d:
        return 0xfe;
    }
    case 0x5222:
        return getItemWaitMotion__6ftUtilFP16soModuleAccesser(self);
    case 0x5223:
        return getItemSquatWaitMotion__6ftUtilFP16soModuleAccesser(self);
    case 0x5224: {
        int r = objAt(subOf(self), 0x64)->s4(0x10000000);
        IpHuman* h = (IpHuman*)__dynamic_cast(g_ftManager->getOwner(r)->f4, 4, lbl_27_data_2BDE0, lbl_27_data_350, 1);
        if (h->isWinPoseIndicate() == 1U) {
            return h->getWinPose() - 1;
        }
        return randi(3);
    }
    case 0x5225: {
        int r = objAt(subOf(self), 0x64)->s4(0x10000000);
        g_ftManager->getOwner(r);
        if (isOperationCpu__7ftOwnerFv() == 1U) {
            return 4;
        }
        g_ftManager->getOwner(r);
        return getTeam__7ftOwnerFv();
    }
    default:
        return 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_14B0BC {
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual int slot4(int); virtual void slot5(); virtual void slot6(); virtual void slot7();
 virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
 virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
 virtual float slot16(int, int, int);
};
struct FtManagerFlags6E {
 char pad[0x6E];
 unsigned char b0:1; unsigned char b1:1; unsigned char b2:1; unsigned char b3:1;
};
extern "C" {
 extern const float lbl_27_rodata_18C0;
 extern int g_soValueVariation;
 float getConstantFloatCore__15soValueAccesserFP16soModuleAccesserUlUl(void*, void*, int);
 float setDamageRatio__7ftOwnerFv(void*);
 float setAttackReactionMul__7ftOwnerFv(void*);
 float setDamageReactionMul__7ftOwnerFv(void*);
 float getAttackReactionMulSeal__7ftOwnerFv(void*);
 float getSpecialGlowAttack__7ftOwnerFv(void*);
 float getDamageReactionMulSeal__7ftOwnerFv(void*);
 float getAutoHandi__7ftOwnerFv(void*);
 float getSlipMul__7ftOwnerFv(void*);
 float getResultWinRotY__7ftOwnerFv(void*);
 float getParamCommonFloat__20ftCommonDataAccesserCFUlUl(void*, int);
}
extern "C" float fn_27_14B0BC(void* p1, void* p2, int p3, int p4) {
 if (p3 <= 0xBB7) return getConstantFloatCore__15soValueAccesserFP16soModuleAccesserUlUl(p1,p2,p3);
 if (p3 > 0xDAC) {
  if (p3 < 0xDB5) {
   Object_vptr_0* m = *(Object_vptr_0**)(*(unsigned char**)((unsigned char*)p2 + 0xd8) + 0x64);
   int id = m->slot4(0x10000000);
   switch (p3) {
   case 0xDAE: return setDamageRatio__7ftOwnerFv(g_ftManager->getOwner(id));
   case 0xDAF: return setAttackReactionMul__7ftOwnerFv(g_ftManager->getOwner(id));
   case 0xDB0: { ftManager* mgr = g_ftManager; float f = setDamageReactionMul__7ftOwnerFv(mgr->getOwner(id)); float r = getAttackReactionMulSeal__7ftOwnerFv(mgr->getOwner(id)); r *= f; return r; }
   case 0xDB1: { ftManager* mgr = g_ftManager; float f = getSpecialGlowAttack__7ftOwnerFv(mgr->getOwner(id)); float r = getDamageReactionMulSeal__7ftOwnerFv(mgr->getOwner(id)); r *= f; return r; }
   case 0xDB2: { ftManager* mgr = g_ftManager; if ((bool)((FtManagerFlags6E*)mgr)->b3 == true) return getAutoHandi__7ftOwnerFv(mgr->getOwner(id)); return lbl_27_rodata_18C0; }
   case 0xDB3: return getSlipMul__7ftOwnerFv(g_ftManager->getOwner(id));
   case 0xDB4: return getResultWinRotY__7ftOwnerFv(g_ftManager->getOwner(id));
   }
  }
 } else {
  if (p3 < 0xC2C) {
   unsigned char* mods = *(unsigned char**)((unsigned char*)p2 + 0xd8);
   Object_vptr_0* o = *(Object_vptr_0**)(mods + 0xc4);
   return o->slot16(g_soValueVariation, p3, p4);
  }
  return getParamCommonFloat__20ftCommonDataAccesserCFUlUl(&g_ftCommonDataAccesser, g_soValueVariation);
 }
 return lbl_27_rodata_18C0;
}
}

// TODO: clean up, give variables real names
namespace jj_14B2B4 {
extern "C" s32 getConstantIntCore__15soValueAccesserFP16soModuleAccesserUlUl(u32,u32,u32);
extern "C" s32 getShieldBreakFrameAdd__7ftOwnerFv(void*);
extern "C" void* getSwimDrownFrameAdd__7ftOwnerFv(void*);
extern "C" u8 getRank__9ftManagerCFi(void*, s32);
extern "C" u32 getParamCommonInt__20ftCommonDataAccesserCFUlUl(void*,s32);
extern "C" u32 getConstInt__6ftInfoFii(void*,s32,s32);
extern s32 g_soValueVariation;
extern u32 g_ftInfo[3];
class VFunc18 { public: virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3(); virtual s32 call(u32); virtual void a5(); virtual void a6(); virtual void a7(); virtual void a8(); virtual void a9(); virtual void a10(); virtual void a11(); virtual void a12(); virtual void a13(); virtual void a14(); virtual s32 call44(s32,s32,s32); };
struct VBase { char pad[0x3C]; };
class VFuncA8 : public VBase { public: virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3(); virtual void a4(); virtual void a5(); virtual void a6(); virtual void a7(); virtual void a8(); virtual void a9(); virtual void a10(); virtual void a11(); virtual void a12(); virtual void a13(); virtual void a14(); virtual void a15(); virtual void a16(); virtual void a17(); virtual void a18(); virtual void a19(); virtual void a20(); virtual void a21(); virtual void a22(); virtual void a23(); virtual void a24(); virtual void a25(); virtual void a26(); virtual void a27(); virtual void a28(); virtual void a29(); virtual void a30(); virtual void a31(); virtual void a32(); virtual void a33(); virtual void a34(); virtual void a35(); virtual void a36(); virtual void a37(); virtual void a38(); virtual void a39(); virtual s32 call(); };
extern "C" s32 fn_27_14B2B4(void* unused, void* self, s32 command, s32 extra) {
 if(command<=0x59D7) return getConstantIntCore__15soValueAccesserFP16soModuleAccesserUlUl((u32)unused,(u32)self,(u32)command);
 if(command>0x5A84) { if(command<0x5A8A) {
 VFunc18* iface=*(VFunc18**)(*(char**)((char*)self+0xD8)+0x64); s32 value=iface->call(0x10000000);
 switch(command) {
 case 0x5A86: return getShieldBreakFrameAdd__7ftOwnerFv(g_ftManager->getOwner(value));
 case 0x5A87: return (s32)getSwimDrownFrameAdd__7ftOwnerFv(g_ftManager->getOwner(value));
 case 0x5A88: { ftManager* manager=g_ftManager; u32 flag=(*(u8*)((char*)manager+0x6E)>>4)&1; u32 result=(0-flag)|flag; result>>=31; if(result==1) return (u8)getRank__9ftManagerCFi(manager,value); return 0; }
 case 0x5A89: { ftManager* manager=g_ftManager; s32 flag=0; if(*(u8*)((char*)manager+0x68)==0&&*(u8*)((char*)manager+0x6A)==1) flag=1; if(flag!=0) return manager->getOwner(value)->getStockCount(); return -1; }
 }
 } else { VFuncA8* iface=*(VFuncA8**)((char*)self+8); return getConstInt__6ftInfoFii(g_ftInfo,iface->call(),command-0x5A8C); }
 } else if(command<0x59F5) { VFunc18* iface=*(VFunc18**)(*(char**)((char*)self+0xD8)+0xC4); return iface->call44(g_soValueVariation,command,extra); }
 else return getParamCommonInt__20ftCommonDataAccesserCFUlUl(&g_ftCommonDataAccesser,g_soValueVariation);
 return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_14B540 {
extern "C" void* isEnableSpeedOperation__6ftUtilFP16soModuleAccesser(void* b);

extern "C" void* fn_27_14B540(void* a, void* b) {
    return isEnableSpeedOperation__6ftUtilFP16soModuleAccesser(b);
}
}

