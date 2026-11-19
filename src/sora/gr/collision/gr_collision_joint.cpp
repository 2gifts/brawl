#include <revolution/GX/GXTypes.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_test_types.h>
#include <gr/ground.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>
#include <mt/mt_ps_minmax.h>

// HYPOTHESIS: set from grCollisionManager::processUpdate (the debug-check flag of the task).
extern bool lbl_805A0358;

// The lines of a collision are 18 bytes apart (grCollisionLine pads itself to 20), so they are walked through this view.
struct grCollLineSlot {
    short m_unk0;
    short m_unk2;
    short m_unk4;
    short m_unk6;
    u8 m_unk8;
    u8 m_unk9;
    short m_unkA;
    short m_unkC;
    short m_unkE;
    short m_unk10;
};
static_assert(sizeof(grCollLineSlot) == 18, "Class is wrong size!");

static inline grCollisionLine* lineAt(grCollisionLine* lines, int index) {
    return (grCollisionLine*)((grCollLineSlot*)lines + index);
}

u8 grCollisionJoint::getCollisionId() {
    return m_collision->m_index;
}

grCollisionJoint::grCollisionJoint(grCollision* collision, grCollData::JointData* data) {
    m_0x48 = 0;
    m_0x54_7 = false;
    m_0x54_6 = true;
    m_0x54_5 = false;
    m_0x54_3 = false;
    m_0x54_2 = false;
    m_0x55_6 = true;
    m_0x55_5 = true;
    m_0x55_4 = false;
    m_0x55_3 = false;
    m_0x55_1 = false;
    m_0x56_7 = false;
    m_matrix = NULL;
    m_0x54_0 = !(data->m_flags & 1);
    m_0x54_4 = m_0x54_6;
    m_0x55_3 = (data->m_flags >> 2) & 1;
    m_0x55_2 = (data->m_flags >> 1) & 1;
    m_0x55_7 = m_0x54_0;
    m_0x52 = 0;
    m_vtxDatas = collision->m_vtxDatas;
    m_vtxPositions = collision->m_vtxPositions;
    m_collision = collision;
    m_lines = collision->m_lines;
    m_unk40 = (grCollData::VtxData*)((char*)m_vtxDatas + data->m_lineStart * 8);
    m_unk44 = (Vec2f*)((char*)m_vtxPositions + data->m_lineStart * 16);
    m_lineStart = data->m_lineStart;
    m_lineLen = data->m_lineLen;
    m_unk4 = data->m_unk4;
    m_unk6 = data->m_unk6;
    m_unk8 = data->m_unk8;
    m_unkA = data->m_unkA;
    m_unkC = data->m_unkC;
    m_unkE = data->m_unkE;
    m_aabBox = data->m_aabBox;
    m_vtxStart = data->m_vtxStart;
    m_vtxLen = data->m_vtxLen;
    m_nodeIndex = data->m_nodeIndex;
    m_ground = NULL;
}

grCollisionLine* grCollisionJoint::getLine(u32 index) {
    return lineAt(lineAt(m_lines, m_lineStart), index);
}

void grCollisionJoint::getSegment(u16 index, grCollSegmentEx* out) {
    m_collision->getSegment(out, getLine(index), 0);
}

void grCollisionJoint::getWorldMatrix(Matrix* out) {
    if (m_0x54_3 == 1 && m_matrix != NULL) {
        *out = *m_matrix;
        return;
    }
    bool found = false;
    if (m_ground != NULL && m_ground->isEnableCalcCollision()) {
        nw4r::g3d::ScnMdl* model = m_ground->m_sceneModels[_0x4E];
        if (m_nodeIndex != 0xFFFF) {
            found = model->GetScnMtxPos(out, nw4r::g3d::ScnObj::MTX_WORLD, m_nodeIndex);
        }
    }
    if (!found) {
        out->setIdentity();
    }
}

void grCollisionJoint::clearVLink() {
    m_0x55_0 = false;
    u32 lineLen = m_lineLen;
    for (u32 i = 0; i < lineLen; i++) {
        grCollLineSlot* line = &(i + (grCollLineSlot*)m_lines)[m_lineStart];
        line->m_unkA = -1;
        line->m_unkC = -1;
        line->m_unk8 = 0;
        line->m_unk9 = 0;
    }
}

void grCollisionJoint::updateStatus() {
    if (this == NULL) {
        return;
    }
    if (m_0x54_5) {
        m_0x54_6 = true;
        m_0x54_5 = false;
    }
    m_0x54_4 = m_0x54_6;
    m_0x55_7 = m_0x54_0;
    if (m_0x56_7 == 1) {
        return;
    }
    m_0x54_1 = false;
    if (m_ground == NULL) {
        return;
    }
    if (m_ground->m_isEnableCollisionStatus) {
        m_0x54_6 = m_0x54_4 = true;
    } else {
        m_0x54_6 = m_0x54_4 = m_0x54_7 = false;
    }
    m_0x54_4 = m_0x54_6;
}

