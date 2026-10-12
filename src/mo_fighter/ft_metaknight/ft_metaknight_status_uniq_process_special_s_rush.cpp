#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialSRush : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialSRush() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// HYPOTHESIS: the three floats of lbl_111_rodata_10 (stick dead-zone reference and clamp bounds) were not read from
// the image; these values are guesses and need verification.
static const float kRushStickTable[3] = { 0.0f, -1.0f, 1.0f };

// MATCH-ONLY: keep the table address in one callee-saved register.
#pragma opt_propagation off
void ftMetaknightStatusUniqProcessSpecialSRush::execStatus(soModuleAccesser* moduleAccesser) {
    const float* table = &kRushStickTable[0];
    if (ftMetaknightController(moduleAccesser).getStickX() != table[0]) {
        float stick = ftMetaknightController(moduleAccesser).getStickX();
        float clamped = stick;
        if (stick < table[1]) {
            clamped = table[1];
        } else if (stick > table[2]) {
            clamped = table[2];
        }
        Vec3f pos = ftMetaknightPosture(moduleAccesser).getPos(0);
        pos.m_x += -clamped * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
        ftMetaknightPosture(moduleAccesser).setPos(&pos, 0);
    }
}

#pragma opt_propagation reset
void ftMetaknightStatusUniqProcessSpecialSRush::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11a) {
        Vec3f zero(kRushStickTable[0], kRushStickTable[0], kRushStickTable[0]);
        ftMetaknightPosture(moduleAccesser).setPos(&zero, 0);
        ftMetaknightPosture(moduleAccesser).unk54();
    }
    // TODO: the 0x11a branch (ground query and situation change) is not reconstructed yet.
}

ftMetaknightStatusUniqProcessSpecialSRush g_ftMetaknightStatusUniqProcessSpecialSRush;
