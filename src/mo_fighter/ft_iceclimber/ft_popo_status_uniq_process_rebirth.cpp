#include <ft/ft_manager.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Base rebirth processes (sora_melee), called by their fn_27 names.
extern "C" void fn_27_1733F8(soStatusUniqProcess* self, soModuleAccesser* moduleAccesser);
extern "C" void fn_27_173BCC(soStatusUniqProcess* self, soModuleAccesser* moduleAccesser, Vec3f* pos);
extern "C" void fn_27_173C74(soStatusUniqProcess* self, soModuleAccesser* moduleAccesser, Vec3f* pos);

// HYPOTHESIS: local view of the posture module; only the vtable slots 0x18 (getPos) and 0x1c (setPos) are used.
class ftPopoPostureModuleView {
public:
    virtual void unk0();
    virtual void unk4();
    virtual void unk8();
    virtual void unkC();
    virtual Vec3f getPos();
    virtual void setPos(Vec3f* pos);
};

static ftPopoPostureModuleView& getPosture(soModuleAccesser* moduleAccesser) {
    return *reinterpret_cast<ftPopoPostureModuleView*>(moduleAccesser->m_enumerationStart->m_postureModule);
}

class ftPopoStatusUniqProcessRebirth : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessRebirth() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    void attachEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos);
    void updateEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos);
};

void ftPopoStatusUniqProcessRebirth::initStatus(soModuleAccesser* moduleAccesser) {
    ftManager* manager = g_ftManager;
    if (manager->getSubFighter(moduleAccesser->getWorkManageModule().getInt(0x10000000)) != 0) {
        Vec3f pos = getPosture(moduleAccesser).getPos();
        if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
            pos.m_x += 3.0f;
        } else {
            pos.m_x -= 3.0f;
        }
        getPosture(moduleAccesser).setPos(&pos);
    }
    fn_27_1733F8(this, moduleAccesser);
}

void ftPopoStatusUniqProcessRebirth::attachEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos) {
    Vec3f local = pos;
    ftManager* manager = g_ftManager;
    if (manager->getSubFighter(moduleAccesser->getWorkManageModule().getInt(0x10000000)) != 0) {
        local.m_x -= 3.0f;
    }
    fn_27_173BCC(this, moduleAccesser, &local);
}

void ftPopoStatusUniqProcessRebirth::updateEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos) {
    Vec3f local = pos;
    ftManager* manager = g_ftManager;
    if (manager->getSubFighter(moduleAccesser->getWorkManageModule().getInt(0x10000000)) != 0) {
        local.m_x -= 3.0f;
    }
    fn_27_173C74(this, moduleAccesser, &local);
}

ftPopoStatusUniqProcessRebirth g_ftPopoStatusUniqProcessRebirth;
