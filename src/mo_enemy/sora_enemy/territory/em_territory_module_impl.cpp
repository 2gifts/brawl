#include <em/enemy.h>
#include <em/territory/em_territory_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: one area data set (0x20 bytes) of the enemy's area module, which emAreaModuleImpl keeps in its area work
// array; the territory side rectangle starts at 0x10. The fields before it are filled by the area module's helper.
struct emAreaDataSet {
    u8 unk0;
    u8 m_kind;
    u16 unk2;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    float m_left;   // 0x10
    float m_right;  // 0x14
    float m_top;    // 0x18
    float m_bottom; // 0x1C
};

// MATCH-ONLY: helpers of emAreaModuleImpl's range (map: unnamed) called as plain functions; they are the area module's
// area data set accessor (null past the area count), the kind of an area index and the filler of a data set's kind.
extern "C" emAreaDataSet* fn_41_23270(emAreaModuleImpl* areaModule, u32 area);
extern "C" u32 fn_41_230F8(u32 area);
extern "C" void fn_41_232A4(emAreaModuleImpl* areaModule, u32 kind, emAreaDataSet* data);

emTerritoryModuleImpl::emTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5,
                                             int p6, int p7, int p8)
    : soTerritoryModuleImpl(acc, territoryData, createTerritory, p4, p5, p6, p7, p8) { }

emTerritoryModuleImpl::~emTerritoryModuleImpl() { }

void emTerritoryModuleImpl::createAreaDataSet(int area, float left, float right, float top, float bottom) {
    if (area == -1) {
        return;
    }
    emAreaModuleImpl& areaModule = dynamic_cast<emAreaModuleImpl&>(m_moduleAccesser->getAreaModule());
    emAreaDataSet* data = fn_41_23270(&areaModule, area);
    if (data != NULL) {
        u32 kind = fn_41_230F8(area);
        data->unk0 = 0;
        fn_41_232A4(&areaModule, kind, data);
        data->unkC = 0;
        data->m_left = left;
        data->m_right = right;
        data->m_top = top;
        data->m_bottom = bottom;
    }
}

bool emTerritoryModuleImpl::checkArea(u32 area) {
    emAreaModuleImpl& areaModule = dynamic_cast<emAreaModuleImpl&>(m_moduleAccesser->getAreaModule());
    return areaModule.checkArea(m_moduleAccesser, area, 4) == 1;
}

bool emTerritoryModuleImpl::isDoCreate() {
    return true;
}
