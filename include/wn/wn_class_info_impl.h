#pragma once

#include <revolution/OS/OSError.h>
#include <ft/ft_common_data_accesser.h>
#include <sr/sr_common.h>
#include <types.h>
#include <wn/wn_article_build_info.h>
#include <wn/wn_class_info.h>

// HYPOTHESIS: the blaster class infos fetch their article data through g_ftCommonDataAccesser (the module
// map names the Fox call target getWeaponData49, the Wolf one getWeaponData57, both in a 151-entry overload
// family whose C++ signature is not recoverable), so the accessers get referenced by their unmangled names.
extern "C" void* fn_27_148414(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14875C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// The data types the dispatch below selects on; the instantiating unit includes its article header,
// which declares the struct with this name (the layout is not needed here, only the pointer type).
struct wnFoxBlasterBulletData;
struct wnWolfBlasterBulletData;

// Data-type dispatch so the template below picks the right family's accesser; extend per blaster family as
// its data class is reconstructed. HYPOTHESIS: helper name; it is inline and leaves no symbol.
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFoxBlasterBulletData*) {
    return fn_27_148414(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnWolfBlasterBulletData*) {
    return fn_27_14875C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}

// Class info for one article kind: registers itself in the class info table during static initialization
// and builds the article array when the article manager asks for it.
//   K: article kind (31 Fox Blaster, 39 Wolf Blaster)
//   T: article class, D: the data type its constructor consumes.
// HYPOTHESIS: template spelling; the data strings prove the instantiation
// wnClassInfoImpl<31, wnFoxBlasterBullet, wnFoxBlasterBulletData>.
template <s32 K, class T, class D>
class wnClassInfoImpl : public wnClassInfo {
public:
    wnClassInfoImpl() {
        setClassInfo(K, this);
    }

    ~wnClassInfoImpl() {
        setClassInfo(K, &g_wnClassInfoNull);
    }

    virtual void createArray(s32 count, s32 heapType, s32 articleId, const wnArticleBuildInfo& info,
                             wnArticleList* list) const {
        bool found = false;
        const D* data = static_cast<const D*>(
            wnFetchBlasterBulletData(info.kindInfo->subKind, &found, static_cast<const D*>(nullptr)));
        for (s32 i = 0; i < count; i++) {
            // The Heaps::HeapType placement form passes sizeof(T) implicitly and returns null on failure.
            T* article = new ((Heaps::HeapType)heapType) T(articleId, info, data);
            if (article == nullptr) {
                OSReport("wnClassInfoImpl::createArray allocation fail\n");
            } else {
                soArticle* entry = static_cast<soArticle*>(article);
                list->insert(entry);
            }
        }
    }
};
