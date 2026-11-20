// Shared source of the archive list destructor pieces: the original emits the list destructors, the Fixed/Contractible/Array base
// destructors and the list unit constructor and destructor in a few separate text ranges, which are the units that include this file.
#pragma force_active off
#include <so/so_array.h>

// TODO: clean up - a minimal stand-in of itManager (the full header drags in the whole item manager); only the nested ItemInfo is needed.
class itManager {
public:
    struct ItemInfo {
        u16 m_variation;
        u32 m_kind : 16;
    };
};
class itGenArchive;
class itArchive;
class BaseItem;
#include <new>

typedef soArrayList<itGenArchive*, 3> GenArchiveList;
typedef soArrayList<itArchive*, 128> ArchiveList;
typedef soArrayList<BaseItem*, 64> BaseItemList;
typedef soArrayList<itManager::ItemInfo, 12> ItemInfoList;

// MATCH-ONLY: constructing/deleting the lists forces their destructors and the list unit members into the unit.
#pragma dont_inline on
void vela_gen_a(void* p) { new (p) GenArchiveList(); }
void vela_gen_del(GenArchiveList* p) { delete p; }
void vela_archive_a(void* p) { new (p) ArchiveList(); }
void vela_archive_del(ArchiveList* p) { delete p; }
void vela_item_a(void* p) { new (p) BaseItemList(); }
void vela_item_del(BaseItemList* p) { delete p; }
void vela_info_a(void* p) { new (p) ItemInfoList(); }
void vela_info_del(ItemInfoList* p) { delete p; }
#pragma dont_inline reset

