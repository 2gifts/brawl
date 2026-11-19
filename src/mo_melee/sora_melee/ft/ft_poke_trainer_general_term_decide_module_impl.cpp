#include <types.h>
#include <ft/ft_poke_trainer_general_term_decide_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"



bool ftPokeTrainerGeneralTermDecideModuleImpl::checkEstablishSub(
    int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
    u32* returnWord, soGeneralTermCache* cache)
{
    if (moduleAccesser->getWorkManageModule().getInt(0x10000045) <= 0 &&
        moduleAccesser->getWorkManageModule().isFlag(0x12000040) &&
        moduleAccesser->getWorkManageModule().isFlag(0x12000041) != true) {
        float distance;
        float remaining;
        float direction;
        double distanceWide = fabs(moduleAccesser->getWorkManageModule().getFloat(0x1100001B));
        distance = distanceWide;
        if (distance > moduleAccesser->getWorkManageModule().getFloat(0x11000020)) {
            direction = moduleAccesser->getWorkManageModule().getFloat(0x11000016) < 0.0f
                ? -1.0f : 1.0f;
            float facing = -1.0f == moduleAccesser->getWorkManageModule().getFloat(0x1100001F)
                ? -1.0f : 1.0f;
            if (direction == facing) {
                double remainingWide = fabs(moduleAccesser->getWorkManageModule().getFloat(0x1100001E));
                remaining = remainingWide;
                if (remaining < soValueAccesser::getConstantFloat(moduleAccesser, 0xFA5, 0)) {
                    return false;
                }
                float cliffDistance = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA5, 0);
                if (moduleAccesser->getGroundModule().isNearCliff(direction, cliffDistance, 0) == true) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

#include <nw4r/math/math_arithmetic.h>
// TODO: clean up, give variables real names
namespace jj_387034 {

class Class_1250 {
public:
 virtual void __dt__18soMotionModuleImplFv(); virtual void fn_27_14068(); virtual void fn_27_140C0(); virtual void fn_27_1412C();
 virtual void fn_27_14134(); virtual void fn_27_14370(); virtual void fn_27_14A28(); virtual void fn_27_14A30();
 virtual void fn_27_14A38(); virtual void fn_27_14A48(); virtual void fn_27_14A68(); virtual void fn_27_14AD8();
 virtual void fn_27_14A60(); virtual void fn_27_14D08(); virtual void fn_27_14EDC(); virtual void fn_27_14D00();
 virtual void fn_27_14B80(); virtual void fn_27_14B88(); virtual float fn_27_16A70();
 virtual void fn_27_16AB8(); virtual void fn_27_1CBB4(); virtual void fn_27_14A20(); virtual void fn_27_1CBAC();
 virtual void fn_27_1CBA4(); virtual void fn_27_14B98(); virtual void fn_27_14B90(); virtual void fn_27_1CB88();
 virtual void fn_27_14CF8(); virtual void fn_27_16778(); virtual void fn_27_1688C(); virtual void fn_27_1C2E4();
 virtual void fn_27_1BC08(); virtual void fn_27_1BCF0(); virtual void fn_27_16AE0(); virtual void fn_27_15604();
 virtual void fn_27_16CA8(); virtual void fn_27_16CB0(); virtual void fn_27_1CB80(); virtual unsigned int fn_27_16CB8();
};

class Class_20E8 {
public:
 virtual void __dt__26soMotionModuleImplMultiResFv(); virtual void fn_27_14068(); virtual void fn_27_140C0(); virtual void fn_27_1412C();
 virtual void fn_27_14134(); virtual void fn_27_14370(); virtual void fn_27_14A28(); virtual void fn_27_14A30();
 virtual void fn_27_14A38(); virtual void fn_27_14A48(); virtual void fn_27_14A68(); virtual void fn_27_14AD8();
 virtual void fn_27_14A60(); virtual void fn_27_14D08(); virtual void fn_27_14EDC(); virtual void fn_27_14D00();
 virtual void fn_27_14B80(); virtual void fn_27_14B88(); virtual int fn_27_16A70(int);
 virtual void fn_27_16AB8(); virtual void fn_27_1CBB4(); virtual void fn_27_14A20(); virtual void fn_27_1CBAC();
 virtual void fn_27_1CBA4(); virtual void fn_27_14B98(); virtual void fn_27_14B90(); virtual void fn_27_1CB88();
 virtual void fn_27_14CF8(); virtual void fn_27_16778(); virtual void fn_27_1688C(); virtual void fn_27_1C2E4();
 virtual void fn_27_1BC08(); virtual void fn_27_1BCF0(); virtual void fn_27_16AE0(); virtual void fn_27_15604();
 virtual void fn_27_16CA8(); virtual void fn_27_16CB0(); virtual void fn_27_1CB80(); virtual unsigned int fn_27_16CB8();
};

extern "C" int fn_27_1524F0(void*, int, int, soModuleAccesser*);
extern "C" int checkEstablishSub__40ftPokeTrainerGeneralTermDecideModuleImplFiP13soGeneralTermP16soModuleAccesserPUlP18soGeneralTermCache(void*, bool*);

extern "C" int fn_27_387034(void* self, int id, int arg, soModuleAccesser* accesser) {
 if (id <= 0x4e1f) return fn_27_1524F0(self, id, arg, accesser);
 switch (id) {
 case 0x4e20: {
  if (!accesser->m_enumerationStart->m_workManageModule->isFlag(0x1200003e)) return 0;
  float previous = accesser->m_enumerationStart->m_workManageModule->getFloat(0x11000018);
  float current = accesser->m_enumerationStart->m_workManageModule->getFloat(0x11000016);
  float limit = soValueAccesser::getConstantFloat(accesser, 0xfa3, 0);
  float low = -limit;
  float clamped = nw4r::math::FSelect(current - low, current, low);
  clamped = nw4r::math::FSelect(clamped - limit, limit, clamped);
  float diff = (float)(double)nw4r::math::FAbs(clamped - previous);
  return diff > soValueAccesser::getConstantFloat(accesser, 0xfa0, 0);
 }
 case 0x4e21: { bool value = false; return checkEstablishSub__40ftPokeTrainerGeneralTermDecideModuleImplFiP13soGeneralTermP16soModuleAccesserPUlP18soGeneralTermCache(self, &value); }
 case 0x4e22: return accesser->m_enumerationStart->m_situationModule->getKind() == 5;
 case 0x4e23: {
  bool result = false;
  bool c = false;
  bool b = false;
  bool a = false;
  Class_1250* controller = (Class_1250*)accesser->m_enumerationStart->m_controllerModule;
  int mask = soController::getButtonMask(soController::Pad_Button_Special);
  unsigned int trigger = controller->fn_27_16CB8();
  if (trigger & mask) {
   Class_1250* controller2 = (Class_1250*)accesser->m_enumerationStart->m_controllerModule;
   float threshold = soValueAccesser::getConstantFloat(accesser, 0xc72, 0);
   if (-controller2->fn_27_16A70() >= threshold) a = true;
  }
  if (a && accesser->m_enumerationStart->m_workManageModule->getInt(0x10000046) <= 0) b = true;
  if (b && !accesser->m_enumerationStart->m_workManageModule->isFlag(0x22000010)) c = true;
  if (c && !((Class_20E8*)accesser->m_enumerationStart->m_generateArticleManageModule)->fn_27_16A70(0)) result = true;
  return result;
 }
 case 0x4e24: return 1;
 default: return 1;
 }
}
}

// TODO: clean up, give variables real names
namespace jj_387518 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_387518(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

