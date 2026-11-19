#include <types.h>
#include <ft/robot/ft_robot_article_pools.h>
#include <ft/ft_slot_manager.h>
#include <so/so_archive_db.h>

#include <ft/ft_common_data_accesser.h>
#include <ft/builder/ft_builder_kinetic.h>

extern "C" s32 fn_27_1454C0();
extern "C" u32 fn_27_12006C(ftSlot* slot, int type, ftKind kind, bool flag);
extern "C" u32 fn_27_120218(ftSlot* slot, ftKind kind, int copyKind);
extern "C" u32 fn_27_122218(ftSlotManager* slotManager, ftKind kind, u8 colorNo);
extern "C" u32 fn_27_122270(ftSlotManager* slotManager, ftKind kind, u8 colorNo);
extern "C" s32 fn_27_1222C8(ftSlotManager* slotManager, s32 slot);
extern "C" void fn_27_145AA0(ftCommonData* commonData);
// HYPOTHESIS: 12-byte ftKindData entries (kind * 0xC), one per ftKind
extern "C" ftKindData lbl_27_bss_3210[];
extern "C" char lbl_27_data_2BBD8[];
extern "C" wnSimpleData lbl_27_bss_4530;

ftCommonDataAccesser g_ftCommonDataAccesser;

// HYPOTHESIS: public address record whose pointer at +0x10 is the article data
struct ftRobotBeamArticle {
    u8 unk0[0x10];
    void* unk10;
    void* unk14; // HYPOTHESIS: second article pointer of the public address record
};

// HYPOTHESIS: ftData continues past extendParam; the pointer read for kind 5 sits at 0x10C
struct ftDataExtended {
    u8 unk0[0x10C];
    void* unk10C;
};

ftData* ftCommonDataAccesser::getData(ftKind kind) const
{
    return lbl_27_bss_3210[kind].data;
}

ftParam* ftCommonDataAccesser::getParamCommon() const
{
    // HYPOTHESIS: the common attribute block is returned through the ftParam view its callers cast from.
    return reinterpret_cast<ftParam*>(g_ftCommonData.commonParams[fn_27_1454C0()]);
}

ftParam* ftCommonDataAccesser::getParam(ftKind kind) const
{
    s32 index = fn_27_1454C0();
    return lbl_27_bss_3210[kind].params[index];
}

ftParam* ftCommonDataAccesser::getParam(ftKind kind, int variation) const
{
    return lbl_27_bss_3210[kind].params[variation];
}

ftParamEtc* ftCommonDataAccesser::getParamEtc(ftKind kind) const
{
    return lbl_27_bss_3210[kind].ParamEtc;
}

float ftCommonDataAccesser::getParamCommonFloat(u32 variation, u32 id) const
{
    return g_ftCommonData.commonFloatParams[variation].m_0[id - 3117];
}

s32 ftCommonDataAccesser::getParamCommonInt(u32 variation, u32 id) const
{
    return g_ftCommonData.commonIntParams[variation].m_0[id - 23030];
}

s32* ftCommonDataAccesser::getParamCommonIndefinite(u32 variation, u32 id) const
{
    return g_ftCommonData.commonIndefiniteParams[variation].m_0[id - 43026];
}

void ftCommonDataAccesser::setupCommon(ftDataCommon* dataCommon)
{
    g_ftCommonData.dataCommon = dataCommon;
    g_ftCommonData.commonParams = dataCommon->ftCommonParams;
    fn_27_145AA0(&g_ftCommonData);
}

bool ftCommonDataAccesser::isValidDat(ftKind kind)
{
    return lbl_27_bss_3210[kind].data != NULL;
}

u32 ftCommonDataAccesser::getResId(ftKind kind) const
{
    return g_ftSlotManager->m_resId[kind];
}

u32 ftCommonDataAccesser::getFinalResId(ftKind kind) const
{
    return g_ftSlotManager->m_finalResId[kind];
}

u32 ftCommonDataAccesser::getMotionResId(int slotNo, ftKind kind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 6, Fighter_Wario, false);
    }
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 6, kind, false);
}

u32 ftCommonDataAccesser::getModelResId(int slotNo, ftKind kind, bool flag) const
{
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 8, kind, flag);
}

u32 ftCommonDataAccesser::getEtcResId(int slotNo, ftKind kind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 5, Fighter_Wario, false);
    }
    if (kind == Fighter_GKoopa) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 5, Fighter_Koopa, false);
    }
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 5, kind, false);
}

u32 ftCommonDataAccesser::getMotionEtcResId(int slotNo, ftKind kind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 7, Fighter_Wario, false);
    }
    if (kind == Fighter_GKoopa) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 7, Fighter_Koopa, false);
    }
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 7, kind, false);
}

u32 ftCommonDataAccesser::getEntryResId(ftKind kind, u8 colorNo) const
{
    return fn_27_122218(g_ftSlotManager, kind, colorNo);
}

u32 ftCommonDataAccesser::getResultResId(ftKind kind, u8 colorNo) const
{
    return fn_27_122270(g_ftSlotManager, kind, colorNo);
}

u32 ftCommonDataAccesser::getKirbyResId(int slotNo, ftKind kind, int copyKind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_120218(&g_ftSlotManager->m_slots[slotNo], Fighter_Wario, copyKind);
    }
    if (kind == Fighter_GKoopa) {
        return fn_27_120218(&g_ftSlotManager->m_slots[slotNo], Fighter_Koopa, copyKind);
    }
    return fn_27_120218(&g_ftSlotManager->m_slots[slotNo], kind, copyKind);
}

void* ftCommonDataAccesser::getKirbyBinData(ftKind kind) const
{
    u32 slot = fn_27_1222C8(g_ftSlotManager, kind);
    return soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
}

void ftCommonDataAccesser::reset(ftKind kind)
{
    lbl_27_bss_3210[kind].data = NULL;
    lbl_27_bss_3210[kind].params = NULL;
    lbl_27_bss_3210[kind].ParamEtc = NULL;
}

void ftCommonDataAccesser::reset()
{
    g_ftCommonData.dataCommon = NULL;
    g_ftCommonData.commonParams = NULL;
    for (int i = 0; i < Fighter_Count; i++) {
        reset(static_cast<ftKind>(i));
    }
}

void* ftCommonDataAccesser::getKirbyStoneData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x134)
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[46];
}

void* ftCommonDataAccesser::getPikachuThunderData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[7];
}

void* ftCommonDataAccesser::getPokeLizardonBreathParam(ftKind kind) const
{
    switch (kind) {
    case Fighter_PokeLizardon:
        return lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[1];
    case Fighter_Kirby:
        // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[12];
    default:
        return NULL;
    }
}

template <>
const wnPokeLizardonRockData* ftCommonDataAccesser::getWeaponData<wnPokeLizardonRockData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnPokeLizardonRockData*>(lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[6]);
}

template <>
const wnPokeLizardonRockStoneData* ftCommonDataAccesser::getWeaponData<wnPokeLizardonRockStoneData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnPokeLizardonRockStoneData*>(lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[7]);
}

void* ftRobotArticleDataAccesser::getGyroData(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return lbl_27_bss_3210[Fighter_Robot].data->extendParam[6];
}

