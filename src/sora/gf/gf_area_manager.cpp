#include <nw4r/ut/ut_LinkList.h>
#include <gf/gf_area.h>
#include <gf/gf_area_manager.h>
#include <gf/gf_heap_manager.h>
#include <memory.h>
#include <mt/mt_coll2d.h>
#include <new>
#include <types.h>

gfAreaManager* lbl_8059FF90;

void gfAreaManager::create(Heaps::HeapType heapType) {
    lbl_8059FF90 = new (heapType) gfAreaManager();
}

gfAreaManager::~gfAreaManager() {
    m_root.releaseChild();
}

void gfAreaManager::testIntersectSub(QuadTree* node) {
    for (u32 i = 0; i < 4; i++) {
        if (node->m_children[i] != NULL) {
            testIntersectSub(node->m_children[i]);
        }
    }
    if (!node->m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = node->m_areaList.GetBeginIter(); it != node->m_areaList.GetEndIter(); ++it) {
            gfArea* area = &*it;
            if (area->m_isDrawFill) {
                nw4r::ut::LinkList<gfArea, 0>::Iterator other = it;
                for (++other; other != node->m_areaList.GetEndIter(); ++other) {
                    if (other->m_isDrawFill) {
                        area->test(&*other);
                    }
                }
                if (node->m_parent != NULL) {
                    testParentNode(area, node->m_parent);
                }
            }
        }
    }
}

void gfAreaManager::testParentNode(gfArea* area, QuadTree* node) {
    for (gfArea* other = node->getArea(); other != NULL; other = reinterpret_cast<gfArea*>(other->m_0x0)) {
        area->testIntersect(other);
    }
    if (node->m_parent != NULL) {
        testParentNode(area, node->m_parent);
    }
}

gfArea* gfAreaManager::QuadTree::getArea() {
    return &m_areaList.GetFront();
}

void gfAreaManager::createWorld(float width, float height) {
    if (!m_root.m_isLeaf) {
        m_root.releaseChild();
    }
    m_root.m_parent = NULL;
    float x = -0.5f * width;
    float y = -0.5f * height;
    m_root.m_min.m_x = x;
    m_root.m_min.m_y = y;
    m_root.m_max.m_x = x;
    m_root.m_max.m_y = y;
    m_root.construct();
}

gfArea* gfAreaManager::testPosInclude(u8 group, const Vec3f& pos) {
    Vec2f pos2(pos);
    return testPosIncludeSub(&m_root, group, &pos2);
}

gfArea* gfAreaManager::testIDInclude(u8 group, u32 id) {
    if (!m_root.m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = m_root.m_areaList.GetBeginIter(); it != m_root.m_areaList.GetEndIter(); ++it) {
            if (it->m_isDrawFill && group == it->m_group && id == it->m_id) {
                return &*it;
            }
        }
    }
    return NULL;
}

gfArea* gfAreaManager::testPosIncludeSub(QuadTree* node, u8 group, Vec2f* pos) {
    u32 idx = node->getNodeIdxByPos(*pos);
    if (idx != 0xFF && node->m_children[idx] != NULL) {
        gfArea* area = testPosIncludeSub(node->m_children[idx], group, pos);
        if (area != NULL) {
            return area;
        }
    }
    if (!node->m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = node->m_areaList.GetBeginIter(); it != node->m_areaList.GetEndIter(); ++it) {
            if (it->m_isDrawFill && group == it->m_group && it->testIncludePos(pos)) {
                return &*it;
            }
        }
    }
    return NULL;
}

void gfAreaManager::registArea(gfArea* area) {
    const clAABBox2D& box = area->m_aabBox;
    QuadTree* node = &m_root;
    while (true) {
        if (node->m_isLeaf) {
            break;
        }
        u32 idx = node->checkChildNode(box);
        if (idx == 0xFF) {
            break;
        }
        QuadTree* child = node->m_children[idx];
        if (child == NULL) {
            break;
        }
        node = child;
    }
    node->m_areaList.PushBack(area);
}

