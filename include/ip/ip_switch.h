// Local BrawlHeaders shadow: controller routing and replay frame state.
#pragma once
#include <StaticAssert.h>
#include <types.h>
#include <ip/ip_padrec.h>
#include <ip/ip_pad_status.h>
#include <stddef.h>

class ipNetworkProducer;
class ipSwitch {
public:
    // HYPOTHESIS: names. Zero selects local pads, nonzero the online queue;
    // only source 1 enables synchronous file I/O.
    int m_source;
    ipNetworkProducer* m_network;
    ipPadStatus m_pads[4];
    ipKeyRecorder* m_keyRecorder;
    int m_groupFrames;
    int m_phaseFrames;
    struct Flags {
        u8 unk7 : 1;
        u8 unk6 : 1;
        u8 skipFirstRecord : 1;
        u8 playbackSuspended : 1;
        u8 nullNetworkOutput : 1;
        u8 unk2 : 1;
        u8 unk1 : 1;
        u8 unk0 : 1;
    } m_flags;
    u8 m_excludedPlayer;
    u8 unk26;
    bool m_networkOutputStarted;

    ipSwitch();
    static ipSwitch* getInstance();
    static void networkOutCallback(void** data, u32* size);
    // HYPOTHESIS: the map mislabels this GameGlobal::setSaveData; its accesses
    // and callers establish an input-source setter on this object.
    void setSource(int source);
    bool setSuspendAndResumeState(bool suspended);
    bool update();
    bool getPadStatus(s32 player, ipPadStatus& pad);
    bool startPadRec(u32 chunk);
    bool stopPadRec();
    bool startPadPlay(int chunk);
    bool stopPadPlay();
    bool isPlaying();
    void suspendPlay();
    void resumePlay();
    bool isBreakGameProcLoop();
};
static_assert(sizeof(ipSwitch) == 0x28, "Input switch size");
static_assert(offsetof(ipSwitch, m_pads) == 8, "Input pad offset");
static_assert(offsetof(ipSwitch, m_keyRecorder) == 0x18, "Recorder offset");
static_assert(offsetof(ipSwitch, m_flags) == 0x24, "Input flags offset");
extern ipSwitch* g_ipSwitch;
