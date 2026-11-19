#include <st/operator/st_operator_appearance_fighter.h>
#include <types.h>

stOperatorAppearanceFighter::~stOperatorAppearanceFighter() { }

bool stOperatorAppearanceFighter::isEnd() {
    return m_state == 7;
}

#include <gm/gm_global.h>
// TODO: clean up, give variables real names
namespace jj_242764 {

extern "C" {
extern char lbl_27_data_4E250[];
extern char lbl_27_data_4E2AC[];
extern const unsigned char lbl_27_rodata_3E08[];
void* __ct__10stOperatorFPCc(void* self, const char* name);
}

struct Appearance {
    char pad0[0x3C];
    void* vtable;
    u8 f40;
    char pad41[7];
    u8 f48;
    char pad49[3];
    float f4C, f50, f54, f58, f5C, f60, f64, f68, f6C;
    char pad70[4];
    int f74, f78, f7C, f80, f84, f88, f8C, f90;
    u8 f94, f95;
    char pad96[0x188 - 0x96];
    float f188;
    char pad18C[4];
    int f190;
    char pad194[4];
    float f198, f19C;
    char pad1A0[4];
    u8 f1A4;
    char pad1A5[3];
    float f1A8;
    int f1AC, f1B0;
    u8 f1B4;
};

extern "C" Appearance* __ct__27stOperatorAppearanceFighterFv(Appearance* self) {
    const float* base = (const float*)lbl_27_rodata_3E08;
    __ct__10stOperatorFPCc(self, lbl_27_data_4E250);
    self->vtable = lbl_27_data_4E2AC;
    self->f48 = 0;
    self->f4C = base[0];
    self->f68 = base[0];
    self->f6C = base[0];
    self->f90 = 0;
    self->f94 = 0;
    self->f95 = 0;
    self->f188 = base[0];
    self->f198 = base[1];
    self->f19C = base[1];
    self->f1A4 = 0;
    self->f1A8 = base[0];
    self->f1AC = 0;
    self->f1B0 = 0;
    self->f1B4 = 0;
    self->f40 = 0;
    self->f190 = 0;
    self->f50 = base[0];
    self->f54 = base[2];
    self->f58 = base[3];
    self->f5C = base[0];
    self->f60 = base[2];
    self->f64 = base[3];
    self->f74 = 0;
    self->f78 = 0;
    self->f7C = 0;
    self->f80 = 0;
    self->f84 = 0;
    self->f88 = 0;
    self->f8C = 0;
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    self->f1A8 = base[1];
    switch ((int)*(u16*)((char*)melee + 0x1A)) {
    case 0x3B:
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[4];
        break;
    default:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[5];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[6];
        break;
    case 0x20:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[5];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[7];
        break;
    case 0x21:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[8];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[9];
        break;
    case 0xB:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[5];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[10];
        break;
    case 2:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[5];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[11];
        break;
    case 3:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[5];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[5];
        break;
    case 4:
        self->f50 = base[0];
        self->f54 = base[2];
        self->f58 = base[5];
        self->f5C = base[0];
        self->f60 = base[2];
        self->f64 = base[12];
        break;
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_242A14 {
struct Object_vptr_3c_base { char pad[0x3c]; };
class Object_vptr_3c : public Object_vptr_3c_base {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
 virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void processCamera();
 virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
 virtual void slot16(); virtual void renderOpa(); virtual void slot18(); virtual void slot19();
 virtual void slot20(); virtual void init(); virtual void slot22(); virtual void slot23();
 virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27();
 virtual void slot28(); virtual void slot29(); virtual void slot30(); virtual void slot31();
 virtual void slot32(); virtual void slot33(); virtual void slot34(); virtual void slot35();
 virtual unsigned int slot36();
};
struct Self {
 char pad0[0x40]; unsigned char state; char pad1[0x33];
 Object_vptr_3c* a; Object_vptr_3c* b; Object_vptr_3c* c; Object_vptr_3c* d;
};
extern "C" void fn_27_242A14(Self* self) {
 unsigned int done = 0;
 unsigned char state = self->state;
 do {
  switch (state) {
  case 1:
   if ((self->a == 0 ? 0 : self->a->slot36()) == 1U) done = 1;
   state = 2; break;
  case 2:
   if ((self->b == 0 ? 0 : self->b->slot36()) == 1U) done = 1;
   state = 3; break;
  case 3:
   if ((self->c == 0 ? 0 : self->c->slot36()) == 1U) done = 1;
   state = 4; break;
  case 4:
   if ((self->d == 0 ? 0 : self->d->slot36()) == 1U) done = 1;
   state = 5; break;
  case 5: state = 6; done = 1; break;
  case 6: state = 7; done = 1; break;
  case 7: done = 1; break;
  }
 } while (done != 1U);
 self->state = state;
}
}

// TODO: clean up, give variables real names
namespace jj_242B7C {
struct gfTask_base { char pad[0x3c]; };
class gfTask : public gfTask_base {
public:
virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void processCamera(); virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15(); virtual void slot16(); virtual void renderOpa(); virtual void slot18(); virtual void slot19(); virtual void slot20(); virtual void init(); virtual void slot22(); virtual void slot23(); virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27(); virtual void slot28(); virtual void slot29(); virtual void slot30(); virtual void slot31(); virtual void slot32(); virtual void slot33(); virtual void slot34(); virtual void slot35(); virtual int slot36();
};
struct Container { char pad[0x74]; gfTask* objects[1]; };
extern "C" int isEntryFighter__27stOperatorAppearanceFighterFi(Container* self, unsigned int index) {
    gfTask* obj = self->objects[index];
    if (obj == 0) {
        return 0;
    }
    return obj->slot36();
}
}