void gfAreaManager::removeArea(gfArea* area) {
    const clAABBox2D& box = area->m_aabBox;
    QuadTree* node = &m_root;
    while (true) {
        if (node->m_isLeaf) {
            break;
        }
        u32 idx = node->checkChildNode(box);
        if (idx == 0xFF) {
            break;
        }
        QuadTree* child = node->m_children[idx];
        if (child == NULL) {
            break;
        }
        node = child;
    }
    node->m_areaList.Erase(area);
}

void gfAreaManager::forEachArea(gfAreaForeachProc* proc, u8 group) {
    forEachAreaSub(&m_root, proc, group);
}

void gfAreaManager::forEachAreaSub(QuadTree* node, gfAreaForeachProc* proc, u8 group) {
    for (u32 i = 0; i < 4; i++) {
        if (node->m_children[i] != NULL) {
            forEachAreaSub(node->m_children[i], proc, group);
        }
    }
    if (!node->m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = node->m_areaList.GetBeginIter(); it != node->m_areaList.GetEndIter(); ++it) {
            if (it->m_isDrawFill && group == it->m_group) {
                proc->exec(&*it);
            }
        }
    }
}

void gfAreaManager::renderDebug() {
    if (m_unk75 != 0) {
        m_root.debugDraw();
    }
}

void gfAreaManager::QuadTree::construct() {
    float width = m_max.m_x - m_min.m_x;
    float height = m_max.m_y - m_min.m_y;
    if (width > 128.0f && height > 128.0f) {
        QuadTree* children = static_cast<QuadTree*>(gfHeapManager::alloc(Heaps::System, sizeof(QuadTree) * 4));
        float x0 = m_min.m_x;
        float y0 = m_min.m_y;
        float x1 = m_max.m_x;
        float y1 = m_max.m_y;
        float cx = 0.5f * (x0 + x1);
        float cy = 0.5f * (y0 + y1);
        m_children[0] = new (&children[0]) QuadTree(x0, cy, cx, y1, this);
        m_children[1] = new (&children[1]) QuadTree(x0, y0, cx, cy, this);
        m_children[2] = new (&children[2]) QuadTree(cx, cy, x1, y1, this);
        m_children[3] = new (&children[3]) QuadTree(cx, y0, x1, cy, this);
        for (u32 i = 0; i < 4; i++) {
            m_children[i]->construct();
        }
        m_isLeaf = false;
    }
}

u32 gfAreaManager::QuadTree::checkChildNode(const clAABBox2D& box) {
    Vec2f center(0.5f * (m_min.m_x + m_max.m_x), 0.5f * (m_min.m_y + m_max.m_y));
    if (m_min.m_x < box.m_unk1 && box.m_unk3 < center.m_x) {
        if (center.m_y < box.m_unk2 && box.m_unk4 < m_max.m_y) {
            return 0;
        }
        if (m_min.m_y < box.m_unk2 && box.m_unk4 < center.m_y) {
            return 1;
        }
    }
    if (center.m_x < box.m_unk1 && box.m_unk3 < m_max.m_x) {
        if (center.m_y < box.m_unk2 && box.m_unk4 < m_max.m_y) {
            return 2;
        }
        if (m_min.m_y < box.m_unk2 && box.m_unk4 < center.m_y) {
            return 3;
        }
    }
    return 0xFF;
}

u32 gfAreaManager::QuadTree::getNodeIdxByPos(const Vec2f& pos) const {
    Vec2f center = (m_min + m_max) * 0.5f;
    if (m_min.m_x < pos.m_x && pos.m_x < center.m_x) {
        if (center.m_y < pos.m_y && pos.m_y < m_max.m_y) {
            return 0;
        }
        if (m_min.m_y < pos.m_y && pos.m_y < center.m_y) {
            return 1;
        }
    }
    if (center.m_x < pos.m_x && pos.m_x < m_max.m_x) {
        if (center.m_y < pos.m_y && pos.m_y < m_max.m_y) {
            return 2;
        }
        if (m_min.m_y < pos.m_y && pos.m_y < center.m_y) {
            return 3;
        }
    }
    return 0xFF;
}

