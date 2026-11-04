#include <so/so_module_accesser.h>
#include <types.h>
#include <wn/ike/wn_ike_sword_status_uniq_process.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnActivateDesc::wnActivateDesc(u32 founderTaskId_, u32 resourceId_, bool flagBit6_, const Vec3f& pos_, float lr_,
                             int team_, int life_, int unk18_, int unk1C_, int unk38_, int unk10_,
                             u32 unkC_, u32 unk8_, int unk48_, bool flagBit3_, int unk3C_)
    : founderTaskId(founderTaskId_), resourceId(resourceId_),
      unk8(unk8_ == 0xFFFF ? resourceId_ : unk8_), unkC(unkC_ == 0xFFFF ? resourceId_ : unkC_),
      unk10(unk10_), unk14(unk10_), unk18(unk18_), unk1C(unk1C_), pos(pos_), lr(lr_),
      team(team_), life(life_), unk38(unk38_), unk3C(unk3C_), unk40(0), unk44(0x35F), unk48(unk48_) {
    flagBit7 = true;
    flagBit6 = flagBit6_;
    flagsMidPair = 0;
    flagBit3 = flagBit3_;
    flagsLow = 0;
    unk4DHigh = 0;
}

void wnIkeSwordStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 3: {
        Vec3f rot = moduleAccesser->getModelModule().getNodeRotate(3);
        rot.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
        moduleAccesser->getModelModule().setNodeRotate(3, &rot);
        break;
    }
    }
}

void wnIkeSwordStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    Vec3f rot = moduleAccesser->getModelModule().getNodeRotate(3);
    rot.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    moduleAccesser->getModelModule().setNodeRotate(3, &rot);
}

wnIkeSwordStatusUniqProcess::~wnIkeSwordStatusUniqProcess() { }

wnIkeSwordStatusUniqProcess::wnIkeSwordStatusUniqProcess() { }
