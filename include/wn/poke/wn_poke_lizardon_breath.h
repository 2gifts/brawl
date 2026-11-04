#pragma once

#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything

struct wnPokeLizardonBreathData;

// The flame breath of Lizardon. Opaque shell: its fields are not reconstructed yet.
class wnPokeLizardonBreath : public Weapon {
    u8 m_unreconstructed[0x164C - sizeof(Weapon)];
public:
    wnPokeLizardonBreath(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonBreathData* data);
    virtual ~wnPokeLizardonBreath();
    virtual void updateNodeSRT();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
};
static_assert(sizeof(wnPokeLizardonBreath) == 0x164C, "wnPokeLizardonBreath layout is wrong!");
