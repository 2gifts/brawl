#pragma force_active off
#include <so/so_array.h>
#include <so/effect/so_effect_time.h>

typedef soEffectTime Elm;
typedef soArrayNull<Elm> Arr;

template Elm& Arr::at(s32);
template const Elm& Arr::at(s32) const;

// TODO: clean up, give variables real names
namespace jj_86708 {
extern "C" void __ct__12soEffectTimeFv(void*);
extern "C" void __dt__12soEffectTimeFv(void*, int);
extern "C" void __register_global_object(void*, void*, void*);

typedef struct {
    int unk0;
    int unk4;
    int unk8;
} FnState;

extern char lbl_27_bss_1AD0;
extern FnState lbl_27_bss_1AD4;
extern FnState lbl_27_bss_1070;

extern "C" void* fn_27_86708() {
    if (!lbl_27_bss_1AD0) {
        __ct__12soEffectTimeFv(&lbl_27_bss_1AD4);
        __register_global_object(&lbl_27_bss_1AD4, &__dt__12soEffectTimeFv, &lbl_27_bss_1070);
        lbl_27_bss_1AD0 = 1;
    }
    return &lbl_27_bss_1AD4;
}
}

// TODO: clean up, give variables real names
namespace jj_86778 {
typedef struct FnState {
    int unk0;
    int unk4;
    int unk8;
} FnState;

extern "C" {
extern signed char lbl_27_bss_1AE0;
extern FnState lbl_27_bss_1AE4;
extern char lbl_27_bss_107C[];
void __ct__12soEffectTimeFv(FnState* p);
void* __dt__12soEffectTimeFv(void* ptr, int size);
void __register_global_object(void* obj, void* dtor, void* regmem);

FnState* fn_27_86778() {
    if (!lbl_27_bss_1AE0) {
        __ct__12soEffectTimeFv(&lbl_27_bss_1AE4);
        __register_global_object(&lbl_27_bss_1AE4, (void*)__dt__12soEffectTimeFv, lbl_27_bss_107C);
        lbl_27_bss_1AE0 = 1;
    }
    return &lbl_27_bss_1AE4;
}
}
}

