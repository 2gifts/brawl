#define SO_PHYSICS_MODULE_IMPL_FULL
#include <ph/ph_2ndary_constraint.h>
#include <ph/phIkController.h>
#include <ph/ph_2ndary_controller.h>
#include <so/physics/so_physics_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/templates/so_array_value_soPhysicsIKHandle.h>
#include <types.h>

extern "C" void fn_8008F3E0(ph2ndary2Controller* controller, int status);
extern "C" void fn_80095990(ph2ndaryController* controller, const int* winding);

soPhysicsModule::~soPhysicsModule() {
}

void soPhysicsModuleImpl::init(void* ikData) {
    m_ikData = ikData;
    for (int i = 0; i < m_ikHandles->size(); i++) {
        m_ikHandles->at(i).unk34 = 0.0f;
    }
}

void soPhysicsModuleImpl::begin() {
    unk44_80 = 0;
    if (m_2ndaryController != NULL) {
        if (m_moduleAccesser->getStopModule().isStop() == true && unk44_08) {
            m_2ndaryController->setSleep(true);
        } else {
            m_2ndaryController->setSleep(m_2ndSleep);
        }
    }
}

bool soPhysicsModuleImpl::isNcessarySetUpMtx2ndBoth(bool reverse) {
    if (m_2ndary2Controller != NULL && !unk44_80) {
        if ((!reverse && is2ndBoth() == true) ||
            (reverse == true && is2ndBothReverse() == true)) {
            return true;
        }
    }
    return false;
}

bool soPhysicsModuleImpl::isIK(int ikId) {
    return m_ikHandles->at(ikId).unk34 > 0.0f;
}

void soPhysicsModuleImpl::setUpMtx2ndPreCollision() {
    if (m_2ndaryController != NULL && (u32)m_2ndPreCollisionNode != 0xFFFFFFFF && is2nd() == true) {
        Matrix* worldMtx = m_moduleAccesser->getModelModule().getWorldMtxArray();
        nw4r::g3d::ResMdl resMdl = m_moduleAccesser->getModelModule().getScnMdl()->m_resMdl;
        u32 nodes[200];
        m_2ndaryController->setAwake(true, m_2ndPreCollisionNode);
        int count = m_2ndaryController->get2ndaryLastNode(nodes, resMdl);
        m_moduleAccesser->getModelModule().setUpMtx(nodes, count);
        m_2ndaryController->calacCallBackTimingCFor2ndary((int)worldMtx,
            (int)m_moduleAccesser->getModelModule().getScnMdl(), 1, 0);
        m_2ndaryController->setAwake(false, m_2ndPreCollisionNode);
    }
}

void soPhysicsModuleImpl::setUpMtx2nd() {
    if (m_2ndaryController != NULL && is2nd() == true) {
        Matrix* worldMtx = m_moduleAccesser->getModelModule().getWorldMtxArray();
        u32 nodes[200];
        int count = m_2ndaryController->get2ndaryLastNode(nodes,
            m_moduleAccesser->getModelModule().getScnMdl()->m_resMdl);
        m_moduleAccesser->getModelModule().setUpMtx(nodes, count);
        m_2ndaryController->calacCallBackTimingCFor2ndary((int)worldMtx,
            (int)m_moduleAccesser->getModelModule().getScnMdl(), 1, 0);
    }
}

void soPhysicsModuleImpl::setUpVisibility() {
    if (m_2ndaryController != NULL && is2nd() == true) {
        m_2ndaryController->setUndispNode(m_moduleAccesser->getModelModule().getScnMdl());
    }
    if (m_2ndary2Controller != NULL && (is2ndBoth() == true || is2ndBothReverse() == true)) {
        m_2ndary2Controller->setUndispNode(m_moduleAccesser->getModelModule().getScnMdl());
    }
}

bool soPhysicsModuleImpl::is2nd() {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        return controller->unk20 != 0;
    }
    return false;
}

void soPhysicsModuleImpl::set2nd(int on) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        if (!controller->unk20 && on) {
            controller->unk0E = 1;
        }
        controller->unk20 = on;
    }
}

void soPhysicsModuleImpl::set2ndGravity(float gravity) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->m_gravity = gravity;
    }
}

void soPhysicsModuleImpl::set2ndType(int type) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->setWeaponType(type);
    }
}

int soPhysicsModuleImpl::get2ndLineCount() {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->m_lines.getSize();
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->m_controller[0]->m_lines.getSize();
    }
    return 0;
}

