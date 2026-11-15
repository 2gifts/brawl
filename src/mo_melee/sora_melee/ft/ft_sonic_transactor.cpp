#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/sonic/ft_sonic_transactor.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_slow.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_weapon_builder.h>

struct wnSimpleData;

// The simple article (wn/wn_simple.h needs the builder chain): the members this unit uses.
class wnSimple : public Weapon {
public:
    void activate(s32 founderTaskId, s32 resourceId, const wnSimpleData* data, bool unk0, bool unk1,
                  const Vec3f& position, s32 unk2, s32 unk3, s32 unk4, bool unk5, bool unk6, s32 unk7, s32 unk8,
                  bool unk9, s32 unk10, s32 unk11, bool unk12);
};


// ftData::unk64 (declared only for the Kirby units) points at Kirby's table of simple-article data, 0x38-byte entries.
struct ftKirbySimpleDataEntry {
    u8 unk0[0x38];
};
static inline const ftKirbySimpleDataEntry* getKirbySimpleDataTable() {
    return **reinterpret_cast<ftKirbySimpleDataEntry** const*>(&g_ftCommonDataAccesser.getData(Fighter_Kirby)->unk58[0xC]);
}

ftSonicTransactor::ftSonicTransactor() { }
ftSonicTransactor::~ftSonicTransactor() { }

// Kirby's copy curls up with simple article 0xd (entry 12 of Kirby's simple-article data).
bool ftSonicTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnSimple* ball = dynamic_cast<wnSimple*>(article);
        if (ball == NULL) {
            return false;
        }
        Vec3f zero(0.0f, 0.0f, 0.0f);
        int founderTaskId = acc->getStageObject().m_taskId;
        soResourceModule& resource = acc->getResourceModule();
        const ftKirbySimpleDataEntry* table = getKirbySimpleDataTable();
        u32 resId = dynamic_cast<ftResourceIdAccesserImpl&>(*resource.getResourceIdAccesser()).getKirbyResId(Fighter_Sonic);
        ball->activate(founderTaskId, resId, reinterpret_cast<const wnSimpleData*>(&table[12]), false, false, zero,
            0xffff, 0xffff, 0, false, false, -1, 0, true, 2, 0, false);
        return true;
    }
    return false;
}

// Kirby's copy: the effects follow his visibility again after the spin statuses (0x139-0x13b), not during them.
void ftSonicTransactor::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
        switch (prevStatusKind) {
        case 0x139:
        case 0x13a:
        case 0x13b:
            moduleAccesser->getEffectModule().setSyncVisibility(true);
            break;
        }
        switch (statusKind) {
        case 0x139:
        case 0x13a:
        case 0x13b:
            moduleAccesser->getEffectModule().setSyncVisibility(false);
            break;
        }
    }
}

// Once per adjusted frame in status 0x112: clear work flag 0x22000010.
void ftSonicTransactor::processCollision(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isAdjust()) {
        if (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true) == 0x112) {
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        }
    }
}
