#include <so/territory/so_territory_module_impl.h>
#include <types.h>

void soTerritoryModuleImpl::activate() {
    initAreaInstance();
}

bool soTerritoryModuleImpl::isObserv(char unk1) {
    return unk1 == 0x15;
}

soHyp::soTerritoryRect* soTerritoryModuleImpl::getRect() {
    return &m_rect;
}
