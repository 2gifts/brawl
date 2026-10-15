#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of the stage object calls.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_status_uniq_process_views.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

int randi(long max);
void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// Local view of the stop (hit-stop) module; no header in this tree. Slots 7 and 9 are called without arguments.
class ftStopModuleView {
public:
    virtual ~ftStopModuleView() { }
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void unk6();
    virtual void unk7();
    virtual void unk8();
    virtual void unk9();
};

class ftStatusUniqProcessIkeFinalDamage : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIkeFinalDamage() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* unk);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessIkeFinalDamage g_ftStatusUniqProcessIkeFinalDamage;

// HYPOTHESIS: the victim of Ike's final smash starts with a random hit-reaction value (work int 0x20000001, one of seven
// values from the table) when none was set, turns away from the link, clears the kinetic energy and keeps the link offset in
// work floats 0x21000004/5.
static const int s_ikeFinalDamageReactionTable[7] = { 0x99, 0x9a, 0x9d, 0x9f, 0xa0, 0xa2, 0xa3 };

void ftStatusUniqProcessIkeFinalDamage::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == -1) {
        int index = randi(7);
        if (index < 0 || index >= 7) {
            index = 0;
        }
        moduleAccesser->getWorkManageModule().setInt(s_ikeFinalDamageReactionTable[index], 0x20000001);
    }
    reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(-reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slotA8(4));
    moduleAccesser->getPostureModule().updateRotYLr();
    moduleAccesser->m_stageObject->updateNodeSRT();
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    Vec3f offset = reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slot98(4, 0, 0);
    moduleAccesser->getWorkManageModule().setFloat(offset.m_x, 0x21000004);
    moduleAccesser->getWorkManageModule().setFloat(offset.m_y, 0x21000005);
}

// Each frame the victim is moved by the change of the link offset (work floats 0x21000004/5 hold the previous one).
void ftStatusUniqProcessIkeFinalDamage::execStatus(soModuleAccesser* moduleAccesser) {
    Vec3f offset = reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slot98(4, 0, 0);
    float previousX = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float previousY = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    pos.m_x += offset.m_x - previousX;
    pos.m_y += offset.m_y - previousY;
    moduleAccesser->getPostureModule().setPos(&pos);
    moduleAccesser->getWorkManageModule().setFloat(offset.m_x, 0x21000004);
    moduleAccesser->getWorkManageModule().setFloat(offset.m_y, 0x21000005);
}

// Once the motion has ended (or loops no more) the victim goes into the throw-away motion 0x29.
void ftStatusUniqProcessIkeFinalDamage::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getMotionModule().isEnd()) {
        int kind = moduleAccesser->getMotionModule().getKind();
        if (kind == 0xa4 || kind == 0xa5 || kind == 0xa6 || kind == 0xa7) {
            soMotionChangeParam param;
            param.m_kind = 0x29;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        } else if (!moduleAccesser->getMotionModule().isLoopFlag()) {
            soMotionChangeParam param;
            param.m_kind = 0x29;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        }
    }
}

void ftStatusUniqProcessIkeFinalDamage::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().chkLinkStop() == false) {
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk7();
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk9();
    }
}

bool ftStatusUniqProcessIkeFinalDamage::checkDamage(soModuleAccesser* moduleAccesser, void* unk) { return true; }

void ftStatusUniqProcessIkeFinalDamage::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    // The 0xf8 and 0xf9 exits keep the link.
    switch (nextStatusKind) {
    case 0xf8:
    case 0xf9:
        return;
    default:
        if (moduleAccesser->getLinkModule().isLink(4)) {
            moduleAccesser->getLinkModule().unlink(4);
        }
        break;
    }
}
