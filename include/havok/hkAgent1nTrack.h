#pragma once

#include <havok/hkArray.h>
#include <havok/hkMemory.h>
#include <havok/hkTransform.h>

struct hkCdBody;
struct hkContactMgr;
struct hkProcessCollisionInput;
struct hkShapeCollection;

// Track of the child agents of a 1-n agent (0x10 bytes), managed by the hkAgent1nMachine functions: a directory of
// agent sectors with room for one entry in place. The deleting destructor frees 0x10 bytes in class 0x25.
// HYPOTHESIS: the SectorDirEntry word is the sector pointer (map name hkArray<hkAgent1nTrack::SectorDirEntry>).
struct hkAgent1nTrack {
    HK_DECLARE_SIZED_DELETE(0x25)

    struct SectorDirEntry {
        void* m_sector;
    };
    hkInplaceArray<SectorDirEntry, 1> m_sectors; // 0x00

    ~hkAgent1nTrack() {}
    static void* operator new(unsigned long, void* where) { return where; }
};

// Input of the agent3 (stream) functions: the bodies, the collision input, the contact manager and A's transform in
// B's space.
struct hkAgent3Input {
    const hkCdBody* m_bodyA;                 // 0x00
    const hkCdBody* m_bodyB;                 // 0x04
    const hkProcessCollisionInput* m_input;  // 0x08
    hkContactMgr* m_contactMgr;              // 0x0C
    hkTransform m_aTb;                       // 0x10
};

// The process input adds the linear time-of-impact info of the step (0x70 bytes).
// HYPOTHESIS: unk50 (not written by hkBvTreeStreamAgent::processCollision).
struct hkAgent3ProcessInput : hkAgent3Input {
    hkVector4 unk50;           // 0x50
    hkVector4 m_linearTimInfo; // 0x60

    hkAgent3ProcessInput() {}
};

// HYPOTHESIS: hkAgent1nMachine_Process (map name): runs the track's agents on the sorted hit keys (all kept agents
// when hitList is null) and adds/removes agents for keys that appeared/disappeared.
extern "C" void fn_802FCCF0(hkAgent1nTrack* track, hkAgent3ProcessInput* input, const hkShapeCollection* collection,
                            const u32* hitList, int numHits, void* output);
