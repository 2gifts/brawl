#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/weapon.h>
#include <types.h>

// HYPOTHESIS: parameter fields named by their use in the crushed status (count threshold and interval).
struct wnPokeLizardonRockParam {
    u8 unk0[0xC];
    int unkC;
    int unk10;
};

// Local view of the rock article: only the parameter pointer at 0x18E8 is used by this status.
class wnPokeLizardonRock : public Weapon {
    u8 unkA8[0x18E8 - sizeof(Weapon)];
public:
    wnPokeLizardonRockParam* m_param;
};

class wnPokeLizardonRockStatusUniqProcessCrushed : public soStatusUniqProcess {
public:
    virtual ~wnPokeLizardonRockStatusUniqProcessCrushed() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPokeLizardonRockStatusUniqProcessCrushed g_wnPokeLizardonRockStatusUniqProcessCrushed;

void wnPokeLizardonRockStatusUniqProcessCrushed::initStatus(soModuleAccesser* a) {
    a->getWorkManageModule().onFlag(0x22000000);
    a->getWorkManageModule().setInt(0, 0x20000001);
}

void wnPokeLizardonRockStatusUniqProcessCrushed::execStatus(soModuleAccesser* a) {
    wnPokeLizardonRock& rock = dynamic_cast<wnPokeLizardonRock&>(a->getStageObject());
    int count = a->getWorkManageModule().getInt(0x20000001);
    if ((count + 1) % rock.m_param->unk10 == 0) {
        a->getWorkManageModule().onFlag(0x22000000);
    }
    a->getWorkManageModule().setInt(count + 1, 0x20000001);
    wnPokeLizardonRockParam* param = rock.m_param;
    if (a->getWorkManageModule().getInt(0x20000000) >= param->unkC) {
        a->getWorkManageModule().onFlag(0x22000001);
    }
}