void* ftRobotArticleDataAccesser::getBeamData(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_Robot:
        // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
        return lbl_27_bss_3210[Fighter_Robot].data->extendParam[8];
    case 5: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Robot);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    default:
        return NULL;
    }
}

void* ftRobotArticleDataAccesser::getGyroHolderData(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return lbl_27_bss_3210[Fighter_Robot].data->extendParam[10];
}

void* ftRobotArticleDataAccesser::getFinalBeamData(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return lbl_27_bss_3210[Fighter_Robot].data->extendParam[12];
}

void* ftCommonDataAccesser::getSZerosuitArmorData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[7];
}

template <>
const wnSZerosuitArmorData* ftCommonDataAccesser::getWeaponData<wnSZerosuitArmorData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnSZerosuitArmorData*>(lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[7]);
}

void* ftCommonDataAccesser::getWeaponData0(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[42];
}

void* ftCommonDataAccesser::getWeaponData1(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[43];
}

void* ftCommonDataAccesser::getWeaponData2(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[45];
}

void* ftCommonDataAccesser::getWeaponData3(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[46];
}

void* ftCommonDataAccesser::getWeaponData4(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[47];
}

void* ftCommonDataAccesser::getWeaponData5(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[48];
}

void* ftCommonDataAccesser::getWeaponData14(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Link].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData15(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_Link:
        return lbl_27_bss_3210[Fighter_Link].data->extendParam[16];
    default:
        return NULL;
    }
}

template <>
const wnSnakeMineData* ftCommonDataAccesser::getWeaponData<wnSnakeMineData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnSnakeMineData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[5]);
}

template <>
const wnSnakeTrenchMortarData* ftCommonDataAccesser::getWeaponData<wnSnakeTrenchMortarData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnSnakeTrenchMortarData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[6]);
}

void* ftCommonDataAccesser::getWeaponData18(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Snake].data->extendParam[7];
}

template <>
const wnSnakeRpg7Data* ftCommonDataAccesser::getWeaponData<wnSnakeRpg7Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnSnakeRpg7Data*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[8]);
}

template <>
const wnSnakeCypherData* ftCommonDataAccesser::getWeaponData<wnSnakeCypherData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnSnakeCypherData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[9]);
}

template <>
const wnSnakeC4Data* ftCommonDataAccesser::getWeaponData<wnSnakeC4Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnSnakeC4Data*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[10]);
}

template <>
const wnSnakeC4SwitchData* ftCommonDataAccesser::getWeaponData<wnSnakeC4SwitchData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return static_cast<const wnSnakeC4SwitchData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[11]);
}

template <>
const wnSnakeNikitaData* ftCommonDataAccesser::getWeaponData<wnSnakeNikitaData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnSnakeNikitaData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[12]);
}

void* ftCommonDataAccesser::getWeaponData24(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Snake].data->extendParam[13];
}

template <>
const wnSnakeLadderData* ftCommonDataAccesser::getWeaponData<wnSnakeLadderData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB4)
    return static_cast<const wnSnakeLadderData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[14]);
}

template <>
const wnSnakeRgb6Data* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB8)
    return static_cast<const wnSnakeRgb6Data*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[15]);
}

template <>
const wnSnakeRgb6SightData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6SightData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xBC)
    return static_cast<const wnSnakeRgb6SightData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[16]);
}

template <>
const wnSnakeRgb6ClipData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6ClipData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xC0)
    return static_cast<const wnSnakeRgb6ClipData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[17]);
}

template <>
const wnSnakeRgb6BulletData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6BulletData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xC4)
    return static_cast<const wnSnakeRgb6BulletData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[18]);
}

void* ftCommonDataAccesser::getWeaponData32(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[7];
}

template <>
const wnPikachuCloudData* ftCommonDataAccesser::getWeaponData<wnPikachuCloudData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnPikachuCloudData*>(lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[8]);
}

template <>
const wnPikachuVortexData* ftCommonDataAccesser::getWeaponData<wnPikachuVortexData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnPikachuVortexData*>(lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[9]);
}

void* ftCommonDataAccesser::getWeaponData38(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeFushigisou].data->extendParam[4];
}

void* ftCommonDataAccesser::getWeaponData40(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeFushigisou].data->extendParam[6];
}

void* ftCommonDataAccesser::getWeaponData42(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeTrainer].data->extendParam[4];
}

void* ftCommonDataAccesser::getWeaponData43(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeTrainer].data->extendParam[5];
}

template <>
const wnZeldaDeinData* ftCommonDataAccesser::getWeaponData<wnZeldaDeinData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnZeldaDeinData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[6]);
}

template <>
const wnZeldaDeinSData* ftCommonDataAccesser::getWeaponData<wnZeldaDeinSData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnZeldaDeinSData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[7]);
}

template <>
const wnZeldaLightingBowData* ftCommonDataAccesser::getWeaponData<wnZeldaLightingBowData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnZeldaLightingBowData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[8]);
}

template <>
const wnZeldaLightingBowArrowData* ftCommonDataAccesser::getWeaponData<wnZeldaLightingBowArrowData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnZeldaLightingBowArrowData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[9]);
}

template <>
const wnFoxIllusionData* ftCommonDataAccesser::getWeaponData<wnFoxIllusionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnFoxIllusionData*>(lbl_27_bss_3210[Fighter_Fox].data->extendParam[10]);
}

void* ftCommonDataAccesser::getWeaponData91(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case 26:
        return lbl_27_bss_3210[26].data->extendParam[7];
    case 5: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, 26);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getWeaponData92(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[8];
}

void* ftCommonDataAccesser::getWeaponData93(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[9];
}

void* ftCommonDataAccesser::getWeaponData94(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[10];
}

void* ftCommonDataAccesser::getWeaponData95(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData96(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[12];
}

void* ftCommonDataAccesser::getParamSamusChargeShot(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[0];
    case 5:
        return reinterpret_cast<ftDataExtended*>(lbl_27_bss_3210[5].data)->unk10C;
    default:
    case 4:
        return NULL;
    }
}

void* ftCommonDataAccesser::getParamSamusSpecialLw(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[2];
    default:
        return NULL;
    }
}

template <>
const wnSamusBombData* ftCommonDataAccesser::getWeaponData<wnSamusBombData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnSamusBombData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[10]);
}

void* ftCommonDataAccesser::getParamSamusSpecialS(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[3];
    default:
        return NULL;
    }
}

template <>
const wnSamusMissileData* ftCommonDataAccesser::getWeaponData<wnSamusMissileData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnSamusMissileData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[12]);
}

template <>
const wnSamusSuperMissileData* ftCommonDataAccesser::getWeaponData<wnSamusSuperMissileData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xCC)
    return static_cast<const wnSamusSuperMissileData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[20]);
}

void* ftCommonDataAccesser::getParamSamusFinal(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[4];
    default:
        return NULL;
    }
}

template <>
const wnSamusLaserData* ftCommonDataAccesser::getWeaponData<wnSamusLaserData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB4)
    return static_cast<const wnSamusLaserData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[14]);
}

template <>
const wnSamusLaser2Data* ftCommonDataAccesser::getWeaponData<wnSamusLaser2Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xBC)
    return static_cast<const wnSamusLaser2Data*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[16]);
}

