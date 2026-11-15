#include <gr/gr_seq_yakumono.h>

// A hit object that plays its gimmick sounds along a motion: each bank is a list of sounds keyed to motion frames, and
// playSeSeq starts the one-shots the frame passed and holds the looped ones while the frame is inside their range.

grSeqYakumono::grSeqYakumono(const char* taskName) : grYakumono(taskName) {
    m_seSeqBanks = NULL;
    m_numSeSeqBanks = 0;
    m_lastFrame = -1.0f;
}

grSeqYakumono::~grSeqYakumono() {
    if (m_seSeqBanks != NULL) {
        delete[] m_seSeqBanks;
        m_seSeqBanks = NULL;
        m_numSeSeqBanks = 0;
    }
}

void grSeqYakumono::createSeSeqWork(int bankCount) {
    if (m_seSeqBanks != NULL) {
        delete[] m_seSeqBanks;
        m_seSeqBanks = NULL;
        m_numSeSeqBanks = 0;
    }
    m_numSeSeqBanks = bankCount;
    m_seSeqBanks = new (Heaps::StageInstance) SeSeqBank[bankCount];
}

void grSeqYakumono::registSeSeq(u32 bank, SeSeq* seqs, u32 numSeqs) {
    if (bank < m_numSeSeqBanks) {
        SeSeqBank* seqBank = &m_seSeqBanks[bank];
        seqBank->m_seqs = seqs;
        seqBank->m_numSeqs = numSeqs;
    }
}

void grSeqYakumono::playSeSeq(float frame, u32 bank) {
    if (m_numSeSeqBanks > bank && m_lastFrame != frame) {
        SeSeqBank* seqBank = &m_seSeqBanks[bank];
        SeSeq* seq = seqBank->m_seqs;
        if (seq != NULL) {
            u32 startMask = 0;
            u32 holdMask = 0;
            u32 loopMask = 0;
            for (u32 i = 0; i < seqBank->m_numSeqs; i++, seq++) {
                if (0.0f == seq->m_endFrame) {
                    float last = m_lastFrame;
                    float start = seq->m_startFrame;
                    bool passed;
                    if (last <= frame) {
                        passed = last < start && start <= frame;
                    } else {
                        passed = last < start || start <= frame;
                    }
                    if (passed) {
                        startMask |= 1 << seq->m_seIndex;
                    }
                } else {
                    float last = m_lastFrame;
                    float start = seq->m_startFrame;
                    float end = seq->m_endFrame;
                    bool inside;
                    if (last <= frame) {
                        inside = (last < start && start <= frame) || (last < end && end <= frame) || (start < last && last < end) ||
                                 (start <= frame && frame <= end);
                    } else {
                        inside = (start < last && last < end) || (start <= frame && frame <= end);
                    }
                    if (inside) {
                        holdMask |= 1 << seq->m_seIndex;
                    }
                    loopMask |= 1 << seq->m_seIndex;
                }
            }
            Effect_Info* effect = m_soundEffects;
            u32 i = 0;
            u32 bit = 1;
            u32 count = m_soundEffectNum;
            for (; i < count; i++, bit <<= 1, effect++) {
                if (loopMask & bit) {
                    if (holdMask & (1 << i)) {
                        if (effect->m_handleId == -1) {
                            startGimmickSE(i);
                        }
                    } else if (effect->m_handleId != -1) {
                        stopGimmickSE(i);
                    }
                } else if (startMask & bit) {
                    startGimmickSE(i);
                }
            }
            m_lastFrame = frame;
        }
    }
}

void grSeqYakumono::playSeSeq(float frame, float lastFrame, u32 bank) {
    m_lastFrame = lastFrame;
    playSeSeq(frame, bank);
}
