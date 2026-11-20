#define SO_COLLISION_CTOR_DTOR
#define SO_EVENT_PRESENTER_UPDATE_CHECK_MANAGE_ID
#define SO_COLLISION_GROUP_ALIGNED
#include <so/collision/so_collision_catch_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/collision/so_collision_log.h>
#include <so/collision/so_collision_manager.h>
#include <so/event/so_event_system.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/stageobject.h>
#include <gf/gf_task_scheduler.h>
#include <gf/gf_camera.h>
#include <cl/cl_collision.h>
#include <nw4r/ut/ut_list.h>
#include <math.h>
#include <types.h>

// TODO: clean up: soCollisionCatchPart's own methods are still unnamed in so_collision_catch_part.cpp.
extern "C" u32 fn_27_4AEBC(soCollisionCatchPart* part, soModelModule* modelModule, newCollision* collision, Vec3f* start, Vec3f* end, float scale);
extern "C" void fn_27_4AFE4(soCollisionCatchPart* part, soModelModule* modelModule, newCollision* collision, float scale);
extern "C" void fn_27_4B088(soCollisionCatchPart* part, soModelModule* modelModule, newCollision* collision, float scale);
extern "C" void fn_27_4B0E8(soCollisionCatchPart* part, nw4r::math::_MTX34* view, newCollision* collision);

// TODO: clean up: the catch observer's notify slot (so_collision_catch_event_presenter.h names it notifyEventTurn with
// other parameters; that header is shared with the enemy units, so it is left alone here).
class soCollisionCatchEventObserverView {
public:
    virtual void addObserver(s16, s8);
    virtual void notifyEventCollisionCatch(float distance, soCollisionLog* log, soModuleAccesser* moduleAccesser);
};

// TODO: clean up: the soCollisionManager flags word (raw offset 0xB4; the class keeps it in an opaque block).
struct soCollisionManagerFlagsView {
    char _0[0xB4];
    u32 m_flags;
};

soCollisionCatchModuleImpl::soCollisionCatchModuleImpl(soModuleAccesser* acc, int taskId, u8 category,
                                                       soArray<soCollisionCatchPart>* parts,
                                                       soEventObserverRegistrationDesc* regDesc, bool checkFront, bool keepLog) :
    soStatusEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), (s8)-1),
    soAnimCmdEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), (s8)-1),
    soEventPresenter<soCollisionCatchEventObserver>(regDesc->checkCollisionCatch() == true ? static_cast<soEventManager&>(acc->getEventManageModule()).getManageId() : (s16)-1, 2, soEventPresenterLocalStore),
    m_moduleAccesser(acc),
    m_collisionCatchPartArray(parts),
    m_scale(1.0f),
    m_collisionGroupArrayVector(1),
    m_collision(taskId, (gfTask::Category)category, (int)acc->getTeamModule().getTeam(), 6, &m_collisionGroupArrayVector, false) {
    m_isCheck = true;
    m_checkFront = checkFront;
    m_isUpdated = false;
    m_keepLog = keepLog;
}

soCollisionCatchModuleImpl::~soCollisionCatchModuleImpl() { }

void soCollisionCatchModuleImpl::activate(float posX, float scale, float lr, int taskId, bool isCheck) {
    m_collision.m_taskId = taskId;
    m_posX = posX;
    m_scale = scale;
    m_lr = lr;
    m_isCheck = isCheck;
    for (int i = 0; i < m_collisionCatchPartArray->size(); i++) {
        m_collisionCatchPartArray->at(i).reset();
    }
}

