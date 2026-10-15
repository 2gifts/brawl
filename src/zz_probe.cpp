#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
void p0_update(soModuleAccesser* a) { a->getSoundModule().update((Vec3f*)0); }
void p1_deactivate(soModuleAccesser* a) { a->getSoundModule().deactivate(); }
void p2_activate(soModuleAccesser* a) { a->getSoundModule().activate((Vec3f*)0); }
void p3_setPos(soModuleAccesser* a) { a->getSoundModule().setPos((Vec3f*)0, 0); }
void p4_playSE(soModuleAccesser* a) { a->getSoundModule().playSE((SndID)0, 0, 0, 0); }
void p5_playSENo3d(soModuleAccesser* a) { a->getSoundModule().playSENo3d((SndID)0, 0); }
void p6_playSEPos(soModuleAccesser* a) { a->getSoundModule().playSEPos((SndID)0, (Vec3f*)0, 0, 0, 0); }
void p7_playStatusSE(soModuleAccesser* a) { a->getSoundModule().playStatusSE((SndID)0, 0, 0); }
void p8_stopStatusSE(soModuleAccesser* a) { a->getSoundModule().stopStatusSE(); }
void p9_setPlayHitSEFlag(soModuleAccesser* a) { a->getSoundModule().setPlayHitSEFlag(0); }
void p10_getPlayHitSEFlag(soModuleAccesser* a) { a->getSoundModule().getPlayHitSEFlag(); }
void p11_playHitSE(soModuleAccesser* a) { a->getSoundModule().playHitSE(0.0f, (void*)0); }
void p12_playDamageSE(soModuleAccesser* a) { a->getSoundModule().playDamageSE(0, 0); }
void p13_setSEVol(soModuleAccesser* a) { a->getSoundModule().setSEVol(0.0f, 0, 0); }
void p14_getSEVol(soModuleAccesser* a) { a->getSoundModule().getSEVol(0); }
void p15_setSEPitch(soModuleAccesser* a) { a->getSoundModule().setSEPitch(0.0f); }
void p16_setSEPitch(soModuleAccesser* a) { a->getSoundModule().setSEPitch(0, 0.0f); }
void p17_getSEPitch(soModuleAccesser* a) { a->getSoundModule().getSEPitch(); }
void p18_stopSE(soModuleAccesser* a) { a->getSoundModule().stopSE(0, 0); }
void p19_stopSEHandle(soModuleAccesser* a) { a->getSoundModule().stopSEHandle(0, 0); }
void p20_setSESpeed(soModuleAccesser* a) { a->getSoundModule().setSESpeed(0, 0.0f); }
void p21_playLandingSE(soModuleAccesser* a) { a->getSoundModule().playLandingSE(); }
void p22_setSoundIdData(soModuleAccesser* a) { a->getSoundModule().setSoundIdData((SoundIdData*)0); }
void p23_getSoundIdData(soModuleAccesser* a) { a->getSoundModule().getSoundIdData(); }
void p24_setFixGeneratorStatus(soModuleAccesser* a) { a->getSoundModule().setFixGeneratorStatus(); }
void p25_setCheckSoundGroup(soModuleAccesser* a) { a->getSoundModule().setCheckSoundGroup(0); }
void p26_getCheckSoundGroup(soModuleAccesser* a) { a->getSoundModule().getCheckSoundGroup(); }
void p27_isPlay(soModuleAccesser* a) { a->getSoundModule().isPlay(0); }