// The box around the current and the previous positions of the vertices, each grown by the margin.
struct grCollJointBounds {
    Vec2f m_minCur;
    Vec2f m_maxCur;
    Vec2f m_minPrev;
    Vec2f m_maxPrev;

    void add(const Vec2f* pos) {
        m_minCur.m_x = psMin(m_minCur.m_x, pos[0].m_x);
        m_minCur.m_y = psMin(m_minCur.m_y, pos[0].m_y);
        m_maxCur.m_x = psMax(m_maxCur.m_x, pos[0].m_x);
        m_maxCur.m_y = psMax(m_maxCur.m_y, pos[0].m_y);
        m_minPrev.m_x = psMin(m_minPrev.m_x, pos[1].m_x);
        m_minPrev.m_y = psMin(m_minPrev.m_y, pos[1].m_y);
        m_maxPrev.m_x = psMax(m_maxPrev.m_x, pos[1].m_x);
        m_maxPrev.m_y = psMax(m_maxPrev.m_y, pos[1].m_y);
    }
};

// Stores the grown boxes in the joint; the joint is flagged as moved unless both boxes are unchanged.
static inline void setBoxes(grCollisionJoint* joint, grCollJointBounds& b) {
    Vec2f margin(1.0f, 1.0f);
    b.m_minCur -= margin;
    b.m_maxCur += margin;
    b.m_minPrev -= margin;
    b.m_maxPrev += margin;
    joint->m_aabBox.m_unk1 = b.m_minCur.m_x;
    joint->m_aabBox.m_unk2 = b.m_minCur.m_y;
    joint->m_aabBox.m_unk3 = b.m_maxCur.m_x;
    joint->m_aabBox.m_unk4 = b.m_maxCur.m_y;
    joint->m_prevAabBox.m_unk1 = b.m_minPrev.m_x;
    joint->m_prevAabBox.m_unk2 = b.m_minPrev.m_y;
    joint->m_prevAabBox.m_unk3 = b.m_maxPrev.m_x;
    joint->m_prevAabBox.m_unk4 = b.m_maxPrev.m_y;
    if (!(b.m_minCur.m_x == b.m_minPrev.m_x && b.m_minCur.m_y == b.m_minPrev.m_y) ||
        !(b.m_maxCur.m_x == b.m_maxPrev.m_x && b.m_maxCur.m_y == b.m_maxPrev.m_y)) {
        joint->m_0x54_1 = true;
    }
}

void grCollisionJoint::updateAABB() {
    Vec2f* pos = m_collision->getDstVtx(this, 0);
    u32 count = m_vtxLen;
    grCollJointBounds bounds;
    bounds.m_minCur.m_x = pos[0].m_x;
    bounds.m_minCur.m_y = pos[0].m_y;
    bounds.m_maxCur.m_x = pos[0].m_x;
    bounds.m_maxCur.m_y = pos[0].m_y;
    bounds.m_minPrev.m_x = pos[1].m_x;
    bounds.m_minPrev.m_y = pos[1].m_y;
    bounds.m_maxPrev.m_x = pos[1].m_x;
    bounds.m_maxPrev.m_y = pos[1].m_y;
    pos += 2;
    for (u32 i = 1; i < count; i++, pos += 2) {
        bounds.add(pos);
    }
    setBoxes(this, bounds);
}

void grCollisionJoint::updatePosInit(Matrix* matrix) {
    grCollData::VtxData* src = m_collision->getSrcVtx(this, 0);
    Vec2f* dst = m_collision->getDstVtx(this, 0);
    u32 count = m_vtxLen;
    grCollJointBounds bounds;
    if (count != 0) {
        Vec3f local(src->m_pos.m_x, src->m_pos.m_y, 0.0f);
        Vec3f world;
        matrix->mulPos(&local, &world);
        dst[0].m_x = world.m_x;
        dst[0].m_y = world.m_y;
        dst[1].m_x = world.m_x;
        dst[1].m_y = world.m_y;
        src++;
        dst += 2;
        bounds.m_minCur = dst[-2];
        bounds.m_maxCur = dst[-2];
    }
    for (u32 i = 1; i < count; i++, src++, dst += 2) {
        Vec3f local(src->m_pos.m_x, src->m_pos.m_y, 0.0f);
        Vec3f world;
        matrix->mulPos(&local, &world);
        dst[0].m_x = world.m_x;
        dst[0].m_y = world.m_y;
        dst[1].m_x = world.m_x;
        dst[1].m_y = world.m_y;
        bounds.m_minCur.m_x = psMin(bounds.m_minCur.m_x, dst[0].m_x);
        bounds.m_minCur.m_y = psMin(bounds.m_minCur.m_y, dst[0].m_y);
        bounds.m_maxCur.m_x = psMax(bounds.m_maxCur.m_x, dst[0].m_x);
        bounds.m_maxCur.m_y = psMax(bounds.m_maxCur.m_y, dst[0].m_y);
    }
    bounds.m_minPrev = bounds.m_minCur;
    bounds.m_maxPrev = bounds.m_maxCur;
    setBoxes(this, bounds);
    m_0x54_1 = true;
}

