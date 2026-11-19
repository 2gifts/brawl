#pragma force_active on
#include <so/so_array.h>
#include <so/templates/so_array_value_soMotionAnimObjCacheUnit.h>

typedef soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> Elm;

template Elm& soArrayVector<Elm, 5>::atFastAbstractSub(s32) const;
