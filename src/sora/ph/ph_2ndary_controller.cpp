#include <nw4r/g3d/g3d_resmdl.h>
#include <gf/gf_application.h>
#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_algorithm.h>
#include <mt/mt_common.h>
#include <math.h>
#include <types.h>
// Brawl physics wrapper translation unit ph_2ndary_controller.o (main.dol 0x8008AC5C-0x80095FC4).
// Not yet decompiled. Functions in address order with their map names:
//   0x8008AC5C   140  __ct
//   0x8008ACE8   124  __dt
//   0x8008AD64   300  copy2ndaryMatrix
//   0x8008AE90   380  __ct
//   0x8008B00C   788  __dt
//   0x8008B320    20  calacCallBackTimingCFor2ndary
//   0x8008B334  9596  ph2ndaryMain
//   0x8008D8B0  1592  call1Line
//   0x8008DEE8   244  get2ndaryLastNode
//   0x8008DFDC   832  get2ndaryLastNode
//   0x8008E31C    88  setUndispNode
//   0x8008E374    88  setUndispNode
//   0x8008E3CC   284  get2ndaryUndispNode
//   0x8008E4E8   948  get2ndaryUndispNode
//   0x8008E89C   672  get2ndaryUndispNodeSamus
//   0x8008EB3C  1352  setLastNodeHit
//   0x8008F084   428  initCallBack
//   0x8008F230   312  getWeaponLineCount
//   0x8008F368   120  getWeaponStartID
//   0x8008F3E0  3276  setWeaponStat
//   0x800900AC   176  setLastVelocity
//   0x8009015C  6560  lastNodeMove
//   0x80091AFC  1068  setWeaponFallWire
//   0x80091F28   808  setWeaponFall
//   0x80092250   456  setWeaponFall2
//   0x80092418   196  setWeaponType
//   0x800924DC   120  set2Vector
//   0x80092554  4708  pos2MoveSub
//   0x800937B8  4040  pos2MoveSub2
//   0x80094780  3636  pos2Connect
//   0x800955B4   584  setPower
//   0x800957FC   404  initNode
//   0x80095990   264  applyWinding
//   0x80095A98    60  resetWinding
//   0x80095AD4   136  setAwake
//   0x80095B5C   120  setSleep
//   0x80095BD4    24  isHitCollisionLastNode2Pos
//   0x80095BEC   900  applyFeedback
//   0x80095F70     4  processDefault
//   0x80095F74     4  draw
//   0x80095F78    28  isHitCollisionLastNode2Pos
//   0x80095F94    48  applyFeedback

#include <ph/ph2ndaryWorld.h>
#include <ph/ph_Interface.h>
#include <ph/ph_2ndary_controller.h>
#include <ph/ph_2ndary_constraint.h>

#include <nw4r/g3d/g3d_scnmdl.h>
#include <ut/ut_nw.h>

extern u8 lbl_8059DC08;
extern u8 lbl_8059DC09;
extern phInterface* lbl_8049DEC8[];

// ph2ndary2Controller: pair of ph2ndaryController objects created through phInterface.
ph2ndary2Controller::ph2ndary2Controller(void* a, void* b) {
    m_controller[0] = nullptr;
    m_controller[1] = nullptr;
    unk08[0] = 0;
    unk08[1] = 0;
    unk08[2] = 0;
    m_controller[0] = phInterface::get2ndaryController(a, 0, 0);
    m_controller[1] = phInterface::get2ndaryController(b, 0, 0);
    m_controller[0]->m_active = 1;
    m_controller[1]->m_active = 1;
}

ph2ndary2Controller::~ph2ndary2Controller() {
    delete m_controller[0];
    m_controller[0] = nullptr;
    delete m_controller[1];
    m_controller[1] = nullptr;
}

void ph2ndaryController::calacCallBackTimingCFor2ndary(int a, int b, int c, int d) {
    ph2ndaryMain(a, b, d, 0, -1, c);
}

int ph2ndaryController::get2ndaryLastNode(u32* nodes, nw4r::g3d::ResMdl resMdl) {
    u32* out;
    int count = 0;
    if (unk0D) {
        for (int i = m_lines.getSize() - 1; i >= 0; i--) {
            if (m_lines[i]->m_array.m_size == 0) {
                *nodes++ = m_lines[i]->m_id;
                count++;
            }
        }
    } else {
        out = nodes;
        for (int i = m_lines.getSize() - 1; i >= 0; i--) {
            ph2ndaryLine* line = m_lines[i];
            if (line->m_array.m_size == 0) {
                nw4r::g3d::ResNode node = resMdl.GetResNode(line->m_nodeName);
                *out++ = node.IsValid() ? node.ptr()->m_nodeIndex : 0;
                count++;
            }
        }
    }
    return count;
}

void ph2ndaryController::setUndispNode(nw4r::g3d::ScnMdl* scnMdl) {
    u32 nodes[200];
    int count = get2ndaryUndispNode(nodes, scnMdl->m_resMdl);
    if (count != 0) {
        nwSMSetNodeVisibility(scnMdl, count, nodes, false);
    }
}

