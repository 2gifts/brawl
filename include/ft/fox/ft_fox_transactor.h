#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnFoxBlaster;
class wnFoxBlasterBullet;

// Fox's per-fighter helper (the code is in sora_melee): starts the blaster article. Fox and Wolf have their own copies.
class ftFoxTransactor : public ftTransactor {
public:
    ftFoxTransactor();
    virtual ~ftFoxTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftFoxTransactor* getInstance();
    // HYPOTHESIS: the two ids are the model and the texture resource of the blaster; the flags are unknown.
    bool activeArticle1(wnFoxBlaster* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2);
    // HYPOTHESIS: starts a bullet; the kind (0x41) and the effect id (0x14000a) are the constants the mediator passes.
    bool activeArticle2(wnFoxBlasterBullet* weapon, soModuleAccesser* acc, int kind, u32 etcResId, u32 effectId);
};
static_assert(sizeof(ftFoxTransactor) == 4, "Transactor singleton layout is wrong!");
