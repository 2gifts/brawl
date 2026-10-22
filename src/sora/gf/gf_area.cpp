// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy.
#define MT_VEC2F_PLAIN_BASE
#include <gf/gf_area.h>
#include <gf/gf_area_manager.h>
#include <mt/mt_coll2d.h>
#include <types.h>

// HYPOTHESIS: unnamed mt_coll2d entry points (map: clAABBox2D/clCircle2D/clTriangle2D checkIntersect/checkInclude/getAABBox/draw); parameter types from register use.
extern "C" bool fn_80041D64(const clAABBox2D* box, const clAABBox2D* other);
extern "C" bool fn_80041DCC(const clAABBox2D* box, const clCircle2D* circle);
extern "C" bool fn_800418D0(const clAABBox2D* box, const Vec2f* pos);
extern "C" void fn_80041F70(const clAABBox2D* box, GXColor color, float width, bool fill);
extern "C" bool fn_800414C0(const clCircle2D* circle, const clCircle2D* other);
extern "C" bool fn_80041528(const clCircle2D* circle, const clTriangle2D* triangle);
extern "C" bool fn_80041538(const clCircle2D* circle, const Vec2f* pos);
extern "C" void fn_800415E8(const clCircle2D* circle, GXColor color, float width, bool fill);
extern "C" void fn_80042034(const clTriangle2D* triangle, clAABBox2D* box);
extern "C" bool fn_80042088(const clTriangle2D* triangle, const clAABBox2D* box);
extern "C" bool fn_8004231C(const clTriangle2D* triangle, const clCircle2D* circle);
extern "C" bool fn_80042584(const clTriangle2D* triangle, const clTriangle2D* other);
extern "C" void fn_800428E8(const clTriangle2D* triangle, GXColor color, float width, bool fill);

int lbl_8059C428 = 1;
gfAreaEventReciverNull lbl_8059FF88;
nw4r::ut::Color lbl_8059FF8C(0x80, 0x80, 0x80, 0x60);

gfArea::gfArea(const Vec2f& pos, bool isRegist)
    : m_0x0(0), m_0x4(0), m_checkTargetGroup(-1), m_checkTargetLocalGroup(0), m_isDrawFill(true), m_unk1C_6(true),
      m_isNoDraw(false), m_isRegist(false), m_group(Group_None), m_pos(pos), m_eventReciver(&lbl_8059FF88), m_ownerId(-1) {
    m_debugColor.r = 0x80;
    m_debugColor.g = 0xC0;
    m_debugColor.b = 0x40;
    m_debugColor.a = 0x80;
    m_teamId = m_id = lbl_8059C428++;
    m_aabBox.m_unk1 = -10.0f;
    m_aabBox.m_unk2 = -10.0f;
    m_aabBox.m_unk3 = 10.0f;
    m_aabBox.m_unk4 = 10.0f;
    if (isRegist == true) {
        regist();
    }
}

gfArea::~gfArea() {
    unRegist();
}

void gfArea::regist() {
    if (!m_isRegist) {
        lbl_8059FF90->registArea(this);
        m_isRegist = true;
    }
}

void gfArea::unRegist() {
    if (m_isRegist == true) {
        lbl_8059FF90->removeArea(this);
        m_isRegist = false;
    }
}

void gfArea::setEventReciver(gfAreaEventReciver* reciver) {
    m_eventReciver = reciver;
}

void gfArea::test(gfArea* other) {
    bool isHit;
    bool isOtherHit;
    if (m_teamId != other->m_teamId) {
        isHit = ((1 << other->m_group) & m_checkTargetGroup) != 0;
        isOtherHit = ((1 << m_group) & other->m_checkTargetGroup) != 0;
    } else {
        isHit = ((1 << other->m_group) & m_checkTargetLocalGroup) != 0;
        isOtherHit = ((1 << m_group) & other->m_checkTargetLocalGroup) != 0;
    }
    if (isHit || isOtherHit) {
        if (fn_80041D64(&m_aabBox, &other->m_aabBox) && testIntersectShape(other)) {
            if (isHit) {
                notifyIntersect(other);
            }
            if (isOtherHit) {
                other->notifyIntersect(this);
            }
        }
    }
}

bool gfArea::testIntersect(gfArea* other) {
    bool result = false;
    if (fn_80041D64(&m_aabBox, &other->m_aabBox) && testIntersectShape(other)) {
        result = true;
    }
    return result;
}

