#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnPokeZenigameWater;

// Zenigame's per-fighter helper (the code is in sora_melee; the class name is its RTTI string there).
class ftPokeZenigameTransactor : public ftTransactor {
public:
    ftPokeZenigameTransactor();
    virtual ~ftPokeZenigameTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    void initTransact(soModuleAccesser* moduleAccesser);
    void processUpdateTransact(soModuleAccesser* moduleAccesser);
    bool activeArticle1(wnPokeZenigameWater* weapon, soModuleAccesser* acc, u32 etcResId);
    // Local declaration of the REL singleton-holder entry point.
    static ftPokeZenigameTransactor* getInstance();
};
static_assert(sizeof(ftPokeZenigameTransactor) == 4, "Transactor singleton layout is wrong!");
