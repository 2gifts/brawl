#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnToonLinkBow;
class wnToonLinkBowArrow;

// ToonLink's per-fighter helper (the code is in sora_melee): starts and shoots the bow and its arrows. A singleton.
class ftToonLinkTransactor : public ftTransactor {
public:
    ftToonLinkTransactor();
    virtual ~ftToonLinkTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    virtual bool shootArticle(soArticle* article, soModuleAccesser* acc);
    // The bow activator and the arrow activator/shooter.
    // HYPOTHESIS: parameter types, from the callers in the fighter unit.
    bool activeArticle1(wnToonLinkBow* weapon, soModuleAccesser* acc, u32 etcResId);
    bool activeArticle2(wnToonLinkBowArrow* weapon, soModuleAccesser* acc, u32 etcResId, float lr);
    bool shootArticle1(wnToonLinkBowArrow* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftToonLinkTransactor) == 4, "Transactor singleton layout is wrong!");