void grCollisionJoint::updatePosNormal(Matrix* matrix) {
    grCollData::VtxData* src = m_collision->getSrcVtx(this, 0);
    Vec2f* dst = m_collision->getDstVtx(this, 0);
    u32 count = m_vtxLen;
    grCollJointBounds bounds;
    for (u32 i = 0; i < count; i++, src++, dst += 2) {
        Vec3f local(src->m_pos.m_x, src->m_pos.m_y, 0.0f);
        Vec3f world;
        dst[1].m_x = dst[0].m_x;
        dst[1].m_y = dst[0].m_y;
        matrix->mulPos(&local, &world);
        dst[0].m_x = world.m_x;
        dst[0].m_y = world.m_y;
        if (i == 0) {
            bounds.m_minCur = bounds.m_maxCur = dst[0];
            bounds.m_minPrev = bounds.m_maxPrev = dst[1];
        } else {
            bounds.add(dst);
        }
    }
    setBoxes(this, bounds);
}

void grCollisionJoint::updatePos(bool init) {
    if (m_0x55_5) {
        Matrix matrix(true);
        getWorldMatrix(&matrix);
        if (init || !m_0x54_7) {
            updatePosInit(&matrix);
            m_0x54_7 = true;
        } else {
            updatePosNormal(&matrix);
        }
        if (lbl_805A0358) {
            float scaleSq = matrix.m[0][0] * matrix.m[0][0] + matrix.m[1][0] * matrix.m[1][0] + matrix.m[2][0] * matrix.m[2][0] +
                            (matrix.m[0][1] * matrix.m[0][1] + matrix.m[1][1] * matrix.m[1][1] + matrix.m[2][1] * matrix.m[2][1]);
            bool degenerate = scaleSq < 0.8f;
            if (degenerate) {
                m_0x54_4 = false;
                m_0x54_7 = false;
            }
        }
    } else {
        updateAABB();
    }
}

// TODO: clean up, give variables real names
namespace jj_80112EBC {
struct Bits54 { u8 b0:1; u8 b1:1; u8 b2:1; u8 b3:1; u8 b4:1; u8 b5:1; u8 b6:1; u8 b7:1; };
struct Obj {
 u16 h[8]; float f[4]; u8 pad[0x10]; void* vec; u32 p34,p38,p3c,p40,p44,p48; u16 h4c,h4e,h50,h52; Bits54 a,b,c; u8 pad57; u32 p58,p5c;
};
struct Vec { u8 pad[12]; u32 a,b,c; };
struct Src { u16 h[8]; struct { u16 pad:13; u16 b2:1; u16 b1:1; u16 b0:1; } flags; u8 pad[2]; float f[4]; u16 a,b,c,d; };
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80112EBC(Obj* self, Vec* vec, Src* src) {
 self->p48 = (u16)self->p48;
 self->a.b0=0; self->a.b1=1; self->a.b2=0; self->a.b4=0; self->a.b5=0;
 self->b.b1=1; self->b.b2=1; self->b.b3=0; self->b.b4=0; self->b.b6=0;
 self->c.b0=0;
 self->p5c=0;
 self->a.b7 = !src->flags.b0;
 self->b.b4 = src->flags.b2;
 self->b.b5 = src->flags.b1;
 self->h52=0;
 self->a.b3 = self->a.b1;
 self->b.b0 = self->a.b7;
 self->p38=vec->a; self->p3c=vec->b; self->vec=vec; self->p34=vec->c;
 self->p40=self->p38+src->h[0]*8;
 self->p44=self->p3c+src->h[0]*16;
 self->h[0]=src->h[0]; self->h[1]=src->h[1]; self->h[2]=src->h[2]; self->h[3]=src->h[3]; self->h[4]=src->h[4]; self->h[5]=src->h[5]; self->h[6]=src->h[6]; self->h[7]=src->h[7];
 { float x, y; y = src->f[1]; x = src->f[0]; self->f[0] = x; self->f[1] = y; }
 { float x, y; y = src->f[3]; x = src->f[2]; self->f[2] = x; self->f[3] = y; }
 *(u16*)((u8*)self+0x4a)=src->a; self->h4c=src->b; self->h50=src->d; self->p58=0;
}
}

