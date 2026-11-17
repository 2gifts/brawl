#include <mt/mt_prng.h>
#include <so/so_param_utility.h>
#include <types.h>

// HYPOTHESIS: the table is an array of {kind, weight} pairs terminated by a kind of -1; weights sum to 100.
struct soParamUtilityEntry {
    s32 m_kind;
    s32 m_weight;
};

int soParamUtility::getRandomMotionKind(const void* table, int kind) {
    if (table != nullptr) {
        s32 roll = randi(100) + 1;
        s32 sum = 0;
        const soParamUtilityEntry* entry = static_cast<const soParamUtilityEntry*>(table);
        while (entry->m_kind != -1) {
            sum += entry->m_weight;
            if (roll <= sum) {
                return entry->m_kind;
            }
            entry++;
        }
    }
    return kind;
}
