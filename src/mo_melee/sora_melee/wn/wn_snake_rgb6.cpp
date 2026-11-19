#include <wn/snake/wn_snake_rgb6.h>
#include <mt/mt_matrix.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <wn/wn_extend_param_accesser.h>

void wnSnakeRgb6::notifyEventChangeMotion(int motionKind, int prevMotionKind, void*, soModuleAccesser*) {
    if (motionKind == 1) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        constraint(-1, 10, 0);
    }
    if (prevMotionKind == 1) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        constraint(-1, 0x39, 1);
    }
}

void wnSnakeRgb6::processFixCamera() {
    Weapon::processFixCamera();
}

Vec3f wnSnakeRgb6::getShootPos() {
    Matrix mtx(true);
    m_moduleAccesser->getModelModule().getNodeGlobalMtx(4, &mtx, false);
    Vec3f pos(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0),
              soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0),
              soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA2, 0));
    mtx.mulPos(&pos, &pos);
    return pos;
}

template class wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>;

// TODO: clean up, give variables real names
namespace jj_3B98D0 {
extern "C" {
extern void __dl__FPv(void*);

void* __dt__24wnSnakeRgb6ParamAccesserFv(void* ptr, int release) {
    if (ptr != 0 && release > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}
}

// TODO: clean up, give variables real names
namespace jj_3B9910 {
extern "C" void* fn_27_3B9910(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3BAB5C {
extern "C" void fn_27_3B965C(void*);

extern "C" void fn_27_3BAB5C(void* self) {
    fn_27_3B965C(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3BAB64 {
extern "C" void fn_27_3B965C(void*);

extern "C" void fn_27_3BAB64(void* self) {
    fn_27_3B965C(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3BAB6C {
extern "C" void fn_27_3B965C(void*);

extern "C" void fn_27_3BAB6C(void* self) {
    fn_27_3B965C(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3BAB84 {
extern "C" void fn_27_3BAAB0(void*);

extern "C" void fn_27_3BAB84(void* self) {
    fn_27_3BAAB0(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3BAB94 {
extern "C" void notifyEventChangeMotion__11wnSnakeRgb6FiiPvP16soModuleAccesser(void*);

extern "C" void fn_27_3BAB94(void* self) {
    notifyEventChangeMotion__11wnSnakeRgb6FiiPvP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x1f14);
}
}

