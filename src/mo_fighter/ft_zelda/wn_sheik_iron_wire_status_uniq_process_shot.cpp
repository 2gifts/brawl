#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_lasso_status_view.h>
#include <wn/weapon.h>
#include <types.h>

// Local views: the iron wire alt status (+0x9C) shares the lasso layout up to 0xB4; this status also reads
// int results at 0xAC. HYPOTHESIS: slot names are offsets from the vptr, as in wn_lasso_status_view.h.
class wnSheikIronWireAltStatusView : public wnLassoViewPad8 {
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual void slot14();
    virtual void slot18();
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void slot30();
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void slot44();
    virtual void slot48();
    virtual void slot4C();
    virtual void slot50();
    virtual void slot54();
    virtual void slot58();
    virtual void slot5C();
    virtual void slot60();
    virtual void slot64();
    virtual void slot68();
    virtual void slot6C();
    virtual int slot70();
    virtual void slot74();
    virtual void slot78();
    virtual void slot7C();
    virtual void slot80();
    virtual void slot84();
    virtual int slot88();
    virtual void slot8C();
    virtual void slot90();
    virtual void slot94();
    virtual void slot98();
    virtual void slot9C();
    virtual void slotA0();
    virtual void slotA4(int);
    virtual int slotA8();
    virtual int slotAC();
    virtual void slotB0(Vec3f* pos, int, int);
    virtual void slotB4(int);
};

// Local view of the iron wire article: the parameter pointer at 0x20E4 is the only field read here.
struct wnSheikIronWireParam {
    float unk0;
};

class wnSheikIronWire : public Weapon {
    u8 unkA8[0x20E4 - sizeof(Weapon)];
public:
    wnSheikIronWireParam* m_param;
};

inline wnSheikIronWireAltStatusView* wnSheikIronWireGetAltStatus(soModuleAccesser* moduleAccesser) {
    return *(wnSheikIronWireAltStatusView**)((char*)moduleAccesser->m_enumerationStart + 0x9c);
}

class wnSheikIronWireStatusUniqProcessShot : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessShot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessShot g_wnSheikIronWireStatusUniqProcessShot;

void wnSheikIronWireStatusUniqProcessShot::initStatus(soModuleAccesser* moduleAccesser) {
    wnSheikIronWireAltStatusView* alt = wnSheikIronWireGetAltStatus(moduleAccesser);
    wnSheikIronWire& wire = dynamic_cast<wnSheikIronWire&>(moduleAccesser->getStageObject());
    wnSheikIronWireParam* param = wire.m_param;
    // HYPOTHESIS: the y and z components start from the rodata constant lbl_104_rodata_1B0 (value unverified).
    float c = 0.0f;
    Vec3f pos(c, c, c);
    pos.m_x = param->unk0 * moduleAccesser->getPostureModule().getLr();
    alt->slotB0(&pos, alt->slot70(), 7);
    alt->slotA4(0);
}

void wnSheikIronWireStatusUniqProcessShot::execStatus(soModuleAccesser* moduleAccesser) {
    wnSheikIronWireAltStatusView* alt = wnSheikIronWireGetAltStatus(moduleAccesser);
    if (alt->slotAC() == 3) {
        moduleAccesser->getStatusModule().changeStatusRequest(3, moduleAccesser);
    } else if (alt->slotA8() == 2) {
        moduleAccesser->getStatusModule().changeStatusRequest(3, moduleAccesser);
    }
}