void gfAreaManager::QuadTree::releaseChild() {
    if (!m_isLeaf) {
        QuadTree* children = m_children[0];
        for (u32 i = 0; i < 4; i++) {
            m_children[i]->releaseChild();
            m_children[i] = NULL;
        }
        gfHeapManager::free(children);
    }
}

void gfAreaManager::QuadTree::debugDraw() {
    if (!m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = m_areaList.GetBeginIter(); it != m_areaList.GetEndIter(); ++it) {
            if (it->m_unk1C_6) {
                it->debugDrawImp();
            }
        }
    }
    if (!m_isLeaf) {
        for (u32 i = 0; i < 4; i++) {
            m_children[i]->debugDraw();
        }
    }
}

// TODO: clean up, give variables real names
namespace jj_80012094 {
struct Obj {
    Obj* next;      // 0x00
    u8 pad04[0x10];
    Obj* unk14;     // 0x14
    u8 pad18[0x14];
    Obj* unk2C;     // 0x2C
};

extern "C" void fn_800110CC(void* ctx, Obj* node);
extern "C" Obj* fn_800121A8(Obj* node);

extern "C" void fn_80012094(void* arg0, void* arg1, Obj* arg2) {
    Obj* node;
    Obj* it;

    for (it = arg2->unk2C; it != NULL; it = it->next) {
        fn_800110CC(arg1, it);
    }
    node = arg2->unk14;
    if (node != NULL) {
        for (it = node->unk2C; it != NULL; it = it->next) {
            fn_800110CC(arg1, it);
        }
        node = node->unk14;
        if (node != NULL) {
            for (it = fn_800121A8(node); it != NULL; it = it->next) {
                fn_800110CC(arg1, it);
            }
            node = node->unk14;
            if (node != NULL) {
                for (it = fn_800121A8(node); it != NULL; it = it->next) {
                    fn_800110CC(arg1, it);
                }
                node = node->unk14;
                if (node != NULL) {
                    fn_80012094(arg0, arg1, node);
                }
            }
        }
    }
}
}

namespace jj_800121B0 {
extern "C" {
void fn_800132E8(void*);
void fn_80012E18(void*);
extern const float lbl_805A13B0;
}

extern "C" void fn_800121B0(char* self, float f0, float f1) {
    unsigned int v = !((self[0x51] >> 7) & 1);
    if (v) {
        if (v) {
            void* c; unsigned int k; void* b; unsigned int j; unsigned int i; void* a; char* p; char* q; char* r;
            a = *(void**)(self + 0x58);
            p = self;
            i = 0;
            do {
                q = *(char**)(p + 0x58);
                if (!(((unsigned char)q[0x11] >> 7) & 1)) {
                    b = *(void**)(q + 0x18);
                    j = 0;
                    do {
                        r = *(char**)(q + 0x18);
                        if (!(((unsigned char)r[0x11] >> 7) & 1)) {
                            c = *(void**)(r + 0x18);
                            k = 0;
                            do {
                                fn_800132E8(*(void**)(r + 0x18));
                                ++k;
                                *(void**)(r + 0x18) = 0;
                                r += 4;
                            } while (k < 4);
                            gfHeapManager::free(c);
                        }
                        ++j;
                        *(void**)(q + 0x18) = 0;
                        q += 4;
                    } while (j < 4);
                    gfHeapManager::free(b);
                }
                ++i;
                *(void**)(p + 0x58) = 0;
                p += 4;
            } while (i < 4);
            gfHeapManager::free(a);
        }
    }

    *(float*)(self + 0x40) = lbl_805A13B0 * f0;
    *(float*)(self + 0x44) = lbl_805A13B0 * f1;
    *(float*)(self + 0x48) = lbl_805A13B0 * f0;
    *(float*)(self + 0x4c) = lbl_805A13B0 * f1;
    *(s32*)(self + 0x54) = 0;
    fn_80012E18((void*)(self + 0x40));
}
}

