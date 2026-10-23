#pragma once

#include <StaticAssert.h>
#include <st/loader/st_loader_info.h>
#include <types.h>

class stLoaderInfoStageEdit : public stLoaderInfo {
public:
    // HYPOTHESIS: flags passed to the stage-edit HUD's setVisible and setVisibleZoomHelp (main.dol, map), and
    // whether its model has been created.
    bool m_isVisible : 1;        // 0x64
    bool m_isModelCreated : 1;
    bool m_isVisibleZoomHelp : 1;

    stLoaderInfoStageEdit();

    virtual void processBegin();
    virtual ~stLoaderInfoStageEdit();
    virtual void entryEntity();
    virtual void removeEntity();
};
static_assert(sizeof(stLoaderInfoStageEdit) == 0x68, "Class is wrong size!");
