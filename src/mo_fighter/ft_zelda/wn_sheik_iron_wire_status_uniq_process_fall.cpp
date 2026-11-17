#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// BYTES: the wire offset is built with the out-of-line Vec3f subtraction, emitted after its first caller.
#define MT_VEC3F_OPERATORS_NOINLINE
// MATCH-ONLY: the weak __mi__5Vec3f copy loads this.xy into f3 and other.z into f0 (the existing native register order).
#define MT_VEC3F_SUB_NATIVE_ORDER
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
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
    virtual void slotC4(int, Vec3f*);
    virtual void slotC8();
    virtual void slotCC();
    virtual void slotD0();
    virtual void slotD4();
    virtual void slotD8();
    virtual void slotDC(float scale);
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

inline wnSheikIronWireAltStatusView* wnSheikIronWireGetAltStatus(soModuleAccesser* moduleAccesser) {
    return static_cast<wnSheikIronWireAltStatusView*>(moduleAccesser->m_enumerationStart->m_physicsModule);
}

class wnSheikIronWireStatusUniqProcessFall : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessFall g_wnSheikIronWireStatusUniqProcessFall;

void wnSheikIronWireStatusUniqProcessFall::initStatus(soModuleAccesser* a) {
    // BYTES: param is declared ahead of wire; the original gives it r31 and lets alt reuse wire's register.
    wnSheikIronWireParam* param;
    wnSheikIronWire* wire = &dynamic_cast<wnSheikIronWire&>(a->getStageObject());
    param = wire->m_param;
    wnLassoGetLink(a)->unk164Int(1);
    wire->Weapon::constraint(-1, -1, -1);
    const soModuleEnumeration* modules = a->m_enumerationStart;
    wnSheikIronWireAltStatusView* alt = static_cast<wnSheikIronWireAltStatusView*>(modules->m_physicsModule);
    alt->slotDC(modules->m_postureModule->getScale());
    wnSheikIronWireGetAltStatus(a)->slotA4(8);
    wnSheikIronWireGetAltStatus(a)->slot7C(1);
    wnSheikIronWireGetAltStatus(a)->slot80(param->unk4, param->unk8, param->unkC);
    a->getWorkManageModule().offFlag(0x12000004);
}

// Keeps the wire end on the parent's node 0x1e (link 3): the offset between the node's two global positions, less the
// parent's summed speed, goes to the alt status; a one-shot sound (0x1037) plays when its state query reports 1.
void wnSheikIronWireStatusUniqProcessFall::execFixPos(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soLinkModule* link = modules->m_linkModule;
    wnSheikIronWireAltStatusView* alt = static_cast<wnSheikIronWireAltStatusView*>(modules->m_physicsModule);
    Vec3f nodePosition = link->getParentModelNodeGlobalPosition(3, 0x1e, true);
    Vec3f previousNodePosition = link->getParentModelNodeGlobalPosition(3, 0x1e, false);
    Vec3f speed(soExternalValueAccesser::getKineticModule(link->getParent(3))->getSumSpeed(soKineticEnergy::AttributeFlag(-1)),
                0.0f);
    Vec3f offset = previousNodePosition - nodePosition - speed;
    alt->slotC4(0, &offset);
    if (wnSheikIronWireGetAltStatus(moduleAccesser)->slotA8() == 1) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x12000004)) {
            moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1037), false);
            moduleAccesser->getWorkManageModule().onFlag(0x12000004);
        }
    } else {
        moduleAccesser->getWorkManageModule().offFlag(0x12000004);
    }
}
