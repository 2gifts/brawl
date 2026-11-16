#pragma once

#include <wn/wn_weapon_builder.h>

// The status configuration of the Paralyzer and its shot: these articles add no uniq process, so the uniq process
// selection is the null one (the guarded singleton of soArrayNull) and the module gets a status kind count of 0.
// HYPOTHESIS: in the original this is one more value of the status build configuration (the native builders of these two
// units have the same truncated map names as the Samus laser's and gun's, with different bodies); kept as a local
// configuration until the shared one takes it.
template <s32 StatusKinds, typename GeneralWorkConfig, s32 StatusDataCount, typename TransitionConfig>
class wnStatusModuleBuildConfigNoUniq : public wnStatusModuleBuildConfig<StatusKinds, GeneralWorkConfig, StatusDataCount, TransitionConfig> {
};

template <s32 StatusKinds, typename GeneralWorkConfig, s32 StatusDataCount, typename TransitionConfig>
class soStatusModuleBuilder<wnStatusModuleBuildConfigNoUniq<StatusKinds, GeneralWorkConfig, StatusDataCount, TransitionConfig> > {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<GeneralWorkConfig> m_generalWorkBuilder;
    soTransitionModuleBuilder<TransitionConfig> m_transitionBuilder;
    // The zero-capacity selection: no array of its own.
    struct NullUniqProcs {
        soSingletonHolder<soArrayNull<soStatusUniqProcess*> > m_holder;
        soArray<soStatusUniqProcess*>* get() { return soSingletonHolder<soArrayNull<soStatusUniqProcess*> >::getInstance(); }
        ~NullUniqProcs() { }
    } m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, StatusDataCount), m_generalWorkBuilder(), m_transitionBuilder(), m_changeRequests(-1),
        m_module(acc, &m_statusTable, m_uniqProcs.get(),
                 m_generalWorkBuilder.getModule(), m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 0, 1) { }
    soStatusModule* getModule() { return &m_module; }
};
