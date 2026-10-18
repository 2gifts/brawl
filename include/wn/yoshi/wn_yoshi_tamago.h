#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 60; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnYoshiTamagoData;
struct wnArticleBuildInfo;

// Partial parameter record reached through article data +0x34. Launch physics
// reads these floats; the first two words remain unreconstructed.
struct wnYoshiTamagoParam {
    u32 unk0;
    u32 unk4;
    float spinPercent;
    float gravity;
    float speedLimitY;
};

// Extent known from the class info createArray allocation (0x1FF8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnYoshiTamago : public Weapon {
    u8 m_storage[0x1FB8 - sizeof(Weapon)];
    const wnYoshiTamagoParam* m_param; // +0x1FB8, set by the owning constructor.
    u8 unk1FBC[0x1FF8 - 0x1FBC];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnYoshiTamago(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiTamagoData* data);
    virtual ~wnYoshiTamago();
    // captureNode is the captured fighter's corrected model node, or -1 when
    // this egg is being spawned for Egg Throw rather than around a victim.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position,
                  u32 captureNode, float lr);
    void shoot(const Vec3f* position, const Vec2f* speed, float spinFactor);
    virtual void onDeactivate();
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnYoshiTamago) == 0x1FF8, "Yoshi egg article extent");
