#define FT_MODULE_BUILDER
#define WN_SIMPLE_INLINE_TEAM
#define SO_NULLABLE_INLINE_CTOR
#define WN_SIMPLE_UNIT
#define WN_SIMPLE_COLOR_BLEND_BUILD_CONFIG soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl>
#include <ft/ft_fighter_builder.h>
#include <wn/wn_simple.h>
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_hyp.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_heap_module.h>
#include <so/so_value_accesser.h>

// HYPOTHESIS: physics slot 0x1C stops the secondary motion; +169A marks an article that deletes itself on deactivation.
void wnSimple::deactivate(bool unk) {
    getPhysicsModule(m_moduleAccesser).unk1C();
    Weapon::deactivate(unk);
    if (unk169A == true) {
        delete this;
    }
}

template <>
void wnClassInfoImpl<133, wnSimple, wnSimpleData>::createArray(s32 count, s32 heapType, s32 articleId,
                                                               const wnArticleBuildInfo& info,
                                                               wnArticleList* list) const {
    bool found = false;
    wnSimpleData* data = g_ftCommonDataAccesser.getSimpleData((ftKind)info.kindInfo->subKind, &found);
    for (s32 i = 0; i < count; i++) {
        // HYPOTHESIS: wnSimpleConstructionInfo is this build info under the constructor's recovered spelling.
        wnSimple* article = new ((Heaps::HeapType)heapType)
            wnSimple(articleId, reinterpret_cast<const wnSimpleConstructionInfo&>(info), data, false);
        if (article == nullptr) {
            OSReport("wnClassInfoImpl::createArray allocation fail\n");
        } else {
            soArticle* entry = static_cast<soArticle*>(article);
            list->insert(entry);
        }
    }
}

wnClassInfoImpl<133, wnSimple, wnSimpleData> g_wnSimpleClassInfo;

soTeamModuleBuilder<wnSimpleTeamBuildConfig>::soTeamModuleBuilder(s32 no, soModuleAccesser* acc) :
    m_primary(no), m_secondary(no), m_module(&m_primary, &m_secondary, &m_primary, acc, g_soTeamModuleNullArg) { }

soPostureModuleBuilder<soPostureModuleBuildConfig<0, soPostureModuleSimple> >::soPostureModuleBuilder(
    soModuleAccesser* accesser, soEventObserverRegistrationDesc* desc) :
    m_module(accesser, get(), desc) {
    m_module.initRot();
}

#pragma push
#pragma inline_depth(8)
#pragma inline_max_size(1000)
#pragma inline_max_auto_size(1000)
#pragma inline_max_total_size(4000)
// The null array has to be this function: its guard is what the shake constructor inlines.
template<>
inline soArray<soShakeTerm>& getNullArray<soShakeTerm>() __attribute__((always_inline)) {
    static soArrayNull<soShakeTerm> NullObj;
    return NullObj;
}
template<>
inline soArray<soPhysicsIKHandle>& getNullArray<soPhysicsIKHandle>() __attribute__((always_inline)) {
    static soArrayNull<soPhysicsIKHandle> NullObj;
    return NullObj;
}
soShakeModuleBuilder<soShakeModuleBuildConfig<0, soShakeModuleSimple> >::soShakeModuleBuilder(soModuleAccesser* accesser, void* data) :
    m_module(accesser, get(), data) { }
#pragma pop

#pragma auto_inline off
wnSimple::wnSimple(s32 articleId, const wnSimpleConstructionInfo& info, wnSimpleData* data, bool unk) :
    wnWeaponBuilder<wnSimpleModuleAccesserBuildConfig>(
        "wnSimple", 0x85, data, articleId, &info, 1,
        wnModuleAccesserBuildData(0x85, data,
            reinterpret_cast<soHeapModule*>(info.heapModule)->getInstanceHeap(),
            reinterpret_cast<soHeapModule*>(info.heapModule)->getNWModelInstanceHeap(),
            reinterpret_cast<soHeapModule*>(info.heapModule)->getNWMotionInstanceHeap(),
            0, -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0),
        unk),
    m_pack(reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(&m_table),
           reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(g_soAnimCmdAddrArrayNull),
           reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(g_soAnimCmdAddrArrayNull)) {
    Weapon::initialize();
    Weapon::deactivate(false);
}
#pragma auto_inline on

