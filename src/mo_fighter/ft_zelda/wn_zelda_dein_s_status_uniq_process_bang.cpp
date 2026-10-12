#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/weapon.h>
#include <types.h>

// HYPOTHESIS: parameter floats named by their use in the bang size and power curves.
struct wnZeldaDeinSParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
};

// Local view of the Dein S article: only the parameter pointer at 0x1F14 is used here.
class wnZeldaDeinS : public Weapon {
    u8 unkA8[0x1F14 - sizeof(Weapon)];
public:
    wnZeldaDeinSParam* m_param;
};

class wnZeldaDeinSStatusUniqProcessBang : public soStatusUniqProcess {
public:
    virtual ~wnZeldaDeinSStatusUniqProcessBang() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnZeldaDeinSStatusUniqProcessBang g_wnZeldaDeinSStatusUniqProcessBang;

void wnZeldaDeinSStatusUniqProcessBang::execFixPosCounter(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soCollisionAttackModule& attack = a->getCollisionAttackModule();
    float t = work.getFloat(0x21000001);
    float size = work.getFloat(0x21000002);
    wnZeldaDeinS& article = dynamic_cast<wnZeldaDeinS&>(a->getStageObject());
    wnZeldaDeinSParam* param = article.m_param;
    float scale = param->unk4 + (param->unk8 - param->unk4) / param->unk0 * t;
    a->getPostureModule().setScale(scale);
    if (attack.isAttack(0, false)) {
        int power = static_cast<int>(param->unkC + param->unk10 * t);
        attack.setPower(0, power, false);
        // HYPOTHESIS: the sentinel compared against the stored size is the rodata constant lbl_104_rodata_D0.
        if (size == 0.0f) {
            size = attack.getSize(0);
        }
        attack.setSize(0, size * scale);
    }
    work.setFloat(size, 0x21000002);
}
