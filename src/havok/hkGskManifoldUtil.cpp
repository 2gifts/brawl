// Havok translation unit hkGskManifoldUtil.o (main.dol 0x80315780-0x803183A0).
// Not yet decompiled. Functions in address order:
//   0x80315780   396  hkGskManifold_garbageCollect
//   0x8031590C   376  hkGskManifold_removePoint
//   0x80315A84   764  hkGskManifold_verifyAndGetPoints
//   0x80315D80   208  hkGskManifold_convertPointPoint
//   0x80315E50   944  hkGskManifold_convertPointEdge
//   0x80316200  1216  hkGskManifold_convertPointFace
//   0x803166C0   900  hkGskManifold_convertEdgePoint
//   0x80316A44  1172  hkGskManifold_convertEdgeEdge
//   0x80316ED8  1352  hkGskManifold_convertFacePoint
//   0x80317420   548  hkGskManifold_doesPointExistAndResort
//   0x80317644  3272  hkGskManifold_addPoint

#include <havok/hkGskManifoldUtil.h>

void hkGskManifoldUtil::hkGskManifold_cleanup(hkGskManifold* manifold, void* arg) {
    u8* p = (u8*)manifold;
    for (int i = 0; i < manifold->m_countC; i++) {
        if (*(u16*)(p + 6) != 0xFFFF) {
            ((hkGskPointRemover*)arg)->removePoint(*(u16*)(p + 6));
        }
        p += 8;
    }
    manifold->m_countC = 0;
    manifold->m_countA = 0;
    manifold->m_countB = 0;
}

// Marks the vertex ids the points use, keeps only those (both lists are compacted in place, the first list's new
// length is the new offset of the old second list) and rewrites every point's offsets. Draft (88%): register
// choice of the compaction pointers and of the final remap loop.
void hkGskManifoldUtil::hkGskManifold_garbageCollect(hkGskManifold* manifold) {
    u8 used[16] = {0};
    hkGskManifoldEntry* entries = manifold->getEntries();
    {
        hkGskManifoldEntry* e = entries;
        for (int i = 0; i < manifold->m_countC; i++) {
            int numVertices = e->unk0 + e->unk1;
            used[e->m_vertexOffsets[0] >> 4] = 1;
            used[e->m_vertexOffsets[1] >> 4] = 1;
            if (numVertices >= 3) {
                used[e->m_vertexOffsets[2] >> 4] = 1;
            }
            if (numVertices == 4) {
                used[e->m_vertexOffsets[3] >> 4] = 1;
            }
            e++;
        }
    }
    int newOffsets[16];
    u16* dst = (u16*)&manifold->m_points[manifold->m_countC];
    u16* src = dst;
    int numKept = 0;
    int offset = 0;
    int numVertices = manifold->m_countA + manifold->m_countB;
    for (int k = 0; k < numVertices; k++) {
        newOffsets[k] = offset;
        if (used[k]) {
            *dst++ = *src;
            numKept++;
            offset += 0x10;
        }
        src++;
    }
    int newCountA = (newOffsets[manifold->m_countA] >> 4) & 0xFF;
    manifold->m_countA = newCountA;
    manifold->m_countB = numKept - newCountA;
    hkGskManifoldEntry* e = entries;
    for (int i = 0; i < manifold->m_countC; i++) {
        e->m_vertexOffsets[0] = newOffsets[e->m_vertexOffsets[0] >> 4];
        e->m_vertexOffsets[1] = newOffsets[e->m_vertexOffsets[1] >> 4];
        e->m_vertexOffsets[2] = newOffsets[e->m_vertexOffsets[2] >> 4];
        e->m_vertexOffsets[3] = newOffsets[e->m_vertexOffsets[3] >> 4];
        e++;
    }
}

// Word copy from lower to higher index (the vertex lists move down over a freed point).
static inline void hkGskManifold_copyWords(const u32* src, u32* dst, int lastIndex) {
    for (int i = lastIndex; i >= 0; i--) {
        *dst++ = *src++;
    }
}

