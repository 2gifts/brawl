#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_raycheck.h>
#include <mt/mt_coll2d.h>
#include <types.h>
#include <mt/mt_ps_minmax.h>

// MATCH-ONLY: the original passes the unextended 32-bit loop counters to these two (every caller in the dol does), so the index
// parameters of grCollision::getJoint / getLine are probably not u16; the stand-ins keep the symbols of the members.
extern "C" grCollisionJoint* getJoint__11grCollisionFUl(grCollision* collision, u32 index);
extern "C" grCollisionLine* getLine__11grCollisionFUs(grCollision* collision, u32 index);

grCollisionRaycheck::grCollisionRaycheck() {
    m_unk10 = 0;
    m_unk12 = 7;
    m_unk14 = 1.0f;
    m_unk3C = 0xFFFF;
    m_unk40 = 0;
    m_unk41 = 0;
}

grCollisionRaycheck::~grCollisionRaycheck() { }

// The point of the nearest hit: the ray start moved along the ray by the hit rate.
static inline void calcHitPos(grCollisionRaycheck* check, clRay2D* ray) {
    check->m_hitPos = ray->m_pos + ray->m_vec * check->m_unk14;
}

// Looks for the nearest line of the collision that the ray crosses (rate below m_unk14) and keeps it.
void grCollisionRaycheck::raycheckSub(grCollision* collision) {
    clSegment2D raySegment;
    ((clRay2D*)&m_rayPos)->getSegment(raySegment);
    u16 categoryBits = m_unk12 << 4;
    u32 jointLen = collision->m_jointLen;
    for (u32 i = 0; i < jointLen; i++) {
        grCollisionJoint* joint = getJoint__11grCollisionFUl(collision, i);
        if (joint->m_0x54_6 && joint->m_aabBox.checkIntersect(raySegment)) {
            u32 lineStart = joint->m_lineStart;
            u32 lineEnd = lineStart + joint->m_lineLen;
            for (u32 index = lineStart; index < lineEnd; index++) {
                grCollisionLine* line = getLine__11grCollisionFUs(collision, index);
                if (line->m_isThroughable && m_unk41 == 0) {
                    continue;
                }
                if ((*(u16*)((char*)line + 0x10) & m_unk10) != 0) {
                    continue;
                }
                if ((line->m_unkE & categoryBits) == 0) {
                    continue;
                }
                clSegment2D segment;
                collision->getSegment(&segment, line);
                float minY = psMin(segment.m_p0Pos.m_y, segment.m_p1Pos.m_y);
                float maxY = psMax(segment.m_p0Pos.m_y, segment.m_p1Pos.m_y);
                float maxX = psMax(segment.m_p0Pos.m_x, segment.m_p1Pos.m_x);
                float minX = psMin(segment.m_p0Pos.m_x, segment.m_p1Pos.m_x);
                // the box of the ray must meet the box of the line
                if (m_aabb.m_unk4 < minY || m_aabb.m_unk2 > maxY || m_aabb.m_unk3 < minX || m_aabb.m_unk1 > maxX) {
                    continue;
                }
                float rate;
                Vec2f normal;
                if (!((clRay2D*)&m_rayPos)->checkIntersect2(segment, &rate, &normal)) {
                    continue;
                }
                if (!(rate < m_unk14)) {
                    continue;
                }
                bool accept;
                if (m_unk40 != 0) {
                    accept = true;
                } else {
                    // only lines that face the ray count: the ray runs against the normal of the line
                    accept = false;
                    Vec2f direction;
                    direction.m_x = segment.m_p1Pos.m_x - segment.m_p0Pos.m_x;
                    direction.m_y = segment.m_p1Pos.m_y - segment.m_p0Pos.m_y;
                    Vec2f lineNormal;
                    lineNormal.m_x = -direction.m_y;
                    lineNormal.m_y = direction.m_x;
                    if (m_rayVec.m_x * lineNormal.m_x + m_rayVec.m_y * lineNormal.m_y < 0.0f) {
                        accept = true;
                    }
                }
                if (accept) {
                    m_unk14 = rate;
                    m_hitNormal.m_x = normal.m_x;
                    m_hitNormal.m_y = normal.m_y;
                    m_unk3C = collision->getLineId(line);
                    m_hit = 1;
                }
            }
        }
    }
}

