#include <types.h>
#include <nw4r/g3d.h>

#include <algorithm>

namespace nw4r {
namespace g3d {

NW4R_G3D_RTTI_DEF(ScnChoice);

ScnChoice* ScnChoice::Construct(MEMAllocator* pAllocator, u32* pSize,
                                int capacity) {
    ScnChoice* pChoice = NULL;
    u32 size = align4(sizeof(ScnChoice) + capacity * sizeof(ScnObj*));

    if (pSize != NULL) {
        *pSize = size;
    }

    if (pAllocator != NULL) {
        u8* pBuffer = reinterpret_cast<u8*>(Alloc(pAllocator, size));

        if (pBuffer != NULL) {
            ScnObj** ppObj =
                reinterpret_cast<ScnObj**>(pBuffer + sizeof(ScnChoice));

            pChoice = new (pBuffer) ScnChoice(pAllocator, ppObj, capacity);
        }
    }

    return pChoice;
}

void ScnChoice::ValidateChoice() {
    if (mpChoice != NULL) {
        ScnObj** ppObj = std::find(Begin(), End(), mpChoice);

        if (ppObj == End()) {
            mpChoice = NULL;
        }
    }
}

bool ScnChoice::SetChoice(ScnObj* pObj) {
    mpChoice = pObj;
    ValidateChoice();

    return mpChoice != NULL;
}

bool ScnChoice::SetChoice(int idx) {
    if (idx >= 0 && idx < static_cast<int>(Size())) {
        mpChoice = Begin()[idx];
        ValidateChoice();

        return mpChoice != NULL;
    }

    return false;
}

void ScnChoice::G3dProc(u32 task, u32 param, void* pInfo) {
    if (IsG3dProcDisabled(task)) {
        return;
    }

    switch (task) {
    case G3DPROC_CALC_WORLD: {
        CheckCallback_CALC_WORLD(CALLBACK_TIMING_A, param, pInfo);

        CalcWorldMtx(static_cast<const math::MTX34*>(pInfo), &param);

        CheckCallback_CALC_WORLD(CALLBACK_TIMING_B, param, pInfo);

        ValidateChoice();

        if (mpChoice != NULL) {
            mpChoice->G3dProc(G3DPROC_CALC_WORLD, param,
                              const_cast<math::MTX34*>(GetMtxPtr(MTX_WORLD)));
        }

        CheckCallback_CALC_WORLD(CALLBACK_TIMING_C, param, pInfo);
        break;
    }

    case G3DPROC_CALC_MAT: {
        CheckCallback_CALC_MAT(CALLBACK_TIMING_A, param, pInfo);

        ValidateChoice();

        if (mpChoice != NULL) {
            mpChoice->G3dProc(G3DPROC_CALC_MAT, param, pInfo);
        }

        CheckCallback_CALC_MAT(CALLBACK_TIMING_C, param, pInfo);
        break;
    }

    case G3DPROC_CALC_VIEW: {
        CheckCallback_CALC_VIEW(CALLBACK_TIMING_A, param, pInfo);

        CalcViewMtx(static_cast<const math::MTX34*>(pInfo));

        CheckCallback_CALC_VIEW(CALLBACK_TIMING_B, param, pInfo);

        ValidateChoice();

        if (mpChoice != NULL) {
            mpChoice->G3dProc(G3DPROC_CALC_VIEW, param, pInfo);
        }

        CheckCallback_CALC_VIEW(CALLBACK_TIMING_C, param, pInfo);
        break;
    }

    case G3DPROC_GATHER_SCNOBJ: {
        IScnObjGather* pCollection = static_cast<IScnObjGather*>(pInfo);

        ValidateChoice();

        IScnObjGather::CullingStatus status =
            pCollection->Add(this, false, false);

        if (status == IScnObjGather::CULLINGSTATUS_INTERSECT) {
            mpChoice->G3dProc(G3DPROC_GATHER_SCNOBJ, param, pCollection);
        } else if (status == IScnObjGather::CULLINGSTATUS_INSIDE) {
            const math::FRUSTUM* pTemp = gpCullingFrustum;
            gpCullingFrustum = NULL;
            mpChoice->G3dProc(G3DPROC_GATHER_SCNOBJ, param, pCollection);
            gpCullingFrustum = pTemp;
        }
        break;
    }

    case G3DPROC_DRAW_OPA:
    case G3DPROC_DRAW_XLU:
    case G3DPROC_CHILD_DETACHED:
    case G3DPROC_ATTACH_PARENT:
    case G3DPROC_DETACH_PARENT: {
        DefG3dProcScnGroup(task, param, pInfo);
        break;
    }

    default: {
        ValidateChoice();

        if (mpChoice != NULL) {
            mpChoice->G3dProc(task, param, pInfo);
        }
        break;
    }
    }
}

} // namespace g3d
} // namespace nw4r

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char gpCullingFrustum__Q24nw4r3g3d[0x8] __attribute__((aligned(4))) = { 0 };

