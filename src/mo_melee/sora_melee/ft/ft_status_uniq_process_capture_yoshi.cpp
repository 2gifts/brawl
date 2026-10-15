#include <ft/ft_status_uniq_process_capture_yoshi.h>
#include <ft/ft_status_uniq_process_views.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

ftStatusUniqProcessCaptureYoshi g_ftStatusUniqProcessCaptureYoshi;

// HYPOTHESIS: model module slot 0xd4 (secondary table at +8) takes a node id and returns that node's position as a Vec3f
// (hidden return pointer); it is not declared in so_model_module_impl.h, so a view with the leading slots reproduces
// the call. The 8-byte padding struct puts the vptr at +8 like the real module.
struct ftModelModulePadding {
    u32 unk0;
    u32 unk4;
};

class ftModelModuleNodePosView : public ftModelModulePadding {
public:
    virtual ~ftModelModuleNodePosView() { }
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
    virtual Vec3f slotD4(u32 nodeId);
};

void ftStatusUniqProcessCaptureYoshi::execStatus(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003B)) {
        ftStatusUniqProcessCapturePulled::execStatus(moduleAccesser);
    }
}

// HYPOTHESIS: leaving the capture (except into status 0xec) moves the fighter to the capture node and unlinks it,
// clears the work flag 0x1200003b, stops its kinetic energy and re-shows the model.
void ftStatusUniqProcessCaptureYoshi::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    soPostureModule* posture = moduleAccesser->m_enumerationStart->m_postureModule;
    soModelModule* modelModule = moduleAccesser->m_enumerationStart->m_modelModule;
    ftLinkModuleSlotView* link = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
    if (link->slot168()) {
        int nodeId = modelModule->getCorrectNodeId(0x12e);
        Vec3f pos = posture->getPos();
        Vec3f nodePos = reinterpret_cast<ftModelModuleNodePosView*>(modelModule)->slotD4(nodeId);
        Vec3fAdd(&pos, &pos, &nodePos);
        posture->initPos(&pos);
        link->unlinkSlot164(1);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x1200003b);
    if (status != 0xec) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.m_speed = Vec2f(0.0f, 0.0f);
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
        moduleAccesser->getVisibilityModule().setWhole(1);
    }
}

bool ftStatusUniqProcessCaptureYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(-1);
}