void soCollisionCatchModuleImpl::deactivate() {
    if (m_keepLog == false) {
        m_collision.clean();
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
    m_collision.m_taskId = -1;
}

void soCollisionCatchModuleImpl::begin() {
    m_isUpdated = false;
}

void soCollisionCatchModuleImpl::clearAll() {
    for (int i = 0; i < m_collisionCatchPartArray->size(); i++) {
        m_collisionCatchPartArray->at(i).unk0 = 0;
    }
}

void soCollisionCatchModuleImpl::clear(u32 partIndex) {
    soCollisionCatchPart& part = m_collisionCatchPartArray->at(partIndex);
    part.unk0 = 0;
}

void soCollisionCatchModuleImpl::set(u32 partIndex, soCollisionCatchData* data) {
    int i;
    m_collision.allocateCollision(partIndex, data->unk14_5, 0);
    newCollision* collision = m_collision.getCollision(partIndex, 0);
    if (m_collisionCatchPartArray->at(partIndex).unk0 == 0) {
        soCollisionCatchPart& newPart = m_collisionCatchPartArray->at(partIndex);
        newPart.unk0 = 1;
        m_collision.initLog(0);
        for (i = 0; i < m_collisionCatchPartArray->size(); i++) {
            if (m_collisionCatchPartArray->at(i).unk0 != 0 && i != (int)partIndex) {
                goto setData;
            }
        }
        m_collision.initPastLog(0, false);
    }
setData:
    m_collisionCatchPartArray->at(partIndex).set(data, reinterpret_cast<soCollisionShieldPartUnit*>(collision));
    if (m_isUpdated == true) {
        soCollisionCatchPart* part;
        soModelModule* modelModule = m_moduleAccesser->m_enumerationStart->m_modelModule;
        part = &m_collisionCatchPartArray->at(partIndex);
        fn_27_4B088(part, modelModule, collision, m_scale);
    }
}

bool soCollisionCatchModuleImpl::isCatch(u32 partIndex) {
    return m_collisionCatchPartArray->at(partIndex).unk0 != 0;
}

float soCollisionCatchModuleImpl::getSize(u32 partIndex) {
    return m_collisionCatchPartArray->at(partIndex).m_data.m_size;
}

void soCollisionCatchModuleImpl::setSize(u32 partIndex, float size) {
    m_collisionCatchPartArray->at(partIndex).m_data.m_size = size;
}

Vec3f soCollisionCatchModuleImpl::getCenterPos(u32 partIndex) {
    return m_collision.getCollision(partIndex, 0)->getCenterPos();
}

soCollisionCatchData* soCollisionCatchModuleImpl::getData(u32 partIndex) const {
    return &m_collisionCatchPartArray->at(partIndex).m_data;
}

void soCollisionCatchModuleImpl::update(float posX, float scale, float lr) {
    soCollisionCatchPart* part;
    soModelModule* modelModule;
    u32 isHit = false;
    int i;
    m_posX = posX;
    m_scale = scale;
    m_lr = lr;
    m_collision.initCheck(0);
    m_collision.updateGroup(posX, 0.0f, 0);
    m_collision.initLog(0);
    m_collision.initPastLog(0, false);
    int partNum = m_collisionCatchPartArray->size();
    for (i = 0; i < partNum; i++) {
        part = &m_collisionCatchPartArray->at(i);
        if (part->unk0 != 0) {
            modelModule = m_moduleAccesser->m_enumerationStart->m_modelModule;
            if (fn_27_4AEBC(part, modelModule, m_collision.getCollision(i, 0), NULL, NULL, m_scale) == true) {
                isHit = true;
            }
        }
    }
    if (isHit == true) {
        if (!g_soCollisionManager) {
            g_soCollisionManager = new (Heaps::System) soCollisionManager;
        }
        if (!(reinterpret_cast<soCollisionManagerFlagsView*>(g_soCollisionManager)->m_flags & 0x40) || !m_isCheck) {
            m_collision.addCheckList();
        }
    }
    m_isUpdated = true;
}

void soCollisionCatchModuleImpl::updatePos(u32 partIndex) {
    soModelModule* modelModule = m_moduleAccesser->m_enumerationStart->m_modelModule;
    soCollisionCatchPart& part = m_collisionCatchPartArray->at(partIndex);
    fn_27_4AFE4(&part, modelModule, m_collision.getCollision(partIndex, 0), m_scale);
}

void soCollisionCatchModuleImpl::check() {
    soInstanceManagerFullProperty<soCollisionCatchEventObserver*>* observerList = getObserverList();
    int groupNum = m_collisionGroupArrayVector.size();
    for (int i = 0; i < groupNum; i++) {
        nw4r::ut::List* logList = static_cast<nw4r::ut::List*>(m_collision.getLogList(i));
        for (soCollisionLog* log = NULL; (log = static_cast<soCollisionLog*>(nw4r::ut::List_GetNext(logList, log))) != NULL;) {
            gfTask* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)log->m_taskCategory, log->m_taskId);
            if (task == NULL) {
                continue;
            }
            soCollisionHitModule* hitModule = soExternalValueAccesser::getCollisionHitModule(dynamic_cast<StageObject*>(task));
            float posX = m_collision.getPosX(i);
            float diff = hitModule->getPosX(log->_35) - posX;
            if (m_checkFront == true && diff * m_lr < 0.0f) {
                continue;
            }
            float distance = fabs(diff);
            for (int j = 0; j < (int)observerList->size(); j++) {
                reinterpret_cast<soCollisionCatchEventObserverView*>(observerList->atIndex(j))->notifyEventCollisionCatch(distance, log, m_moduleAccesser);
            }
        }
    }
}

