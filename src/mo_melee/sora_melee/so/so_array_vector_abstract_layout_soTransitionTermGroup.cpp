#pragma force_active on
#define SO_TRANSITION_TERM_GROUP_ASSIGN
#include <so/so_array.h>
#include <so/transition/so_transition_module_impl.h>

template soTransitionTermGroup& soArrayVectorAbstract<soTransitionTermGroup>::at(s32);
template const soTransitionTermGroup& soArrayVectorAbstract<soTransitionTermGroup>::at(s32) const;
template void soArrayVectorAbstract<soTransitionTermGroup>::unshift(const soTransitionTermGroup&);
template void soArrayVectorAbstract<soTransitionTermGroup>::shift();
template void soArrayVectorAbstract<soTransitionTermGroup>::push(const soTransitionTermGroup&);
template void soArrayVectorAbstract<soTransitionTermGroup>::pop();
template void soArrayVectorAbstract<soTransitionTermGroup>::insert(s32, const soTransitionTermGroup&);
template void soArrayVectorAbstract<soTransitionTermGroup>::erase(s32);
template void soArrayVectorAbstract<soTransitionTermGroup>::set(s32, const soTransitionTermGroup&, s32);
template void soArrayVectorAbstract<soTransitionTermGroup>::clear();
template bool soArrayVectorAbstract<soTransitionTermGroup>::isNull() const;
template void soArrayVectorAbstract<soTransitionTermGroup>::substitution(s32, s32);
