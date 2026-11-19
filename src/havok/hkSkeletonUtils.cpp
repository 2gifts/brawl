// Havok translation unit hkSkeletonUtils.o (main.dol 0x8032F10C-0x8032F3EC). Names from the DebugMode map.
// Functions in address order:
//   0x8032F10C   148  hkSkeletonUtils::findBoneWithName
//   0x8032F1A0   588  hkSkeletonUtils::getDescendants

#include <havok/hkSkeleton.h>
#include <havok/hkLocalArray.h>
#include <havok/hkString.h>

// Bone of a skeleton; only the name is read here (layout from main.dol's hkBoneClass).
struct hkBone {
    const char* m_name; // 0x00
};

// hkArray::setSize with a fill value for the new elements.
template <typename T>
static inline void hkArray_setSize(hkArray<T>& a, int n, const T& fill) {
    int oldSize = a.m_size;
    if (n > oldSize) {
        a.reserveSmart(n);
        for (int i = oldSize; i < n; i++) {
            ((T*)a.m_data)[i] = fill;
        }
    }
    a.m_size = n;
}

typedef int (*hkStringCompareFunc)(const char* a, const char* b);

struct hkSkeletonUtils {
    static s16 findBoneWithName(const hkSkeleton& skeleton, const char* name, hkStringCompareFunc compare);
    static void getDescendants(const hkSkeleton& skeleton, s16 boneIndex, hkArray<s16>& bonesOut);
};

// Index of the first bone whose name compares equal (case-insensitively by default), or -1.
s16 hkSkeletonUtils::findBoneWithName(const hkSkeleton& skeleton, const char* name, hkStringCompareFunc compare) {
    int numBones = skeleton.m_numBones;
    if (compare == 0) {
        compare = hkString::strCasecmp;
    }
    for (s16 i = 0; i < numBones; i++) {
        if (compare(name, skeleton.m_bones[i]->m_name) == 0) {
            return i;
        }
    }
    return -1;
}

// Appends every bone below boneIndex (parents precede their children in the bone order).
void hkSkeletonUtils::getDescendants(const hkSkeleton& skeleton, s16 boneIndex, hkArray<s16>& bonesOut) {
    const int numBones = skeleton.m_numBones;
    hkLocalArray<hkBool> isDescendant(numBones);
    hkArray_setSize<hkBool>(isDescendant, skeleton.m_numBones, false);
    isDescendant[boneIndex] = true;
    for (s16 b = boneIndex; b < skeleton.m_numBones; b++) {
        if (isDescendant[skeleton.m_parentIndices[b]]) {
            isDescendant[b] = true;
            bonesOut.pushBack(b);
        }
    }
}
