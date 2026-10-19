#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnPitBow;
class wnPitBowArrow;

// Pit's per-fighter helper (the code is in sora_melee): starts and shoots the arrows of the bow.
class ftPitTransactor : public ftTransactor {
public:
    ftPitTransactor();
    virtual ~ftPitTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftPitTransactor* getInstance();
    // HYPOTHESIS: the ids are the model and the texture resource of the bow; the flags are unknown.
    bool activeArticle(wnPitBow* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2);
    bool activeArticle1(wnPitBowArrow* weapon, soModuleAccesser* acc, u32 etcResId);
    bool shootArticle(wnPitBowArrow* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftPitTransactor) == 4, "Transactor singleton layout is wrong!");
