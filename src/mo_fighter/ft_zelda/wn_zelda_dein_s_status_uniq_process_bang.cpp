#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/weapon.h>
#include <types.h>

// The explosion phase of the article: size and damage grow over growFrames. Offsets and use agree with the published
// Din's Fire parameter table; the names are descriptive.
struct wnZeldaDeinSParam {
    float growFrames;
    float initialSizeMul;
    float terminalSizeMul;
    float initialDamage;
    float damageGrowthRate;
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
    float scale = param->initialSizeMul + (param->terminalSizeMul - param->initialSizeMul) / param->growFrames * t;
    a->getPostureModule().setScale(scale);
    if (attack.isAttack(0, false)) {
        int power = static_cast<int>(param->initialDamage + param->damageGrowthRate * t);
        attack.setPower(0, power, false);
        // HYPOTHESIS: the sentinel compared against the stored size is the rodata constant lbl_104_rodata_D0.
        if (size == 0.0f) {
            size = attack.getSize(0);
        }
        attack.setSize(0, size * scale);
    }
    work.setFloat(size, 0x21000002);
}
