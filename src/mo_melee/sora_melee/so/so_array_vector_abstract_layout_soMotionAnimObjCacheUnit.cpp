#pragma force_active on
#include <so/so_array.h>
#include <so/templates/so_array_value_soMotionAnimObjCacheUnit.h>

typedef soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> Elm;

template Elm& soArrayVectorAbstract<Elm>::at(s32);
template const Elm& soArrayVectorAbstract<Elm>::at(s32) const;
template void soArrayVectorAbstract<Elm>::unshift(const Elm&);
template void soArrayVectorAbstract<Elm>::shift();
template void soArrayVectorAbstract<Elm>::push(const Elm&);
template void soArrayVectorAbstract<Elm>::pop();
template void soArrayVectorAbstract<Elm>::insert(s32, const Elm&);
template void soArrayVectorAbstract<Elm>::erase(s32);
template void soArrayVectorAbstract<Elm>::set(s32, const Elm&, s32);
template void soArrayVectorAbstract<Elm>::clear();
template bool soArrayVectorAbstract<Elm>::isNull() const;
template void soArrayVectorAbstract<Elm>::substitution(s32, s32);
