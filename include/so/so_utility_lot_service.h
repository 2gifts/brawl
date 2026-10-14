#pragma once

// HYPOTHESIS: the lottery helper of sora_melee; lotRate(rate) is true with the given probability (0..1).
class soUtilityLotService {
public:
    static bool lotRate(float rate);
};
