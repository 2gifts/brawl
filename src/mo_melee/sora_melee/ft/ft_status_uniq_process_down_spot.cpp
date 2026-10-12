#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: class shape from the shared Down_Spot status (kind 0x4B); the status keeps the spot motion
// kind (0xbc or 0xbd) in work int 0x20000008 and the flag 0x22000019 mirrors which motion was chosen.
// HYPOTHESIS: the work manager slot 0x3c takes one u32 work id and its result is unused. Its
// slot is not in so_work_manage_module_impl.h (getFloat is at 0x38 there), so a view class with the
// leading slots reproduces the call. Verify the name against the native work manager interface.
class ftWorkManageSlotView {
public:
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0c();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1c();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2c();
    virtual void unk30();
    virtual float getFloatSlot3c(u32 index);
};

class ftStatusUniqProcessDownSpot : public soStatusUniqProcess {
public:
    ftStatusUniqProcessDownSpot() { }
    virtual ~ftStatusUniqProcessDownSpot() { }
    virtual void initStatus(soModuleAccesser*);
};

void ftStatusUniqProcessDownSpot::initStatus(soModuleAccesser* moduleAccesser) {
    int kind;
    int motionId = 0xbc;
    if (moduleAccesser->getMotionModule().isAnimResFile(motionId) == 1) {
        kind = 0xbc;
        int flag = 0x22000019;
        moduleAccesser->getWorkManageModule().onFlag(flag);
    } else {
        kind = 0xbd;
        int flag = 0x22000019;
        moduleAccesser->getWorkManageModule().offFlag(flag);
    }
    moduleAccesser->getWorkManageModule().setInt(kind, 0x20000008);
    moduleAccesser->getMotionModule().getEndFrame(kind);
    reinterpret_cast<ftWorkManageSlotView&>(moduleAccesser->getWorkManageModule()).getFloatSlot3c(0x21000008);
}

ftStatusUniqProcessDownSpot g_ftStatusUniqProcessDownSpot;
