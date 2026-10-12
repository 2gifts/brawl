#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: link event sent when the missile stops (kind 0x1a) with a float and a flag after the base fields.
struct wnDededeStarmissileLinkEventArgs : soLinkEventArgs {
    wnDededeStarmissileLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind) { }
    float m_value;
    u8 unk0c[4];
    bool m_flag;
};

class wnDededeStarmissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnDededeStarmissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnDededeStarmissileStatusUniqProcess g_wnDededeStarmissileStatusUniqProcess;

void wnDededeStarmissileStatusUniqProcess::initStatus(soModuleAccesser*) { }

void wnDededeStarmissileStatusUniqProcess::execStatus(soModuleAccesser*) { }

void wnDededeStarmissileStatusUniqProcess::execStop(soModuleAccesser* acc) {
    // HYPOTHESIS: node 1 is the missile node; its rotation X follows work float 0x21000002.
    Vec3f rot = acc->getModelModule().getNodeRotate(1);
    rot.m_x = acc->getWorkManageModule().getFloat(0x21000002);
    acc->getModelModule().setNodeRotate(1, &rot);
}

void wnDededeStarmissileStatusUniqProcess::execFixPos(soModuleAccesser*) { }

void wnDededeStarmissileStatusUniqProcess::exitStatus(soModuleAccesser* acc, int) {
    if (!acc->getWorkManageModule().isFlag(0x22000000)) {
        wnDededeStarmissileLinkEventArgs args(0x1a);
        // HYPOTHESIS: the value is constant lbl_27_rodata_7DC8 (not verified).
        args.m_value = 0.0f;
        args.m_flag = false;
        acc->getLinkModule().sendEventNodes(0, args, 0);
        acc->getWorkManageModule().onFlag(0x22000000);
    }
}
