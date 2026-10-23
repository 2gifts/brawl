#pragma once

// Compile-time type lists of the so* builder templates (the module builders, the instance pools and the article
// mediators are all configured with them; the names come from the RTTI strings of the instantiations).

template <class H, class T>
struct soTypeList {
    typedef H Head;
    typedef T Tail;
};
struct soTypeListNullType { };

template <int N>
struct soIntToType {
    enum { value = N };
};

template <class TL>
struct soTypeListLength;
template <>
struct soTypeListLength<soTypeListNullType> {
    enum { value = 0 };
};
template <class H, class T>
struct soTypeListLength<soTypeList<H, T> > {
    enum { value = 1 + soTypeListLength<T>::value };
};