// TODO: clean up, give variables real names
namespace jj_801B3C74 {
extern "C" {

extern unsigned char lbl_8040ABE8[0x10];

// TODO: clean up, give variables real names
unsigned char *fn_801B3C74(void) {
    return lbl_8040ABE8;
}

}
}

// TODO: clean up, give variables real names
namespace jj_801B3440 {
extern "C" void* fn_80204E5C(void*, unsigned int);
extern "C" void fn_801AC13C(void*, int, int, int);
extern "C" int lbl_80466410[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_801B3440(void* arg0, int* arg1, int arg2) {
    void* result = 0;
    unsigned int size = (unsigned int)(arg2 * 4 + 0xEF) & 0xFFFFFFFC;

    if (arg1 != 0) {
        *arg1 = (int)size;
    }
    if (arg0 != 0) {
        result = fn_80204E5C(arg0, size);
        if (result == 0) {
            return 0;
        }
        if (result != 0) {
            fn_801AC13C(result, (int)arg0, (int)((char*)result + 0xEC), arg2);
            *(int*)result = (int)lbl_80466410;
            *(int*)((char*)result + 0xE8) = 0;
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_801B35D0 {
class Child {
public:
 virtual void vf8();
 virtual void vf0c(int, void*, int, void*);
 virtual void vf10(int, void*, int, void*);
 virtual void vf14(int, void*, int, void*);
};
class Sub {
public:
 virtual void vf8();
 virtual void vf0c(int, int, void*);
};
class Class_80466418 {
public:
 virtual void fn_801B3BDC();
 virtual int fn_801B35D0(void*, int, int);
 virtual void fn_801B3C80();
 virtual void fn_801B3C74();
 virtual void fn_801B3C44();
 virtual void fn_801AB788();
 virtual void fn_801AAC9C();
 virtual void fn_801AAE34();
 virtual void fn_801AAFDC();
 virtual void fn_801AAFE8();
 virtual void fn_801AAA70(void*, int*);
 char pad04[0xCC - 4];
 unsigned int unkCC;
 char padD0[4];
 Child* unkD4;
 unsigned char unkD8;
 char padD9[1];
 unsigned short unkDA;
 unsigned int* unkDC;
 char padE0[4];
 int unkE4;
 Sub* unkE8;
};
inline Sub** findSub(Sub** p, Sub** end, Sub* const& target) {
 while (p != end && *p != target) ++p;
 return p;
}
extern "C" {
extern int lbl_805A0558;
void fn_801AAB18(void*, void*);
void fn_801ABA14(void*, int, int, void*);
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_801B35D0(Class_80466418* self, int arg1, int arg2, Class_80466418* arg3) {
 int sp8 = arg2;
 int var5;
 if ((unsigned int)arg1 < 9U && ((1 << (arg1 - 1)) & self->unkCC))
  var5 = 1;
 else
  var5 = 0;
 if (var5 == 0) {
  switch (arg1) {
  case 1:
   if (self->unkD4 && (self->unkDA & 1) && (self->unkD8 & 1))
    self->unkD4->vf0c(1, self, arg2, arg3);
   self->fn_801AAA70(arg3, &sp8);
   {
    int updated = sp8;
    if (self->unkD4 && (self->unkDA & 1) && (self->unkD8 & 2))
     self->unkD4->vf0c(2, self, updated, arg3);
   }
   if (self->unkE8) {
    Sub** p = (Sub**)self->unkDC;
    Sub** end = p + self->unkE4;
    p = findSub(p, end, self->unkE8);
    if (p == end)
     self->unkE8 = 0;
   }
   if (self->unkE8)
    self->unkE8->vf0c(1, sp8, (char*)self + 0x3c);
   {
    int updated = sp8;
    if (self->unkD4 && (self->unkDA & 1) && (self->unkD8 & 4))
     self->unkD4->vf0c(4, self, updated, arg3);
   }
   break;
  case 2:
   if (self->unkD4 && (self->unkDA & 2) && (self->unkD8 & 1))
    self->unkD4->vf10(1, self, arg2, arg3);
   if (self->unkE8) {
    Sub** p = (Sub**)self->unkDC;
    Sub** end = p + self->unkE4;
    p = findSub(p, end, self->unkE8);
    if (p == end)
     self->unkE8 = 0;
   }
   if (self->unkE8)
    self->unkE8->vf0c(2, sp8, arg3);
   {
    int updated = sp8;
    if (self->unkD4 && (self->unkDA & 2) && (self->unkD8 & 4))
     self->unkD4->vf10(4, self, updated, arg3);
   }
   break;
  case 4:
   if (self->unkD4 && (self->unkDA & 4) && (self->unkD8 & 1))
    self->unkD4->vf14(1, self, arg2, arg3);
   fn_801AAB18(self, arg3);
   {
    int updated = sp8;
    if (self->unkD4 && (self->unkDA & 4) && (self->unkD8 & 2))
     self->unkD4->vf14(2, self, updated, arg3);
   }
   if (self->unkE8) {
    Sub** p = (Sub**)self->unkDC;
    Sub** end = p + self->unkE4;
    p = findSub(p, end, self->unkE8);
    if (p == end)
     self->unkE8 = 0;
   }
   if (self->unkE8)
    self->unkE8->vf0c(4, sp8, arg3);
   {
    int updated = sp8;
    if (self->unkD4 && (self->unkDA & 4) && (self->unkD8 & 4))
     self->unkD4->vf14(4, self, updated, arg3);
   }
   break;
  case 6:
  case 7:
  case 0x10001:
  case 0x10002:
  case 0x10003:
   fn_801ABA14(self, arg1, arg2, arg3);
   break;
  case 5:
   if (self->unkE8) {
    Sub** p = (Sub**)self->unkDC;
    Sub** end = p + self->unkE4;
    p = findSub(p, end, self->unkE8);
    if (p == end)
     self->unkE8 = 0;
   }
   if (self->unkE8) {
    int r = arg3->fn_801B35D0(self, 0, 0);
    if (r == 0) {
     self->unkE8->vf0c(arg1, sp8, arg3);
    } else if (r == 1) {
     int saved = lbl_805A0558;
     lbl_805A0558 = 0;
     int updated = sp8;
     self->unkE8->vf0c(arg1, updated, arg3);
     lbl_805A0558 = saved;
    }
   }
   break;
  case 3:
  case 8:
  default:
   if (self->unkE8) {
    Sub** p = (Sub**)self->unkDC;
    Sub** end = p + self->unkE4;
    p = findSub(p, end, self->unkE8);
    if (p == end)
     self->unkE8 = 0;
   }
   if (self->unkE8)
    self->unkE8->vf0c(arg1, sp8, arg3);
   break;
  }
 }
}
}
}

