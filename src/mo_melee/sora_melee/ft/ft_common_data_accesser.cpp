#include <types.h>
#include <ft/robot/ft_robot_article_pools.h>
#include <ft/ft_slot_manager.h>
#include <so/so_archive_db.h>

#include <ft/ft_common_data_accesser.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern "C" s32 fn_27_1222C8(ftSlotManager* slotManager, s32 slot);
// HYPOTHESIS: 12-byte ftKindData entries (kind * 0xC), data pointer at +0
extern "C" ftKindData lbl_27_bss_3210[];
extern "C" char lbl_27_data_2BBD8[];

// HYPOTHESIS: public address record whose pointer at +0x10 is the beam article
struct ftRobotBeamArticle {
    u8 unk0[0x10];
    void* unk10;
};

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


extern "C" s32 fn_27_1454C0();
// HYPOTHESIS: 12-byte ftKindData entries (kind * 0xC), params pointer at +4
extern "C" ftKindData lbl_27_bss_3210[];

ftParam* ftCommonDataAccesser::getParam(ftKind kind) const
{
    s32 index = fn_27_1454C0();
    return lbl_27_bss_3210[kind].params[index];
}