float soPhysicsModuleImpl::get2ndLineLength(int lineId) {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->m_lines[lineId]->unk80;
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->m_controller[0]->m_lines[lineId]->unk80;
    }
    return 0.0f;
}

void soPhysicsModuleImpl::set2ndLineLength(int lineId, float length) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        if (controller->unk55 == 0 && lineId == controller->m_lines.getSize() - 1) {
            if (controller->unk55 != 0 && lineId != 0) {
                controller->m_lines[lineId]->unk80 = length;
            }
        }
        return;
    }
    ph2ndary2Controller* controller2 = m_2ndary2Controller;
    if (controller2 != NULL) {
        if (controller2->m_controller[0]->unk55 == 0 && lineId == controller2->m_controller[0]->m_lines.getSize() - 1) {
            if (controller2->m_controller[0]->unk55 != 0 && lineId != 0) {
                controller2->m_controller[0]->m_lines[lineId]->unk80 = length;
            }
        }
        lineId = controller2->m_controller[0]->m_lines.getSize() - 1 - lineId;
        if (controller2->m_controller[1]->unk55 == 0 && lineId == controller2->m_controller[1]->m_lines.getSize() - 1) {
            if (controller2->m_controller[1]->unk55 != 0 && lineId != 0) {
                controller2->m_controller[1]->m_lines[lineId]->unk80 = length;
            }
        }
    }
}

int soPhysicsModuleImpl::get2ndActiveNodeNum() {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->getWeaponLineCount();
    }
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        if (controller->unk08[0]) {
            return controller->m_controller[1]->getWeaponLineCount();
        }
        return controller->m_controller[0]->getWeaponLineCount();
    }
    return 0;
}

void soPhysicsModuleImpl::set2ndActiveNodeNum(int num) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unk42 = num;
        controller->unk43 = num;
        return;
    }
    if (m_2ndary2Controller != NULL) {
        controller = m_2ndary2Controller->m_controller[0];
        controller->unk42 = num;
        controller->unk43 = num;
    }
}

void soPhysicsModuleImpl::set2ndBoth(int on) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        ph2ndaryController* first = controller->m_controller[0];
        if (first != NULL) {
            if (!first->unk20 && on) {
                first->unk0E = 1;
            }
            first->unk20 = on;
        }
        if (controller->m_controller[1] != NULL) {
            controller->m_controller[1]->unk20 = on;
        }
    }
}

void soPhysicsModuleImpl::set2ndBothStatus(int status) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        fn_8008F3E0(controller, status);
    }
}

int soPhysicsModuleImpl::get2ndBothSituation() {
    switch (m_2ndary2Controller->m_controller[0]->unk59 % 10) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 3;
        case 0:
        default:
            return 0;
    }
}

int soPhysicsModuleImpl::get2ndBothNodeMax() {
    return m_2ndary2Controller->m_controller[0]->unk5A;
}

void soPhysicsModuleImpl::set2ndBothSpeed(const Vec3f* speed, int lineId, u8 flag) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    controller->m_controller[0]->setLastVelocity(speed, lineId, flag);
    if (controller->m_controller[1] != NULL) {
        controller->m_controller[1]->unk5B = flag;
    }
}

void soPhysicsModuleImpl::set2ndBothRewind(int rewind) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (!controller->unk08[0]) {
        fn_80095990(controller->m_controller[0], &rewind);
    } else {
        fn_80095990(controller->m_controller[1], &rewind);
    }
}

void soPhysicsModuleImpl::set2ndBothWaver(int waver) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    ph2ndaryController* first = controller->m_controller[0];
    first->m_waver = waver;
    first->m_active = 1;
    ph2ndaryController* second = controller->m_controller[1];
    second->m_waver = waver;
    second->m_active = 1;
}

void soPhysicsModuleImpl::set2ndBothHitStat(int hitStat) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    controller->m_controller[0]->unk5B = hitStat;
    if (controller->m_controller[1] != NULL) {
        controller->m_controller[1]->unk5B = hitStat;
    }
}

void soPhysicsModuleImpl::set2ndBothReverseFlag(int flag) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        controller->unk08[2] = flag;
    }
}

u32 soPhysicsModuleImpl::get2ndNodeId(int lineId) {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->getWeaponStartID(lineId);
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->m_controller[0]->getWeaponStartID(lineId);
    }
    return 0;
}

void soPhysicsModuleImpl::set2ndPosSplit(int split) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unkE8 = split;
        return;
    }
    if (m_2ndary2Controller != NULL) {
        m_2ndary2Controller->m_controller[0]->unkE8 = split;
    }
}