int ph2ndaryController::get2ndaryUndispNode(u32* nodes, nw4r::g3d::ResMdl resMdl) {
    int count = 0;
    if (!unk0D) {
        return 0;
    }
    u8 type = m_active;
    if (type == 0) {
        return 0;
    }
    if (type == 2) {
        return 0;
    }
    if (unk42 > m_lines.getSize()) {
        return 0;
    }
    int i;
    int skip;
    if (type == 1 && !unk57 && unk50 != 0.0f) {
        skip = 2;
    } else {
        skip = 1;
    }
    for (i = 0; i < m_lines.getSize() - unk43 - skip; i++) {
        nodes[i] = m_lines[i]->m_id;
        count++;
    }
    u32* undisp = &nodes[count];
    for (; i < m_lines.getSize(); i++) {
        if (m_lines[i]->m_winding) {
            *undisp++ = m_lines[i]->m_id;
            count++;
        }
    }
    return count;
}

int ph2ndaryController::getWeaponLineCount() {
    u8 limit = unk43;
    int count = 0;
    for (int i = 0; i < m_lines.getSize(); i++) {
        if (!unk55) {
            if (m_lines[i]->m_winding) {
                count = 0;
            } else {
                count++;
            }
        } else {
            if (m_lines[i]->m_winding) {
                if (count == 1 && unk5D) {
                    return 0;
                }
                break;
            }
            count++;
        }
    }
    if (count < limit) {
        if (m_active == 1 && !unk55 && count != 0) {
            return count - 1;
        }
        if (m_active == 1 && unk55 && count != 0) {
            return count - 1;
        }
        return count;
    }
    if (m_active == 1 && !unk55 && limit != 0) {
        return limit - 1;
    }
    if (m_active == 1 && unk55 && limit != 0) {
        return limit - 1;
    }
    return limit;
}

u32 ph2ndaryController::getWeaponStartID(int index) {
    int size = m_lines.getSize();
    if (size > index) {
        int i = size - index - 1;
        if (i < size) {
            return m_lines[i]->m_id;
        }
        return m_lines[size - 1]->m_id;
    }
    if (size > 0) {
        return m_lines[0]->m_id;
    }
    return m_lines[size - 1]->m_id;
}

void ph2ndaryController::setLastVelocity(const Vec3f* velocity, int lineIndex, u8 a) {
    m_active = 1;
    unk41 = 0;
    m_lastVelocity = *velocity;
    if (lineIndex > m_lines.getSize() - 1) {
        unk42 = m_lines.getSize() - 1;
    } else {
        unk42 = lineIndex;
    }
    unk43 = 0;
    unk50 = 0.0f;
    unk57 = 0;
    unk58 = 0;
    unk59 = 0;
    unk5A = 0;
    unk5B = a;
    if (m_lastVelocity.m_x < 0.0f) {
        unk56 = 1;
    } else {
        unk56 = 0;
    }
    if (!unk20 && lbl_8059DC08) {
        unk0E = 1;
    }
    unk20 = lbl_8059DC08;
}

void ph2ndaryController::setWeaponType(int type) {
    unkE9 = 1;
    m_active = type;
    Vec3f zero(0.0f, 0.0f, 0.0f);
    m_lastVelocity = zero;
    unk56 = 0;
    unk50 = 0.0f;
    if (m_active == 2) {
        unk41 = 0;
        PSMTXIdentity(m_vectorMtx1);
        PSMTXIdentity(m_vectorMtx2);
        unk57 = 0;
        unk43 = m_lines.getSize();
        unk42 = m_lines.getSize();
        unk59 = 0;
        unkE8 = 1;
    } else {
        unk43 = 0;
        unk42 = 0;
        unk57 = 0;
        unk59 = 0;
    }
    unk30 = m_lines.getSize() - 1;
}

void ph2ndaryController::set2Vector(const Vec3f* first, const Vec3f* second) {
    Mtx mtx;
    PSMTXIdentity(mtx);
    PSMTXTransApply(mtx, m_vectorMtx1, first->m_x, first->m_y, first->m_z);
    PSMTXTransApply(mtx, m_vectorMtx2, second->m_x, second->m_y, second->m_z);
}

void ph2ndaryController::resetWinding() {
    for (int i = 0; i < m_lines.getSize(); i++) {
        m_lines[i]->m_winding = 0;
    }
    m_winding = 0;
}

void ph2ndaryController::setAwake(bool awake, u32 lineId) {
    if (awake) {
        if (!unk0D) {
            m_awake = 0;
            return;
        }
        int i;
        m_awakeIndex = 0;
        for (i = 0; i < m_lines.getSize(); i++) {
            if (lineId == m_lines[i]->m_id) {
                m_awakeIndex = i;
                break;
            }
        }
        if (i == m_lines.getSize()) {
            m_awake = 0;
            return;
        }
    }
    m_awake = awake;
}

void ph2ndaryController::setSleep(bool sleep) {
    if (!unk20) {
        return;
    }
    if (sleep && !m_sleep) {
        for (int i = 0; i < m_sleepFlagCount; i++) {
            m_sleepFlags[i] = 1;
        }
    }
    if (!sleep) {
        for (int i = 0; i < m_sleepFlagCount; i++) {
            m_sleepFlags[i] = 0;
        }
    }
    m_sleep = sleep;
}

bool ph2ndaryController::isHitCollisionLastNode2Pos(u32 mask) {
    return (m_flags & mask) != 0;
}

void ph2ndaryController::processDefault() {
}

