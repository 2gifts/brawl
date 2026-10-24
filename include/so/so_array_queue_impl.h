#pragma once

// soArrayQueueImpl<V>: a queue over the array V it holds (the status change requests of the status modules).
// The same class as in ft/builder/ft_builder_status.h, which the units that include em/enemy.h cannot include (it pulls
// in the fighters' list of sora_melee array instances). TODO: have ft_builder_status.h include this header instead.

#include <so/so_array.h>
#include <types.h>

template <typename V>
class soArrayQueueImpl {
    V* m_array;
    V m_vector;
public:
    soArrayQueueImpl(const s32& initial) : m_array(&m_vector), m_vector(1, initial, 0) {
        soArray<s32>* array = &m_vector;
        array->capacity();
    }
    soArrayQueueImpl() : m_array(&m_vector), m_vector(0) {
        soArray<typename V::value_type>* array = &m_vector;
        array->capacity();
    }
    template <class E>
    soArrayQueueImpl(s32 size, const E& initial) : m_array(&m_vector), m_vector(size, initial, 0) {
        soArray<E>* array = &m_vector;
        array->capacity();
    }
    V* getArray() { return m_array; }
};