// TODO: clean up, give variables real names
namespace jj_80012C6C {

using namespace nw4r::ut;

class List : public nw4r::ut::detail::LinkListImpl {
public:
    using nw4r::ut::detail::LinkListImpl::Erase;
    using nw4r::ut::detail::LinkListImpl::Insert;
    using nw4r::ut::detail::LinkListImpl::GetEndIter;
};

struct Rect {
    f32 left;
    f32 top;
    f32 right;
    f32 bottom;
};

struct Words {
    u32 a;
    u32 b;
    u32 c;
    u32 d;
};

struct Node {
    f32 x;
    f32 y;
    f32 x2;
    f32 y2;
    u8 pad[1];
    u8 flags;
    u8 pad2[2];
    Node* parent;
    Node* child[4];
    List list;
};

struct Object {
    char pad[0x2c];
    Rect rect;
};

extern "C" int fn_80041920(float*, float*);
extern "C" unsigned int fn_800130A8(void*, void*);

extern "C" void fn_80012C6C(Node* self, LinkListNode* obj) {
    Node* cur = self;
    Rect key = ((Object*)obj)->rect;
    Rect* keyp = &key;
    Rect temp;
    Rect bounds;

    {
        register f32 t0, p1, p2, t3, p4, p5;
        p4 = self->x2;
        p5 = self->x;
        p1 = self->y2;
        p2 = self->y;
        asm { ps_sub t3, p5, p4 }
        asm { ps_sub t0, p2, p1 }
        asm { ps_sel t3, t3, p4, p5 }
        asm { ps_sel t0, t0, p1, p2 }
        bounds.left = t3;
        bounds.top = t0;
    }
    {
        register f32 t0, p1, p2, t3, p4, p5;
        p4 = self->x2;
        p5 = self->x;
        p1 = self->y2;
        p2 = self->y;
        asm { ps_sub t3, p5, p4 }
        asm { ps_sub t0, p2, p1 }
        asm { ps_sel t3, t3, p5, p4 }
        asm { ps_sel t0, t0, p2, p1 }
        bounds.right = t3;
        bounds.bottom = t0;
    }

    for (;;) {
        if (cur->parent == 0 || fn_80041920(&bounds.left, &keyp->left)) {
            break;
        }

        cur = cur->parent;
        {
            register f32 dy2, dx2, dy1, y2, y, dx1, x2, x;
            x2 = cur->x2;
            x = cur->x;
            y2 = cur->y2;
            y = cur->y;
            asm { ps_sub dx1, x, x2 }
            asm { ps_sub dx2, x, x2 }
            asm { ps_sub dy1, y, y2 }
            asm { ps_sub dy2, y, y2 }
            asm { ps_sel dx1, dx1, x2, x }
            asm { ps_sel dy1, dy1, y2, y }
            asm { ps_sel dx2, dx2, x, x2 }
            asm { ps_sel dy2, dy2, y, y2 }
            temp.left = dx1;
            temp.top = dy1;
            temp.right = dx2;
            temp.bottom = dy2;
        }
        *(Words*)&bounds = *(Words*)&temp;
    }

    for (;;) {
        if ((cur->flags >> 7) & 1) {
            break;
        }

        unsigned int idx = fn_800130A8(cur, &key);
        if (idx == 0xff || !cur->child[idx]) {
            break;
        }
        cur = cur->child[idx];
    }

    if (cur != self) {
        self->list.Erase(obj);
        cur->list.Insert(cur->list.GetEndIter(), obj);
    }
}
}

