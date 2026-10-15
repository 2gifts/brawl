#pragma once

#include <types.h>

class soHeapModule;

// The kind/sub kind pair that identifies an article (the first word of its construction info).
// HYPOTHESIS: source spelling of the construction info.
struct wnArticleKindInfo {
    s32 kind;
    s32 subKind;
};
struct wnArticleBuildInfo {
    const wnArticleKindInfo* kindInfo;
    soHeapModule* heapModule;
};
