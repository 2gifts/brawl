#pragma force_active on
#include <so/so_array.h>
#include <so/templates/so_array_value_soMotionAnimObjCacheUnit.h>

typedef soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> Elm;

template s32 soArrayVector<Elm, 5>::getTopIndex() const;
template void soArrayVector<Elm, 5>::setTopIndex(s32);
template s32 soArrayVector<Elm, 5>::getLastIndex() const;
template void soArrayVector<Elm, 5>::setLastIndex(s32);
template Elm& soArrayVector<Elm, 5>::getArrayValueConst(s32);
template void soArrayVector<Elm, 5>::onFull();
template void soArrayVector<Elm, 5>::offFull();
template bool soArrayVector<Elm, 5>::isFull() const;
template s32 soArrayVector<Elm, 5>::capacity() const;
template s32 soArrayVector<Elm, 5>::size() const;
template void soArrayVector<Elm, 5>::setSize(s32);
