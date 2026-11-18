#pragma force_active off
#include <so/so_array.h>
#include <so/model/so_model_virtual_node.h>

typedef soModelVirtualNode Elm;
typedef soArrayNull<Elm> Arr;

template Elm& Arr::at(s32);
template const Elm& Arr::at(s32) const;
