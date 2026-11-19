#pragma force_active on
#include <so/so_array.h>
#include <so/templates/so_array_value_soMotionAnimObjCacheUnit.h>

typedef soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> Elm;

template s32 soArrayVector<Elm, 4>::getTopIndex() const;
template void soArrayVector<Elm, 4>::setTopIndex(s32);
template s32 soArrayVector<Elm, 4>::getLastIndex() const;
template void soArrayVector<Elm, 4>::setLastIndex(s32);
template Elm& soArrayVector<Elm, 4>::getArrayValueConst(s32);
template void soArrayVector<Elm, 4>::onFull();
template void soArrayVector<Elm, 4>::offFull();
template bool soArrayVector<Elm, 4>::isFull() const;
template s32 soArrayVector<Elm, 4>::capacity() const;
template s32 soArrayVector<Elm, 4>::size() const;
template Elm& soArrayVector<Elm, 4>::atFastAbstractSub(s32) const;
template void soArrayVector<Elm, 4>::setSize(s32);
