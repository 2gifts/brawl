// MATCH-ONLY: keeps the original local order of the checks in soEventObserver::addObserverSub (see so_event_presenter.h).
#define SO_EVENT_OBSERVER_CHECK_ORDER
#include <so/anim/so_anim_cmd_event_presenter.h>

// Registers the observer with the given event manager (the body of soEventObserver::addObserverSub is expanded here).
void soAnimCmdEventObserver::addObserver(short manageId, s8 p2) {
    addObserverSub(manageId, this, p2);
}
