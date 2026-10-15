#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
void p0_activate(soModuleAccesser* a) { a->getKineticModule().activate(); }
void p1_deactivate(soModuleAccesser* a) { a->getKineticModule().deactivate(); }
void p2_addEnergy(soModuleAccesser* a) { a->getKineticModule().addEnergy((soKineticEnergy*)0, 0, (soKineticEnergy::AttributeFlag)0, 0); }
void p3_eraseEnergy(soModuleAccesser* a) { a->getKineticModule().eraseEnergy(0); }
void p4_eraseEnergyAll(soModuleAccesser* a) { a->getKineticModule().eraseEnergyAll(); }
void p5_getEnergy(soModuleAccesser* a) { a->getKineticModule().getEnergy(0); }
void p6_isContain(soModuleAccesser* a) { a->getKineticModule().isContain(0); }
void p7_updateEnergy(soModuleAccesser* a) { a->getKineticModule().updateEnergy((soModuleAccesser*)0, (soKineticEnergy::AttributeFlag)0); }
void p8_clearSpeedAll(soModuleAccesser* a) { a->getKineticModule().clearSpeedAll(); }
void p9_suspendEnergyAll(soModuleAccesser* a) { a->getKineticModule().suspendEnergyAll(); }
void p10_resumeEnergyAll(soModuleAccesser* a) { a->getKineticModule().resumeEnergyAll(); }
void p11_unableEnergyAll(soModuleAccesser* a) { a->getKineticModule().unableEnergyAll(); }
void p12_getSumSpeed(soModuleAccesser* a) { a->getKineticModule().getSumSpeed((soKineticEnergy::AttributeFlag)0); }
void p13_getSumSpeed3f(soModuleAccesser* a) { a->getKineticModule().getSumSpeed3f((soKineticEnergy::AttributeFlag)0); }
void p14_getSumRotation(soModuleAccesser* a) { a->getKineticModule().getSumRotation((soKineticEnergy::AttributeFlag)0); }
void p15_setConsiderGroundFriction(soModuleAccesser* a) { a->getKineticModule().setConsiderGroundFriction(0, (soKineticEnergy::AttributeFlag)0); }
void p16_mulSpeed(soModuleAccesser* a) { a->getKineticModule().mulSpeed((Vec3f*)0, (soKineticEnergy::AttributeFlag)0); }
void p17_mulAccel(soModuleAccesser* a) { a->getKineticModule().mulAccel((Vec3f*)0, (soKineticEnergy::AttributeFlag)0); }
void p18_reflectSpeed(soModuleAccesser* a) { a->getKineticModule().reflectSpeed((Vec3f*)0, (soKineticEnergy::AttributeFlag)0); }
void p19_reflectAccel(soModuleAccesser* a) { a->getKineticModule().reflectAccel((Vec3f*)0, (soKineticEnergy::AttributeFlag)0); }
void p20_changeKinetic(soModuleAccesser* a) { a->getKineticModule().changeKinetic(0, (soModuleAccesser*)0); }
void p21_getKineticType(soModuleAccesser* a) { a->getKineticModule().getKineticType(); }
void p22_addSpeed(soModuleAccesser* a) { a->getKineticModule().addSpeed((Vec3f*)0, (soModuleAccesser*)0); }
void p23_addSpeedOutside(soModuleAccesser* a) { a->getKineticModule().addSpeedOutside((soKineticEnergy::OutsideType)0, (Vec3f*)0); }
