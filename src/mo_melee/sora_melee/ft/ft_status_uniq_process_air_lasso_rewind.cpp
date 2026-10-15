#include <ft/ft_status_uniq_process_air_lasso_rewind.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <types.h>

ftStatusUniqProcessAirLassoRewind g_ftStatusUniqProcessAirLassoRewind;

void ftStatusUniqProcessAirLassoRewind::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the rewind speed is the float at +4 of the constant block 0xA80E (same block as the lasso frames).
    float rewindSpeed = *(float*)((u8*)soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0) + 4);
    moduleAccesser->getWorkManageModule().setFloat(rewindSpeed, 0x21000004);
}

// HYPOTHESIS: while the lasso is pulled back the fighter's position follows the hip node, but never above either foot.
void ftStatusUniqProcessAirLassoRewind::execStatus(soModuleAccesser* moduleAccesser) {
    soModelModule& model = moduleAccesser->getModelModule();
    int leftFoot = model.getNodeId("LFootJ");
    int rightFoot = model.getNodeId("RFootJ");
    int hip = model.getCorrectNodeId(0x12D);
    Vec3f leftPos = model.getNodeGlobalPosition(leftFoot, false);
    Vec3f rightPos = model.getNodeGlobalPosition(rightFoot, false);
    Vec3f hipPos = model.getNodeGlobalPosition(hip, false);
    if (leftPos.m_y > rightPos.m_y) {
        if (rightPos.m_y < hipPos.m_y) {
            hipPos.m_y = rightPos.m_y;
        }
    } else if (leftPos.m_y < hipPos.m_y) {
        hipPos.m_y = leftPos.m_y;
    }
    moduleAccesser->getPostureModule().setPos(&hipPos);
}

// Leaving the rewind puts the fighter at the node the lasso was attached to (at the ground shape's z) and unlinks it;
// unless the next status is 0x75 it also leaves the cliff.
void ftStatusUniqProcessAirLassoRewind::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    soModelModule& model = moduleAccesser->getModelModule();
    int node = model.getCorrectNodeId(0x12E);
    Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
    pos.m_z = moduleAccesser->getGroundModule().getZ();
    reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unlinkSlot164(1);
    pos = moduleAccesser->getModelModule().getTopNodeGlobalPosFromNode(node, &pos);
    moduleAccesser->getPostureModule().setPos(&pos);
    moduleAccesser->m_stageObject->updateNodeSRT();
    if (nextStatusKind != 0x75) {
        moduleAccesser->getGroundModule().leaveCliff(0);
    }
}
