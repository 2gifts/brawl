#pragma force_active on
#include <so/templates/so_instance_manager.h>
class soTurnEventObserver;

template s32 soInstanceManagerFullPropertyVector<soTurnEventObserver*, 6>::getId(s32);
template u32 soInstanceManagerFullPropertyVector<soTurnEventObserver*, 6>::capacity();
template soTurnEventObserver*& soInstanceManagerFullPropertyVector<soTurnEventObserver*, 6>::atIndexFast(s32);
template soInstanceUnitFullProperty<soTurnEventObserver*>& soInstanceManagerFullPropertyVector<soTurnEventObserver*, 6>::atUnitIndexFast(s32);
