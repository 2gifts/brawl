#pragma once

#include <types.h>

// Four-letter ids of the Subspace Emissary gimmick/trigger data records (the data section names of the adventure stage
// files). Every translation unit of sora_adv_stage carries its own copy of these two tables: the ids are built at
// static-initialization time from the string literals (`__sinit_*` copies the first word of each literal into the
// zero-initialized arrays), which is what the original compiler emitted for a header-defined table of this form.
// HYPOTHESIS: the names of the tables and of the helper; the id values and their order are from the binary.

static inline u32 advGimmickId(const char* fourcc) {
    return *reinterpret_cast<const u32*>(fourcc);
}

static const u32 g_advGimmickIds[73] = {
    advGimmickId("GMWA"), advGimmickId("GMWO"), advGimmickId("GET1"), advGimmickId("GET2"), advGimmickId("GFRT"),
    advGimmickId("GFT2"), advGimmickId("GITR"), advGimmickId("GTRR"), advGimmickId("GEPT"), advGimmickId("GEG1"),
    advGimmickId("GFG1"), advGimmickId("GFSR"), advGimmickId("GCAM"), advGimmickId("GITM"), advGimmickId("GRIM"),
    advGimmickId("GFIM"), advGimmickId("GNDV"), advGimmickId("GQUK"), advGimmickId("GEFF"), advGimmickId("GSTG"),
    advGimmickId("GMOV"), advGimmickId("GMV4"), advGimmickId("GSHT"), advGimmickId("GMOT"), advGimmickId("GMOA"),
    advGimmickId("GMVA"), advGimmickId("GMCA"), advGimmickId("GMEA"), advGimmickId("GMTA"), advGimmickId("GBRF"),
    advGimmickId("GFRK"), advGimmickId("GFPR"), advGimmickId("GMAO"), advGimmickId("GMRL"), advGimmickId("GELA"),
    advGimmickId("GTIK"), advGimmickId("GBC2"), advGimmickId("GBC1"), advGimmickId("GSPR"), advGimmickId("GCAT"),
    advGimmickId("GLAD"), advGimmickId("GTRC"), advGimmickId("GWND"), advGimmickId("GWD2"), advGimmickId("GBLT"),
    advGimmickId("GWAT"), advGimmickId("GHPE"), advGimmickId("GELE"), advGimmickId("GMSJ"), advGimmickId("GDOR"),
    advGimmickId("GDBF"), advGimmickId("GDTP"), advGimmickId("GDBS"), advGimmickId("GWAP"), advGimmickId("GSAV"),
    advGimmickId("GLOK"), advGimmickId("GLK2"), advGimmickId("GBLK"), advGimmickId("GSBK"), advGimmickId("GMIN"),
    advGimmickId("GENC"), advGimmickId("GDMD"), advGimmickId("GBKI"), advGimmickId("GIBS"), advGimmickId("GIB2"),
    advGimmickId("GMPS"), advGimmickId("GPS2"), advGimmickId("GSCN"), advGimmickId("GCMP"), advGimmickId("GKNG"),
    advGimmickId("GFIN"), advGimmickId("EPBM"), advGimmickId("EPSP"),
};

static const u32 g_advBlockIds[2] = {
    advGimmickId("BLOC"),
    advGimmickId("ADSJ"),
};
