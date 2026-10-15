#pragma once

#include <revolution/OS/OSError.h>
#include <ft/ft_common_data_accesser.h>
#include <sr/sr_common.h>
#include <types.h>
#include <wn/wn_article_build_info.h>
#include <wn/wn_class_info.h>

// HYPOTHESIS: the blaster class infos fetch their article data through g_ftCommonDataAccesser (the module
// map names the call targets getWeaponData6/13/48/49/53/54/56/57/66/74/91/101/109/121/144, all in a 151-entry overload family whose C++
// signature is not recoverable), so the accessers get referenced by their unmangled names.
extern "C" void* fn_27_148414(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14875C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148380(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14858C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148620(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1486C8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147E6C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1492F8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1490FC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149A64(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148AB0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147C88(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14959C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148E54(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148910(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// The data types the dispatch below selects on; the instantiating unit includes its article header,
// which declares the struct with this name (the layout is not needed here, only the pointer type).
struct wnFoxBlasterBulletData;
struct wnWolfBlasterBulletData;
struct wnFoxBlasterData;
struct wnFalcoBlasterData;
struct wnFalcoBlasterBulletData;
struct wnWolfBlasterData;
struct wnLinkBowArrowData;
struct wnSamusCShotData;
struct wnPitBowArrowData;
struct wnToonLinkBowArrowData;
struct wnYoshiTamagoData;
struct wnKirbyPikminData;
struct wnNessPKFlashData;
struct wnLucasPKFlashData;
struct wnDededeStarmissileData;

// Data-type dispatch so the template below picks the right family's accesser; extend per blaster family as
// its data class is reconstructed. HYPOTHESIS: helper name; it is inline and leaves no symbol.
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFoxBlasterBulletData*) {
    return fn_27_148414(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnWolfBlasterBulletData*) {
    return fn_27_14875C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFoxBlasterData*) {
    return fn_27_148380(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFalcoBlasterData*) {
    return fn_27_14858C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFalcoBlasterBulletData*) {
    return fn_27_148620(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnWolfBlasterData*) {
    return fn_27_1486C8(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLinkBowArrowData*) {
    return fn_27_147E6C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnSamusCShotData*) {
    return fn_27_1492F8(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPitBowArrowData*) {
    return fn_27_1490FC(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnToonLinkBowArrowData*) {
    return fn_27_149A64(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnYoshiTamagoData*) {
    return fn_27_148AB0(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnKirbyPikminData*) {
    return fn_27_147C88(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnNessPKFlashData*) {
    return fn_27_14959C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLucasPKFlashData*) {
    return fn_27_148E54(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnDededeStarmissileData*) {
    return fn_27_148910(&g_ftCommonDataAccesser, subKind, resourceGroup);
}

// Class info for one article kind: registers itself in the class info table during static initialization
// and builds the article array when the article manager asks for it.
//   K: article kind (30/31 Fox, 35/36 Falco, 38/39 Wolf; blaster first, bullet second; plus 5 Link,
//      83 Samus, 91 Pit, 140 Toon Link one-shot articles, 53/60/77/98/102/117/150 single-kind articles).
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
