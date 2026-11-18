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
extern const float lbl_805A1CA8;
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
extern float lbl_805A1C98;
extern float lbl_805A1C9C;

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

extern const float lbl_805A1CA8;
extern const float lbl_805A1CDC;

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

