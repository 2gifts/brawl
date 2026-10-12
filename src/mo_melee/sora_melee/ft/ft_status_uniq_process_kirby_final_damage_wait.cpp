#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
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
void ftStatusUniqProcessKirbyFinalDamageWait::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessKirbyFinalDamageWait::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageWait::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessKirbyFinalDamageWait::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
