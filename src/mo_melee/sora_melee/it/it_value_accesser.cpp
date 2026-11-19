#include <it/it_extend_param_accesser.h>
#include <it/it_resource_module_impl.h>
#include <it/it_value_accesser.h>
#include <it/item.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

itValueAccesser::itValueAccesser() : soValueAccesser(4) { }

itValueAccesser::~itValueAccesser() { }

extern itExtendParamAccesser lbl_27_bss_71DC;
extern const float lbl_27_rodata_5FD8[6];
extern const float lbl_27_rodata_5FF0[2];

float itValueAccesser::getConstantFloatCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if ((int)id > 3999) {
        return static_cast<soParamAccesser*>(&lbl_27_bss_71DC)->getParamFloat(moduleAccesser, id, index);
    }
    if ((int)id <= 2999) {
        return soValueAccesser::getConstantFloatCore(moduleAccesser, id, index);
    }
    if ((int)id > 3000 && (int)id < 3017) {
        return static_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule()).unk30->unk0[(int)id - 3001];
    }
    if ((int)id > 3400 && (int)id < 3443) {
        return static_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule()).unk34->unk0[(int)id - 3401];
    }
    if ((int)id > 3500 && (int)id < 3502) {
        BaseItem* item = static_cast<BaseItem*>(moduleAccesser->m_stageObject);
        switch (id) {
        case 3501:
            int parameter = -1;
            if (item->getTrait() & itParam::TRAIT_MASK_RECOVER) {
                switch (item->m_kind) {
                case 0x20:
                    if ((int)item->m_variation == 1) {
                        return lbl_27_rodata_5FF0[0];
                    }
                    parameter = 0x15E0;
                    break;
                case 0x24:
                    parameter = 0x16A8;
                    break;
                case 0x40:
                    parameter = item->m_variation + 0x1C20;
                    break;
                case 0x58:
                    parameter = 0x20D0;
                    break;
                case 0x4A:
                    parameter = 0x1E14;
                    break;
                case 0x4E:
                    parameter = item->m_variation + 0x1EDC;
                    break;
                }
            }
            if (parameter >= 0) {
                return soValueAccesser::getConstantFloat(moduleAccesser, parameter, 0);
            }
            return lbl_27_rodata_5FD8[0];
        }
    }
    return lbl_27_rodata_5FD8[0];
}

void* itValueAccesser::getConstantIndefiniteCore(soModuleAccesser* moduleAccesser, u32 id, u32 index) {
    if ((int)id > 43999) {
        return static_cast<soParamAccesser*>(&lbl_27_bss_71DC)->getParamIndefinite(moduleAccesser, id, index);
    }
    if ((int)id <= 42999) {
        return soValueAccesser::getConstantIndefiniteCore(moduleAccesser, id, index);
    }
    itResourceModuleImpl& resource = dynamic_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule());
    __typeof__(resource.unk34) specialized = resource.unk34;
    __typeof__(resource.unk38) animation = resource.unk38;
    switch (id) {
    case Common_Param_Indefinite_Effect_Anim_Cmd:
        return resource.unk30->unk60 ? *resource.unk30->unk60 : 0;
    case Anim_Param_Indefinite_Motion_Data:
        return animation->unk0;
    case Anim_Param_Indefinite_Status_Data:
        return animation->unk30;
    case Anim_Param_Indefinite_Collision_Hit_Data:
        return animation->unk34;
    case Anim_Param_Indefinite_Collision_Shield_Data:
        return animation->unk38;
    case Anim_Param_Indefinite_Ground_Shape_Data:
        return animation->unk8;
    case Anim_Param_Indefinite_Visibility_Data:
        return animation->unk10;
    case Anim_Param_Indefinite_Physics_IK_Data:
        return animation->unk14;
    case Anim_Param_Indefinite_Effect_Node_Data:
        return animation->unk18;
    case Anim_Param_Indefinite_Motion_Anim_Cmd_Game:
        return animation->unk1C;
    case Anim_Param_Indefinite_Motion_Anim_Cmd_Effect:
        return animation->unk20;
    case Anim_Param_Indefinite_Motion_Anim_Cmd_Sound:
        return animation->unk24;
    case Anim_Param_Indefinite_Status_Anim_Cmd:
        return animation->unk28;
    case Anim_Param_Indefinite_Event_Anim_Cmd:
        return animation->unk2C;
    case Sub_Param_Indefinite:
        return specialized->unkEC;
    }
    return 0;
}

#include <gf/gf_task.h>
#include <so/so_external_value_accesser.h>
// TODO: clean up, give variables real names
namespace jj_2BF65C {
struct lbl_27_bss_71DC_t {
    u32 unk0;
    u32 unk4;
    u32 unk8;
};

extern lbl_27_bss_71DC_t lbl_27_bss_71DC;

extern "C" lbl_27_bss_71DC_t* fn_27_2BF65C()
{
    return &lbl_27_bss_71DC;
}
}

