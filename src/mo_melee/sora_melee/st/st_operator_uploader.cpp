#include <st/operator/st_operator_uploader.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_etc_upload_eval.h>
#include <nt/nt_etc_watching.h>
#include <revolution/OS/OSError.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: muWifiInterfaceTask::isPermitUploadWatching (unnamed in the dol).
bool fn_800D99D8(muWifiInterfaceTask* task);
// HYPOTHESIS: nteWatching::upload (unnamed in the dol).
bool fn_80155C50(nteWatching* watching);
}

// HYPOTHESIS: the step values; the steps resume at the source line they were left at.
#pragma explicit_zero_data on
static u32 s_stepInit = 0;
#pragma explicit_zero_data reset

inline stOperatorUploader::Step::Step() {
    m_value = s_stepInit;
    m_line = 0;
}

inline void stOperatorUploader::Step::reset() {
    m_value = s_stepInit;
    m_line = 0;
}

stOperatorUploader::stOperatorUploader() : stOperator("StOperatorUploader"), m_isDone(false) {
    m_step.reset();
    unk2C_b1 = false;
}

stOperatorUploader::~stOperatorUploader() { }

static u32 s_stepUpload = 1;
static u32 s_stepCancel = 1;
static u32 s_stepWait = 1;
static u32 s_stepEnd = 2;

void stOperatorUploader::processBegin() {
    if (!m_isStart) {
        return;
    }
    if (m_isDone) {
        return;
    }
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    if (wifi == NULL) {
        m_isDone = true;
        return;
    }
    switch (m_step.m_line) {
    case 0:
        m_step.m_value = s_stepUpload;
        m_step.m_line = 67;
    case 67:
        if (nteUpdateEvaluator::getInstance() != NULL && fn_800D99D8(wifi)) {
            m_uploader = new (Heaps::StageInstance) nteWatching(Heaps::StageInstance);
            if (m_uploader == NULL) {
                OSReport("[stOperatorUploader] _netUploader == 0\n");
            } else {
                m_uploader->m_unk358 = 12000;
                if (!fn_80155C50(m_uploader)) {
                    OSReport("[stOperatorUploader] !_netUploader->upload()\n");
                    delete m_uploader;
                } else {
                    while (!m_uploader->m_unk348) {
                        if (m_isExit) {
                            m_uploader->cancel();
                            while (!m_uploader->m_unk348) {
                                m_step.m_value = s_stepCancel;
                                m_step.m_line = 126;
                                return;
    case 126:;
                            }
                            break;
                        }
                        m_step.m_value = s_stepWait;
                        m_step.m_line = 130;
                        return;
    case 130:;
                    }
                    switch (m_uploader->m_unk34C) {
                    case 0:
                        OSReport("[stOperatorUploader] nteContribution::STATUS_NOERROR\n");
                        break;
                    case 1:
                        OSReport("[stOperatorUploader] nteContribution::STATUS_FAILED\n");
                        break;
                    case 2:
                        OSReport("[stOperatorUploader] nteContribution::STATUS_ERROR_DWC\n");
                        break;
                    case 3:
                        OSReport("[stOperatorUploader] nteContribution::STATUS_ERROR_SBS\n");
                        break;
                    default:
                        OSReport("[stOperatorUploader] nteContribution::%d ???\n", m_uploader->m_unk34C);
                        break;
                    }
                    delete m_uploader;
                }
            }
        }
        m_isDone = true;
        m_step.m_value = s_stepEnd;
        m_step.m_line = 174;
    }
}
