#pragma force_active on
#define MT_VEC3F_IMPLICIT_ASSIGN
#include <so/so_array.h>
#include <so/posture/so_posture_module_impl.h>

template interpolationVec3f& soArrayVectorAbstract<interpolationVec3f>::at(s32);
template const interpolationVec3f& soArrayVectorAbstract<interpolationVec3f>::at(s32) const;
template void soArrayVectorAbstract<interpolationVec3f>::unshift(const interpolationVec3f&);
template void soArrayVectorAbstract<interpolationVec3f>::shift();
template void soArrayVectorAbstract<interpolationVec3f>::push(const interpolationVec3f&);
template void soArrayVectorAbstract<interpolationVec3f>::pop();
template void soArrayVectorAbstract<interpolationVec3f>::insert(s32, const interpolationVec3f&);
template void soArrayVectorAbstract<interpolationVec3f>::erase(s32);
template void soArrayVectorAbstract<interpolationVec3f>::set(s32, const interpolationVec3f&, s32);
template void soArrayVectorAbstract<interpolationVec3f>::clear();
template bool soArrayVectorAbstract<interpolationVec3f>::isNull() const;

// TODO: clean up -- MATCH-ONLY specialization. The generic substitution copies the whole soInterpolation in one
// aggregate copy; the original copies it element by element, so soInterpolation probably has a user-written
// operator= (loop over m_elements) instead of this explicit specialization.
template <>
void soArrayVectorAbstract<interpolationVec3f>::substitution(s32 subIndex, s32 targetIndex) {
    interpolationVec3f& s = this->getArrayValueConst(subIndex);
    interpolationVec3f& t = this->getArrayValueConst(targetIndex);
    for (int i = 0; i < 4; i++)
        t.m_elements[i] = s.m_elements[i];
}