namespace jj_80012660 {

struct Bounds { u32 v[4]; };
struct Rect { f32 v[4]; };
struct Class_804207E4_base {
    struct Class_804207E4* next;
    char pad4[0x18];
    u8 flag7 : 1;
    u8 flag6 : 1;
    u8 dirty : 1;
    u8 flagRest : 5;
    char pad1D[0xF];
    Bounds bounds;
    char pad3C[8];
};
class Class_804207E4 : public Class_804207E4_base {
public:
    virtual void fn_80011148();
    virtual void fn_80011170();
    virtual void fn_80011150();
    virtual void fn_80011C2C();
};
struct Tree {
    f32 rect[4];
    char pad10;
    u8 leaf : 1;
    u8 rest : 7;
    char pad12[2];
    Tree* parent;
    Tree* children[4];
    u32 size;
    Class_804207E4* head;
    void* prev;
};
class ListAccess : public nw4r::ut::detail::LinkListImpl {
public:
    using nw4r::ut::detail::LinkListImpl::Erase;
    using nw4r::ut::detail::LinkListImpl::Insert;
};
extern "C" {
int fn_80041920(Rect*, Bounds*);
u32 fn_800130A8(Tree*, Bounds*);
void fn_80012C6C(Tree*, Class_804207E4*);
void fn_800129FC(Tree*);
void fn_80011F0C(char*, Tree*);
}
inline void bounds(Rect* out, f32* e) {
    {
        register f32 p5, p4, t3, p2, p1, t0;
        p4 = e[2]; p5 = e[0]; p1 = e[3]; p2 = e[1];
        asm { ps_sub t3, p5, p4 }
        asm { ps_sub t0, p2, p1 }
        asm { ps_sel t3, t3, p4, p5 }
        asm { ps_sel t0, t0, p1, p2 }
        out->v[0] = t3; out->v[1] = t0;
    }
    {
        register f32 p5, p4, t3, p2, p1, t0;
        p4 = e[2]; p5 = e[0]; p1 = e[3]; p2 = e[1];
        asm { ps_sub t3, p5, p4 }
        asm { ps_sub t0, p2, p1 }
        asm { ps_sel t3, t3, p5, p4 }
        asm { ps_sel t0, t0, p2, p1 }
        out->v[2] = t3; out->v[3] = t0;
    }
}
inline void process(Tree* c) {
    Class_804207E4* end = (Class_804207E4*)&c->head;
    Class_804207E4* m = c->head;
    while (m != end) {
        m->fn_80011C2C();
        if (m->dirty) {
            fn_80012C6C(c, m);
            m->dirty = 0;
        }
        m = m->next;
    }
    for (u32 j = 0; j < 4; j++) {
        if (c->children[0]) fn_800129FC(c->children[0]);
        c = (Tree*)((char*)c + 4);
    }
}
extern "C" void fn_80012660(char* arg) {
    Bounds* bp;
    Class_804207E4* n;
    Tree* t;
    Tree* root;
    char* self = arg;
    if (*(u8*)(self + 0x74) == 0) {
        root = (Tree*)(self + 0x40);
        n = root->head;
        Bounds b;
        bp = &b;
        while (n != (Class_804207E4*)&root->head) {
            n->fn_80011C2C();
            if (n->dirty) {
                t = root;
                Rect tmp;
                Rect r;
                b = n->bounds;
                bounds(&r, root->rect);
            loop_parent:
                if (t->parent != 0 && fn_80041920(&r, bp) == 0) {
                    t = t->parent;
                    bounds(&tmp, t->rect);
                    r = tmp;
                    goto loop_parent;
                }
            loop_child:
                if (!t->leaf) {
                    u32 idx = fn_800130A8(t, &b);
                    if (idx != 0xFF) {
                        Tree* c = t->children[idx];
                        if (c != 0) {
                            t = c;
                            goto loop_child;
                        }
                    }
                }
                if (t != root) {
                    ((ListAccess*)&root->size)->Erase((nw4r::ut::LinkListNode*)n);
                    ((ListAccess*)&t->size)->Insert(nw4r::ut::detail::LinkListImpl::Iterator((nw4r::ut::LinkListNode*)&t->head), (nw4r::ut::LinkListNode*)n);
                }
                n->dirty = 0;
            }
            n = n->next;
        }
        for (u32 i = 0; i < 4; i++) {
            Tree* c = root->children[0];
            if (c != 0) process(c);
            root = (Tree*)((char*)root + 4);
        }
        fn_80011F0C(self, (Tree*)(self + 0x40));
    }
}
}

