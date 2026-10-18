#include <it/it_manager.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance.
struct ftStatusUniqProcessHeartSwapPair {
    u32 m_0;
    u32 m_4;
    ftStatusUniqProcessHeartSwapPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftStatusUniqProcessHeartSwapPair g_ftStatusUniqProcessHeartSwapPairA(0xff, 0);
static ftStatusUniqProcessHeartSwapPair g_ftStatusUniqProcessHeartSwapPairB(0xff, 1);

// Local view of the motion module: the shared soMotionModule header puts getFrame/setFrame at other slots,
// so these two calls are made through slots 8 and 9 of the real vtable.
class ftMotionModuleView {
public:
    virtual ~ftMotionModuleView() { }
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void unk6();
    virtual void unk7();
    virtual float getFrameView();
    virtual void setFrameView(float frame);
};

class ftStatusUniqProcessHeartSwap : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessHeartSwap() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessHeartSwap g_ftStatusUniqProcessHeartSwap;

void ftStatusUniqProcessHeartSwap::initStatus(soModuleAccesser* moduleAccesser) { }

// Frame constants from module rodata (lbl_27_rodata_25F8 and lbl_27_rodata_25FC).
static const float s_heartSwapCheckFrame = 0.0f;
static const float s_heartSwapResetFrame = 1.0f;

void ftStatusUniqProcessHeartSwap::execStatus(soModuleAccesser* moduleAccesser) {
    if (s_heartSwapCheckFrame == reinterpret_cast<ftMotionModuleView*>(moduleAccesser->m_enumerationStart->m_motionModule)->getFrameView()) {
        int instanceId = moduleAccesser->getWorkManageModule().getInt(0x10000030);
        if (instanceId != 0) {
            if (itManager::getInstance()->getItemFromInstanceId(instanceId) == 0) {
                reinterpret_cast<ftMotionModuleView*>(moduleAccesser->m_enumerationStart->m_motionModule)->setFrameView(s_heartSwapResetFrame);
            }
        } else {
            reinterpret_cast<ftMotionModuleView*>(moduleAccesser->m_enumerationStart->m_motionModule)->setFrameView(s_heartSwapResetFrame);
        }
    }
}

void ftStatusUniqProcessHeartSwap::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessHeartSwap::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    // HYPOTHESIS: work int 0x10000030 holds the instance id of the item that is removed on exit.
    int instanceId = moduleAccesser->getWorkManageModule().getInt(0x10000030);
    if (instanceId != 0) {
        BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
        if (item != 0) {
            itManager::getInstance()->removeItem(item);
        }
    }
}