template <>
const wnSamusGunData* ftCommonDataAccesser::getWeaponData<wnSamusGunData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xC4)
    return static_cast<const wnSamusGunData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[18]);
}

void* ftCommonDataAccesser::getWeaponData121(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_Ness:
        return lbl_27_bss_3210[Fighter_Ness].data->extendParam[7];
    case 5: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Ness);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getWeaponData122(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[8];
}

void* ftCommonDataAccesser::getWeaponData123(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[9];
}

void* ftCommonDataAccesser::getWeaponData124(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[10];
}

void* ftCommonDataAccesser::getWeaponData125(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData126(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[12];
}

void* ftCommonDataAccesser::getWeaponData127(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB0)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[13];
}

void* ftCommonDataAccesser::getWeaponData129(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return lbl_27_bss_3210[kind].data->extendParam[6];
}

void* ftCommonDataAccesser::getWeaponData130(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return lbl_27_bss_3210[kind].data->extendParam[7];
}

void* ftCommonDataAccesser::getKoopaBreathParam(ftKind kind) const
{
    switch (kind) {
    case Fighter_Koopa:
        return lbl_27_bss_3210[Fighter_Koopa].data->extendParam[0];
    case Fighter_Kirby:
        // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x110)
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[37];
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getGKoopaBreathParam(ftKind kind) const
{
    switch (kind) {
    case Fighter_GKoopa:
        return lbl_27_bss_3210[Fighter_GKoopa].data->extendParam[1];
    default:
        return NULL;
    }
}

template <>
const wnDonkeyBarrelCongaData* ftCommonDataAccesser::getWeaponData<wnDonkeyBarrelCongaData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnDonkeyBarrelCongaData*>(lbl_27_bss_3210[Fighter_Donkey].data->extendParam[5]);
}

template <>
const wnDonkeySoundEffectData* ftCommonDataAccesser::getWeaponData<wnDonkeySoundEffectData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnDonkeySoundEffectData*>(lbl_27_bss_3210[Fighter_Donkey].data->extendParam[6]);
}

template <>
const wnGameWatchRescueData* ftCommonDataAccesser::getWeaponData<wnGameWatchRescueData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnGameWatchRescueData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[7]);
}

template <>
const wnGameWatchParachuteData* ftCommonDataAccesser::getWeaponData<wnGameWatchParachuteData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnGameWatchParachuteData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[8]);
}

template <>
const wnGameWatchPanelData* ftCommonDataAccesser::getWeaponData<wnGameWatchPanelData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnGameWatchPanelData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[9]);
}

template <>
const wnGameWatchOilData* ftCommonDataAccesser::getWeaponData<wnGameWatchOilData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnGameWatchOilData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[10]);
}

template <>
const wnGameWatchOctopusData* ftCommonDataAccesser::getWeaponData<wnGameWatchOctopusData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return static_cast<const wnGameWatchOctopusData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[11]);
}

const wnGameWatchNormalWeaponData* ftCommonDataAccesser::getGameWatchNormalWeaponData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnGameWatchNormalWeaponData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[12]);
}

template <>
const wnGameWatchNormalWeaponData* ftCommonDataAccesser::getWeaponData<wnGameWatchNormalWeaponData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnGameWatchNormalWeaponData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[12]);
}

void* ftCommonDataAccesser::getGanonBeastData() const
{
    return lbl_27_bss_3210[Fighter_Ganon].data->extendParam[4];
}

wnSimpleData* ftCommonDataAccesser::getSimpleData(ftKind kind, bool* resourceGroup) const
{
    return &lbl_27_bss_4530;
}

ftCommonDataAccesser::~ftCommonDataAccesser() { }

extern "C" const wnDummyFighterData lbl_27_rodata_14A4;

const wnDummyFighterData* ftCommonDataAccesser::getDummyFighterData() const
{
    return &lbl_27_rodata_14A4;
}

template <>
const wnDummyFighterData* ftCommonDataAccesser::getWeaponData<wnDummyFighterData>(int kind, bool* resourceGroup) const
{
    return &lbl_27_rodata_14A4;
}

template <>
const wnKirbyPikminData* ftCommonDataAccesser::getWeaponData<wnKirbyPikminData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnKirbyPikminData*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[61]);
}

template <>
const wnMarioPumpWaterData* ftCommonDataAccesser::getWeaponData<wnMarioPumpWaterData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnMarioPumpWaterData*>(lbl_27_bss_3210[kind].data->extendParam[8]);
}

const ftLucarioAuraBallParam* ftCommonDataAccesser::getLucarioAuraBallParam(int kind) const
{
    switch (kind) {
    case Fighter_Lucario:
    default:
        return static_cast<const ftLucarioAuraBallParam*>(lbl_27_bss_3210[Fighter_Lucario].data->extendParam[0]);
    case Fighter_Kirby:
        return static_cast<const ftLucarioAuraBallParam*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[35]);
    }
}

template <>
const wnLucarioQigongData* ftCommonDataAccesser::getWeaponData<wnLucarioQigongData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnLucarioQigongData*>(lbl_27_bss_3210[Fighter_Lucario].data->extendParam[10]);
}

float* ftCommonDataAccesser::getLinkBowParam(int kind) const
{
    switch (kind) {
    case Fighter_Link:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_Link].data->extendParam[0]);
    case Fighter_Kirby:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[33]);
    default:
        return NULL;
    }
}

template <>
const wnPokeFushigisouSeedData* ftCommonDataAccesser::getWeaponData<wnPokeFushigisouSeedData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_PokeFushigisou:
        return static_cast<const wnPokeFushigisouSeedData*>(lbl_27_bss_3210[Fighter_PokeFushigisou].data->extendParam[5]);
    case Fighter_Kirby:
        return static_cast<const wnPokeFushigisouSeedData*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[54]);
    default:
        return NULL;
    }
}

template <>
const wnPokeZenigameWaterData* ftCommonDataAccesser::getWeaponData<wnPokeZenigameWaterData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_PokeZenigame:
        return static_cast<const wnPokeZenigameWaterData*>(lbl_27_bss_3210[Fighter_PokeZenigame].data->extendParam[5]);
    case Fighter_Kirby:
        return static_cast<const wnPokeZenigameWaterData*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[55]);
    default:
        return NULL;
    }
}

float* ftCommonDataAccesser::getToonLinkBowParam(int kind) const
{
    switch (kind) {
    case Fighter_ToonLink:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[0]);
    case Fighter_Kirby:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[34]);
    default:
        return NULL;
    }
}

template <>
const wnFoxLandMasterShellData* ftCommonDataAccesser::getWeaponData<wnFoxLandMasterShellData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case 6:
        return static_cast<const wnFoxLandMasterShellData*>(lbl_27_bss_3210[6].data->extendParam[12]);
    case 19:
        return static_cast<const wnFoxLandMasterShellData*>(lbl_27_bss_3210[19].data->extendParam[10]);
    case 44:
        return static_cast<const wnFoxLandMasterShellData*>(lbl_27_bss_3210[44].data->extendParam[12]);
    default:
        return NULL;
    }
}

