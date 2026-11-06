#pragma once

#include <wn/weapon.h>
#include <wn/wn_article_build_info.h>

struct wnYoshiWingData;

// Partial call-side view. Only the Weapon base, the activation interface and the extent that the article
// holders of ft_yoshi.cpp need are recovered; the full class lives in the wing unit.
class wnYoshiWing : public Weapon {
    u8 m_unreconstructed[0x2110 - sizeof(Weapon)];
public:
    wnYoshiWing(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiWingData* data);
    virtual ~wnYoshiWing();
    void activate(int founderTaskId, int resourceId, int team);
};
