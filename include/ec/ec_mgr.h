// Local BrawlHeaders shadow: ecMgr with typed trace storage and the effect-group calls.
#pragma once

#include <StaticAssert.h>
#include <ec/ec_trace_mgr.h>
#include <ef/ef_id.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <so/ground/so_ground_module_impl.h>
#include <types.h>

class ecMgr;
extern ecMgr* g_ecMgr;
class ecMgr : public gfTask {
public:
    u8 unk40[0x50];
    u32 unk90;
    u32 unk94;
    union {
        u8 unk98[0x10];
        ecTraceMgr m_traceMgr;
    };

    ecMgr();
    void setDrawPrio(int prio);
    u32 setEffect(EfID effectID);
    u32 setEffect(EfID effectID, Vec3f* pos);
    u32 setEffect(EfID effectID, const Vec3f* pos, const Vec3f* rot, const Vec3f* scale);
    void endEffect(u32);
    void killEffect(u32, u32, u32);
    void setParent(u32 handleId, nw4r::g3d::ScnMdl*, const char*, bool);
    void setParent(u32 handleId, nw4r::g3d::ScnMdl*, u32, bool);
    void setParent(u32 handleId1, u32 handleId2, const char*, bool);
    void pushsetCurrentGroup(u32 p1);
    void popCurrentGroup();
    // HYPOTHESIS: parameter types from argument registers and effect call sites.
    void preSet2dScene(bool);
    void pauseEffectGroup(u32, u32);
    void playEffectGroup(u32, u32);
    void preSetAnmIdx(int chrIndex, int visIndex, int patIndex, int srtIndex, int clrIndex, int shpIndex);
    void setGroundAttach(u32, soGroundModule* groundModule);
    void setGroundAttach(u32, grCollisionLineWork* lineWork);
    void setVisible(u32, bool isVisible);
    void setVisibleGroup(u32, bool isVisible, u32, u32);
    void setPos(u32, Vec3f* pos);
    void setRot(u32, Vec3f* rot);
    void setScl(u32, Vec3f* scale);
    void setMdlTexpat(u32, u32);
    void setMdlAnmIdx(u32, u32, s8);
    void setMdlEnableUpdate(u32, bool, s8);
    void setMdlFrame(u32, float, s8);
    void setSlowRate(u32 effectHandleID, u32 slowMagnitude);
    u32 searchResourceID(char* resourceName);
    virtual ~ecMgr();

    inline static ecMgr* getInst() __attribute__((never_inline));
    inline static ecMgr* getInstance() { return g_ecMgr; }

};
static_assert(sizeof(ecMgr) == 0xa8, "Class is wrong size!");


inline ecMgr* ecMgr::getInst() { return g_ecMgr; }
