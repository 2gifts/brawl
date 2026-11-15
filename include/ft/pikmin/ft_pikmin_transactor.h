#pragma once

#include <StaticAssert.h>
#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;

// Olimar's per-fighter helper (the code is in sora_melee): the start glow and the pull-out parameters.
class ftPikminTransactor : public ftTransactor {
public:
    // HYPOTHESIS: one row of the pull-out table, chosen by the floor material (0 is the default row).
    struct PullOutParam {
        int m_materialType;
        float m_motRate;
        char _8[0x14];
    };

    ftPikminTransactor();
    virtual ~ftPikminTransactor();
    virtual void init(soModuleAccesser* acc);
    virtual void exit(soModuleAccesser* acc);
    // Local declaration of the REL singleton-holder entry point.
    static ftPikminTransactor* getInstance();

    static float getPullOutMotRate(soModuleAccesser* acc);
    static int getPikminVariation(soModuleAccesser* acc);
    static PullOutParam* getPullOutParam(soModuleAccesser* acc) __attribute__((never_inline)); // BYTES: called, not inlined
    static PullOutParam* getPullOutParam(int materialType, PullOutParam* table, int count) __attribute__((never_inline));
};
static_assert(sizeof(ftPikminTransactor) == 4, "Transactor singleton layout is wrong!");