bool gfArea::testIntersectShape(gfArea* other) {
    return true;
}

void gfArea::notifyIntersect(gfArea* other) {
    m_eventReciver->reciveEvent(this, other);
}

void gfArea::debugDrawImp() {
    if (!m_isNoDraw) {
        fn_80041F70(&m_aabBox, m_debugColor, 1.0f, false);
    }
}

void gfAreaRectangle::update() {
    Vec2f v0 = m_vertices[0] + m_pos;
    Vec2f v1 = m_vertices[1] + m_pos;
    Vec2f v2 = m_vertices[2] + m_pos;
    Vec2f v3 = m_vertices[3] + m_pos;
    Vec2f min = v0;
    Vec2f max = v0;
    if (v1.m_x < min.m_x) min.m_x = v1.m_x;
    m_aabBox.m_unk1 = min.m_x;
    m_aabBox.m_unk2 = min.m_y;
    m_aabBox.m_unk3 = max.m_x;
    m_aabBox.m_unk4 = max.m_y;
}

void gfAreaRectangle::setShape(const Vec2f& center, const Vec2f& size) {
    m_isNoDraw = true;
    m_vertices[0] = Vec2f(center.m_x - size.m_x * 0.5f, center.m_y - size.m_y * 0.5f);
    m_vertices[1] = Vec2f(center.m_x + size.m_x * 0.5f, center.m_y - size.m_y * 0.5f);
    m_vertices[2] = Vec2f(center.m_x + size.m_x * 0.5f, center.m_y + size.m_y * 0.5f);
    m_vertices[3] = Vec2f(center.m_x - size.m_x * 0.5f, center.m_y + size.m_y * 0.5f);
}

bool gfAreaRectangle::testIntersectShape(gfArea* other) {
    bool result = false;
    switch (other->m_shapeType) {
    case Shape_Rectangle:
        result = true;
        break;
    case Shape_Circle: {
        gfAreaCircle* circle = static_cast<gfAreaCircle*>(other);
        clCircle2D shape;
        shape.m_centerPos.m_x = circle->m_circle.m_centerPos.m_x + circle->m_pos.m_x;
        shape.m_centerPos.m_y = circle->m_circle.m_centerPos.m_y + circle->m_pos.m_y;
        shape.m_radius = circle->m_circle.m_radius;
        result = fn_80041DCC(&m_aabBox, &shape);
        break;
    }
    }
    return result;
}

bool gfAreaRectangle::testIncludePos(Vec2f* pos) {
    return fn_800418D0(&m_aabBox, pos);
}

void gfAreaRectangle::debugDrawImp() {
    if (!m_isNoDraw) {
        if (m_isDrawFill) {
            fn_80041F70(&m_aabBox, m_debugColor, 1.0f, true);
        } else {
            fn_80041F70(&m_aabBox, lbl_8059FF8C, 1.0f, false);
        }
    }
}

void gfAreaCircle::update() {
    clCircle2D shape;
    shape = m_circle;
    shape.m_centerPos.m_x = shape.m_centerPos.m_x + m_pos.m_x;
    shape.m_centerPos.m_y = shape.m_centerPos.m_y + m_pos.m_y;
    shape.getAABBox(m_aabBox);
}

void gfAreaCircle::setShape(const Vec2f& center, float radius) {
    m_circle.m_centerPos = center;
    m_circle.m_radius = radius;
    m_isNoDraw = true;
}

bool gfAreaCircle::testIntersectShape(gfArea* other) {
    bool result = false;
    clCircle2D shape;
    shape.m_centerPos.m_x = m_circle.m_centerPos.m_x + m_pos.m_x;
    shape.m_centerPos.m_y = m_circle.m_centerPos.m_y + m_pos.m_y;
    shape.m_radius = m_circle.m_radius;
    switch (other->m_shapeType) {
    case Shape_Rectangle:
        result = fn_80041DCC(&dynamic_cast<gfAreaRectangle*>(other)->m_aabBox, &shape);
        break;
    case Shape_Circle: {
        gfAreaCircle* circle = static_cast<gfAreaCircle*>(other);
        clCircle2D otherShape;
        otherShape.m_centerPos.m_x = circle->m_circle.m_centerPos.m_x + circle->m_pos.m_x;
        otherShape.m_centerPos.m_y = circle->m_circle.m_centerPos.m_y + circle->m_pos.m_y;
        otherShape.m_radius = circle->m_circle.m_radius;
        result = fn_800414C0(&shape, &otherShape);
        break;
    }
    case Shape_Triangle: {
        gfAreaTriangle* triangle = static_cast<gfAreaTriangle*>(other);
        clTriangle2D otherShape = triangle->m_triangle;
        otherShape.m_vertices[0].m_x = otherShape.m_vertices[0].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[0].m_y = otherShape.m_vertices[0].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[1].m_x = otherShape.m_vertices[1].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[1].m_y = otherShape.m_vertices[1].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[2].m_x = otherShape.m_vertices[2].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[2].m_y = otherShape.m_vertices[2].m_y + triangle->m_pos.m_y;
        result = fn_80041528(&shape, &otherShape);
        break;
    }
    }
    return result;
}

