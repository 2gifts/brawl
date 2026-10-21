#include <ip/ip_switch.h>
#include <ip/ip_network_producer.h>
#include <ip/ip_padconfig.h>
#include <gf/gf_file_io_manager.h>
#include <gf/gf_pad_system.h>
#include <mt/mt_prng.h>
#include <nt/network.h>
#include <nt/nt_report.h>
#include <sr/sr_common.h>
#include <memory.h>

// HYPOTHESIS: network configuration view; the callback reads only these fields.
struct ipNetworkLocalPads {
    u8 unk0[0x199];
    u8 count;
    u8 unk19A[3];
    u8 pads[4];
};
extern ipNetworkLocalPads* lbl_805A0390;
static ipPadStatus s_networkPads[4] __attribute__((aligned(8)));
ipSwitch* g_ipSwitch;
static u32 s_consumedNetworkFrames;

ipSwitch::ipSwitch() {
    m_source = 0;
    m_network = NULL;
    m_keyRecorder = NULL;
    m_groupFrames = 0;
    m_flags.unk7 = false;
    m_flags.unk6 = false;
    m_flags.nullNetworkOutput = false;
    m_excludedPlayer = 0xFF;
    unk26 = 0xF;
    m_keyRecorder = new (Heaps::SystemFW) ipKeyRecorder(Heaps::Replay, 0x4B000);
    g_Network.setLocalDataSize(sizeof(ipPadStatus));
    NtShared::ntSetPutProductDataCallback(networkOutCallback);
    for (u32 i = 0; i < 4; ++i) NtBase::setSIDPlayer(i, 1);
    m_network = ipNetworkProducer::getInstance();
}

#pragma dont_inline on
void NtShared::ntSetPutProductDataCallback(ntPutProductDataCallback_t callback) {
    NtPutProductDataCallback = callback;
}
#pragma dont_inline reset

ipSwitch* ipSwitch::getInstance() {
    if (!g_ipSwitch) g_ipSwitch = new (Heaps::SystemFW) ipSwitch;
    return g_ipSwitch;
}

// Produce one local input record for each locally assigned online player.
// A missing assignment, an excluded player, or the null-output flag supplies
// neutral input while still consuming the queued controller frame.
void ipSwitch::networkOutCallback(void** data, u32* size) {
    ipSwitch* input = getInstance();
    if (input == NULL) { *data = NULL; *size = 0; return; }
    if (!input->m_networkOutputStarted) {
        input->m_networkOutputStarted = true;
        s_consumedNetworkFrames = 0;
    }
    gfPadSystem* pads = g_gfPadSystem;
    if (pads->getGamePadQueueCount() != 0) {
        ipPadStatus* output = s_networkPads;
        u32 count = lbl_805A0390->count;
        const u8* localPad = lbl_805A0390->pads;
        for (u32 i = 0; i < count; ++i, ++localPad) {
            u32 pad = *localPad;
            if (pad == 0xFF) pad = 0;
            gfPadStatus status;
            pads->getGamePadStatus(pad, &status);
            int player = ipPadConfig::getInstance()->getPlayerNoFast(pad);
            if (player < 0 || player == input->m_excludedPlayer || input->m_flags.nullNetworkOutput) {
                output->m_button = 0;
                output->m_stickY = 0;
                output->m_stickX = 0;
            } else {
                ipPadConfig::getInstance()->getPadInput(player, &status, output);
            }
            ++output;
        }
        *data = s_networkPads;
        *size = count * sizeof(ipPadStatus);
        pads->updateGame();
    } else {
        *data = NULL;
        *size = 0;
    }
}

void ipSwitch::setSource(int source) {
    m_source = source;
    m_flags.nullNetworkOutput = false;
    m_excludedPlayer = 0xFF;
    if (m_network != NULL) m_network->clear();
    g_gfFileIOManager->setSyncMode(source == 1);
}

bool ipSwitch::setSuspendAndResumeState(bool suspended) {
    if (suspended) {
        if (isPlaying()) m_flags.playbackSuspended = true;
        return m_keyRecorder->setSyncCommand(m_keyRecorder->unk49, 1);
    }
    if (m_flags.playbackSuspended) m_flags.playbackSuspended = false;
    return m_keyRecorder->setSyncCommand(m_keyRecorder->unk49, 0);
}