template <>
const wnFalcoIllusionData* ftCommonDataAccesser::getWeaponData<wnFalcoIllusionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnFalcoIllusionData*>(lbl_27_bss_3210[Fighter_Falco].data->extendParam[8]);
}

template <>
const wnWolfIllusionData* ftCommonDataAccesser::getWeaponData<wnWolfIllusionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnWolfIllusionData*>(lbl_27_bss_3210[Fighter_Wolf].data->extendParam[10]);
}

template <>
const wnDededeStarData* ftCommonDataAccesser::getWeaponData<wnDededeStarData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnDededeStarData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[8]);
}

template <>
const wnDededeJethammerData* ftCommonDataAccesser::getWeaponData<wnDededeJethammerData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnDededeJethammerData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[10]);
}

template <>
const wnDededeWaddledooBeamData* ftCommonDataAccesser::getWeaponData<wnDededeWaddledooBeamData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnDededeWaddledooBeamData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[6]);
}

template <>
const wnDededeFollowerData* ftCommonDataAccesser::getWeaponData<wnDededeFollowerData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB4)
    return static_cast<const wnDededeFollowerData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[14]);
}

template <>
const wnDiddyBarrelData* ftCommonDataAccesser::getWeaponData<wnDiddyBarrelData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnDiddyBarrelData*>(lbl_27_bss_3210[Fighter_Diddy].data->extendParam[8]);
}

template <>
const wnDiddyPeanutsData* ftCommonDataAccesser::getWeaponData<wnDiddyPeanutsData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnDiddyPeanutsData*>(lbl_27_bss_3210[Fighter_Diddy].data->extendParam[10]);
}

template <>
const wnDiddyExplosionData* ftCommonDataAccesser::getWeaponData<wnDiddyExplosionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnDiddyExplosionData*>(lbl_27_bss_3210[Fighter_Diddy].data->extendParam[12]);
}

template <>
const wnIkeSwordData* ftCommonDataAccesser::getWeaponData<wnIkeSwordData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnIkeSwordData*>(lbl_27_bss_3210[Fighter_Ike].data->extendParam[7]);
}

template <>
const wnLinkBowData* ftCommonDataAccesser::getWeaponData<wnLinkBowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Link);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnLinkBowData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    switch (kind) {
    case Fighter_Link:
    return static_cast<const wnLinkBowData*>(lbl_27_bss_3210[Fighter_Link].data->extendParam[13]);
    default:
        return NULL;
    }
}

template <>
const wnLinkBowArrowData* ftCommonDataAccesser::getWeaponData<wnLinkBowArrowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Link);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnLinkBowArrowData*>(static_cast<ftRobotBeamArticle*>(address)->unk14);
    }
    switch (kind) {
    case Fighter_Link:
    return static_cast<const wnLinkBowArrowData*>(lbl_27_bss_3210[Fighter_Link].data->extendParam[15]);
    default:
        return NULL;
    }
}

template <>
const wnToonLinkBowData* ftCommonDataAccesser::getWeaponData<wnToonLinkBowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_ToonLink);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnToonLinkBowData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    switch (kind) {
    case Fighter_ToonLink:
    return static_cast<const wnToonLinkBowData*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[13]);
    default:
        return NULL;
    }
}

template <>
const wnToonLinkBowArrowData* ftCommonDataAccesser::getWeaponData<wnToonLinkBowArrowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_ToonLink);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnToonLinkBowArrowData*>(static_cast<ftRobotBeamArticle*>(address)->unk14);
    }
    switch (kind) {
    case Fighter_ToonLink:
    return static_cast<const wnToonLinkBowArrowData*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[15]);
    default:
        return NULL;
    }
}

template <>
const wnCaptainBlueFalconData* ftCommonDataAccesser::getWeaponData<wnCaptainBlueFalconData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnCaptainBlueFalconData*>(lbl_27_bss_3210[Fighter_Captain].data->extendParam[6]);
}

template <>
const wnCaptainFinalBgData* ftCommonDataAccesser::getWeaponData<wnCaptainFinalBgData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnCaptainFinalBgData*>(lbl_27_bss_3210[Fighter_Captain].data->extendParam[7]);
}

template <>
const wnYoshiTamagoData* ftCommonDataAccesser::getWeaponData<wnYoshiTamagoData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case 4:
        return static_cast<const wnYoshiTamagoData*>(lbl_27_bss_3210[4].data->extendParam[6]);
    case Fighter_Kirby: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, 4);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnYoshiTamagoData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    default:
        return NULL;
    }
}

template <>
const wnPikminPikminData* ftCommonDataAccesser::getWeaponData<wnPikminPikminData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnPikminPikminData*>(lbl_27_bss_3210[Fighter_Pikmin].data->extendParam[10]);
}

template <>
const wnPikminDolfinData* ftCommonDataAccesser::getWeaponData<wnPikminDolfinData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return static_cast<const wnPikminDolfinData*>(lbl_27_bss_3210[Fighter_Pikmin].data->extendParam[11]);
}

template <>
const wnPikminChainData* ftCommonDataAccesser::getWeaponData<wnPikminChainData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnPikminChainData*>(lbl_27_bss_3210[Fighter_Pikmin].data->extendParam[12]);
}

template <>
const wnSZerosuitParalyzerData* ftCommonDataAccesser::getWeaponData<wnSZerosuitParalyzerData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnSZerosuitParalyzerData*>(lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[4]);
}

template <>
const wnSZerosuitWhipData* ftCommonDataAccesser::getWeaponData<wnSZerosuitWhipData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnSZerosuitWhipData*>(lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[6]);
}

template <>
const wnSheikNeedleData* ftCommonDataAccesser::getWeaponData<wnSheikNeedleData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case 14:
        return static_cast<const wnSheikNeedleData*>(lbl_27_bss_3210[14].data->extendParam[5]);
    case Fighter_Kirby: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, 14);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnSheikNeedleData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    default:
        return NULL;
    }
}

// HYPOTHESIS: ftData's declared extendParam array is shorter than the slots read below.

extern "C" void* fn_27_147D34(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[5];
}

extern "C" void* fn_27_147D4C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[7];
}

extern "C" void* fn_27_147D7C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[9];
}

extern "C" void* fn_27_148A9C(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[5];
}

extern "C" void* fn_27_148B44(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[7];
}

extern "C" void* fn_27_148B58(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[8];
}

extern "C" void* fn_27_148B6C(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[9];
}

extern "C" void* fn_27_148D7C(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Sheik].data->extendParam[7];
}

extern "C" void* fn_27_148D90(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Sheik].data->extendParam[8];
}

extern "C" void* fn_27_148E3C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[7];
}

extern "C" void* fn_27_148F4C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[11];
}

extern "C" void* fn_27_149194(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[11];
}

extern "C" void* fn_27_1491AC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[12];
}

extern "C" void* fn_27_1491C4(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[12];
}

extern "C" void* fn_27_14925C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[14];
}

extern "C" void* fn_27_149274(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[15];
}

extern "C" void* fn_27_14928C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[16];
}

extern "C" void* fn_27_1494BC(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Wario].data->extendParam[5];
}

template <>
const wnGanonBeastData* ftCommonDataAccesser::getWeaponData<wnGanonBeastData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnGanonBeastData*>(lbl_27_bss_3210[Fighter_Ganon].data->extendParam[4]);
}

