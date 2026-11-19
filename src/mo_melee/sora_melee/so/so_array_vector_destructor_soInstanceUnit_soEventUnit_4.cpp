#pragma force_active off
#define SO_INSTANCE_UNIT_PTR_DTOR
#include <so/so_array.h>
#include <so/templates/so_instance_unit.h>
class soEventUnit;

template soArrayVector<soInstanceUnit<soEventUnit*>, 4>::~soArrayVector();
