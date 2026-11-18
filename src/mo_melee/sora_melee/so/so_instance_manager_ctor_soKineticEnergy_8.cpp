#pragma force_active off
#define SO_INSTANCE_MANAGER_EXTERNAL_SIZE
#define SO_INSTANCE_MANAGER_EXTERNAL_AT
#define SO_INSTANCE_MANAGER_EXTERNAL_FREE_ID
#define SO_INSTANCE_MANAGER_EXTERNAL_IS_CONTAIN
#include <so/templates/so_instance_manager.h>
class soKineticEnergy;

#include <new>

typedef soInstanceManagerFullPropertyVector<soKineticEnergy*, 8> VelaMgr;

// MATCH-ONLY: constructing the manager forces its constructor into this unit.
#pragma dont_inline on
void vela_a(void* p, bool flag) { new (p) VelaMgr(flag); }
#pragma dont_inline reset
