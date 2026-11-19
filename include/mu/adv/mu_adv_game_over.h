#pragma once

// Local shadow of the BrawlHeaders file (this directory comes first in the include paths): utility::StandardMenu now
// declares its own members up to 0x1AC, so the padding before unk1AC is gone.

#include <StaticAssert.h>
#include <ad/ad_list.h>
#include <gf/gf_task.h>
#include <if/if_utility.h>
#include <sr/sr_common.h>
#include <types.h>

struct muAdvGameOverTaskParam {
    bool unk0;
    u32 unk4;
    adList<u32, Heaps::MenuInstance>* unk8;
    adList<u32, Heaps::MenuInstance>* unkC;
    adList<u32, Heaps::MenuInstance>* unk10;
    adList<u32, Heaps::MenuInstance>* unk14;
    adList<u32, Heaps::MenuInstance>* unk18;
    adList<u32, Heaps::MenuInstance>* unk1C;
};
static_assert(sizeof(muAdvGameOverTaskParam) == 0x20, "Size is the wrong size!");

class scResourceLoader;
class tyFigListDataManager;
class tySealListDataManager;

// The part of the object that the task of the cursor keeps that getSelection reads (HYPOTHESIS: the name of the object).
struct muAdvGameOverObj {
    u8 unk000[0x1AC];
    s32 unk1AC;                         // 0x1AC
    u8 unk1B0[4];
    s32 unk1B4;                         // 0x1B4
};

struct muAdvGameOverTask : public utility::StandardMenu {
    s32 unk1AC;
    u8 unk1B0;                          // 0x1B0
    u8 unk1B1[0x22C - 0x1B1];
    s32 unk22C;                         // 0x22C the entry the cursor is on
    s32 unk230;
    s32 unk234[18];                     // 0x234 what each entry does (1: continue, ...)
    tyFigListDataManager* unk27C;       // 0x27C
    tySealListDataManager* unk280;      // 0x280
    u8 unk284[0x2BC - 0x284];
    muAdvGameOverObj* unk2BC;           // 0x2BC
    u8 unk2C0[0x3F4 - 0x2C0];
    scResourceLoader* unk3F4;           // 0x3F4 reads the pack of the menu

    virtual ~muAdvGameOverTask();
    virtual bool preLoad();
    virtual void loadResource();
    virtual bool checkLoadFinish() const;
    virtual void createModel();
    virtual void update();

    s32 getSelection() const;

    static muAdvGameOverTask* create(const muAdvGameOverTaskParam* param);
};
static_assert(sizeof(muAdvGameOverTask) == 0x3F8, "Class is wrong size!");
