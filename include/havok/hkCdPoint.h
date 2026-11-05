#pragma once

#include <havok/hkContactPoint.h>

// HYPOTHESIS: link of a body chain attached to a collision point. The chain is walked through
// the next pointer at 0x0C; the 32-bit id at 0x04 is read from the head element.
struct hkCdPointChain {
    int unk00;               // 0x00
    int m_id;                // 0x04 HYPOTHESIS
    int unk08;               // 0x08
    hkCdPointChain* m_next;  // 0x0C
};

// Collision point handed to the collector callbacks: the contact (position, normal, distance in the normal's w)
// and the chains of the two bodies of the pair.
struct hkCdPoint {
    hkContactPoint m_contact; // 0x00..0x1F
    hkCdPointChain* m_chainA; // 0x20
    hkCdPointChain* m_chainB; // 0x24
};