bool soPhysicsModuleImpl::is2ndHitCollision() {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->isHitCollisionLastNode2Pos(7);
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->isHitCollisionLastNode2Pos();
    }
    return false;
}

void soPhysicsModuleImpl::set2ndWeaponScale(float scale) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->m_weaponScale = scale;
        return;
    }
    ph2ndary2Controller* controller2 = m_2ndary2Controller;
    if (controller2 != NULL) {
        controller2->m_controller[0]->m_weaponScale = scale;
        controller2->m_controller[1]->m_weaponScale = scale;
    }
}

void soPhysicsModuleImpl::set2ndCliffDir(float lr) {
    bool left = lr == -1.0f;
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unk56 = left;
        return;
    }
    ph2ndary2Controller* controller2 = m_2ndary2Controller;
    if (controller2 != NULL) {
        controller2->m_controller[0]->unk56 = left;
        controller2->m_controller[1]->unk56 = left;
    }
}

void soPhysicsModuleImpl::sync2ndSavedPosition() {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unk0E = 1;
    }
}

bool soPhysicsModuleImpl::isIK() {
    if (unk30 != 0) {
        for (int i = 0; i < m_ikHandles->size(); i++) {
            if (isIK(i) == true) {
                return true;
            }
        }
    }
    return false;
}

bool soPhysicsModuleImpl::isObserv(char group) {
    return group == 0x17;
}

void soPhysicsModuleImpl::set2ndSleep(bool sleep) {
    m_2ndSleep = sleep;
}

void soPhysicsModuleImpl::set2ndPreCollisionNode(int nodeId) {
    m_2ndPreCollisionNode = nodeId;
}

void soPhysicsModuleImpl::set2ndDefault() {
    set2nd(m_2ndDefault);
}

void soPhysicsModuleImpl::set2ndDefault(int on) {
    m_2ndDefault = on;
}

void soPhysicsModuleImpl::destructController() {
    if (m_2ndaryController != NULL) {
        delete m_2ndaryController;
        m_2ndaryController = NULL;
    }
    if (m_2ndary2Controller != NULL) {
        delete m_2ndary2Controller;
        m_2ndary2Controller = NULL;
    }
    if (unk30 != NULL) {
        delete unk30;
        unk30 = NULL;
    }
}

void soPhysicsModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    set2ndGravity(1.0f);
    set2ndSleep(false);
    if (unk30 != NULL) {
        for (int index = 0; index < m_ikHandles->size(); ++index) {
            unk30->resetToeAngle(m_ikHandles->at(index).unk0);
        }
    }
}

bool soPhysicsModuleImpl::is2ndBothReverse() {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        int state;
        ph2ndaryController& first = *controller->m_controller[0];
        state = first.unk57;
        int status;
        ph2ndaryController* second = controller->m_controller[1];
        if (second != NULL && !first.unk20 && !second->unk20) {
            if (state == 8) {
                status = 3;
            } else {
                status = 6;
            }
        } else if (state == 0) {
            if (first.m_winding) {
                status = 4;
            } else {
                status = 0;
            }
        } else if (state == 4) {
            if (first.m_winding) {
                status = 5;
            } else {
                status = 1;
            }
        } else if (state == 5) {
            status = 2;
        } else if (state == 6) {
            if (first.m_winding) {
                status = 5;
            } else {
                status = 1;
            }
        } else if (state == 7) {
            status = 6;
        } else if (state == 9) {
            status = 7;
        } else if (state == 10) {
            status = 8;
        } else {
            status = 3;
        }
        return status == 2;
    }
    return false;
}


void soPhysicsModuleImpl::set2ndRewind(int rewind) {
    if (m_2ndaryController != NULL) {
        int winding = rewind;
        fn_80095990(m_2ndaryController, &winding);
    }
}

void soPhysicsModuleImpl::off2ndWind() {
    set2ndWind(NULL);
    unk44_10 = 1;
}

void soPhysicsModuleImpl::setToeAngleLimit(int ikId, float limit) {
    if (unk30 != NULL) {
        unk30->setToeAngle(m_ikHandles->at(ikId).unk0, limit);
    }
}

// HYPOTHESIS: layout of one entry of the IK table behind m_ikData.
struct soPhysicsIKNodeTable {
    const char** m_names;
    int m_count;
};

const char* soPhysicsModuleImpl::getIKEndNodeName(int ikId, int index) {
    soPhysicsIKNodeTable* table = static_cast<soPhysicsIKNodeTable*>(m_ikData);
    return table[ikId].m_names[table[ikId].m_count - (index + 1)];
}
