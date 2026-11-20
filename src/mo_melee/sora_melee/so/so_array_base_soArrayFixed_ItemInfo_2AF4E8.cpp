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
#include <new>

typedef soArrayList<itManager::ItemInfo, 12> VelaList;

// MATCH-ONLY: constructing/deleting the list forces its base-class members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaList(); }
void vela_del(VelaList* p) { delete p; }
#pragma dont_inline reset
