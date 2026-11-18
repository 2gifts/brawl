#pragma once

// Shadows include/Brawl/Include/gf/gf_3d_scene_event.h: the event list derives from utList (gfSceneRoot embeds it as a
// 0xC byte list at 0x468, its constructor clears the list and its notify walks the nodes from the head).

#include <StaticAssert.h>
#include <types.h>
#include <ut/ut_list.h>

class gfSceneRootEvent : public utListNode {
public:
    virtual void notify(s32 p1) = 0;
};

class gfSceneRootEventList : public utList {
public:
    gfSceneRootEventList() { clear(); }
    ~gfSceneRootEventList();
    void notify(s32 p1);
};
static_assert(sizeof(gfSceneRootEventList) == 0xC, "Class is the wrong size!");
