#pragma force_active off
#define SO_INSTANCE_MANAGER_EXTERNAL_SIZE
#define SO_INSTANCE_MANAGER_EXTERNAL_AT
#define SO_INSTANCE_MANAGER_EXTERNAL_FREE_ID
#define SO_INSTANCE_MANAGER_EXTERNAL_IS_CONTAIN
#include <so/templates/so_array_value_soAnimCmdControlUnit.h>
#include <so/templates/so_instance_manager.h>
class soAnimCmdControlUnit;
#include <new>

typedef soInstanceManagerFullPropertyVector<soAnimCmdControlUnit, 11> VelaMgr;

// MATCH-ONLY: constructing/deleting the manager forces its base-class members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaMgr(false); }
void vela_del(VelaMgr* p) { delete p; }
#pragma dont_inline reset
