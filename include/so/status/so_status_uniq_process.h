#pragma once

// The status process interface on its own, so that a status unit can define its class ahead of the module headers
// (class definition order decides the order of the vtables and RTTI a unit emits). soModuleAccesser must be declared
// by the includer: so_status_module_impl.h does it, and adding the declaration here would change what its other
// includers see.

#include <StaticAssert.h>
#include <types.h>

// MATCH-ONLY: selected original units own these shared virtual definitions.
#ifdef SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define SO_STATUS_UNIQ_PROCESS_BODY(body) ;
#else
#define SO_STATUS_UNIQ_PROCESS_BODY(body) { body }
#endif
class soStatusUniqProcess {
public:
    virtual ~soStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void execStatus(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void execStop(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void execFixPos(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual void execFixCamera(soModuleAccesser* moduleAccesser) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*) SO_STATUS_UNIQ_PROCESS_BODY(return false;)
    virtual void checkAttack(soModuleAccesser* moduleAccesser, void*, float) SO_STATUS_UNIQ_PROCESS_BODY()
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float) SO_STATUS_UNIQ_PROCESS_BODY(return false;)
    virtual void leaveStop(soModuleAccesser* moduleAccesser, int, bool) SO_STATUS_UNIQ_PROCESS_BODY()
    // HYPOTHESIS: (accesser, last transition info, target status); the BrawlHeaders copy has (accesser, int*).
    virtual bool checkTransitionPrecede(soModuleAccesser* moduleAccesser, void* transitionInfo, int target) SO_STATUS_UNIQ_PROCESS_BODY(return true;)
};
#undef SO_STATUS_UNIQ_PROCESS_BODY
static_assert(sizeof(soStatusUniqProcess) == 4, "Class is the wrong size!");
