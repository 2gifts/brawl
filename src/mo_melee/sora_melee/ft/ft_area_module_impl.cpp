#include <types.h>
#include <ft/ft_area_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void ftAreaModuleImpl::activate(int ownerId) {
    soAreaModuleImpl::activate(ownerId);
    GXColor color;
    color.r = 0x40; color.g = 0x80; color.b = 0xC0; color.a = 0x80;
    setAreaDbgColor(1, color);
    setAreaDbgColor(2, color);
    color.r = 0xFF; color.g = 0x80; color.b = 0x00; color.a = 0x40;
    setAreaDbgColor(4, color);
    setAreaDbgColor(5, color);
    color.r = 0x40; color.g = 0xD0; color.b = 0x40; color.a = 0x10;
    setAreaDbgColor(6, color);
    setAreaDbgColor(7, color);
}

void ftAreaModuleImpl::update() {
    soAreaModuleImpl::update();
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000008);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000007);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000005);
}

// TODO: clean up, give variables real names
namespace jj_14E050 {
extern "C" {
void* __dt__34soAreaEnviromentElementCheckerImplFv(void*, short);
void* __dt__16soAreaModuleImplFv(void*, short);
void __dl__FPv(void*);
}

extern "C" void* __dt__16ftAreaModuleImplFv(void* p, short memberDeleting) {
    if (p) {
        __dt__34soAreaEnviromentElementCheckerImplFv((char*)p + 0x4c, -1);
        __dt__16soAreaModuleImplFv(p, 0);
        if (memberDeleting > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_14F050 {
extern "C" u32 soGetSubKind__6WeaponFv(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0xb8);
}
}

