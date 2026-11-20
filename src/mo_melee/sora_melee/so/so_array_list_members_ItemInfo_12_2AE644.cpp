#pragma force_active off
#define SO_ARRAY_LIST_EXTERNAL_QUERY_MEMBERS
#define SO_ARRAY_LIST_EXTERNAL_size
#define SO_ARRAY_LIST_EXTERNAL_capacity
#define SO_ARRAY_LIST_EXTERNAL_getArrayIndex
#define SO_ARRAY_LIST_EXTERNAL_insertSub
#define SO_ARRAY_LIST_EXTERNAL_eraseSub
#include <so/so_array.h>
// TODO: clean up - a minimal stand-in of itManager (the full header drags in the whole item manager); only the nested ItemInfo is needed.
class itManager {
public:
    struct ItemInfo {
        u16 m_variation;
        u32 m_kind : 16;
    };
};


#pragma force_active on
template itManager::ItemInfo& soArrayList<itManager::ItemInfo, 12>::at(s32);
template const itManager::ItemInfo& soArrayList<itManager::ItemInfo, 12>::at(s32) const;
template void soArrayList<itManager::ItemInfo, 12>::unshift(const itManager::ItemInfo&);
template void soArrayList<itManager::ItemInfo, 12>::shift();
template void soArrayList<itManager::ItemInfo, 12>::pop();
template void soArrayList<itManager::ItemInfo, 12>::insert(s32,const itManager::ItemInfo&);
template void soArrayList<itManager::ItemInfo, 12>::set(s32,const itManager::ItemInfo&,s32);
template bool soArrayList<itManager::ItemInfo, 12>::isNull() const;
template s32 soArrayList<itManager::ItemInfo, 12>::shiftFreeArrayIndex(s32);

template <>
bool soArrayList<itManager::ItemInfo, 12>::isFull() const { return m_freeIndex < 0; }
