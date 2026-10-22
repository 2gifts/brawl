#pragma once

#include <StaticAssert.h>
#include <gf/gf_area.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>

// HYPOTHESIS: layouts from the constructor, the tree walks and the debug draw of gf_area_manager.cpp
// (main.dol 0x80011C74-0x800134C0); field names other than the map's are local.
class gfAreaManager : public gfTask {
public:
    class QuadTree {
    public:
        Vec2f m_min;
        Vec2f m_max;
        u8 m_depth;
        bool m_isLeaf : 1;
        QuadTree* m_parent;
        QuadTree* m_children[4];
        nw4r::ut::LinkList<gfArea, 0> m_areaList;

        QuadTree(float minX, float minY, float maxX, float maxY, QuadTree* parent) : m_min(minX, minY), m_max(maxX, maxY), m_depth(0), m_isLeaf(true), m_parent(parent) {
            if (parent != NULL) {
                m_depth = parent->m_depth + 1;
            }
            m_children[0] = NULL;
            m_children[1] = NULL;
            m_children[2] = NULL;
            m_children[3] = NULL;
        }
        ~QuadTree() { releaseChild(); }

        gfArea* getArea();
        void update();
        static void relocate(QuadTree* node, gfArea& area);
        void construct();
        u32 checkChildNode(const clAABBox2D& box);
        u32 getNodeIdxByPos(const Vec2f& pos) const;
        void releaseChild();
        void debugDraw();
    };

    QuadTree m_root;
    u8 m_unk74;
    u8 m_unk75;

    gfAreaManager() : gfTask("AreaManager", Category_None, 2, 6, true), m_root(-1.0f, 1.0f, -1.0f, 1.0f, NULL), m_unk74(0), m_unk75(0) { }
    virtual void processFixPosition();
    virtual void renderDebug();
    virtual ~gfAreaManager();

    static void create(Heaps::HeapType heapType);
    void testIntersectSub(QuadTree* node);
    void testParentNode(gfArea* area, QuadTree* node);
    void createWorld(float width, float height);
    // HYPOTHESIS: parameter types from the sora_melee callers' register use.
    gfArea* testPosInclude(u8 group, const Vec3f& pos);
    gfArea* testIDInclude(u8 group, u32 id);
    gfArea* testPosIncludeSub(QuadTree* node, u8 group, Vec2f* pos);
    void registArea(gfArea* area);
    void removeArea(gfArea* area);
};
static_assert(sizeof(gfAreaManager::QuadTree) == 0x34, "Class is wrong size!");
static_assert(sizeof(gfAreaManager) == 0x78, "Class is wrong size!");

extern gfAreaManager* lbl_8059FF90;
