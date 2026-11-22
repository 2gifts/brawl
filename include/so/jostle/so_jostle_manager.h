#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/ut/ut_list.h>
#include <so/jostle/so_jostle.h>
#include <types.h>

// Collects the jostle entries of every object each frame (soJostleModuleImpl::update adds them by layer) and pushes
// overlapping entries apart in processEffect.
class soJostleManager : public gfTask {
public:
    nw4r::ut::List m_checkList[4];
    nw4r::ut::List m_referList[4];
    s32 m_count;

    soJostleManager();
    virtual void processBegin();
    virtual void processEffect();
    virtual ~soJostleManager();
    void addCheckList(soJostle* jostle, s32 layer);
    void addReferList(soJostle* jostle, s32 layer);

    static soJostleManager* m_instance;
};
static_assert(sizeof(soJostleManager) == 0xA4, "Class is wrong size!");