bool gfAreaCircle::testIncludePos(Vec2f* pos) {
    bool result = false;
    if (fn_800418D0(&m_aabBox, pos)) {
        clCircle2D shape;
        shape.m_centerPos.m_x = m_circle.m_centerPos.m_x + m_pos.m_x;
        shape.m_centerPos.m_y = m_circle.m_centerPos.m_y + m_pos.m_y;
        shape.m_radius = m_circle.m_radius;
        result = fn_80041538(&shape, pos);
    }
    return result;
}

void gfAreaCircle::debugDrawImp() {
    clCircle2D shape;
    shape.m_centerPos.m_x = m_circle.m_centerPos.m_x + m_pos.m_x;
    shape.m_centerPos.m_y = m_circle.m_centerPos.m_y + m_pos.m_y;
    shape.m_radius = m_circle.m_radius;
    if (m_isDrawFill) {
        fn_800415E8(&shape, m_debugColor, 1.0f, true);
    } else {
        fn_800415E8(&shape, lbl_8059FF8C, 1.0f, false);
    }
}

void gfAreaTriangle::update() {
    clTriangle2D shape = m_triangle;
    shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
    shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
    shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
    shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
    shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
    shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    fn_80042034(&shape, &m_aabBox);
}

bool gfAreaTriangle::testIntersectShape(gfArea* other) {
    bool result = false;
    clTriangle2D shape = m_triangle;
    shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
    shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
    shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
    shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
    shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
    shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    switch (other->m_shapeType) {
    case Shape_Rectangle:
        result = fn_80042088(&shape, &dynamic_cast<gfAreaRectangle*>(other)->m_aabBox);
        break;
    case Shape_Circle: {
        gfAreaCircle* circle = static_cast<gfAreaCircle*>(other);
        clCircle2D otherShape;
        otherShape.m_centerPos.m_x = circle->m_circle.m_centerPos.m_x + circle->m_pos.m_x;
        otherShape.m_centerPos.m_y = circle->m_circle.m_centerPos.m_y + circle->m_pos.m_y;
        otherShape.m_radius = circle->m_circle.m_radius;
        result = fn_8004231C(&shape, &otherShape);
        break;
    }
    case Shape_Triangle: {
        gfAreaTriangle* triangle = static_cast<gfAreaTriangle*>(other);
        clTriangle2D otherShape = triangle->m_triangle;
        otherShape.m_vertices[0].m_x = otherShape.m_vertices[0].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[0].m_y = otherShape.m_vertices[0].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[1].m_x = otherShape.m_vertices[1].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[1].m_y = otherShape.m_vertices[1].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[2].m_x = otherShape.m_vertices[2].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[2].m_y = otherShape.m_vertices[2].m_y + triangle->m_pos.m_y;
        result = fn_80042584(&shape, &otherShape);
        break;
    }
    }
    return result;
}

bool gfAreaTriangle::testIncludePos(Vec2f* pos) {
    if (fn_800418D0(&m_aabBox, pos)) {
        clTriangle2D shape = m_triangle;
        shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
        shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
        shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
        shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
        shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
        shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    }
    return false;
}

void gfAreaTriangle::debugDrawImp() {
    clTriangle2D shape = m_triangle;
    shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
    shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
    shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
    shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
    shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
    shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    if (m_isDrawFill) {
        fn_800428E8(&shape, m_debugColor, 1.0f, true);
    } else {
        fn_800428E8(&shape, lbl_8059FF8C, 1.0f, false);
    }
}
