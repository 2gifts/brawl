#include <mu/mu_menuroot.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" MenuRoot* fn_800AFC74(MenuRoot* obj, const char* name, int arg1, const char* cameraPath, int);

MenuRoot* MenuRoot::create(const char* name, int arg1, const char* cameraPath) {
    MenuRoot* obj = (MenuRoot*) operator new(0x1AC, (Heaps::HeapType) 0x2A);
    if (obj != 0) {
        obj = fn_800AFC74(obj, name, arg1, cameraPath, 0x2B);
    }
    return obj;
}
