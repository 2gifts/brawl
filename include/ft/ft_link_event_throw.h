#pragma once

#include <so/link/so_link_event_presenter.h>
#include <types.h>

// HYPOTHESIS: the link event (kind 0xB) a thrower sends to the thrown fighter when a throw starts; the receiver fills in the
// fields marked as outputs.
struct ftLinkEventThrow : soLinkEventArgs {
    int m_motionKind;      // +8: throw motion
    int m_output0;         // +0xC
    int m_output1;         // +0x10
    int m_unk14;           // +0x14
    float m_output2;       // +0x18
    bool m_hasMotionBit;   // +0x1C
    ftLinkEventThrow(int eventKind, int motionKind) :
        soLinkEventArgs(eventKind), m_motionKind(motionKind), m_output0(-1), m_output1(-1), m_unk14(-1), m_output2(1.0f) { }
    ftLinkEventThrow(int eventKind, int motionKind, bool hasMotionBit) :
        soLinkEventArgs(eventKind), m_motionKind(motionKind), m_output0(-1), m_output1(-1), m_unk14(-1), m_output2(1.0f),
        m_hasMotionBit(hasMotionBit) { }
};
