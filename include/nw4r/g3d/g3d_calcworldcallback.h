#pragma once

// Local shadow of the BrawlHeaders placeholder: grCalcWorldCallBack derives from the real nw4r::g3d::ICalcWorldCallback
// (its vtable keeps ICalcWorldCallback::ExecCallbackB/C), so the placeholder name derives from that class.

#include <nw4r/g3d/g3d_calcworld.h>

namespace nw4r {
namespace g3d {
// HYPOTHESIS: no class of this name exists in the game; it stands for ICalcWorldCallback where the headers use it.
class ICalcWorldCallBack : public ICalcWorldCallback { };
} // namespace g3d
} // namespace nw4r
