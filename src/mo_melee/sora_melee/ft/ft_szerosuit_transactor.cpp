#include <StaticAssert.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/szerosuit/ft_szerosuit_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/zelda/wn_szerosuit_paralyzer.h>
#include <wn/zelda/wn_szerosuit_paralyzer_bullet.h>
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

ftSZerosuitTransactor::ftSZerosuitTransactor() { }
ftSZerosuitTransactor::~ftSZerosuitTransactor() { }

bool ftSZerosuitTransactor::activeArticle1(wnSZerosuitParalyzer* weapon, soModuleAccesser* acc, u32 mdlResId) {
    acc->getStageObject().soGetSubKind(); // HYPOTHESIS: a Kirby check whose result is unused.
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, mdlResId, acc->getTeamModule().getTeam()->getNo());
    return true;
}

// The bullet leaves the paralyzer node (Kirby's own node for the copy), pushed forward by the muzzle offset; work flag
// 0x22000011 marks a fully charged shot.
bool ftSZerosuitTransactor::activeArticle2(wnSZerosuitParalyzerBullet* weapon, soModuleAccesser* acc) {
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(acc->getStageObject().soGetSubKind() == Fighter_Kirby ? 0x1b8 : 0x36, false);
    float lr = acc->getPostureModule().getLr();
    float offsetX = acc->getConstantFloatKirby(0xfa0);
    float offsetY = acc->getConstantFloatKirby(0xfa1);
    pos.m_x += offsetX * lr;
    pos.m_y += offsetY;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), pos,
                     acc->getWorkManageModule().isFlag(0x22000011), lr);
    return false;
}

// Kirby's copy whips with simple article 0xd (entry 4 of Kirby's simple-article data); 0xe is the paralyzer bullet.
bool ftSZerosuitTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        ftKirbySimpleDataEntry* const* tableRef =
            *reinterpret_cast<ftKirbySimpleDataEntry* const* const*>(&g_ftCommonDataAccesser.getData(Fighter_Kirby)->unk58[0xC]);
        Vec3f zero(0.0f, 0.0f, 0.0f);
        int founderTaskId = acc->getStageObject().m_taskId;
        soResourceModule& resource = acc->getResourceModule();
        u32 resId = dynamic_cast<ftResourceIdAccesserImpl&>(*resource.getResourceIdAccesser()).getKirbyResId(Fighter_SZeroSuit);
        dynamic_cast<wnSimple&>(*article).activate(founderTaskId, resId, reinterpret_cast<const wnSimpleData*>(&(*tableRef)[4]), true, false, zero,
            0xffff, 0xffff, 0, true, false, -1, 0, false, 2, 0, false);
        return true;
    }
    case 0xe:
        return activeArticle2(dynamic_cast<wnSZerosuitParalyzerBullet*>(article), acc);
    default:
        return false;
    }
}
