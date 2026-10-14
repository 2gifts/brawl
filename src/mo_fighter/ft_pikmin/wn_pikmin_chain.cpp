#define MT_VEC3F_ASSIGN_NOINLINE
#include <mt/mt_vector.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/pikmin/wn_pikmin_chain.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node or color the chain model is attached with).
extern u32 lbl_113_data_BAA8;

void wnPikminChain::activate(float lr, int founderTaskId, int kind, int team, bool unk, int pikminNum) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = kind;
    desc.unk8 = kind;
    desc.unkC = kind;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = zero;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    wnLassoGetLink(m_moduleAccesser)->unk148Int(2);
    wnLassoGetLink(m_moduleAccesser)->unk194Int(3, 8, false);
    wnLassoGetLink(m_moduleAccesser)->unk194Int(3, 8, false);
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    u32 nodeId = lbl_113_data_BAA8;
    alt->unk18Attach(m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getArchiveId(), 0, nodeId, nodeId, true, 0, 0, -1);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setFlag(unk, 0x22000000);
    m_moduleAccesser->getWorkManageModule().setInt(pikminNum, 0x10000007);
    m_moduleAccesser->getWorkManageModule().setInt(0xb, 0x10000005);
    alt = wnLassoGetAltStatus(m_moduleAccesser);
    alt->unkDCFloat(m_moduleAccesser->getPostureModule().getScale());
    wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
}

void wnPikminChain::onDeactivate() {
    m_moduleAccesser->getLinkModule().unlinkAll();
    m_moduleAccesser->getLinkModule().unlinkNodeAll();
}

void wnPikminChain::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    float scaleValue = acc->getPostureModule().getScale();
    Vec3f scale(scaleValue, scaleValue, scaleValue);
    Vec3f pos;
    pos = acc->getPostureModule().getPos();
    Vec3f rot;
    rot = acc->getPostureModule().getRot(0);
    rot.m_y += acc->getPostureModule().getRotYLr();
    acc->getModelModule().setNodeSRT(1, &scale, &rot, &pos);
    acc->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

void wnPikminChain::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 7) {
        // Reaching the end of the chain: remember how many Pikmin it carried.
        int pikminNum = m_moduleAccesser->getWorkManageModule().getInt(0x10000007);
        m_moduleAccesser->getWorkManageModule().setInt(pikminNum, 0x20000002);
    }
}

void wnPikminChain::processFixCamera() {
    Weapon::processFixCamera();
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == false) {
        int status = m_moduleAccesser->getStatusModule().getStatusKind();
        if (status < 6) {
            if (3 < status) {
                return;
            }
            if (status < 1) {
                return;
            }
        } else if (7 < status) {
            return;
        }
        wnPikminChainUtility::updatePikminVisibility(m_moduleAccesser);
    }
}

void wnPikminChain::renderDebug() {
}
