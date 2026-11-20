#pragma once

#include <wn/wn_weapon_builder.h>

// An effect module with no continual or screen arrays of its own (the null singletons are handed in).
// HYPOTHESIS: source spelling of the selection; the only member left is the word list.
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<0, 0, 0, 1, -1, T> > {
    u8 unk0[4];
    soArrayVector<u32, 1> m_u32s; // +0x4
    T m_effectModule;             // +0x14
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        m_u32s(1, 0, 0),
        m_effectModule(acc, soSingletonHolder<soArrayNull<soEffectContinual> >::getInstance(), nodeData, &m_u32s,
                       soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData, regDesc, commonData, -1,
                       screenData, soSingletonHolder<soArrayNull<efScreenHandle> >::getInstance()) { }
    T* getModule() { return &m_effectModule; }
};