template <>
const wnToonLinkBoomerangData* ftCommonDataAccesser::getWeaponData<wnToonLinkBoomerangData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnToonLinkBoomerangData*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[11]);
}

void* ftCommonDataAccesser::getWeaponData128(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[kind].data->extendParam[4];
}

extern "C" void* fn_27_149464(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[21];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_1497CC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_GKoopa:
        return lbl_27_bss_3210[Fighter_GKoopa].data->extendParam[5];
    default:
        return NULL;
    }
}

template <>
const wnToonLinkHookShotData* ftCommonDataAccesser::getWeaponData<wnToonLinkHookShotData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_ToonLink:
        return static_cast<const wnToonLinkHookShotData*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[16]);
    default:
        return NULL;
    }
}

extern "C" void* fn_27_14948C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Wario) {
        return lbl_27_bss_3210[Fighter_Wario].data->extendParam[4];
    }
    return lbl_27_bss_3210[Fighter_WarioMan].data->extendParam[5];
}

extern "C" void* fn_27_148198(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_PokeLizardon:
        return lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[5];
    case Fighter_Kirby:
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[53];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_148BD0(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_SZeroSuit:
        return lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[5];
    case Fighter_Kirby:
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[59];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_1484BC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_Fox:
        return lbl_27_bss_3210[Fighter_Fox].data->extendParam[11];
    case Fighter_Falco:
        return lbl_27_bss_3210[Fighter_Falco].data->extendParam[9];
    case Fighter_Wolf:
        return lbl_27_bss_3210[Fighter_Wolf].data->extendParam[11];
    default:
        return NULL;
    }
}

// Owner's extendParam slot, or Kirby's copy from that fighter's public "data" archive.
#define FT_OWNER_OR_KIRBY(owner, index, field)                                                         \
    switch (kind) {                                                                                    \
    case owner:                                                                                        \
        return lbl_27_bss_3210[owner].data->extendParam[index];                                        \
    case Fighter_Kirby: {                                                                              \
        u32 slot = fn_27_1222C8(g_ftSlotManager, owner);                                               \
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U); \
        return static_cast<ftRobotBeamArticle*>(address)->field;                                       \
    }                                                                                                  \
    default:                                                                                           \
        return NULL;                                                                                   \
    }

#define FT_KIND_OR_KIRBY(owner, index, field)                                                          \
    switch (kind) {                                                                                    \
    case owner:                                                                                        \
        return lbl_27_bss_3210[kind].data->extendParam[index];                                         \
    case Fighter_Kirby: {                                                                              \
        u32 slot = fn_27_1222C8(g_ftSlotManager, owner);                                               \
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U); \
        return static_cast<ftRobotBeamArticle*>(address)->field;                                       \
    }                                                                                                  \
    default:                                                                                           \
        return NULL;                                                                                   \
    }

extern "C" void* fn_27_147C9C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Mario, 6, unk10)
}

extern "C" void* fn_27_148050(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Pikachu);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[5];
}

extern "C" void* fn_27_1480CC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Pikachu);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk14;
    }
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[6];
}

extern "C" void* fn_27_148380(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Fox, 8, unk10)
}

extern "C" void* fn_27_148414(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Fox, 9, unk14)
}

extern "C" void* fn_27_14858C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Falco, 6, unk10)
}

extern "C" void* fn_27_148620(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Falco, 7, unk14)
}

extern "C" void* fn_27_1486C8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Wolf, 8, unk10)
}

extern "C" void* fn_27_14875C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Wolf, 9, unk14)
}

extern "C" void* fn_27_148910(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Dedede, 12, unk10)
}

extern "C" void* fn_27_1489B8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Diddy, 6, unk10)
}

extern "C" void* fn_27_148CE8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Sheik, 6, unk14)
}

extern "C" void* fn_27_148DA4(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Luigi, 6, unk10)
}

extern "C" void* fn_27_148F64(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Peach);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[kind].data->extendParam[12];
}

extern "C" void* fn_27_148FE4(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Peach);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk14;
    }
    return lbl_27_bss_3210[kind].data->extendParam[13];
}

extern "C" void* fn_27_149064(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Pit, 9, unk10)
}

extern "C" void* fn_27_1490FC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Pit, 10, unk14)
}

extern "C" void* fn_27_1491DC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Popo);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[kind].data->extendParam[13];
}

extern "C" void* fn_27_1492F8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Samus);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[Fighter_Samus].data->extendParam[8];
}

// The aura ball data: Kirby's copy reads it from Lucario's loaded archive, Lucario from his own data record.
template <>
const wnLucarioAuraBallData* ftCommonDataAccesser::getWeaponData<wnLucarioAuraBallData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Lucario);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnLucarioAuraBallData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    return static_cast<const wnLucarioAuraBallData*>(lbl_27_bss_3210[Fighter_Lucario].data->extendParam[9]);
}

extern "C" void* fn_27_1496F0(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Koopa, 4, unk10)
}

extern "C" void* fn_27_149844(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_GameWatch, 6, unk10)
}

#undef FT_OWNER_OR_KIRBY
#undef FT_KIND_OR_KIRBY

// HYPOTHESIS: ftData + 0x1C is the common status-flag table, eight bytes per status. Nana shares Popo's.
struct ftDataStatusFlags {
    u8 unk0[0x1C];
    const ftStatusFlagData* statusFlags;
};

const ftStatusFlagData* ftCommonDataAccesser::getFtStatusData(ftKind kind, int status) const
{
    if (kind == Fighter_Nana) {
        kind = Fighter_Popo;
    }
    return reinterpret_cast<ftDataStatusFlags*>(lbl_27_bss_3210[kind].data)->statusFlags + status;
}

extern int g_soValueVariation;

// Defined in this file; dont_inline keeps the out-of-line call in getParam / getParamCommon.
#pragma dont_inline on
extern "C" s32 fn_27_1454C0()
{
    return g_soValueVariation;
}
#pragma dont_inline reset