// The last point moves into the freed slot, the vertex id lists move down over the old last point (copied as
// words), then the unused vertex ids are dropped.
void hkGskManifoldUtil::hkGskManifold_removePoint(hkGskManifold* manifold, int index) {
    manifold->m_countC--;
    manifold->m_points[index] = manifold->m_points[manifold->m_countC];
    hkGskManifold_copyWords((const u32*)&manifold->m_points[manifold->m_countC + 1],
                            (u32*)&manifold->m_points[manifold->m_countC],
                            (manifold->m_countA + manifold->m_countB - 1) >> 1);
    hkGskManifold_garbageCollect(manifold);
}

#include <havok/hkGskCache.h>

// The vertex id at byte offset (point vertex offset >> 3) of the id list that follows the point list.
static inline u16 hkGskManifold_vertexId(const hkGskManifold* manifold, int offset) {
    return *(const u16*)((const u8*)manifold + manifold->m_countC * 8 + 4 + (offset >> 3));
}

// Looks for the point of the cache's simplex in the manifold. A found point is moved to the front (returns 1).
// Otherwise a single-vertex point that shares the cache's vertex of a one-vertex body is turned into a zombie
// (dimensions cleared) and 0 is returned.
// TODO: clean up: draft (register numbering of the point pointers differs).
int hkGskManifoldUtil::hkGskManifold_doesPointExistAndResort(hkGskManifold* manifold, const hkGskCache& cache) {
    hkGskManifoldEntry* points = manifold->m_points;
    hkGskManifoldEntry* e = points;
    int i = 0;
    for (; i < manifold->m_countC; i++, e++) {
        int dimA = cache.m_dimA;
        if (dimA != e->unk0) {
            continue;
        }
        int dimB = cache.m_dimB;
        if (dimB != e->unk1) {
            continue;
        }
        int dim = dimA + dimB;
        if (cache.m_vertices[0] != hkGskManifold_vertexId(manifold, e->m_vertexOffsets[0])) {
            continue;
        }
        if (cache.m_vertices[1] != hkGskManifold_vertexId(manifold, e->m_vertexOffsets[1])) {
            continue;
        }
        if (dim > 2 && cache.m_vertices[2] != hkGskManifold_vertexId(manifold, e->m_vertexOffsets[2])) {
            continue;
        }
        if (dim <= 3 || cache.m_vertices[3] == hkGskManifold_vertexId(manifold, e->m_vertexOffsets[3])) {
            goto found;
        }
    }
    if (cache.m_dimA == 1 || cache.m_dimB == 1) {
        for (i = 0; i < manifold->m_countC; i++, points++) {
            if (cache.m_dimA == 1 && points->unk0 == 1 &&
                cache.m_vertices[0] == hkGskManifold_vertexId(manifold, points->m_vertexOffsets[0])) {
                points->unk0 = 0;
                points->unk1 = 0;
                break;
            }
            if (cache.m_dimB == 1 && points->unk1 == 1 &&
                cache.m_vertices[cache.m_dimA] ==
                    hkGskManifold_vertexId(manifold, points->m_vertexOffsets[points->unk0])) {
                points->unk0 = 0;
                points->unk1 = 0;
                break;
            }
        }
    }
    return 0;

found:
    if (i > 0) {
        hkGskManifoldEntry t = manifold->m_points[i];
        manifold->m_points[i] = manifold->m_points[0];
        manifold->m_points[0] = t;
    }
    return 1;
}

#include <havok/hkPredGskAgent3.h>

hkResult hkGskManifoldWork::convertPointPoint(hkGskManifoldEntry& point, hkGskManifoldPointContact& out) const {
    int offsetA = point.m_vertexOffsets[0];
    int offsetB = point.m_vertexOffsets[1];
    const hkVector4& a = *(const hkVector4*)((const u8*)m_vertices + offsetA);
    const hkVector4& b = *(const hkVector4*)((const u8*)m_vertices + offsetB);
    hkVector4 separation;
    separation.setSub4(a, b);
    if (separation.lengthSquared3() > m_maxDistanceSquared) {
        return 1;
    }
    out.m_separation = separation;
    out.m_separatingNormal = m_separatingNormal;
    out.m_position = a;
    return 0;
}
