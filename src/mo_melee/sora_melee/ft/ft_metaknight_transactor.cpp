#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/metaknight/ft_metaknight_transactor.h>
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

ftMetaknightTransactor::ftMetaknightTransactor() { }
ftMetaknightTransactor::~ftMetaknightTransactor() { }

// Kirby's copy of Meta Knight draws the sword: simple article 0xd with entry 11 of Kirby's simple-article data.
bool ftMetaknightTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnSimple* sword = dynamic_cast<wnSimple*>(article);
        if (sword == NULL) {
            return false;
        }
        Vec3f zero(0.0f, 0.0f, 0.0f);
        int founderTaskId = acc->getStageObject().m_taskId;
        soResourceModule& resource = acc->getResourceModule();
        const ftKirbySimpleDataEntry* table = getKirbySimpleDataTable();
        u32 resId = dynamic_cast<ftResourceIdAccesserImpl&>(*resource.getResourceIdAccesser()).getKirbyResId(Fighter_MetaKnight);
        sword->activate(founderTaskId, resId, reinterpret_cast<const wnSimpleData*>(&table[11]), false, false, zero,
            0xffff, 0xffff, 0, true, false, -1, 0, false, 2, 0, false);
        return true;
    }
    return false;
}
