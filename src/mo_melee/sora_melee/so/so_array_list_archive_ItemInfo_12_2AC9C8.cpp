#pragma force_active off
#define SO_ARRAY_LIST_EXTERNAL_QUERY_MEMBERS
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
template void soArrayList<itManager::ItemInfo, 12>::push(const itManager::ItemInfo&);
template void soArrayList<itManager::ItemInfo, 12>::erase(s32);
template void soArrayList<itManager::ItemInfo, 12>::clear();
template s32 soArrayList<itManager::ItemInfo, 12>::getArrayIndex(s32) const;
template s32 soArrayList<itManager::ItemInfo, 12>::insertSub(s32,s32);
template void soArrayList<itManager::ItemInfo, 12>::eraseSub(s32);

// size and capacity are declared but not defined by the header in this mode (the list query members live in the unit that
// uses them out of line); this unit owns their weak copies.
template <>
s32 soArrayList<itManager::ItemInfo, 12>::size() const { return m_size; }
template <>
s32 soArrayList<itManager::ItemInfo, 12>::capacity() const { return 12; }
