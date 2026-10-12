#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: view of the catch module (enumeration +0x3C, typed void* in the shared header). Only the slots
// used by the throw statuses are named: 0x2C and 0x3C. The filler slots are not identified.
class ftCatchModuleView {
public:
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void notifyThrowExit(int, int); // slot 0x2C
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool checkThrowDamage(soModuleAccesser* moduleAccesser, void* damageInfo); // slot 0x3C
};

class ftStatusUniqProcessThrow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessThrow g_ftStatusUniqProcessThrow;

// TODO: initStatus builds a small grab-throw info record and calls the link, work and collision modules (not decoded).
void ftStatusUniqProcessThrow::initStatus(soModuleAccesser*) {}

void ftStatusUniqProcessThrow::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (kind == 0x3b) {
        return;
    }
    if (kind > 0x3b || kind != 0) {
        reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->notifyThrowExit(0, 0);
    }
}

bool ftStatusUniqProcessThrow::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkThrowDamage(moduleAccesser, damageInfo);
}
