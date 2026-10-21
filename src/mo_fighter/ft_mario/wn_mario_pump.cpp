#include <types.h>
#include <ft/mario/ft_mario_article_pools.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern f32 lbl_91_rodata_40;

void wnMarioPump::activate(int founderTaskId, int modelResourceId, int team, int etcResourceId) {
    Vec3f origin(lbl_91_rodata_40, lbl_91_rodata_40, lbl_91_rodata_40);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = modelResourceId;
    desc.unk8 = modelResourceId;
    desc.unkC = modelResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: the original copies the position word by word instead of as three float stores.
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
    desc.lr = lbl_91_rodata_40;
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
    m_moduleAccesser->getLinkModule().setAttribute(3, (soLinkConnection::Attribute)4, true);
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
    m_moduleAccesser->getWorkManageModule().setInt(etcResourceId, 0x10000005);
}
