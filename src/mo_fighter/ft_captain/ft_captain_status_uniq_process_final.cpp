#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <gf/gf_task_scheduler.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <st/stage.h>
#include <types.h>

// Captain Falcon's Final Smash (Blue Falcon): the start, the ride, the three cut-in scenes and the end, plus the link
// message handler of the hit fighters. The scene work lives in ft_captain_final_sub_routine.cpp.
// TODO: the status bodies (only the class frame, the message handler and the static instances are reconstructed).

class ftCaptainStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalStart() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftCaptainStatusUniqProcessFinalRide : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalRide() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFinalScene01 : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalScene01() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftCaptainStatusUniqProcessFinalScene02 : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalScene02() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftCaptainStatusUniqProcessFinalScene03 : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalScene03() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: ftCaptainFinalSubRoutine::invalidate_work (the map name, ft_captain_final_sub_routine.cpp) drops the target
// work of a fighter that the link message names.
extern "C" void fn_100_109D4(soModuleAccesser* moduleAccesser, int taskId);

// HYPOTHESIS: the link message handler of the Final Smash; message 0x3d carries the task id of a fighter at 0x28 of the
// sender. Its vtable has the size of a status process's.
class ftCaptainStatusUniqProcessFinalMessageHandler : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalMessageHandler() { }
    static void linkMessageProcedure(soLinkEventArgs* message, soModuleAccesser* moduleAccesser, void* sender);
    static void update();
};

void ftCaptainStatusUniqProcessFinalMessageHandler::linkMessageProcedure(soLinkEventArgs* message,
                                                                         soModuleAccesser* moduleAccesser, void* sender) {
    switch (message->m_eventKind) {
    case 0x3d:
        fn_100_109D4(moduleAccesser, *reinterpret_cast<int*>(static_cast<u8*>(sender) + 0x28));
        break;
    }
}

// The stage's shadows are cleared while the stage task has its flag at 0x2C (bit 5) set.
void ftCaptainStatusUniqProcessFinalMessageHandler::update() {
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Stage, 0);
    if (task && task->unk2C_b5) {
        g_Stage->clearShadow();
    }
}

ftCaptainStatusUniqProcessFinalStart g_ftCaptainStatusUniqProcessFinalStart;
ftCaptainStatusUniqProcessFinalRide g_ftCaptainStatusUniqProcessFinalRide;
ftCaptainStatusUniqProcessFinalScene01 g_ftCaptainStatusUniqProcessFinalScene01;
ftCaptainStatusUniqProcessFinalScene02 g_ftCaptainStatusUniqProcessFinalScene02;
ftCaptainStatusUniqProcessFinalScene03 g_ftCaptainStatusUniqProcessFinalScene03;
ftCaptainStatusUniqProcessFinalEnd g_ftCaptainStatusUniqProcessFinalEnd;
ftCaptainStatusUniqProcessFinalMessageHandler g_ftCaptainStatusUniqProcessFinalMessageHandler;