// Casts the ray against every collision that has finished updating; true when a line was hit.
u8 grCollisionRaycheck::raycheck(clRay2D* ray) {
    m_hit = 0;
    Vec2f::copy(m_rayPos, ray->m_pos);
    Vec2f::copy(m_rayVec, ray->m_vec);
    m_unk14 = 1.0f;
    ray->getAABBox(m_aabb);
    typedef nw4r::ut::LinkList<grCollision, 0x30> NodeList;
    NodeList& list = (NodeList&)g_grCollisionList;
    for (NodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* collision = &*it;
        if (collision->m_isEnabled && collision->m_lineStatusDone && collision->m_noUpdate) {
            raycheckSub(collision);
        }
    }
    if (m_hit == 1) {
        calcHitPos(this, ray);
    }
    return m_hit;
}

// Like raycheck, against one collision.
u8 grCollisionRaycheck::raycheck1(clRay2D* ray, grCollision* collision) {
    m_hit = 0;
    if (collision->m_isEnabled && collision->m_lineStatusDone && collision->m_noUpdate) {
        Vec2f::copy(m_rayPos, ray->m_pos);
    Vec2f::copy(m_rayVec, ray->m_vec);
        m_unk14 = 1.0f;
        ray->getAABBox(m_aabb);
        raycheckSub(collision);
        if (m_hit == 1) {
            calcHitPos(this, ray);
        }
    }
    return m_hit;
}

// TODO: clean up, give variables real names
namespace jj_8013848C {
extern "C" {
void getAABBox__7clRay2DCFR10clAABBox2D(void*, void*, u32, u32, u32);
void fn_801380BC(void*, void*);
extern f32 lbl_805A2A80;

struct Vec2f {
    union {
        struct { f32 x, y; };
        u32 words[2];
    };
};
struct Segment {
    Vec2f a, b;
};
struct Obj {
    u8 pad0[0x14];
    f32 val;
    u8 pad18[0x8];
    Vec2f res;
    u8 flag;
    u8 pad29[3];
    u32 w[4];
};

// TODO: clean up, give variables real names
u8 fn_8013848C(void* arg0, void* arg1, const u8* arg2) {
    Obj* o = (Obj*)arg0;
    u32* a1 = (u32*)arg1;
    Segment* seg = (Segment*)arg1;
    u32 w0, w1, w2, w3;
    Vec2f tmp2, tmp1, prod, sum;
    f32 val;
    u8 flags;

    o->flag = 0;
    flags = arg2[0x2c];

    if (((flags >> 7) & 1) && (flags & 1) && ((flags >> 2) & 1)) {
        w0 = a1[0];
        w1 = a1[1];
        w2 = a1[2];
        w3 = a1[3];
        val = lbl_805A2A80;
        o->w[0] = w0;
        o->w[1] = w1;
        o->w[2] = w2;
        o->w[3] = w3;
        o->val = val;
        getAABBox__7clRay2DCFR10clAABBox2D(arg1, arg0, w2, w1, w0);
        fn_801380BC(arg0, (void*)arg2);
        if (o->flag == 1) {
            f32 scale = o->val;
            prod.x = seg->b.x * scale;
            prod.y = seg->b.y * scale;
            tmp1.words[0] = prod.words[0];
            tmp1.words[1] = prod.words[1];
            sum.x = tmp1.x + seg->a.x;
            sum.y = tmp1.y + seg->a.y;
            tmp2.words[0] = sum.words[0];
            tmp2.words[1] = sum.words[1];
            o->res.x = tmp2.x;
            o->res.y = tmp2.y;
        }
    }

    return o->flag;
}
}
}