void ph2ndaryController::draw() {
}

bool ph2ndary2Controller::isHitCollisionLastNode2Pos() {
    return (m_controller[0]->m_flags & 7) != 0;
}

void ph2ndary2Controller::setUndispNode(nw4r::g3d::ScnMdl* scnMdl) {
    u32 nodes[200];
    int count = get2ndaryUndispNode(nodes, scnMdl->m_resMdl);
    if (count != 0) {
        nwSMSetNodeVisibility(scnMdl, count, nodes, false);
    }
}

void ph2ndaryController::initNode(nw4r::math::MTX34* matrices, nw4r::g3d::ResMdl resMdl) {
    if (unk20) {
        if (!unk0F[0]) {
            ph2ndaryWorld* world = lbl_8049DEC8[0]->m_2ndaryWorld;
            unk24 = (world ? world : 0)->addConstraintArray(this, &lbl_8059DC09);
            unk0F[0] = 1;
        }
        if (!unk0D) {
            initCallBack(matrices, resMdl);
            unk0D = 1;
        }
        if (unk0E) {
            int i = 0;
            if (m_lines.getSize() > 0) {
                for (; i < m_lines.getSize(); i++) {
                    nw4r::g3d::ResNode node = resMdl.GetResNode(m_lines[i]->m_id);
                    u32 index = node.IsValid() ? node.ptr()->m_id : 0;
                    m_lines[i]->unkDC = matrices[index];
                    m_lines[i]->updateMatrixForMyPosture();
                }
            }
            unk0E = 0;
        }
    }
}

void ph2ndary2Controller::applyFeedback(f32 scale, int a, nw4r::g3d::ResMdl resMdl, Vec3f* position) {
    m_controller[1]->applyFeedback(scale, a, resMdl, position);
}

// TODO: clean up, give variables real names
namespace jj_80095990 {
struct Entry {
    char pad0[0xB0];
    float value;
    char padB4[0x110];
    unsigned char marked;
};
struct Context {
    Entry** entries;
    int count;
    char pad8[0x4D];
    unsigned char reverse;
    char pad56[6];
    unsigned char active;
    unsigned char empty;
};
extern "C" const float lbl_805A1CA8;
// TODO: clean up, give variables real names
extern "C" void fn_80095990(Context* self, int* __restrict count) {
    switch (self->reverse) {
    case 0: {
        self->active = 1;
        int remaining = *count;
        for (int i = 0; i < self->count; ++i) {
            if (remaining > 0) {
                Entry* entry = self->entries[i];
                if (lbl_805A1CA8 != entry->value && entry->marked == 0) {
                    --remaining;
                    entry->marked = 1;
                    if (remaining == 0) return;
                }
            }
        }
        if (remaining == *count) self->empty = 1;
    break;
    }
    default: {
        self->active = 1;
        int remaining = *count;
        for (int i = self->count - 1; i >= 0; --i) {
            if (remaining > 0) {
                Entry* entry = self->entries[i];
                if (lbl_805A1CA8 != entry->value && entry->marked == 0) {
                    --remaining;
                    entry->marked = 1;
                    if (remaining == 0) return;
                }
            }
        }
        if (remaining == *count) self->empty = 1;
    }
    }
}
}

