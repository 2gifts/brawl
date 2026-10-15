#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
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

// HYPOTHESIS: the link event (kind 0xB) a thrower sends to the thrown fighter when a throw starts; the receiver fills in the
// fields marked as outputs.
struct ftLinkEventThrow : soLinkEventArgs {
    int m_motionKind;  // +8: throw motion (throw status - 0x72 + 0x76)
    int m_output0;     // +0xC
    int m_output1;     // +0x10
    int m_unk14;       // +0x14
    float m_output2;   // +0x18
    bool m_hasMotionBit; // +0x1C: the throw bit is set in the common int constant 0x59F2
    ftLinkEventThrow(int eventKind, int motionKind) :
        soLinkEventArgs(eventKind), m_motionKind(motionKind), m_output0(-1), m_output1(-1), m_unk14(-1), m_output2(1.0f) { }
};

// Starts a throw: tells the thrown fighter (the link node) about the throw through a link event, stores the node's task ID and
// the event's outputs in the work variables, and gives the thrower the invincibility frames of the throw.
void ftStatusUniqProcessThrow::initStatus(soModuleAccesser* moduleAccesser) {
    int throwIndex = moduleAccesser->getWorkManageModule().getInt(0x20000000) - 0x72;
    int motionBits = soValueAccesser::getConstantInt(moduleAccesser, 0x59F2, 0);
    ftLinkEventThrow event(0xB, throwIndex + 0x76);
    event.m_hasMotionBit = ((1 << throwIndex) & motionBits) != 0;
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_output0, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_output1, 0x20000004);
    moduleAccesser->getWorkManageModule().setFloat(event.m_output2, 0x21000004);
    moduleAccesser->getCollisionHitModule().setInvincibleFrameGlobal(soValueAccesser::getConstantInt(moduleAccesser, 0x5A2E, 0), false, 0);
}

void ftStatusUniqProcessThrow::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (kind == 0x3b) {
        return;
    }
    if (kind >= 0x3b || kind != 0) {
        reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->notifyThrowExit(0, 0);
    }
}

bool ftStatusUniqProcessThrow::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkThrowDamage(moduleAccesser, damageInfo);
}
