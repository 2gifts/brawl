#pragma once

#include <mt/mt_vector.h>
#include <snd/snd_id.h>
#include <st/se_util.h>
#include <st_gw/gr_gw.h>

// The alarm clock that rings when the time of a scene is nearly over (it shakes with its own sound).
class grGWAlarm : public grGW {
protected:
    float* m_frameWork;                         // 0x174 the frame of the scene (from the stage)
    StSeUtil::SeSeqInstance<1, 1> m_seSeq;      // 0x178 the ringing
    SndID m_seSeqId;                            // 0x1B4
    StSeUtil::UnkStruct m_seSeqData;            // 0x1B8

public:
    static grGWAlarm* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    grGWAlarm(const char* taskName);
    virtual ~grGWAlarm();
    virtual void update(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual float getMotionFrameMatCol();
    virtual void setFrameWork(float* frame);
};
static_assert(sizeof(grGWAlarm) == 0x1C8, "Class is wrong size!");
