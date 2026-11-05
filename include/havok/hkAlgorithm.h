#pragma once

#include <havok/hkBase.h>

// Sorting templates (map names hkAlgorithm::quickSort<T>, quickSort<T, L>, quickSortRecursive<T, L>). They are emitted
// out of line in each unit that sorts (hkBvTreeAgent.o for its hit keys, hk3AxisSweep.o for end points and value pairs).
namespace hkAlgorithm {

// The default order (the functor is an empty struct passed by value: a one-byte copy on the caller's stack).
template <typename T>
struct less {
    hkBool operator()(const T& a, const T& b) {
        return a < b;
    }
};

// BYTES: never inlined into the callers (the target calls all three out of line).
template <typename T, typename L>
void quickSortRecursive(T* pArr, int d, int h, L cmpLess) __attribute__((never_inline));
template <typename T, typename L>
void quickSort(T* pArr, int numElem, L cmpLess) __attribute__((never_inline));
template <typename T>
void quickSort(T* pArr, int numElem) __attribute__((never_inline));

// Hoare partition around the middle element; recurses into the left part and loops on the right part.
template <typename T, typename L>
void quickSortRecursive(T* pArr, int d, int h, L cmpLess) {
    for (;;) {
        int i = d;
        int j = h;
        T pivot = pArr[(d + h) >> 1];
        do {
            while (cmpLess(pArr[i], pivot)) {
                i++;
            }
            while (cmpLess(pivot, pArr[j])) {
                j--;
            }
            if (i <= j) {
                if (i != j) {
                    T t = pArr[i];
                    pArr[i] = pArr[j];
                    pArr[j] = t;
                }
                i++;
                j--;
            }
        } while (i <= j);
        if (d < j) {
            quickSortRecursive(pArr, d, j, cmpLess);
        }
        if (i >= h) {
            return;
        }
        d = i;
    }
}

template <typename T, typename L>
void quickSort(T* pArr, int numElem, L cmpLess) {
    if (numElem > 1) {
        quickSortRecursive(pArr, 0, numElem - 1, cmpLess);
    }
}

template <typename T>
void quickSort(T* pArr, int numElem) {
    quickSort(pArr, numElem, less<T>());
}

} // namespace hkAlgorithm
