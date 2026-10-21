#include <ft/samus/ft_samus_article_pools.h>
#include <so/so_module_accesser.h>
#include <wn/wn_class_info_impl.h>

// Registers the Samus charge shot article class (kind 83) in the class info table during static initialization.
wnClassInfoImpl<83, wnSamusCShot, wnSamusCShotData> g_wnSamusCShotClassInfo;

// HYPOTHESIS: the shot spawns at the model origin, offset along x for kind 3 (work int 0x10000007), then enters the status for its charge level.
void wnSamusCShot::toShoot(int charge, float lr) {
    float offsetX = m_moduleAccesser->getWorkManageModule().getInt(0x10000007) == 3 ? 1.0f : 0.0f;
    Vec3f offset(offsetX, 0.0f, 0.0f);
    Vec3f position = m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false);
    position.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().init(&position, lr);
    removeConstraint();
    switch (charge) {
    case 0:
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        break;
    case 1:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    case 2:
        m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        break;
    case 3:
        m_moduleAccesser->getStatusModule().changeStatusRequest(4, m_moduleAccesser);
        break;
    case 4:
        m_moduleAccesser->getStatusModule().changeStatusRequest(5, m_moduleAccesser);
        break;
    case 5:
        m_moduleAccesser->getStatusModule().changeStatusRequest(6, m_moduleAccesser);
        break;
    case 6:
        m_moduleAccesser->getStatusModule().changeStatusRequest(7, m_moduleAccesser);
        break;
    default:
        m_moduleAccesser->getStatusModule().changeStatusRequest(8, m_moduleAccesser);
        break;
    }
    updateNodeSRT();
}
