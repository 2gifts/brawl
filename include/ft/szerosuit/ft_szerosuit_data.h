#pragma once

#include <ft/ft_common_data_accesser.h>
#include <so/area/so_area_module_impl.h>
#include <types.h>

// One Power Suit piece of the table the armor articles are started and thrown from.
// HYPOTHESIS: field names (the two words go to wnSZerosuitArmor::activate as node and target node, the second one and
// the float to shoot).
struct ftSZerosuitArmorPiece {
    u32 node;       // +0x0
    u32 targetNode; // +0x4
    float angle;    // +0x8
    u32 unkC;
};
static_assert(sizeof(ftSZerosuitArmorPiece) == 0x10, "Power Suit piece entry size is wrong!");

// HYPOTHESIS: Zero Suit Samus's ftData continues past extendParam (as Samus's, ft/samus/ft_samus_data.h).
struct ftSZerosuitData : ftData {
    u8 unk94[0x9c - sizeof(ftData)];
    soSet<soAreaData> additionalAreas;        // +0x9C
    const ftSZerosuitArmorPiece* armorPieces; // +0xA4: indexed by the work counter 0x10000040
};
static_assert(offsetof(ftSZerosuitData, armorPieces) == 0xa4, "Power Suit piece table offset is wrong!");
