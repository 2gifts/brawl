#define FT_SNAKE_ARTICLE_POOLS
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <types.h>
#include <ft/snake/ft_snake_builder.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnSnakeC4Switch::activate(int founderTaskId, int etcResourceId, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: the native position copy is a word-wise memcpy, not a float assignment.
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}
