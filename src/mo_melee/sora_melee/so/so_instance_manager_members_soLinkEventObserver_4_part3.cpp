#pragma force_active on
#include <so/templates/so_instance_manager.h>
class soLinkEventObserver;

template s32 soInstanceManagerFullPropertyVector<soLinkEventObserver*, 4>::getId(s32);
template u32 soInstanceManagerFullPropertyVector<soLinkEventObserver*, 4>::capacity();
template soLinkEventObserver*& soInstanceManagerFullPropertyVector<soLinkEventObserver*, 4>::atIndexFast(s32);
template soInstanceUnitFullProperty<soLinkEventObserver*>& soInstanceManagerFullPropertyVector<soLinkEventObserver*, 4>::atUnitIndexFast(s32);
