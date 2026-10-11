#pragma once

#include <st_gw/gr_gw.h>

// A diver of the fire scene (three on each side): it is shown while the stage says that its kind of diver jumps, follows the
// place that the stage gives (the third place of the divers) and waves its hands (three hand nodes take turns).
class grGWFireDiver : public grGW {
protected:
    Vec3f* m_posWork;           // 0x174 the places of the divers (from the stage; the third is where the diver is)
    u8* m_typeWork;             // 0x178 the kind of diver that jumps (from the stage)
    u8* m_eventWork;            // 0x17C the event of the scene (from the stage)
    u8 m_type;                  // 0x180 the kind of diver that this ground is (10 - 15)
    char _181[3];
    u32 m_nodeHand[3];          // 0x184 the nodes of the hands
    u8 m_handIndex;             // 0x190 the hand that is shown
    u8 m_handDir;               // 0x191 0 = the hands go up, 1 = down

public:
    static grGWFireDiver* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    grGWFireDiver(const char* taskName);
    virtual ~grGWFireDiver();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setPosWork(Vec3f* pos);
    virtual void setTypeWork(u8* type);
    virtual void setEventWork(u8* event);
    virtual void setType(u8 type);
};
static_assert(sizeof(grGWFireDiver) == 0x194, "Class is wrong size!");