// TODO: clean up: an empty function between check and renderDebug that nothing references (name unknown).
extern "C" void fn_27_4C030() { }

void soCollisionCatchModuleImpl::renderDebug() {
    soCollisionCatchPart* part;
    int i;
    nw4r::math::_MTX34 view = gfCameraManager::getManager()->m_cameras[0].m_viewMatrix;
    for (i = 0; i < m_collisionCatchPartArray->size(); i++) {
        if (m_collisionCatchPartArray->at(i).unk0 != 0) {
            part = &m_collisionCatchPartArray->at(i);
            fn_27_4B0E8(part, &view, m_collision.getCollision(i, 0));
        }
    }
}

bool soCollisionCatchModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 0x30) {
        return false;
    }
    switch (acmd->getType()) {
    case 10: {
        soArrayContractibleTable<const acCmdArgConv> argList = acmd->getArgList();
        soCollisionCatchData data;
        int partIndex = acGetCmdArg(argList, 0).getIntData();
        acShiftCmdArg(argList);
        data.m_nodeIndex = acGetCmdArg(argList, 0).getIntData();
        acShiftCmdArg(argList);
        data.m_size = acGetCmdArg(argList, 0).getFloatData();
        acShiftCmdArg(argList);
        data.m_offsetX = acGetCmdArg(argList, 0).getFloatData();
        acShiftCmdArg(argList);
        data.m_offsetY = acGetCmdArg(argList, 0).getFloatData();
        acShiftCmdArg(argList);
        data.m_offsetZ = acGetCmdArg(argList, 0).getFloatData();
        acShiftCmdArg(argList);
        acCmdArg arg = acGetCmdArg(argList, 0);
        // TODO: clean up: the shift and the next read are repeated in both branches (two inline copies in the original).
        if (arg.getArgType() == 5) {
            data.m_unk10_9 = soValueAccesser::getValueInt(moduleAccesser, arg.getValueIndexData(), 0);
            acShiftCmdArg(argList);
            data.m_unk10_28 = acGetCmdArg(argList, 0).getIntData();
        } else {
            data.m_unk10_9 = arg.getIntData();
            acShiftCmdArg(argList);
            data.m_unk10_28 = acGetCmdArg(argList, 0).getIntData();
        }
        acShiftCmdArg(argList);
        data.m_targetMask = 3;
        bool isNoTeam = false;
        if (!argList.isEmpty()) {
            int mask = acGetCmdArg(argList, 0).getIntData();
            if (mask != 0) {
                data.m_targetMask &= ~mask;
            }
            acShiftCmdArg(argList);
            if (!argList.isEmpty()) {
                if (acGetCmdArg(argList, 0).getBoolData() == true) {
                    isNoTeam = true;
                }
            }
        }
        data.unk14_0 = 9;
        data.unk14_4 = false;
        data.unk14_5 = false;
        data.m_isNoTeam = isNoTeam;
        set(partIndex, &data);
        return true;
    }
    case 12: {
        soArrayContractibleTable<const acCmdArgConv> argList = acmd->getArgList();
        const acCmdArg& partArg = acGetCmdArg(argList, 0);
        if (partArg.getArgType() != 0) {
            return true;
        }
        clear(partArg.getIntData());
        return true;
    }
    case 13:
        clearAll();
        return true;
    case 11: {
        soArrayContractibleTable<const acCmdArgConv> argList = acmd->getArgList();
        const acCmdArg& partArg = acGetCmdArg(argList, 0);
        if (partArg.getArgType() != 0) {
            return true;
        }
        acShiftCmdArg(argList);
        const acCmdArg& sizeArg = acGetCmdArg(argList, 0);
        if (sizeArg.getArgType() != 1) {
            return true;
        }
        setSize(partArg.getIntData(), sizeArg.getFloatData());
        return true;
    }
    }
    return false;
}