namespace jj_8008AD64 {
typedef unsigned char u8;

struct Item { char pad[0x1d4]; float val; };
struct Obj {
    Item** items;
    int count;
    char pad[0x4];
    u8 unkC;
    char pad2[0x54 - 0xd];
    u8 unk54;
};
struct Self {
    Obj* a;
    Obj* b;
    u8 unk8;
    u8 unk9;
    u8 unkA;
};

extern "C" void fn_8009A8EC(Obj*, Obj*);
extern "C" void fn_8009ACA0(Obj*, Obj*);
extern "C" float lbl_805A1C98;
extern "C" float lbl_805A1C9C;

// TODO: clean up, give variables real names
extern "C" void fn_8008AD64(Self* self) {
    if (self->unk8 == 0) {
        if (self->unkA == 0) {
            fn_8009A8EC(self->a, self->b);
            float lo = lbl_805A1C98;
            float hi = lbl_805A1C9C;
            for (int i = 0; i < self->b->count; i++) {
                self->b->items[i]->val = lo +
                    ((float)i * (hi - lo)) / (float)self->b->count;
            }
        } else {
            fn_8009ACA0(self->a, self->b);
        }
        self->b->unk54 = 1;
    } else if (self->unkA == 0) {
        fn_8009A8EC(self->b, self->a);
    } else {
        fn_8009ACA0(self->b, self->a);
    }
    self->unk8 ^= 1;
    self->a->unkC = 0;
    self->b->unkC = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_80092250 {
extern "C" {
void fn_800937B8(void*, int);
void fn_8009BD44(void*, void*, void*);
void fn_801EC158(void*);
void fn_801EC868(void*, void*, float, float, float);
}

struct V3 {
    float x, y, z;
};

struct Pair {
    V3 b, a;
};

struct Blk {
    unsigned int w[12];
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80092250(char* self) {
    Blk blk;
    float f98[12];
    float f68[12];
    float f38[12];
    Pair v;
    unsigned int* q;
    unsigned int* q2;
    int cnt;

    q = *(unsigned int**)(*(char**)self);
    blk = *(Blk*)(q + 0x37);
    fn_8009BD44(&blk, f98, f68);

    cnt = *(int*)(self + 4);
    q2 = ((unsigned int**)(*(char**)self))[cnt - 1];
    v.a.x = f98[3];
    v.a.y = f98[7];
    v.a.z = f98[11];

    blk = *(Blk*)(q2 + 0x37);
    fn_8009BD44(&blk, f98, f68);

    v.b.x = f98[3];
    v.b.y = f98[7];
    v.b.z = f98[11];

    Pair p = v;
    fn_801EC158(f38);
    fn_801EC868(f38, self + 0x68, p.a.x, p.a.y, p.a.z);
    fn_801EC868(f38, self + 0x98, p.b.x, p.b.y, p.b.z);
    fn_800937B8(self, 1);
}
}

namespace jj_80091F28 {

extern "C" const float lbl_805A1CA8;
extern "C" const float lbl_805A1CDC;

struct Node {
    char pad0[0x34];
    nw4r::math::VEC3 previous;
    nw4r::math::VEC3 position;
    char pad4c[0x34];
    float limit;
    char pad84[0x2c];
    float value;
};

struct Object {
    Node** nodes;
    int count;
    char pad08[0x3b];
    unsigned char active;
    char pad44[0x11];
    unsigned char flag;
    char pad56[0xa];
    float value;
};

// TODO: clean up, give variables real names
extern "C" void fn_80091F28(Object* self) {
    self->value = self->nodes[1]->value;

    if (self->flag == 0) {
        for (int i = (int)((unsigned)self->count - self->active - 1U);
             i >= 0; --i) {
            self->nodes[i]->value = lbl_805A1CA8;
        }
    } else {
        if (self->active <= self->count && self->active >= 2) {
            nw4r::math::VEC3 delta;
            delta = self->nodes[self->active - 1]->position
                  - self->nodes[self->active - 2]->position;

            float sq = delta.z * delta.z
                     + (delta.x * delta.x + delta.y * delta.y);
            float length;
            if ((float)fabs(sq) <= lbl_805A1CDC) {
                length = lbl_805A1CA8;
            } else {
                length = sq * rsqrtf(sq);
            }

            Node* preceding = self->nodes[self->active - 2];
            if (length < preceding->limit) {
                delta = preceding->previous
                      - self->nodes[self->active - 1]->position;
                nw4r::math::VEC3Add(
                    &self->nodes[self->active - 1]->position,
                    &self->nodes[self->active - 1]->position,
                    &delta);
                nw4r::math::VEC3Add(
                    &self->nodes[self->active - 1]->previous,
                    &self->nodes[self->active - 1]->previous,
                    &delta);
            }
        }

        int i = self->active;
        while (i < self->count) {
            self->nodes[i]->value = lbl_805A1CA8;
            ++i;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8008EB3C {

struct Entry {
    f32 f[12];
};

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Obj {
    char _0[0x34];
    Vec3 v34;
    Vec3 v40;
    char _4C[0xC];
    Vec3 v58;
    char _64[0x1C];
    f32 f80;
    char _84[0x58];
    Entry ent;
    char _10C[0x94];
    u32 m1A0;
};

struct Self {
    Obj** objs;
    u32 count;
    char _08[0x5];
    u8 flagD;
    char _0E[0x1];
    u8 flagF;
    char _10[0x14];
    void* f24;
    char _28[0x18];
    u8 b40;
    char _41[0x18];
    u8 b59;
    char _5A[0x1];
    u8 b5B;
};

struct Mgr {
    char _0[0x58];
    void* m58;
};

struct GlobalMgrRef {
    Mgr* ptr;
    char _4[0x1C];
};

struct ResNodeData {
    char _0[0x10];
    s32 n10;
};

extern "C" GlobalMgrRef lbl_8049DEC8;
extern "C" char lbl_8059DBFA;
extern "C" char lbl_8059DBFC;
extern "C" const f32 lbl_805A1C98;

extern "C" {
void* fn_8008653C(void* mgr, Self* self, char* name);
void fn_8008F084(Self* self, Entry* arr, u32* p);
void fn_80086694(void* mgr, Vec3* v, char* name, Obj* o, f32 a, f32 b);
s32 fn_800867E8(void* mgr, Obj* o, u8 b, f32* out, f32 a, f32 c);
s32 fn_80086AC0(void* mgr, Obj* o, u8 b);
ResNodeData* GetResNode__Q34nw4r3g3d6ResMdlCFUl(const nw4r::g3d::ResMdl* mdl, u32 idx);
}

// TODO: clean up, give variables real names
static inline void* getMgr() {
    void* m = lbl_8049DEC8.ptr->m58;
    return m ? m : 0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
static inline bool isNaNf(f32& x) {
    u32 b = *(u32*)&x;
    return ((b >> 23) & 0xFF) == 0xFF && (b & 0x7FFFFF) != 0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8008EB3C(Self* self, Entry* arr, nw4r::g3d::ResMdl* mdl) {
    self->b59 = 0;
    if (self->flagF == 0) {
        self->f24 = fn_8008653C(getMgr(), self, &lbl_8059DBFA);
        self->flagF = 1;
    }
    if (self->flagD == 0) {
        u32 mtmp = *(u32*)mdl;
        fn_8008F084(self, arr, &mtmp);
        self->flagD = 1;
    }
    s32 i = (s32)self->count - 1;
    void* mgr = getMgr();
    ResNodeData* node = GetResNode__Q34nw4r3g3d6ResMdlCFUl(mdl, self->objs[i]->m1A0);
    s32 n = node ? node->n10 : 0;
    Entry* e = arr + n;
    self->objs[i]->ent = *e;
    Entry tmp = *e;

    f32 t11, t10, t9, t8, t7, t6, t5, t4, t3, t2, t1, t0;
    bool bad = false;
    if (!bad) bad = isNaNf(t0 = tmp.f[0]);
    if (!bad) bad = isNaNf(t1 = tmp.f[1]);
    if (!bad) bad = isNaNf(t2 = tmp.f[2]);
    if (!bad) bad = isNaNf(t3 = tmp.f[3]);
    if (!bad) bad = isNaNf(t4 = tmp.f[4]);
    if (!bad) bad = isNaNf(t5 = tmp.f[5]);
    if (!bad) bad = isNaNf(t6 = tmp.f[6]);
    if (!bad) bad = isNaNf(t7 = tmp.f[7]);
    if (!bad) bad = isNaNf(t8 = tmp.f[8]);
    if (!bad) bad = isNaNf(t9 = tmp.f[9]);
    if (!bad) bad = isNaNf(t10 = tmp.f[10]);
    if (!bad) bad = isNaNf(t11 = tmp.f[11]);

    if (!bad) {
        Vec3 p;
        f32 sp40[2];
        p.x = tmp.f[3];
        p.y = tmp.f[7];
        p.z = tmp.f[11];
        self->objs[i]->v34 = p;
        self->objs[i]->v40 = p;
        Obj* o2 = self->objs[i];
        o2->v58 = p;
        fn_80086694(mgr, &p, &lbl_8059DBFC, o2, tmp.f[7], tmp.f[3]);
        f32 s1 = self->objs[i]->f80;
        self->objs[i]->v34.x = self->objs[i]->v34.x + s1 * lbl_805A1C98;
        f32 s2 = self->objs[i]->f80;
        self->objs[i]->v40.x = self->objs[i]->v40.x - s2 * lbl_805A1C98;
        f32 s3 = self->objs[i]->f80;
        f32 q = s3 * lbl_805A1C98;
        self->objs[i]->v58.x = self->objs[i]->v58.x - q;
        Obj* o = self->objs[i];
        s32 r29 = fn_800867E8(mgr, o, self->b5B, sp40, q, lbl_805A1C98);
        s32 r3 = fn_80086AC0(mgr, o, self->b40);
        if (r29 != 0) r3 = 1;
        if (r29 != 0) {
            self->b59 = 2;
        } else if (r3 != 0) {
            self->b59 = 1;
        }
    }
}
}

namespace jj_8008D8B0 {

struct Matrix { u32 v[12]; };
struct Position { float x, y, z; };
struct Vec2 { float x, y; };

struct Node {
    char p0[0x34];
    Position pos;
    char p40[0x30];
    Position result;
    char p7c[0x34];
    float b0;
    char pb4[0x1c];
    float d0, d4;
    char pd8[4];
    Matrix dc;
    char p10c[0x30];
    Matrix m13c, m16c;
    char p19c[4];
    u32 index;
    char p1a4[0x5c];
    int end;
};

struct Self {
    Node** nodes;
    int count;
    char p8[5];
    u8 initModel, reset, init;
    char p10[0x10];
    u8 mode;
    char p21[3];
    int handle;
    char p28[4];
    int selected;
    char p30[8];
    int state;
    char p3c[0x28];
    u8 flag;
    char p65[0x84];
    u8 value;
};

struct Context {
    char pad[0x4c];
    int count;
    u8 flag;
};

struct Global {
    char pad[0x58];
    Context* ctx;
};

extern "C" Global* lbl_8049DEC8[9];
extern "C" char lbl_8059DBF9;
extern "C" const float lbl_805A1C98, lbl_805A1CA8;

extern "C" int fn_8008653C(Context*, Self*, char*);
extern "C" void fn_8008F084(Self*, Matrix*, nw4r::g3d::ResMdl*);
extern "C" void fn_80081DC4(Node*);
extern "C" void fn_80081EE0(Node*, Matrix*);
extern "C" void fn_80082130(Node*);
extern "C" void fn_8008674C(Context*, Vec2*, Vec2*);
extern "C" void fn_8003E388(Matrix*, Matrix*);
extern "C" void fn_8003E6DC(Matrix*, Position*);
extern "C" void fn_80083390(
    Context*, Node*, Node*, void*, int, bool, int, int,
    int, int, int, int, int, int, int, int);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8008D8B0(
    Self* self, Matrix* matrices, nw4r::g3d::ResMdl* model,
    int start, int count)
{
    if (!self->init) {
        Context* ctx = lbl_8049DEC8[0]->ctx;
        self->handle = fn_8008653C(ctx ? ctx : 0, self, &lbl_8059DBF9);
        self->init = 1;
    }

    Context* ctx = lbl_8049DEC8[0]->ctx;
    (ctx ? ctx : 0)->flag = self->value;

    ctx = lbl_8049DEC8[0]->ctx;
    ctx = ctx ? ctx : 0;
    if (count > 1)
        ctx->count = count;
    else
        ctx->count = 1;

    if (!self->initModel) {
        nw4r::g3d::ResMdl local = *model;
        fn_8008F084(self, matrices, &local);
        self->initModel = 1;
    }

    if (((u8*)g_gfApplication)[0xed] & 1) {
        int i;
        int offset = start * 4;
        int end = self->count - 1;
        for (int j = start; j < self->count; ++j) {
            if (self->nodes[j]->end == 0) {
                end = j;
                break;
            }
        }

        if (self->reset) {
            i = start;
            if (start <= end) {
                for (; i <= end; ++i) {
                    nw4r::g3d::ResNodeData* data =
                        model->GetResNode(self->nodes[i]->index).ptr();
                    u32 id = data ? *(u32*)((char*)data + 0x10) : 0;
                    self->nodes[i]->dc = matrices[id];
                    fn_80081DC4(self->nodes[i]);
                }
            }
        }

        count = start;
        if (start <= end) {
            for (; count <= end; ++count) {
                nw4r::g3d::ResNodeData* data =
                    model->GetResNode(self->nodes[count]->index).ptr();
                u32 id = data ? *(u32*)((char*)data + 0x10) : 0;
                Matrix* m = &matrices[id];

                self->nodes[count]->m13c = *m;
                fn_80081EE0(self->nodes[count], m);
                self->nodes[count]->m16c = *m;
                fn_80082130(self->nodes[count]);

                float a = lbl_805A1CA8;
                float b = a;
                if (self->mode == 2) {
                    a = self->nodes[count]->d0;
                    b = self->nodes[count]->d4;
                }

                if ((self->state == 0 &&
                     lbl_805A1CA8 == self->nodes[count]->b0) ||
                    (self->mode == 2 && a >= b * lbl_805A1C98) ||
                    (self->state == 1 &&
                     lbl_805A1CA8 == self->nodes[count]->b0 &&
                     self->nodes[count]->end == 0)) {
                    self->nodes[count]->dc = *m;
                    fn_80081DC4(self->nodes[count]);
                }
            }
        }

        Context* context = lbl_8049DEC8[0]->ctx;
        context = context ? context : 0;

        float minx, maxx, maxy, miny;
        miny = lbl_805A1CA8;
        maxy = miny;
        maxx = miny;
        minx = miny;
        int first = 0;

        for (int j = end; j >= start; --j) {
            Position pos = {
                self->nodes[j]->pos.x,
                self->nodes[j]->pos.y,
                self->nodes[j]->pos.z
            };
            if (!first) {
                maxx = pos.x;
                first = 1;
                minx = pos.x;
                miny = pos.y;
                maxy = pos.y;
            } else {
                if (pos.x < minx)
                    minx = pos.x;
                else if (maxx < pos.x)
                    maxx = pos.x;

                if (pos.y < miny)
                    miny = pos.y;
                else if (maxy < pos.y)
                    maxy = pos.y;
            }
        }

        Vec2 lo, hi;
        hi.x = maxx;
        hi.y = miny;
        lo.x = minx;
        lo.y = maxy;
        fn_8008674C(context, &lo, &hi);

        Node* selected;
        if (self->selected == -1) {
            selected = 0;
        } else {
            selected = self->nodes[self->selected];
            Matrix tmp;
            Position copy;
            Position out;
            fn_8003E388(&selected->dc, &tmp);
            fn_8003E6DC(&tmp, &out);
            copy.x = out.x;
            copy.y = out.y;
            copy.z = out.z;
            selected->result = copy;
        }

        fn_80083390(
            context, *(Node**)((char*)self->nodes + offset),
            selected, (char*)self + 0x3c,
            0, self->state == 0, 0, 0, 0, self->mode,
            0, 0, 0, 0, self->flag, 0);
    }
}
}

namespace jj_8008F3E0 {

struct Node {
    char pad0[0xb0];
    float value;
    char padb4[0x110];
    u8 marked;
    char pad1c5[3];
    nw4r::math::VEC3 first;
    char pad1d4[4];
    nw4r::math::VEC3 second;
};
struct Object {
    Node** nodes;
    int count;
    char pad08[4];
    u8 flag0c;
    char pad0d;
    u8 flag0e;
    char pad0f[0x11];
    u8 mode;
    char pad21[0x21];
    u8 active;
    u8 saved;
    char pad44[0x13];
    u8 state;
    char pad58[4];
    u8 flag5c;
    u8 flag5d;
    u8 flag5e;
    char pad5f;
    float value;
    char pad64[4];
    char matrix1[0x30];
    char matrix2[0x30];
    char padc8[0x20];
    u8 flage8;
};
struct Context {
    Object* a;
    Object* b;
    u8 flag8, flag9, flaga;
};
extern "C" {
void fn_8008AD64(Context*);
void fn_80091F28(Object*);
void fn_80091AFC(Object*);
int fn_8008F230(Object*);
void fn_80095990(Object*, int*);
void fn_801EC158(void*);
void fn_801EC868(void*, void*, float, float, float);
extern "C" u8 lbl_8059DC01, lbl_8059DC02, lbl_8059DC03, lbl_8059DC04, lbl_8059DC05, lbl_8059DC06, lbl_8059DC07;
extern "C" const float lbl_805A1CA8;
}
static inline void clearObject(Object* o) {
    for (int i = 0; i < o->count; i++) o->nodes[i]->marked = 0;
    o->flag5c = 0;
}
static inline void clearMarks(Context* self) {
    if (self->a->flag5c || self->b->flag5c) {
        clearObject(self->a);
        if (self->b) clearObject(self->b);
        Object* a = self->a;
        a->flag5c = 0;
        a->flag5d = 0;
        Object* b = self->b;
        b->flag5c = 0;
        b->flag5d = 0;
    }
}
// TODO: clean up, give variables real names
static inline void resetSecond(Object* o) {
    for (int i = 0; i < o->count; i++)
        o->nodes[i]->second = nw4r::math::VEC3(lbl_805A1CA8, lbl_805A1CA8, lbl_805A1CA8);
}
// TODO: clean up, give variables real names
static inline void resetFirst(Object* o) {
    for (int i = 0; i < o->count; i++)
        o->nodes[i]->first = nw4r::math::VEC3(lbl_805A1CA8, lbl_805A1CA8, lbl_805A1CA8);
}
// TODO: clean up, give variables real names
static inline void setMatrices(Object* target, nw4r::math::VEC3 v1, nw4r::math::VEC3 v2) {
    char m[0x30];
    fn_801EC158(m);
    fn_801EC868(m, target->matrix1, v1.x, v1.y, v1.z);
    fn_801EC868(m, target->matrix2, v2.x, v2.y, v2.z);
}
// TODO: clean up, give variables real names
static inline void restore(Object* o) {
    for (int i = 1; i < o->count; i++) {
        if (lbl_805A1CA8 == o->nodes[i]->value)
            o->nodes[i]->value = o->value;
    }
    o->flag0c = 0;
    o->flage8 = 0;
}

// TODO: clean up, give variables real names
extern "C" void fn_8008F3E0(Context* self, int mode) {
    Object* o = self->a;
    if (!o) return;
    if (mode == 2) {
        Object* other = self->b;
        int state = o->state;
        int kind;
        if (other && !o->mode && !other->mode) {
            if (state == 8) kind = 3;
            else kind = 6;
        } else if (state == 0) {
            if (o->flag5c) kind = 4;
            else kind = 0;
        } else if (state == 4) {
            if (o->flag5c) kind = 5;
            else kind = 1;
        } else if (state == 5) {
            kind = 2;
        } else if (state == 6) {
            if (o->flag5c) kind = 5;
            else kind = 1;
        } else if (state == 7) {
            kind = 6;
        } else if (state == 9) {
            kind = 7;
        } else {
            kind = 3;
            if (state == 10) kind = 8;
        }
        if (kind == 6 || state == 8) {
            o->saved = o->active;
            self->flag9 = 1;
            self->flag8 = 0;
            self->flaga = 1;
            self->a->flag0e = 1;
            __memcpy(&self->a->mode, &lbl_8059DC01, sizeof(u8));
        } else if (other) {
            fn_8008AD64(self);
            fn_80091F28(self->b);
        }
        clearMarks(self);
    } else if (mode == 1) {
        resetSecond(o);
        resetFirst(self->a);
        if (self->b) {
            if ((int)self->a->state == 5) {
                fn_8008AD64(self);
            } else {
                fn_80091F28(self->a);
                self->a->state = 4;
                if (self->b) self->b->state = 5;
            }
        }
        clearMarks(self);
    } else if (mode == 0) {
        restore(o);
        restore(self->b);
        self->flag8 = 0;
        resetSecond(self->a);
        resetFirst(self->a);
        if ((int)self->a->state == 8) self->a->flag0e = 1;
        self->a->state = 0;
        if (self->b) self->b->state = 0;
        clearMarks(self);
    } else if (mode == 8) {
        resetSecond(o);
        resetFirst(self->a);
        if (self->b) {
            if ((int)self->a->state == 5) {
                fn_8008AD64(self);
            } else {
                fn_80091AFC(self->a);
                fn_80091F28(self->a);
                self->a->state = 10;
                if (self->b) self->b->state = 5;
            }
        }
        clearMarks(self);
    } else if (mode == 7) {
        restore(o);
        restore(self->b);
        self->flag8 = 0;
        resetSecond(self->a);
        resetFirst(self->a);
        setMatrices(self->a,
            nw4r::math::VEC3(lbl_805A1CA8, lbl_805A1CA8, lbl_805A1CA8),
            nw4r::math::VEC3(lbl_805A1CA8, lbl_805A1CA8, lbl_805A1CA8));
        if ((int)self->a->state == 8) self->a->flag0e = 1;
        self->a->state = 9;
        if (self->b) self->b->state = 0;
        clearMarks(self);
        __memcpy(&self->a->mode, &lbl_8059DC02, sizeof(u8));
        if (self->b) self->b->mode = lbl_8059DC03;
    } else if (mode == 3) {
        if ((int)o->state == 5) {
            if (self->b) fn_8008AD64(self);
            clearMarks(self);
        }
        self->a->state = 8;
        __memcpy(&self->a->mode, &lbl_8059DC04, sizeof(u8));
        if (self->b) self->b->mode = lbl_8059DC05;
    } else if (mode == 6) {
        o->mode = lbl_8059DC06;
        if (self->b) self->b->mode = lbl_8059DC07;
    } else if (mode == 4) {
        if (!o->flag5c) {
            o->flag5e = o->count - o->saved;
            int n = o->flag5e;
            fn_80095990(o, &n);
            o->flag5c = 1;
            if (self->b) {
                u8 count = fn_8008F230(self->b);
                Object* b = self->b;
                b->flag5e = count;
                int n2 = count;
                fn_80095990(b, &n2);
                b->flag5c = 1;
            }
        }
    } else if (mode == 5) {
        if (o->flag5c || self->b->flag5c) {
            clearObject(o);
            if (self->b) clearObject(self->b);
        }
        Object* b = self->b;
        b->flag5c = 1;
        b->flag5d = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8008B00C {
extern "C" void __dl__FPv(void*);
extern "C" void removeConstraintArray__13ph2ndaryWorldFPv(void*, void*);
extern "C" void* __dt__12ph2ndaryLineFv(void*, int);
extern "C" void* getArray2ndaryController__11phInterfaceFv(void*);
extern "C" void delete2ndaryWorld__11phInterfaceFv(void*);
extern "C" void delete2ndaryController__11phInterfaceFv(void*);
extern "C" void deallocateChunk__14hkThreadMemoryFPvii(void*, void*, unsigned int, int);
extern "C" void* lbl_8049DEC8[3];
extern "C" void* s_instance__14hkThreadMemory;
struct Mgr { char pad[0x58]; void* f58; };
struct Obj { char pad[0x1fc]; void** data; int size; };
struct Array {
    void** data; int size; unsigned int raw;
    unsigned int rawValue() const { return raw; }
};
struct Self { Array a; char pad1[0xcc - sizeof(Array)]; Array b; Array c; };
struct Reg {
    void** data; int size; unsigned int raw;
    int sizeValue() const { return size; }
    unsigned int rawValue() const { return raw; }
};
extern "C" Self* __dt__18ph2ndaryControllerFv(Self* self, int flag) {
    if (self) {
        Obj* obj; int j; int off; int joff; int i;
        off = 0;
        void* x = ((Mgr*)lbl_8049DEC8[0])->f58;
        void* t = x ? x : 0;
        if (t) { removeConstraintArray__13ph2ndaryWorldFPv(t, self); }
        i = 0;
        off = 0;
        while (i < self->a.size) {
            obj = *(Obj**)((char*)self->a.data + off);
            if (obj) {
                j = 0; joff = 0;
                while (j < obj->size) {
                    void* p = *(void**)((char*)obj->data + joff);
                    if (p) {
                        if (p) { __dl__FPv(p); }
                        *(void**)((char*)obj->data + joff) = 0;
                    }
                    joff += 4; j++;
                }
            }
            __dt__12ph2ndaryLineFv(*(void**)((char*)self->a.data + off), 1);
            i++;
            *(void**)((char*)self->a.data + off) = 0;
            off += 4;
        }
        if (!(self->b.raw & 0x80000000)) {
            deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, self->b.data, self->b.rawValue() << 2, 0x15);
        }
        self->b.data = 0; self->b.size = 0;
        self->b.raw = (self->b.raw & 0x40000000) | 0x80000000;
        if (!(self->c.raw & 0x80000000)) {
            deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, self->c.data, self->c.rawValue() & 0x3fffffff, 0x15);
        }
        self->c.data = 0; self->c.size = 0;
        self->c.raw = (self->c.raw & 0x40000000) | 0x80000000;
        if (!(self->a.raw & 0x80000000)) {
            deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, self->a.data, self->a.rawValue() << 2, 0x15);
        }
        self->a.data = 0; self->a.size = 0;
        self->a.raw = (self->a.raw & 0x40000000) | 0x80000000;
        Reg* reg = (Reg*)getArray2ndaryController__11phInterfaceFv(lbl_8049DEC8[0]);
        int index = 0;
        int byteOffset = 0;
        for (int j = 0; j < reg->size; j++, index++) {
            if (*(void**)((char*)reg->data + byteOffset) == (void*)self) {
                if (reg->sizeValue() == 1) {
                    if (!(reg->raw & 0x80000000)) {
                        deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, reg->data, reg->rawValue() << 2, 0x15);
                    }
                    reg->data = 0; reg->size = 0;
                    reg->raw = (reg->raw & 0x40000000) | 0x80000000;
                } else {
                    reg->size--;
                    reg->data[index] = reg->data[reg->size];
                }
                break;
            }
            byteOffset += 4;
        }
        delete2ndaryWorld__11phInterfaceFv(lbl_8049DEC8[0]);
        delete2ndaryController__11phInterfaceFv(lbl_8049DEC8[0]);
        if ((unsigned int)self + 0xd8 != 0 && !(self->c.raw & 0x80000000)) {
            deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, self->c.data, self->c.rawValue() & 0x3fffffff, 0x15);
        }
        if ((unsigned int)self + 0xcc != 0 && !(self->b.raw & 0x80000000)) {
            deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, self->b.data, self->b.rawValue() << 2, 0x15);
        }
        if (self != 0 && !(self->a.raw & 0x80000000)) {
            deallocateChunk__14hkThreadMemoryFPvii(s_instance__14hkThreadMemory, self->a.data, self->a.rawValue() << 2, 0x15);
        }
        if (flag > 0) { __dl__FPv(self); }
    }
    return self;
}
}

