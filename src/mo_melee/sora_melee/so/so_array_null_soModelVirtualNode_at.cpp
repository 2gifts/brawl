#pragma force_active off
#include <so/so_array.h>
#include <so/model/so_model_virtual_node.h>

typedef soModelVirtualNode Elm;
typedef soArrayNull<Elm> Arr;

template Elm& Arr::at(s32);
template const Elm& Arr::at(s32) const;

// TODO: clean up, give variables real names
namespace jj_86008 {
extern "C" {
    void __ct__18soModelVirtualNodeFv(void*);
    void __register_global_object(void* obj, void (*dtor)(void*, int), void* handle);
    void __dt__18soModelVirtualNodeFv(void*, int);
    extern signed char lbl_27_bss_1880;
    extern char lbl_27_bss_1884[0x14];
    extern char lbl_27_bss_1010[0xC];
}

extern "C" void* fn_27_86008() {
    if (!lbl_27_bss_1880) {
        __ct__18soModelVirtualNodeFv(lbl_27_bss_1884);
        __register_global_object(lbl_27_bss_1884, __dt__18soModelVirtualNodeFv, lbl_27_bss_1010);
        lbl_27_bss_1880 = 1;
    }
    return lbl_27_bss_1884;
}
}

// TODO: clean up, give variables real names
namespace jj_86078 {
extern signed char lbl_27_bss_1898;
extern char lbl_27_bss_189C[];
extern char lbl_27_bss_101C[];

extern "C" {
void __ct__18soModelVirtualNodeFv(void*);
void __dt__18soModelVirtualNodeFv(void*, int);
int __register_global_object(void*, void*, void*);
}

extern "C" void* fn_27_86078() {
    if (lbl_27_bss_1898 == 0) {
        __ct__18soModelVirtualNodeFv(lbl_27_bss_189C);
        __register_global_object(lbl_27_bss_189C, (void*)__dt__18soModelVirtualNodeFv, lbl_27_bss_101C);
        lbl_27_bss_1898 = 1;
    }
    return lbl_27_bss_189C;
}
}

