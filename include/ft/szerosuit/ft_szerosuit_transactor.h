#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnSZerosuitParalyzer;
class wnSZerosuitParalyzerBullet;

// Zero Suit Samus's per-fighter helper as sora_melee defines it (the fighter unit keeps its own view in
// ft/szerosuit/ft_szerosuit.h): the paralyzer and its bullet, and the whip simple article of Kirby's copy.
class ftSZerosuitTransactor : public ftTransactor {
public:
    ftSZerosuitTransactor();
    virtual ~ftSZerosuitTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    bool activeArticle1(wnSZerosuitParalyzer* weapon, soModuleAccesser* acc, u32 mdlResId);
    bool activeArticle2(wnSZerosuitParalyzerBullet* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftSZerosuitTransactor) == 4, "Transactor singleton layout is wrong!");
