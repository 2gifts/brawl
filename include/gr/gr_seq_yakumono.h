#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/gr/gr_seq_yakumono.h): the sound sequence banks and the
// two playSeSeq forms of sora_melee gr_seq_yakumono.o.

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>

class grSeqYakumono : public grYakumono {
public:
    // One sound of a sequence: a one-shot at m_startFrame when m_endFrame is 0, else held from m_startFrame to m_endFrame.
    struct SeSeq {
        u32 m_seIndex;     // the gimmick sound effect index
        float m_startFrame;
        float m_endFrame;
    };
    static_assert(sizeof(SeSeq) == 0xC, "Class is wrong size!");

    class SeSeqBank {
    public:
        SeSeq* m_seqs;
        u32 m_numSeqs;

        SeSeqBank() : m_seqs(NULL), m_numSeqs(0) { }
    };
    static_assert(sizeof(SeSeqBank) == 8, "Class is wrong size!");

protected:
    SeSeqBank* m_seSeqBanks; // 0x150
    u32 m_numSeSeqBanks;     // 0x154
    float m_lastFrame;       // 0x158, the frame playSeSeq last ran for (-1 before the first call)

public:
    grSeqYakumono(const char* taskName);
    void createSeSeqWork(int bankCount);
    void registSeSeq(u32 bank, SeSeq* seqs, u32 numSeqs);
    void playSeSeq(float frame, u32 bank);
    void playSeSeq(float frame, float lastFrame, u32 bank);

    virtual ~grSeqYakumono();
};
static_assert(sizeof(grSeqYakumono) == 348, "Class is wrong size!");
