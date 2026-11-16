#pragma once

#include <wn/wn_anim_cmd_full_property.h>

// The uniq command thread without an interpreter stack of its own (stack capacity 0): the interpreter runs on the
// shared null stack (the guarded singleton of soArrayNull), as in the Paralyzer's command threads.
template <int Id>
class soAnimCmdControlUnitBuilderUniq<soAnimCmdControlUnitBuildConfigUniq<Id, 0> > {
    soAnimCmdInterpreter m_interpreter;                                  // +0
    // +0x50, the zero-capacity selection: no array of its own.
    struct NullStack {
        soSingletonHolder<soArrayNull<acCmdInterpreterStackData> > m_holder;
        soArray<acCmdInterpreterStackData>* get() { return soSingletonHolder<soArrayNull<acCmdInterpreterStackData> >::getInstance(); }
        ~NullStack() { }
    } m_stack;
public:
    // The reset mode is a defaulted reference parameter: the caller builds its box before the null stack is looked up.
    soAnimCmdControlUnitBuilderUniq(soModuleAccesser* acc, const u8& resetMode = *ftAddressOf((u8)1)) :
        m_interpreter(ftGetManageId(acc), ftInterpreterStack(m_stack.get()), Id, 1, 1.0f, &resetMode) {
        soAttributeFlag attr(4);
        soAnimCmdControlUnit unit;
        unit.m_animCmdInterpreter = &m_interpreter;
        unit.m_animCmdAddressPackArraySeparate = (soAnimCmdAddressPackArraySeparate*)g_soAnimCmdAddressPackArrayNull;
        acc->getAnimCmdModule().registInterpreter(unit, attr);
    }
};

// The full-property command threads with a uniq thread of stack capacity 0.
// HYPOTHESIS: in the original this is one more value of wnAnimCmdModuleSubBuildConfigFullProperty (its map name has
// seven values more than the twenty spelled there); kept local until the shared configuration takes it.
template <int E0, int C0, int A0, int B0, int S0, int E1, int C1, int A1, int B1, int S1, int E2, int C2, int A2, int B2, int S2,
          int E3, int C3, int A3, int B3, int S3>
class wnAnimCmdModuleSubBuildConfigFullPropertyNoUniqStack
    : public wnAnimCmdModuleSubBuildConfigFullProperty<E0, C0, A0, B0, S0, E1, C1, A1, B1, S1, E2, C2, A2, B2, S2, E3, C3, A3, B3, S3> {
public:
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 0> Unit4Config;
};