// TODO: clean up, give variables real names
namespace jj_2BFDD0 {
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" int getParamInt__8BaseItemFUl(void*, int);
extern "C" int fn_27_14D6F4(void*, int);
extern "C" int getVariableIntCore__15soValueAccesserFP16soModuleAccesserUlUl(void*, void*, int);
extern "C" const char __RTTI__6gfTask[];
extern "C" const char __RTTI__8BaseItem[];
extern "C" const char __RTTI__11StageObject[];
extern "C" const char __RTTI__7Fighter[];
struct SoValue {
 int pad0, v4, v8, padC[5], v20, v24, v28, v2c, pad30[3], v3c, v40;
 unsigned char v44, v45;
};
struct Obj38Base { char pad[8]; };
class Obj38V : public Obj38Base {
public:
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
 virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11();
 virtual void f12(); virtual void f13(); virtual void f14(); virtual void f15();
 virtual void f16(); virtual void f17(); virtual void f18(); virtual void f19();
 virtual void f20(); virtual void f21(); virtual void f22(); virtual void f23();
 virtual void f24(); virtual void f25(); virtual void f26(); virtual void f27();
 virtual void f28(); virtual void f29(); virtual void f30(); virtual void f31();
 virtual SoValue* f34();
};
class Obj38 : public Obj38V {};
class M54 {
public:
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
 virtual void f8(); virtual void* f9(int); virtual void f9a(); virtual void* f10(int);
};
class VClass2 {
public:
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual int f4(int, void*);
};
struct So {
 char pad38[0x38]; Obj38* m38; char pad54[0x54-0x3c]; M54* m54;
 char pad64[0x64-0x58]; VClass2* m64;
};
struct Src {
 char pad3c[0x3c]; void* v2; char pad40[0x8c0-0x40]; int m8C0, m8C4, m8C8;
};
struct Acc { char pad8[8]; Src* src; char padC[0xd8-0xc]; So* so; };
extern "C" int fn_27_2BFDD0(void* arg1, Acc* acc, int code) {
 if (code <= 0x5207) return getVariableIntCore__15soValueAccesserFP16soModuleAccesserUlUl(arg1, acc, code);
 Src* src = (Src*)__dynamic_cast(acc->src, 0x3c, __RTTI__8BaseItem, __RTTI__11StageObject, 1);
 switch (code - 0x5208) {
 case 0x07: {
  int id=src->m8C8;
  if ((unsigned)id + 0x10000U != 0xFFFFU) {
   gfTask* task=gfTask::getTask(id);
   void* cast=task ? __dynamic_cast(task,0x3c,__RTTI__8BaseItem,__RTTI__6gfTask,0) : 0;
   if (cast) return ((Src*)cast)->m8C0;
   return -1;
  }
  return -1;
 }
 case 0x08: {
  int id=src->m8C8;
  if ((unsigned)id + 0x10000U != 0xFFFFU) {
   gfTask* task=gfTask::getTask(id);
   void* cast=task ? __dynamic_cast(task,0x3c,__RTTI__8BaseItem,__RTTI__6gfTask,0) : 0;
   if (cast) return getParamInt__8BaseItemFUl(cast,0x1000000D);
   return 0;
  }
  return 0;
 }
 case 0x09: {
  int id=acc->so->m64->f4(0x10000014,acc->so);
  if (id != -1) {
   gfTask* task=gfTask::getTask(id);
   void* cast=task ? __dynamic_cast(task,0x3c,__RTTI__8BaseItem,__RTTI__6gfTask,0) : 0;
   if (cast) return getParamInt__8BaseItemFUl(cast,0x1000000D);
   return 0;
  }
  return 0;
 }
 case 0x0A: {
  int id=acc->so->m64->f4(0x10000013,acc->so);
  if ((unsigned)id + 0x10000U != 0xFFFFU) {
   gfTask* task=gfTask::getTask(id);
   void* cast=task ? __dynamic_cast(task,0x3c,__RTTI__11StageObject,__RTTI__6gfTask,0) : 0;
   if (!cast) return 2;
   return soExternalValueAccesser::getSituationKind((StageObject*)cast);
  }
  return 2;
 }
 case 0x0B: return src->m8C4;
 case 0x0D: return src->m8C0;
 case 0x00: return acc->so->m38->f34()->v4;
 case 0x01: return acc->so->m38->f34()->v8;
 case 0x02: return 0;
 case 0x03: return acc->so->m38->f34()->v20;
 case 0x04: return acc->so->m38->f34()->v24;
 case 0x05: return acc->so->m38->f34()->v3c;
 case 0x14: return acc->so->m38->f34()->v40;
 case 0x0E: return acc->so->m38->f34()->v44;
 case 0x06: return acc->so->m38->f34()->v2c;
 case 0x0F: return (acc->so->m38->f34()->v45 >> 2) & 1;
 case 0x0C: {
  if (acc->so->m54->f9(3)) {
   void* value=acc->so->m54->f10(3);
   if (value) return ((SoValue*)value)->v28;
   return -1;
  }
  return -1;
 }
 case 0x10: return src->m8C8;
 case 0x11: case 0x12: case 0x13: {
  if (acc->so->m54->f9(3)) {
  void* value=acc->so->m54->f10(3);
  if (!value) return 0;
  void* cast=__dynamic_cast(value,0x3c,__RTTI__7Fighter,__RTTI__11StageObject,0);
  if (!cast) return 0;
  if (code==0x5219) return fn_27_14D6F4(cast,0);
  if (code==0x521A) return fn_27_14D6F4(cast,1);
  if (code==0x521B) return fn_27_14D6F4(cast,2);
  }
  return 0;
 }
 }
 return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_2C0440 {
extern "C" {
int getConstantIntCore__15soValueAccesserFP16soModuleAccesserUlUl(unsigned int, unsigned int, unsigned int);
int getParamInt__8BaseItemFUl(void*, int);
int getTrait__8BaseItemFv(void*);
int getResult__8BaseItemFv(void*);
}
extern unsigned int lbl_27_bss_71DC[];

struct Base8 { int a; int b; };
class Vtbl8_Class : public Base8 {
public:
 virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int v5(void*, int);
};
class Vtbl0_Class {
public:
 virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(int);
};
struct Obj_Q { unsigned char pad[0x30]; void* field_30; void* field_34; void* field_38; };
struct Obj_P { Obj_Q* field_0; unsigned char pad[0x60]; Vtbl0_Class* field_64; };
struct Obj_X { unsigned char pad[0x8c0]; int field_8c0; };
struct Self { unsigned char pad[8]; Obj_X* field_8; unsigned char pad2[0xcc]; Obj_P* field_d8; };

extern "C" int fn_27_2C0440(int a, Self* self, int id) {
 if (id > 0x5dbf) { Vtbl8_Class* obj=(Vtbl8_Class*)lbl_27_bss_71DC; return obj->v5(self,id); }
 if (id <= 0x59d7) return getConstantIntCore__15soValueAccesserFP16soModuleAccesserUlUl((unsigned int)a,(unsigned int)self,(unsigned int)id);
 if (id > 0x59d8 && id < 0x59e1) { Obj_P* P=self->field_d8; Obj_Q* Q=P->field_0; unsigned char* arr=(unsigned char*)Q->field_30; return *(int*)(arr+(id-0x59d9)*4+0x40); }
 if (id > 0x5b67 && id < 0x5b76) { Obj_P* P=self->field_d8; Obj_Q* Q=P->field_0; unsigned char* arr=(unsigned char*)Q->field_34; return *(int*)(arr+(id-0x5b68)*4+0xa8); }
 if (id > 0x5bcb && id < 0x5bd3) {
  unsigned int idx=(unsigned int)(id-0x5bcc); Obj_X* X=self->field_8;
  switch(idx) {
  case 2: return getTrait__8BaseItemFv(X);
  case 0: { int state=X->field_8c0; switch(state) { case 4:return getParamInt__8BaseItemFUl(X,0x5e88); case 0x2f:return getParamInt__8BaseItemFUl(X,0x66ee); default:return self->field_d8->field_64->v4(0x10000007); } }
  case 1: return self->field_d8->field_64->v4(0x10000007);
  case 3: { int state=X->field_8c0; Vtbl0_Class* v=self->field_d8->field_64; if(state==0x1d||state==0x1b)return v->v4(0x10000000); return 0; }
  case 4: return getResult__8BaseItemFv(X);
  case 5: { int state=X->field_8c0; Vtbl0_Class* v=self->field_d8->field_64; if(state==0xf)return v->v4(0x10000000); if(state==5)return v->v4(0x10000000); return 0; }
  case 6: { int state=X->field_8c0; switch(state) { case 0x40:return self->field_d8->field_64->v4(0x10000000); case 0x58:return 0x67; case 0x4e:return 0x1c8e; default:return 0x65; } }
 }
 }
 Obj_Q* q=self->field_d8->field_0; void* A=q->field_34; void* B=q->field_38;
 switch(id) {
 case 0x5b77: return *(int*)((char*)B+4);
 case 0x5b78: return *(int*)((char*)B+0xc);
 case 0x5b7b: return *(int*)((char*)B+0x3c);
 case 0x5b79: return 0;
 case 0x5b7a: { void* p=*(void**)((char*)A+0xec); if(!p)return 0; return *(int*)((char*)p+4); }
 default: return 0;
 }
}
}

// TODO: clean up, give variables real names
namespace jj_2C0828 {
extern "C" s32 fn_27_2C0828(void) {
    return 0x0;
}
}

