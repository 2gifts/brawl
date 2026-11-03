#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/turn/so_turn_module.h>
#include <types.h>

void ftStatusUniqProcessGimmickLadder::correctX(soModuleAccesser* moduleAccesser, float x, int interpolate) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    if (interpolate == 0) {
        pos.m_x = x;
    } else {
        soMotionModule& motion = moduleAccesser->getMotionModule();
        float endFrame = motion.getEndFrame();
        float frame = motion.getFrame();
        pos.m_x += (0.4f + 0.6f * (frame / endFrame)) * (x - pos.m_x);
    }
    moduleAccesser->getPostureModule().setPos(&pos);
}

void ftStatusUniqProcessGimmickLadder::correctY(soModuleAccesser* moduleAccesser, float upperY, float lowerY, int interpolate) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    float scale = moduleAccesser->getPostureModule().getScale();
    float height = soValueAccesser::getConstantFloat(moduleAccesser, 0xBE5, 0);
    float offset = height * scale;
    offset *= moduleAccesser->getPostureModule().getScale();
    float y = pos.m_y;
    float upper = upperY - offset;
    if (upper < y) {
        if (interpolate == 0) {
            pos.m_y = upper;
        } else {
            pos.m_y = y + 0.4f * (upper - y);
        }
    }
    if (lowerY > pos.m_y) {
        lowerY += 0.1f;
        if (interpolate == 0) {
            pos.m_y = lowerY;
        } else {
            pos.m_y = y + 0.4f * (lowerY - y);
        }
    }
    moduleAccesser->getPostureModule().setPos(&pos);
}

void ftStatusUniqProcessGimmickLadder::adjustLadderMovement(soModuleAccesser* moduleAccesser, float x, float y) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    float oldX = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    if (oldX != x) {
        pos.m_x += x - oldX;
        moduleAccesser->getWorkManageModule().setFloat(x, 0x21000004);
    }
    float oldY = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    if (oldY != y) {
        pos.m_y += y - oldY;
        moduleAccesser->getWorkManageModule().setFloat(y, 0x21000005);
    }
    moduleAccesser->getPostureModule().setPos(&pos);
}

void ftStatusUniqProcessGimmickLadder::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessGimmick::initStatus(moduleAccesser);
    soGimmickLadderEventArgs_GetInfo info;
    presentEvent(&info, moduleAccesser);
    moduleAccesser->getWorkManageModule().setFloat(info.m_upperNodePos.m_x, 0x21000004);
    moduleAccesser->getWorkManageModule().setFloat(info.m_upperNodePos.m_y, 0x21000005);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x92:
        static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule)->end();
        initLadderCatch(moduleAccesser, &info);
        break;
    case 0x93:
        static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule)->end();
        initLadderCatchBottom(moduleAccesser, &info);
        break;
    case 0x94:
        initLadder(moduleAccesser, &info);
        break;
    case 0x95:
        initLadderEnd(moduleAccesser, &info);
        break;
    }
}

void ftStatusUniqProcessGimmickLadder::execStatus(soModuleAccesser* moduleAccesser) {
    soGimmickLadderEventArgs_GetInfo info;
    presentEvent(&info, moduleAccesser);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x92: execLadderCatch(moduleAccesser, &info); break;
    case 0x93: execLadderCatchBottom(moduleAccesser, &info); break;
    case 0x94: execLadder(moduleAccesser, &info); break;
    case 0x95: execLadderEnd(moduleAccesser, &info); break;
    }
}

void ftStatusUniqProcessGimmickLadder::execFixPos(soModuleAccesser* moduleAccesser) {
    soGimmickLadderEventArgs_GetInfo info;
    presentEvent(&info, moduleAccesser);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x92: fixPosLadderCatch(moduleAccesser, &info); break;
    case 0x93: fixPosLadderCatchBottom(moduleAccesser, &info); break;
    case 0x94: fixPosLadder(moduleAccesser, &info); break;
    case 0x95: fixPosLadderEnd(moduleAccesser, &info); break;
    }
}

void ftStatusUniqProcessGimmickLadder::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x92: exitLadderCatch(moduleAccesser); break;
    case 0x93: exitLadderCatchBottom(moduleAccesser); break;
    case 0x94: exitLadder(moduleAccesser); break;
    case 0x95: exitLadderEnd(moduleAccesser); break;
    }
    Vec3f rot = moduleAccesser->getPostureModule().getRot(0);
    rot.m_y = 0.0f;
    moduleAccesser->getPostureModule().setRot(&rot, 0);
    moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x59E1, 0), 0x10000011);
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, nextStatus);
}