#include <ft/ft_extend_param_accesser.h>
#include <revolution/OS/OSError.h>
// TODO: clean up, give variables real names
namespace jj_1455FC {

struct Entry {
    const u8* m_0;
    const u8* m_4;
    u32 m_8;
};
extern Entry lbl_27_bss_3210[];
extern char lbl_27_data_2BBB0[];

extern "C" void fn_27_1455FC(ftKind kind, const u8* param) {
    lbl_27_bss_3210[kind].m_0 = param;
    lbl_27_bss_3210[kind].m_4 = param + 8;
    lbl_27_bss_3210[kind].m_8 = *(const u32*)(param + 0x10);
    if (ftExtendParamAccesser::isExistAccesser(kind)) {
        ftExtendParamAccesser::getAccesser(kind)->setup(param);
    } else {
        OSReport(lbl_27_data_2BBB0, kind);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_145AA0 {
extern "C" {

typedef unsigned char u8;
typedef unsigned int u32;
typedef float f32;

#define SRCP(i) (((u8**)(*(u8**)((u8*)arg + 4)))[i])
#define CF(d, s) *(f32*)(d5 + (d)) = *(f32*)(SRCP(i) + (s))
#define CW(d, s) *(u32*)(d6 + (d)) = *(u32*)(SRCP(i) + (s))
#define CP(d, s) *(u8**)(d7 + (d)) = SRCP(i) + (s)

void fn_27_145AA0(void* arg) {
    u8* d5 = (u8*)arg;
    u8* d6 = (u8*)arg;
    u8* d7 = (u8*)arg;
    int i;

    for (i = 0; i < 2; i++) {
        CF(0x8, 0x4);
        CF(0xC, 0x8);
        CF(0x10, 0xC);
        CF(0x14, 0x10);
        CF(0x18, 0x14);
        CF(0x1C, 0x18);
        CF(0x20, 0x20);
        CF(0x24, 0x24);
        CF(0x28, 0x30);
        CF(0x2C, 0x34);
        CF(0x30, 0x38);
        CF(0x34, 0x3C);
        CF(0x38, 0x40);
        CF(0x3C, 0x48);
        CF(0x40, 0x4C);
        CF(0x44, 0x50);
        CF(0x48, 0x54);
        CF(0x4C, 0x58);
        CF(0x50, 0x5C);
        CF(0x54, 0x60);
        CF(0x58, 0x64);
        CF(0x5C, 0x68);
        CF(0x60, 0x70);
        CF(0x64, 0x74);
        CF(0x68, 0x78);
        CF(0x6C, 0x7C);
        CF(0x70, 0x80);
        CF(0x74, 0x84);
        CF(0x78, 0x8C);
        CF(0x7C, 0x90);
        CF(0x80, 0x94);
        CF(0x84, 0x98);
        CF(0x88, 0xA0);
        CF(0x8C, 0xA8);
        CF(0x90, 0xAC);
        CF(0x94, 0xB0);
        CF(0x98, 0xB4);
        CF(0x9C, 0xB8);
        CF(0xA0, 0xBC);
        CF(0xA4, 0xC0);
        CF(0xA8, 0xC4);
        CF(0xAC, 0xC8);
        CF(0xB0, 0xCC);
        CF(0xB4, 0xD0);
        CF(0xB8, 0xD4);
        CF(0xBC, 0xD8);
        CF(0xC0, 0xDC);
        CF(0xC4, 0xE0);
        CF(0xC8, 0xE4);
        CF(0xCC, 0xE8);
        CF(0xD0, 0xEC);
        CF(0xD4, 0xF4);
        CF(0xD8, 0xFC);
        CF(0xDC, 0x100);
        CF(0xE0, 0x104);
        CF(0xE4, 0x108);
        CF(0xE8, 0x10C);
        CF(0xEC, 0x114);
        CF(0xF0, 0x118);
        CF(0xF4, 0x11C);
        CF(0xF8, 0x120);
        CF(0xFC, 0x124);
        CF(0x100, 0x12C);
        CF(0x104, 0x130);
        CF(0x108, 0x134);
        CF(0x10C, 0x138);
        CF(0x110, 0x13C);
        CF(0x114, 0x144);
        CF(0x118, 0x148);
        CF(0x11C, 0x14C);
        CF(0x120, 0x158);
        CF(0x124, 0x160);
        CF(0x128, 0x164);
        CF(0x12C, 0x168);
        CF(0x130, 0x16C);
        CF(0x134, 0x170);
        CF(0x138, 0x178);
        CF(0x13C, 0x184);
        CF(0x140, 0x188);
        CF(0x144, 0x194);
        CF(0x148, 0x198);
        CF(0x14C, 0x1A8);
        CF(0x150, 0x1AC);
        CF(0x154, 0x1B8);
        CF(0x158, 0x1BC);
        CF(0x15C, 0x1C0);
        CF(0x160, 0x1C4);
        CF(0x164, 0x1C8);
        CF(0x168, 0x1CC);
        CF(0x16C, 0x1D0);
        CF(0x170, 0x1D4);
        CF(0x174, 0x1DC);
        CF(0x178, 0x1E0);
        CF(0x17C, 0x1E4);
        CF(0x180, 0x1E8);
        CF(0x184, 0x1EC);
        CF(0x188, 0x1F8);
        CF(0x18C, 0x200);
        CF(0x190, 0x204);
        CF(0x194, 0x210);
        CF(0x198, 0x228);
        CF(0x19C, 0x22C);
        CF(0x1A0, 0x230);
        CF(0x1A4, 0x238);
        CF(0x1A8, 0x23C);
        CF(0x1AC, 0x240);
        CF(0x1B0, 0x248);
        CF(0x1B4, 0x254);
        CF(0x1B8, 0x25C);
        CF(0x1BC, 0x260);
        CF(0x1C0, 0x264);
        CF(0x1C4, 0x26C);
        CF(0x1C8, 0x270);
        CF(0x1CC, 0x278);
        CF(0x1D0, 0x27C);
        CF(0x1D4, 0x284);
        CF(0x1D8, 0x288);
        CF(0x1DC, 0x28C);
        CF(0x1E0, 0x294);
        CF(0x1E4, 0x298);
        CF(0x1E8, 0x2A0);
        CF(0x1EC, 0x2A4);
        CF(0x1F0, 0x2A8);
        CF(0x1F4, 0x2AC);
        CF(0x1F8, 0x2B0);
        CF(0x1FC, 0x2B4);
        CF(0x200, 0x2B8);
        CF(0x204, 0x2BC);
        CF(0x208, 0x2C0);
        CF(0x20C, 0x2C4);
        CF(0x210, 0x2C8);
        CF(0x214, 0x2CC);
        CF(0x218, 0x2D0);
        CF(0x21C, 0x2D4);
        CF(0x220, 0x2E0);
        CF(0x224, 0x2E4);
        CF(0x228, 0x2E8);
        CF(0x22C, 0x2EC);
        CF(0x230, 0x2F0);
        CF(0x234, 0x2F4);
        CF(0x238, 0x2F8);
        CF(0x23C, 0x304);
        CF(0x240, 0x30C);
        CF(0x244, 0x310);
        CF(0x248, 0x314);
        CF(0x24C, 0x318);
        CF(0x250, 0x31C);
        CF(0x254, 0x328);
        CF(0x258, 0x32C);
        CF(0x25C, 0x334);
        CF(0x260, 0x338);
        CF(0x264, 0x33C);
        CF(0x268, 0x340);
        CF(0x26C, 0x344);
        CF(0x270, 0x348);
        CF(0x274, 0x350);
        CF(0x278, 0x354);
        CF(0x27C, 0x358);
        CF(0x280, 0x368);
        CF(0x284, 0x36C);
        CF(0x288, 0x370);
        CF(0x28C, 0x374);
        CF(0x290, 0x378);
        CF(0x294, 0x37C);
        CF(0x298, 0x388);
        CF(0x29C, 0x38C);
        CF(0x2A0, 0x390);
        CF(0x2A4, 0x394);
        CF(0x2A8, 0x39C);
        CF(0x2AC, 0x3A0);
        CF(0x2B0, 0x3A8);
        CF(0x2B4, 0x3AC);
        CF(0x2B8, 0x3B0);
        CF(0x2BC, 0x3B4);
        CF(0x2C0, 0x3BC);
        CF(0x2C4, 0x3C0);
        CF(0x2C8, 0x3C4);
        CF(0x2CC, 0x3C8);
        CF(0x2D0, 0x3CC);
        CF(0x2D4, 0x3D0);
        CF(0x2D8, 0x3D4);
        CF(0x2DC, 0x3D8);
        CF(0x2E0, 0x3DC);
        CF(0x2E4, 0x3F0);
        CF(0x2E8, 0x3F4);
        CF(0x2EC, 0x3F8);
        CF(0x2F0, 0x468);
        CF(0x2F4, 0x46C);
        CF(0x2F8, 0x470);
        CF(0x2FC, 0x474);
        CF(0x300, 0x478);
        CF(0x304, 0x47C);
        CF(0x308, 0x480);
        CF(0x30C, 0x484);
        CF(0x310, 0x488);
        CF(0x314, 0x48C);
        CF(0x318, 0x490);
        CF(0x31C, 0x494);
        CF(0x320, 0x498);
        CF(0x324, 0x49C);
        CF(0x328, 0x4A0);
        CF(0x32C, 0x4A4);
        CF(0x330, 0x4A8);
        CF(0x334, 0x4AC);
        CF(0x338, 0x4B0);
        CF(0x33C, 0x4B4);
        CF(0x340, 0x4B8);
        CF(0x344, 0x4BC);
        CF(0x348, 0x4C0);
        CF(0x34C, 0x4C4);
        CF(0x350, 0x4CC);
        CF(0x354, 0x4D0);
        CF(0x358, 0x4D4);
        CF(0x35C, 0x4D8);
        CF(0x360, 0x4DC);
        CF(0x364, 0x4E0);
        CF(0x368, 0x4E4);
        CF(0x36C, 0x4E8);
        CF(0x370, 0x4EC);
        CF(0x374, 0x4F4);
        CF(0x378, 0x4F8);
        CF(0x37C, 0x4FC);
        CF(0x380, 0x500);
        CF(0x384, 0x504);
        CF(0x388, 0x508);
        CF(0x38C, 0x50C);
        CF(0x390, 0x510);
        CF(0x394, 0x514);
        CF(0x398, 0x518);
        CF(0x39C, 0x51C);
        CF(0x3A0, 0x520);
        CF(0x3A4, 0x524);
        CF(0x3A8, 0x528);
        CF(0x3AC, 0x534);
        CF(0x3B0, 0x538);
        CF(0x3B4, 0x53C);
        CF(0x3B8, 0x544);
        CF(0x3BC, 0x548);
        CF(0x3C0, 0x54C);
        CF(0x3C4, 0x554);
        CF(0x3C8, 0x558);
        CF(0x3CC, 0x568);
        CF(0x3D0, 0x56C);
        CF(0x3D4, 0x578);
        CF(0x3D8, 0x580);
        CF(0x3DC, 0x594);
        CF(0x3E0, 0x598);
        CF(0x3E4, 0x59C);
        CF(0x3E8, 0x5A4);
        CF(0x3EC, 0x5A8);
        CF(0x3F0, 0x5AC);
        CF(0x3F4, 0x5B0);
        CF(0x3F8, 0x5B4);
        CF(0x3FC, 0x5B8);
        CF(0x400, 0x5BC);
        CF(0x404, 0x5C0);
        CF(0x408, 0x5C8);
        CF(0x40C, 0x5CC);
        CF(0x410, 0x5D0);
        CF(0x414, 0x5D4);
        CF(0x418, 0x5D8);
        CF(0x41C, 0x5DC);
        CF(0x420, 0x5E0);
        CF(0x424, 0x5E4);
        CF(0x428, 0x5EC);
        CF(0x42C, 0x5F0);
        CF(0x430, 0x5F4);
        CF(0x434, 0x5F8);
        CF(0x438, 0x5FC);
        CF(0x43C, 0x600);
        CF(0x440, 0x604);
        CF(0x444, 0x608);
        CF(0x448, 0x60C);
        CF(0x44C, 0x610);
        CF(0x450, 0x614);
        CF(0x454, 0x618);
        CF(0x458, 0x61C);
        CF(0x45C, 0x620);
        CF(0x460, 0x624);
        CF(0x464, 0x628);
        CF(0x468, 0x62C);
        CF(0x46C, 0x630);
        CF(0x470, 0x634);
        CF(0x474, 0x638);
        CF(0x478, 0x63C);
        CF(0x47C, 0x640);
        CF(0x480, 0x644);
        CF(0x484, 0x648);
        CF(0x488, 0x64C);
        CF(0x48C, 0x650);
        CF(0x490, 0x654);
        CF(0x494, 0x658);
        CF(0x498, 0x65C);
        CF(0x49C, 0x660);
        CF(0x4A0, 0x664);
        CF(0x4A4, 0x668);
        CF(0x4A8, 0x66C);
        CF(0x4AC, 0x670);
        CF(0x4B0, 0x674);
        CF(0x4B4, 0x678);
        CF(0x4B8, 0x67C);
        CF(0x4BC, 0x680);
        CF(0x4C0, 0x684);
        CF(0x4C4, 0x688);
        CF(0x4C8, 0x68C);
        CF(0x4CC, 0x690);
        CF(0x4D0, 0x694);
        CF(0x4D4, 0x698);
        CF(0x4D8, 0x69C);
        CF(0x4DC, 0x6A0);
        CF(0x4E0, 0x6A4);
        CF(0x4E4, 0x6A8);
        CF(0x4E8, 0x6AC);
        CF(0x4EC, 0x6B0);
        CF(0x4F0, 0x6B4);
        CF(0x4F4, 0x6B8);
        CF(0x4F8, 0x6BC);
        CF(0x4FC, 0x6C0);
        CF(0x500, 0x6C4);
        CF(0x504, 0x6C8);
        CF(0x508, 0x6CC);
        CF(0x50C, 0x6D0);
        CF(0x510, 0x6D8);
        CF(0x514, 0x6DC);
        CF(0x518, 0x6E0);
        CF(0x51C, 0x6E4);
        CF(0x520, 0x70C);
        CF(0x524, 0x710);
        CF(0x528, 0x718);
        CF(0x52C, 0x720);
        CF(0x530, 0x724);
        CF(0x534, 0x728);
        CF(0x538, 0x74C);
        CF(0x53C, 0x750);
        CF(0x540, 0x754);
        CF(0x544, 0x758);
        CF(0x548, 0x75C);
        CF(0x54C, 0x760);
        CF(0x550, 0x770);
        CF(0x554, 0x77C);
        CF(0x558, 0x780);
        CF(0x55C, 0x794);
        CF(0x560, 0x798);
        CF(0x564, 0x79C);
        CF(0x568, 0x7A0);
        CF(0x56C, 0x7A4);
        CF(0x570, 0x7A8);
        CF(0x574, 0x7AC);
        CF(0x578, 0x7B0);
        CF(0x57C, 0x7B8);
        CF(0x580, 0x7C0);
        CF(0x584, 0x7C4);
        CF(0x588, 0x7C8);
        CF(0x58C, 0x7CC);
        CF(0x590, 0x804);
        CF(0x594, 0x808);
        CF(0x598, 0x80C);
        CF(0x59C, 0x810);
        CF(0x5A0, 0x814);
        CF(0x5A4, 0x818);
        CF(0x5A8, 0x81C);
        CF(0x5AC, 0x824);
        CF(0x5B0, 0x82C);
        CF(0x5B4, 0x830);
        CF(0x5B8, 0x834);
        CF(0x5BC, 0x838);
        CF(0x5C0, 0x83C);
        CF(0x5C4, 0x840);
        CF(0x5C8, 0x850);
        CF(0x5CC, 0x854);
        CF(0x5D0, 0x858);
        CF(0x5D4, 0x85C);
        CF(0x5D8, 0x860);
        CF(0x5DC, 0x874);
        CF(0x5E0, 0x878);
        CF(0x5E4, 0x87C);
        CF(0x5E8, 0x880);
        CF(0x5EC, 0x884);
        CF(0x5F0, 0x888);
        CF(0x5F4, 0x88C);
        CF(0x5F8, 0x890);
        CF(0x5FC, 0x898);

        CW(0xBF8, 0x0);
        CW(0xBFC, 0x1C);
        CW(0xC00, 0x28);
        CW(0xC04, 0x2C);
        CW(0xC08, 0x44);
        CW(0xC0C, 0x6C);
        CW(0xC10, 0x88);
        CW(0xC14, 0x9C);
        CW(0xC18, 0xA4);
        CW(0xC1C, 0xF0);
        CW(0xC20, 0xF8);
        CW(0xC24, 0x110);
        CW(0xC28, 0x128);
        CW(0xC2C, 0x140);
        CW(0xC30, 0x150);
        CW(0xC34, 0x154);
        CW(0xC38, 0x15C);
        CW(0xC3C, 0x174);
        CW(0xC40, 0x17C);
        CW(0xC44, 0x180);
        CW(0xC48, 0x18C);
        CW(0xC4C, 0x190);
        CW(0xC50, 0x19C);
        CW(0xC54, 0x1A0);
        CW(0xC58, 0x1A4);
        CW(0xC5C, 0x1B0);
        CW(0xC60, 0x1B4);
        CW(0xC64, 0x1D8);
        CW(0xC68, 0x1F0);
        CW(0xC6C, 0x1F4);
        CW(0xC70, 0x1FC);
        CW(0xC74, 0x208);
        CW(0xC78, 0x20C);
        CW(0xC7C, 0x214);
        CW(0xC80, 0x218);
        CW(0xC84, 0x21C);
        CW(0xC88, 0x220);
        CW(0xC8C, 0x224);
        CW(0xC90, 0x234);
        CW(0xC94, 0x244);
        CW(0xC98, 0x24C);
        CW(0xC9C, 0x250);
        CW(0xCA0, 0x258);
        CW(0xCA4, 0x268);
        CW(0xCA8, 0x274);
        CW(0xCAC, 0x280);
        CW(0xCB0, 0x290);
        CW(0xCB4, 0x29C);
        CW(0xCB8, 0x2D8);
        CW(0xCBC, 0x2DC);
        CW(0xCC0, 0x2FC);
        CW(0xCC4, 0x300);
        CW(0xCC8, 0x308);
        CW(0xCCC, 0x320);
        CW(0xCD0, 0x324);
        CW(0xCD4, 0x330);
        CW(0xCD8, 0x34C);
        CW(0xCDC, 0x35C);
        CW(0xCE0, 0x360);
        CW(0xCE4, 0x364);
        CW(0xCE8, 0x380);
        CW(0xCEC, 0x384);
        CW(0xCF0, 0x398);
        CW(0xCF4, 0x3A4);
        CW(0xCF8, 0x3B8);
        CW(0xCFC, 0x3E0);
        CW(0xD00, 0x3E4);
        CW(0xD04, 0x3E8);
        CW(0xD08, 0x3EC);
        CW(0xD0C, 0x414);
        CW(0xD10, 0x458);
        CW(0xD14, 0x45C);
        CW(0xD18, 0x460);
        CW(0xD1C, 0x464);
        CW(0xD20, 0x4C8);
        CW(0xD24, 0x4F0);
        CW(0xD28, 0x52C);
        CW(0xD2C, 0x530);
        CW(0xD30, 0x540);
        CW(0xD34, 0x550);
        CW(0xD38, 0x55C);
        CW(0xD3C, 0x560);
        CW(0xD40, 0x564);
        CW(0xD44, 0x570);
        CW(0xD48, 0x574);
        CW(0xD4C, 0x57C);
        CW(0xD50, 0x584);
        CW(0xD54, 0x588);
        CW(0xD58, 0x58C);
        CW(0xD5C, 0x590);
        CW(0xD60, 0x5A0);
        CW(0xD64, 0x5C4);
        CW(0xD68, 0x5E8);
        CW(0xD6C, 0x6D4);
        CW(0xD70, 0x6E8);
        CW(0xD74, 0x6EC);
        CW(0xD78, 0x6F0);
        CW(0xD7C, 0x6F4);
        CW(0xD80, 0x6F8);
        CW(0xD84, 0x6FC);
        CW(0xD88, 0x700);
        CW(0xD8C, 0x704);
        CW(0xD90, 0x708);
        CW(0xD94, 0x714);
        CW(0xD98, 0x71C);
        CW(0xD9C, 0x734);
        CW(0xDA0, 0x738);
        CW(0xDA4, 0x73C);
        CW(0xDA8, 0x740);
        CW(0xDAC, 0x744);
        CW(0xDB0, 0x748);
        CW(0xDB4, 0x764);
        CW(0xDB8, 0x774);
        CW(0xDBC, 0x778);
        CW(0xDC0, 0x784);
        CW(0xDC4, 0x788);
        CW(0xDC8, 0x78C);
        CW(0xDCC, 0x790);
        CW(0xDD0, 0x7B4);
        CW(0xDD4, 0x7BC);
        CW(0xDD8, 0x7D0);
        CW(0xDDC, 0x7D4);
        CW(0xDE0, 0x7D8);
        CW(0xDE4, 0x7E4);
        CW(0xDE8, 0x7E8);
        CW(0xDEC, 0x7EC);
        CW(0xDF0, 0x7F0);
        CW(0xDF4, 0x7F4);
        CW(0xDF8, 0x7F8);
        CW(0xDFC, 0x7FC);
        CW(0xE00, 0x800);
        CW(0xE04, 0x820);
        CW(0xE08, 0x828);
        CW(0xE0C, 0x844);
        CW(0xE10, 0x848);
        CW(0xE14, 0x84C);
        CW(0xE18, 0x864);
        CW(0xE1C, 0x868);
        CW(0xE20, 0x86C);
        CW(0xE24, 0x870);
        CW(0xE28, 0x894);
        d6 += 0x234;

        CP(0x1060, 0x3FC);
        CP(0x1064, 0x418);
        CP(0x1068, 0x72C);
        CP(0x106C, 0x768);
        CP(0x1070, 0x7DC);
        d7 += 0x14;

        *(f32*)(d5 + 0x48) *= -1.0f;
        *(f32*)(d5 + 0x134) *= -1.0f;
        *(f32*)(d5 + 0xD0) *= -1.0f;
        d5 += 0x5F8;
    }
}

}
}

