#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the fireball class infos fetch (getWeaponData89 for the Luigi kind);
// only passed as a pointer, the layout is unreconstructed (mirror of the blaster family's data role).
struct wnMarioFireballData;
struct wnArticleBuildInfo;

// Extent known from the Luigi fireball class info createArray allocation (0x1F44); the internal layout
// and the builder chain stay blob-provided until the constructor is reconstructed.
class wnMarioFireball : public Weapon {
    u8 m_storage[0x1F44 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class infos' createArray can construct articles.
    wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data);
    virtual ~wnMarioFireball();
};
static_assert(sizeof(wnMarioFireball) == 0x1F44, "Mario fireball article extent");
