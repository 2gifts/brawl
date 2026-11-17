#pragma force_active on
#include <so/templates/so_instance_manager.h>
class soArticleEventObserver;

template s32 soInstanceManagerFullPropertyVector<soArticleEventObserver*, 5>::getId(s32);
template u32 soInstanceManagerFullPropertyVector<soArticleEventObserver*, 5>::capacity();
template soArticleEventObserver*& soInstanceManagerFullPropertyVector<soArticleEventObserver*, 5>::atIndexFast(s32);
template soInstanceUnitFullProperty<soArticleEventObserver*>& soInstanceManagerFullPropertyVector<soArticleEventObserver*, 5>::atUnitIndexFast(s32);
