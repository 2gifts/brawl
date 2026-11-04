#include <wn/diddy/wn_diddy_gun.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// Registers the Diddy popgun article (kind 54) in the class info table during static initialization.
wnClassInfoImpl<54, wnDiddyGun, wnDiddyGunData> g_wnDiddyGunClassInfo;

void wnDiddyGun::activate(int founderTaskId, int resourceId, bool constraint, int nodeId) {
    if (constraint) {
        Vec3f origin(0.0f, 0.0f, 0.0f);
        wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                            origin, 0.0f, -1, 0, 2, 0x80, 0, 0x35F, 0);
        desc.flagsHigh = 1;
        desc.flagsMidHigh = 0;
        desc.flagsMidLow = 1;
        desc.flagsLow = 0;
        desc.unk4DHigh = 0;
        Weapon::activate(&desc);
        Weapon::constraint(founderTaskId, nodeId, 1);
    } else {
        Vec3f origin(0.0f, 0.0f, 0.0f);
        wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                            origin, 0.0f, -1, 0, 2, 0x80, 0, 0x35F, 0);
        desc.flagBit7 = 1;
        desc.flagBit6 = 1;
        desc.flagsMidPair = 0;
        desc.flagsBit3 = 1;
        desc.flagsLow = 0;
        desc.unk4DHigh = 0;
        Weapon::activate(&desc);
        m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    }
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
}

void wnDiddyGun::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser,
                                StageObject* sender, int selector) {
    switch (event->m_eventKind) {
    case 0x838:
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        break;
    case 0x839:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    }
    Weapon::notifyEventLink(event, moduleAccesser, sender, selector);
}
