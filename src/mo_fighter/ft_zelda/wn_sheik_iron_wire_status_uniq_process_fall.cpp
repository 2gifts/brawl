#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_lasso_status_view.h>
#include <wn/weapon.h>
#include <types.h>

// Local view of the iron wire alt status (+0x9C, lasso layout); signatures are the ones this status calls.
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
    virtual void slot7C(int);
    virtual void slot80(float, float, float);
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
    virtual void slotB8();
    virtual void slotBC();
    virtual void slotC0();
    virtual void slotC4();
    virtual void slotC8();
    virtual void slotCC();
    virtual void slotD0();
    virtual void slotD4();
    virtual void slotD8();
    virtual void slotDC();
    virtual void slotE0();
};


// Local view of the iron wire article: parameter pointer at 0x20E4 and a Vec3f at its +0x4.
// HYPOTHESIS: the Vec3f fields are named by their use in the fall status.
struct wnSheikIronWireParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
};

class wnSheikIronWire : public Weapon {
    u8 unkA8[0x20E4 - sizeof(Weapon)];
public:
    wnSheikIronWireParam* m_param;
};

// HYPOTHESIS: Weapon constraint helper with three -1 arguments, called by its sora_melee fn_27 symbol.
extern "C" void fn_27_1DA698(void* self, int a, int b, int c);

inline wnSheikIronWireAltStatusView* wnSheikIronWireGetAltStatus(soModuleAccesser* moduleAccesser) {
    return *(wnSheikIronWireAltStatusView**)((char*)moduleAccesser->m_enumerationStart + 0x9c);
}

class wnSheikIronWireStatusUniqProcessFall : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessFall g_wnSheikIronWireStatusUniqProcessFall;

void wnSheikIronWireStatusUniqProcessFall::initStatus(soModuleAccesser* a) {
    wnSheikIronWire* wire = &dynamic_cast<wnSheikIronWire&>(a->getStageObject());
    wnSheikIronWireParam* param = wire->m_param;
    wnLassoGetLink(a)->unk164Int(1);
    fn_27_1DA698(wire, -1, -1, -1);
    wnSheikIronWireAltStatusView* alt = wnSheikIronWireGetAltStatus(a);
    a->getPostureModule().getScale();
    alt->slotDC();
    wnSheikIronWireGetAltStatus(a)->slotA4(8);
    wnSheikIronWireGetAltStatus(a)->slot7C(1);
    wnSheikIronWireGetAltStatus(a)->slot80(param->unk4, param->unk8, param->unkC);
    a->getWorkManageModule().offFlag(0x12000004);
}

void wnSheikIronWireStatusUniqProcessFall::execFixPos(soModuleAccesser* moduleAccesser) { }
