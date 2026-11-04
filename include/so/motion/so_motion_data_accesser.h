#pragma once

#include <types.h>

template <class T> class soArrayFixed;
template <class T> class soSet;
struct soMotionData;

class soMotionDataAccesser {
    soArrayFixed<const soMotionData>* unk0;
    s32 unk4;

public:
    soMotionDataAccesser(soArrayFixed<const soMotionData>* data);
    ~soMotionDataAccesser();
    void setData(soSet<const soMotionData>* data);
    const soMotionData& get(s32 index, bool useOffset);
};
