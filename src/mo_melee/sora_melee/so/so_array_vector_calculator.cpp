#include <so/so_array.h>
#include <types.h>

#pragma dont_inline on

static void rotateArrayR(soArrayVectorCalcInterface& iface, s32 start, s32 count, s32 capacity);

namespace soArrayVectorCalculator {

s32 insert(soArrayVectorCalcInterface& interface, s32 index, bool isFull, s32 size, s32 capacity, s32 topIndex, s32 lastIndex) {
    if (isFull == true) {
        return 0;
    }
    if (index < 0 || index > size) {
        return 0;
    }
    s32 ret = lastIndex;
    if (size <= 0 || index == size) {
        s32 newLast = lastIndex + 1;
        if (newLast >= capacity) {
            newLast -= capacity;
        }
        interface.setLastIndex(newLast);
        if (topIndex == newLast) {
            interface.onFull();
        }
    } else if (index == 0) {
        return unshift(interface, isFull, capacity, topIndex, lastIndex);
    } else {
        rotateArrayR(interface, index + topIndex, size - index, capacity);
        s32 newLast = lastIndex + 1;
        if (newLast >= capacity) {
            newLast = 0;
        }
        interface.setLastIndex(newLast);
        ret = index;
        ret += topIndex;
        if (ret >= capacity) {
            ret -= capacity;
        }
        if (topIndex == newLast) {
            interface.onFull();
        }
    }
    return ret;
}

void erase(soArrayVectorCalcInterface& interface, s32 index, s32 size, s32 capacity, s32 topIndex, s32 lastIndex) {
    if (size <= 0 || index < 0 || index >= capacity) {
        return;
    }
    {
        if (index == 0) {
            s32 newTop = topIndex + 1;
            if (newTop >= capacity) {
                newTop -= capacity;
            }
            interface.setTopIndex(newTop);
        } else if (index == size - 1) {
            s32 newLast = lastIndex - 1;
            if (newLast < 0) {
                newLast = capacity - 1;
            }
            interface.setLastIndex(newLast);
        } else {
            rotateArrayR(interface, topIndex, index, capacity);
            s32 newTop = topIndex + 1;
            if (newTop >= capacity) {
                newTop = 0;
            }
            interface.setTopIndex(newTop);
        }
        interface.offFull();
    }
}

s32 unshift(soArrayVectorCalcInterface& interface, bool isFull, s32 capacity, s32 topIndex, s32 lastIndex) {
    if (isFull == true) {
        return 0;
    }
    s32 newTop = topIndex - 1;
    if (newTop < 0) {
        newTop = capacity - 1;
    }
    interface.setTopIndex(newTop);
    if (newTop == lastIndex) {
        interface.onFull();
    }
    return newTop;
}

void shift(soArrayVectorCalcInterface& interface, bool isEmpty, s32 capacity, s32 topIndex) {
    if (isEmpty != true) {
        s32 newTop = topIndex + 1;
        if (newTop >= capacity) {
            newTop -= capacity;
        }
        interface.setTopIndex(newTop);
        interface.offFull();
    }
}

s32 push(soArrayVectorCalcInterface& interface, bool isFull, s32 capacity, s32 topIndex, s32 lastIndex) {
    if (isFull == true) {
        return 0;
    }
    s32 newLast = lastIndex + 1;
    if (newLast >= capacity) {
        newLast -= capacity;
    }
    interface.setLastIndex(newLast);
    if (topIndex == newLast) {
        interface.onFull();
    }
    return lastIndex;
}

void pop(soArrayVectorCalcInterface& interface, bool isEmpty, s32 capacity, s32 lastIndex) {
    if (isEmpty != true) {
        s32 newLast = lastIndex - 1;
        if (newLast < 0) {
            newLast = capacity - 1;
        }
        interface.setLastIndex(newLast);
        interface.offFull();
    }
}

static s32 calcUsedSize(s32 topIndex, s32 lastIndex, s32 capacity, bool isFull) {
    if (topIndex < lastIndex) {
        return lastIndex - topIndex;
    }
    if (topIndex == lastIndex && !isFull) {
        return 0;
    }
    return capacity - (topIndex - lastIndex);
}

void clear(soArrayVectorCalcInterface& interface) {
    interface.setTopIndex(0);
    interface.setLastIndex(0);
    interface.offFull();
}

void postInitialize(soArrayVectorCalcInterface& interface, s32 size, s32 capacity) {
    if (size >= capacity) {
        interface.setLastIndex(0);
        interface.onFull();
    } else {
        interface.setLastIndex(size);
    }
}

s32 resize(soArrayVectorCalcInterface& interface, s32 size, bool isEmpty, bool isFull, s32 capacity, s32 topIndex, s32 lastIndex) {
    if (size > capacity) {
        return 0;
    }
    if (size < 0) {
        return 0;
    }
    if (size == 0) {
        clear(interface);
        return 0;
    }
    s32 diff = size - calcUsedSize(topIndex, lastIndex, capacity, isFull);
    if (diff > 0) {
        return diff;
    }
    s32 count = -diff;
    for (s32 i = 0; i < count; i++) {
        pop(interface, isEmpty, capacity, lastIndex);
    }
    return 0;
}

} // namespace soArrayVectorCalculator

static void rotateArrayR(soArrayVectorCalcInterface& iface, s32 start, s32 count, s32 capacity) {
    for (s32 i = count - 1; i >= 0; i--) {
        s32 from = start + i;
        if (from >= capacity) {
            from -= capacity;
        }
        s32 to = from + 1;
        if (to >= capacity) {
            to -= capacity;
        }
        iface.substitution(from, to);
    }
}
