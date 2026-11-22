#define SO_EVENT_OBSERVER_CHANGE_MANAGE_ID
#define SO_STATUS_DATA_FIELDS
#define SO_SITUATION_OBSERVER_CTOR_INLINE
#include <ac/ac_anim_cmd_impl.h>
#include <ft/ft_cancel_module.h>
#include <ft/ft_common_data_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_module_accesser.h>
#include <types.h>

ftCancelModuleImpl::ftCancelModuleImpl(soModuleAccesser* moduleAccesser) :
    ftCancelModule(false),
    soStatusEventObserver(-1, -1),
    soAnimCmdEventObserver(-1, (s8)-1),
    soSituationEventObserver(-1, -1),
    m_moduleAccesser(moduleAccesser),
    m_cancelData(g_ftCommonData.dataCommon->cancelData),
    m_statusKind(-1),
    m_isEnableCancel(false),
    m_isUnableCancelStatus(false) {
}

inline void ftCancelModuleImpl::unableCancelAll() {
    int j;
    int i;
    for (i = 0; i < m_cancelData->m_numGroups; i++) {
        for (j = 0; j < m_cancelData->m_groups[i].m_numTransitionGroups; j++) {
            m_moduleAccesser->getStatusModule().unableTransitionTermGroup(m_cancelData->m_groups[i].m_transitionGroups[j]);
        }
    }
    m_isEnableCancel = false;
    m_statusKind = -1;
}

bool ftCancelModuleImpl::isObserv(char unitId) {
    return unitId == 100;
}

void ftCancelModuleImpl::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { }

void ftCancelModuleImpl::processFixPosition() {
    if (m_statusKind != -1 && isEnableCancel() == true) {
        if (m_statusKind != m_moduleAccesser->getSituationModule().getKind()) {
            enableCancelTransitionGroup(m_moduleAccesser->getSituationModule().getKind());
        }
    }
}

bool ftCancelModuleImpl::isEnableCancel() const {
    return m_isEnableCancel;
}

void ftCancelModuleImpl::unableCancelStatus() {
    if (!m_isUnableCancelStatus) {
        m_isUnableCancelStatus = true;
        unableCancelAll();
    }
}

void ftCancelModuleImpl::enableCancelTransitionGroup(SituationKind kind) {
    m_isEnableCancel = false;
    m_statusKind = kind;
    if (m_isUnableCancelStatus != true) {
        for (int i = 0; i < m_cancelData->m_numGroups; i++) {
            if (kind == m_cancelData->m_groups[i].m_situationKind) {
                for (int j = 0; j < m_cancelData->m_groups[i].m_numTransitionGroups; j++) {
                    m_moduleAccesser->getStatusModule().enableTransitionTermGroup(m_cancelData->m_groups[i].m_transitionGroups[j]);
                    m_moduleAccesser->getStatusModule().enableTransitionTermAll(m_cancelData->m_groups[i].m_transitionGroups[j]);
                    m_isEnableCancel = true;
                }
            } else {
                for (int j = 0; j < m_cancelData->m_groups[i].m_numTransitionGroups; j++) {
                    m_moduleAccesser->getStatusModule().unableTransitionTermGroup(m_cancelData->m_groups[i].m_transitionGroups[j]);
                }
            }
        }
    }
}

// TODO: clean up - near-miss: the original schedules the status data load above the two cancel resets.
void ftCancelModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    unableCancelAll();
    if (statusData->m_cancelKind != 0) {
        if (!m_moduleAccesser->getWorkManageModule().isFlag(0x1200000B) &&
            !m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) &&
            !m_moduleAccesser->getWorkManageModule().isFlag(0x1200002D) &&
            !m_moduleAccesser->getWorkManageModule().isFlag(0x12000022)) {
            m_moduleAccesser->getStatusModule().enableTransitionTermGroup(10);
        }
    }
    m_isUnableCancelStatus = false;
}

// TODO: clean up - near-miss (register allocation of the re-registration checks in changeManageId).
void ftCancelModuleImpl::postInitialize(soModuleAccesser* moduleAccesser) {
    soStatusEventObserver::changeManageId(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId());
    soAnimCmdEventObserver::changeManageId(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId());
    soSituationEventObserver::changeManageId(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId());
}

bool ftCancelModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 3) {
        return false;
    }
    switch (acmd->getType()) {
        case 0:
            if (!m_moduleAccesser->getWorkManageModule().isFlag(0x1200000B) &&
                !m_moduleAccesser->getWorkManageModule().isFlag(0x1200002B) &&
                !m_moduleAccesser->getWorkManageModule().isFlag(0x1200002D) &&
                !m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) &&
                m_isUnableCancelStatus != true) {
                gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
                if (scheduler == NULL || scheduler->getProcessType() >= gfTask::Process_FixPosition) {
                    enableCancelTransitionGroup(m_moduleAccesser->getSituationModule().getPrevKind());
                    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x29) {
                        m_moduleAccesser->getStatusModule().unableTransitionTerm(0x2743, 8);
                    }
                    m_isEnableCancel = true;
                }
            }
            return true;
        case 1:
            unableCancelAll();
            return true;
        case 2:
            unableCancelStatus();
            return true;
    }
    return false;
}
