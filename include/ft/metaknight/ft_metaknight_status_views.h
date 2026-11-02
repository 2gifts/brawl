#pragma once

// Shared helpers for Meta Knight's Final Smash statuses.
#include <so/so_module_accesser.h>
#include <so/link/so_link_event_presenter.h>
#include <mt/mt_vector.h>
#include <types.h>

// Final-smash helpers shared by the Metaknight final statuses. Defined in ft_metaknight_status_uniq_process_final_hit_wait.cpp.
struct ftMetaknightFinalLinkEvent : public soLinkEventArgs {
    ftMetaknightFinalLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

class ftMetaknightStatusUniqProcessFinalUtil {
public:
    static void notifyMetaknightFinalEnd(soModuleAccesser* moduleAccesser);
    static void unLinkFinalTargetAll(soModuleAccesser* moduleAccesser);
    static void correctCameraRange(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: parameter types; the union of two camera ranges.
    static Rect2D sumRect(const Rect2D* a, const Rect2D* b);
};
