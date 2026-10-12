#pragma once

// Local view of the posture module for the shared status units. The shared header does not declare the
// virtuals used by these status processes, so the slots are named by their vtable offsets in the target
// (vtable offset = 0xc + 4 * p, where p is the 0-based position after the destructor).
class ftStatusPostureView {
public:
    virtual ~ftStatusPostureView() { }
    virtual void unk02();
    virtual void unk03();
    virtual void unk04();
    virtual void unk05();
    virtual void unk06();
    virtual void unk07();
    virtual void unk08();
    virtual void unk09();
    virtual float unk2c(); // vtable 0x2c: returns a float, used by the turn and fly statuses
    virtual void unk30(float); // vtable 0x30
    virtual void unk34();  // vtable 0x34
};
