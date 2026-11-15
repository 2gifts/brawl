#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/snake/ft_snake_transactor.h>
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

// TODO: shootArticle (the grenade throw) and notifyArticleEventEject.

ftSnakeTransactor::ftSnakeTransactor() { }
ftSnakeTransactor::~ftSnakeTransactor() { }

bool ftSnakeTransactor::activeArticle1(wnSimple* article, soModuleAccesser* acc, u32 resId, const wnSimpleData* data) {
    article->activate(acc->getStageObject().m_taskId, resId, data, true, false, Vec3f(0.0f, 0.0f, 0.0f),
        0xffff, 0xffff, 0, true, false, -1, 0, false, 2, 0, false);
    return true;
}

// Kirby's copy hides in the box: simple article 0xd with entry 9 of Kirby's simple-article data.
bool ftSnakeTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnSimple* box = dynamic_cast<wnSimple*>(article);
        if (box == NULL) {
            return false;
        }
        const ftKirbySimpleDataEntry* table = getKirbySimpleDataTable();
        const wnSimpleData* data = reinterpret_cast<const wnSimpleData*>(&table[9]);
        u32 resId = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser()).getKirbyResId(Fighter_Snake);
        return activeArticle1(box, acc, resId, data);
    }
    return false;
}