// Online starvation stalls the game before replay/recording can advance. Local
// pads retain the previous frame when the pad system has no fresh game data.
bool ipSwitch::update() {
    gfPadSystem* pads = g_gfPadSystem;
    if (m_source == 0) {
        pads->updateGame();
        if (pads->m_flags34.f7) {
            for (u32 i = 0; i < 4; ++i) {
                int pad = ipPadConfig::getInstance()->m_playerPads[(int)i];
                if (pad >= 0 && pad < 8) {
                    gfPadStatus status;
                    pads->getGamePadStatus(pad & 0xF, &status);
                    ipPadConfig::getInstance()->getPadInput(i, &status, &m_pads[i]);
                } else {
                    m_pads[i].m_button = 0;
                    m_pads[i].m_stickY = 0;
                    m_pads[i].m_stickX = 0;
                }
            }
        }
    } else {
        if (m_network->getCount() == 0) return false;
        const ipPadStatus* packet = (const ipPadStatus*)m_network->front();
        for (int i = 0; i < 4; ++i) m_pads[i] = packet[i];
        m_network->pop();
        ++s_consumedNetworkFrames;
    }
    if (m_keyRecorder != NULL) {
        if (m_keyRecorder->m_mode == ipKeyRecorder::Playing) {
            if (m_flags.playbackSuspended != 1) {
                u8* packet;
                if (m_keyRecorder->play(&packet, &m_groupFrames, &m_phaseFrames)) {
                    m_keyRecorder->extractPadStatus(m_pads, packet);
                }
            }
        } else if (m_keyRecorder->m_mode == ipKeyRecorder::Recording && m_keyRecorder->m_pauseStatus != 1) {
            // Recording lags input sampling by one update; startup skips the
            // initial packet, whose pads have not yet been sampled.
            if (m_flags.skipFirstRecord) m_flags.skipFirstRecord = false;
            else m_keyRecorder->record();
            if (m_keyRecorder->m_pauseStatus == 2) {
                m_keyRecorder->m_randomSeed = 0;
                ipPadStatus neutral[4];
                memset(neutral, 0, sizeof(neutral));
                m_keyRecorder->setPadStatus(neutral, 4);
            } else {
                m_keyRecorder->m_randomSeed = randseed();
                m_keyRecorder->setPadStatus(m_pads, 4);
            }
        }
    }
    return true;
}

bool ipSwitch::getPadStatus(s32 player, ipPadStatus& pad) {
    pad = m_pads[player];
    return true;
}

bool ipSwitch::startPadRec(u32 chunk) {
    bool started = false;
    if (m_keyRecorder != NULL) started = m_keyRecorder->startRec(chunk);
    m_flags.skipFirstRecord = true;
    m_networkOutputStarted = false;
    return started;
}

bool ipSwitch::stopPadRec() {
    bool stopped = false;
    if (m_keyRecorder != NULL) stopped = m_keyRecorder->endRec();
    return stopped;
}

bool ipSwitch::startPadPlay(int chunk) {
    bool started = false;
    if (m_keyRecorder != NULL) {
        m_flags.playbackSuspended = false;
        started = m_keyRecorder->startPlay(chunk);
    }
    return started;
}

bool ipSwitch::stopPadPlay() {
    bool stopped = false;
    if (m_keyRecorder != NULL) stopped = m_keyRecorder->endPlay();
    return stopped;
}

bool ipSwitch::isPlaying() {
    bool playing = false;
    if (m_keyRecorder != NULL) playing = m_keyRecorder->m_mode == ipKeyRecorder::Playing;
    return playing;
}

void ipSwitch::suspendPlay() {
    if (isPlaying()) m_flags.playbackSuspended = true;
}

void ipSwitch::resumePlay() {
    if (m_flags.playbackSuspended) m_flags.playbackSuspended = false;
}

bool ipSwitch::isBreakGameProcLoop() {
    bool playing = isPlaying();
    bool stop = false;
    if (playing && m_groupFrames <= 1) stop = true;
    return stop;
}
