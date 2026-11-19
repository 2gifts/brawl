#pragma force_active off
#include <so/so_array.h>
#include <so/effect/so_effect_continual.h>

typedef soEffectContinual Elm;
typedef soArrayNull<Elm> Arr;

template Elm& Arr::at(s32);
template const Elm& Arr::at(s32) const;

// TODO: clean up, give variables real names
namespace jj_85B38 {
extern "C" {
extern signed char lbl_27_bss_16A0;
extern u8 lbl_27_bss_16A4[0x2C];
extern u8 lbl_27_bss_FC8[0xC];
void __ct__17soEffectContinualFv(void*);
void __dt__17soEffectContinualFv(void*, int);
void __register_global_object(void*, void*, void*);

void* fn_27_85B38() {
    if (!lbl_27_bss_16A0) {
        __ct__17soEffectContinualFv(lbl_27_bss_16A4);
        __register_global_object(lbl_27_bss_16A4, (void*)__dt__17soEffectContinualFv, lbl_27_bss_FC8);
        lbl_27_bss_16A0 = 1;
    }
    return lbl_27_bss_16A4;
}
}
}

// TODO: clean up, give variables real names
namespace jj_85BA8 {
extern "C" {
extern signed char lbl_27_bss_16D0;
extern u8 lbl_27_bss_16D4[];
extern u8 lbl_27_bss_FD4[];
void __ct__17soEffectContinualFv(void*);
void __dt__17soEffectContinualFv(void*, int);
void __register_global_object(void*, void*, void*);

void* fn_27_85BA8() {
    if (!lbl_27_bss_16D0) {
        __ct__17soEffectContinualFv(lbl_27_bss_16D4);
        __register_global_object(lbl_27_bss_16D4, (void*)__dt__17soEffectContinualFv, lbl_27_bss_FD4);
        lbl_27_bss_16D0 = 1;
    }
    return lbl_27_bss_16D4;
}
}
}

