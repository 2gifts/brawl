#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: slotLinkConstraint is the model-constraint setter (u32 no, int node, target, u16 flag, bool); name and arguments unverified. Link module view (local). Two destructor entries, then 74 pads, put slotLinkConstraint at vtable index 77 (0x134).
class ftLinkModuleConstraintView {
public:
    virtual ~ftLinkModuleConstraintView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void pad15();
    virtual void pad16();
    virtual void pad17();
    virtual void pad18();
    virtual void pad19();
    virtual void pad20();
    virtual void pad21();
    virtual void pad22();
    virtual void pad23();
    virtual void pad24();
    virtual void pad25();
    virtual void pad26();
    virtual void pad27();
    virtual void pad28();
    virtual void pad29();
    virtual void pad30();
    virtual void pad31();
    virtual void pad32();
    virtual void pad33();
    virtual void pad34();
    virtual void pad35();
    virtual void pad36();
    virtual void pad37();
    virtual void pad38();
    virtual void pad39();
    virtual void pad40();
    virtual void pad41();
    virtual void pad42();
    virtual void pad43();
    virtual void pad44();
    virtual void pad45();
    virtual void pad46();
    virtual void pad47();
    virtual void pad48();
    virtual void pad49();
    virtual void pad50();
    virtual void pad51();
    virtual void pad52();
    virtual void pad53();
    virtual void pad54();
    virtual void pad55();
    virtual void pad56();
    virtual void pad57();
    virtual void pad58();
    virtual void pad59();
    virtual void pad60();
    virtual void pad61();
    virtual void pad62();
    virtual void pad63();
    virtual void pad64();
    virtual void pad65();
    virtual void pad66();
    virtual void pad67();
    virtual void pad68();
    virtual void pad69();
    virtual void pad70();
    virtual void pad71();
    virtual void pad72();
    virtual void pad73();
    virtual void slotLinkConstraint(u32 index, int node, int targetNode, u16 flag, bool attribute);
};

// Link setup record handed to link slot 0x44 (names unverified).
struct ftLinkSetupData {
    int m_kind;
    u8 m_flagA;
    int m_taskId;
    int m_unk;
    u8 m_flagB;
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessKirbyFinalDamageWait : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKirbyFinalDamageWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessKirbyFinalDamageWait g_ftStatusUniqProcessKirbyFinalDamageWait;

void ftStatusUniqProcessKirbyFinalDamageWait::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftLinkModuleConstraintView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotLinkConstraint(5, 0, 0, 1, false);
}
void ftStatusUniqProcessKirbyFinalDamageWait::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageWait::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessKirbyFinalDamageWait::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }

static ftLinkModuleSlotView* linkView(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
}

// HYPOTHESIS: leaving the final-smash damage wait detaches link slots 4 and 5 (re-registering slot 4 with this
// fighter's task id) and, when linked to a parent, resets the link transform from constants 0xd44/0xd45.
void ftStatusUniqProcessKirbyFinalDamageWait::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (linkView(moduleAccesser)->slot2c(4)) {
        ftLinkSetupData data;
        data.m_kind = 0x33;
        data.m_flagA = 0;
        data.m_taskId = 0;
        data.m_unk = 0;
        data.m_flagB = 0;
        data.m_taskId = moduleAccesser->m_stageObject->m_taskId;
        linkView(moduleAccesser)->slot44(4, &data);
        linkView(moduleAccesser)->slot28(4);
    }
    if (linkView(moduleAccesser)->slot2c(5)) {
        if (linkView(moduleAccesser)->slot168() == true) {
            float x = soValueAccesser::getConstantFloat(moduleAccesser, 0xd45, 0);
            float y = soValueAccesser::getConstantFloat(moduleAccesser, 0xd44, 0);
            struct {
                Vec3f m_second;
                Vec3f m_first;
            } vecs;
            vecs.m_first.m_x = y;
            vecs.m_first.m_y = x;
            vecs.m_first.m_z = 0.0f;
            vecs.m_second.m_x = 0.0f;
            vecs.m_second.m_y = 0.0f;
            vecs.m_second.m_z = 0.0f;
            linkView(moduleAccesser)->slot15c(&vecs.m_first, &vecs.m_second);
            linkView(moduleAccesser)->unlinkSlot164(1);
        }
        linkView(moduleAccesser)->slot28(5);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000001);
    moduleAccesser->getVisibilityModule().setWhole(1);
    moduleAccesser->getEffectModule().setWhole(true);
}
