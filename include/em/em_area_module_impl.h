#pragma once

// SHADOW of BrawlHeaders em/em_area_module_impl.h (-I include comes first): adds the members after soAreaModuleImpl (the
// class is 0x5D8 bytes, embedded in Enemy) and the constructor the Enemy constructor calls (fn_41_22EE8).

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/area/so_area_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class emAreaModuleImpl : public soAreaModuleImpl {
    soModuleAccesser* m_moduleAccesser;                 // 0x4C
    void* m_areaParams;                                 // 0x50, emInfo::getAreaParamAryPtrGetId
    void* m_areaWork;                                   // 0x54, the derived enemy's area work
    int m_areaNum;                                      // 0x58, emInfo::getAreaNum
    void* unk5C;                                        // 0x5C, a soValueAccesser constant
    soArrayVector<soAreaContactLog, 44> m_contactLogs;  // 0x60
    soAreaEnviromentElementCheckerImpl m_envChecker;    // 0x48C
    // HYPOTHESIS: sixteen 12-byte entries (a constructor loop without a body walks them), then unknown work.
    u8 unk4A8[0x5D8 - 0x4A8];

public:
    // HYPOTHESIS: wind is the shared null area object of sora_melee the derived enemies pass on.
    emAreaModuleImpl(soModuleAccesser* acc, int areaNum, void* areaParams, void* unk5C, soArray<soAreaInstance>* instances,
                     void* areaWork, void* wind);
    virtual ~emAreaModuleImpl();

    static s32 getTargetFoundDistance(soModuleAccesser* acc, s32 p2);
    static bool isTargetFoundDir(soModuleAccesser* acc, s32 p2, s32 p3);
    static Vec3f getTargetPos(soModuleAccesser* acc, u32 p2, bool p3);
    static bool isExistTarget(soModuleAccesser* acc, u32 p2);

    bool checkArea(soModuleAccesser* acc, u32 p2, u32 p3);
    void createAreaDataSet();
    void createAreaInstance();
};
static_assert(sizeof(emAreaModuleImpl) == 0x5D8, "Class is wrong size!");
