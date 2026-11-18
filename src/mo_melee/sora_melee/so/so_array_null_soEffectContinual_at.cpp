#pragma force_active off
#include <so/so_array.h>
#include <so/effect/so_effect_continual.h>

typedef soEffectContinual Elm;
typedef soArrayNull<Elm> Arr;

template Elm& Arr::at(s32);
template const Elm& Arr::at(s32) const;
