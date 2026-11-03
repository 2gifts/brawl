#pragma once

#include <ft/ft_common_data_accesser.h>
#include <types.h>

// HYPOTHESIS: Samus's ftData continues past extendParam, as for Fox and Wolf; the charge-shot article id sits at 0x98.
struct ftSamusData : ftData {
    u8 unk94[0x98 - sizeof(ftData)];
    int chargeShotArticleId;
};
static_assert(sizeof(ftSamusData) == 0x9c, "Charge-shot article id offset is wrong!");
