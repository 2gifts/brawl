#pragma once
// Local BrawlHeaders shadow: verified gfArea layout, the shape subclasses and the
// recovered area operations of gf_area.cpp (main.dol 0x80010D5C-0x80011C74).

#include <StaticAssert.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_Color.h>
#include <types.h>

class gfAreaEventReciver;

class gfArea {

public:
    enum ShapeType {
        Shape_Rectangle = 0x0,
        Shape_Circle = 0x1,
        Shape_Triangle = 0x2,
    };

    enum Group {
        Group_None = 0x0,
        Group_Fighter = 0x1,
        Fighter_Group_Body = 0x1,
        Fighter_Group_Tread_Passive = 0x2,
        Fighter_Group_Tread_Passive_No_Trigger = 0x3,
        Fighter_Group_Item_Pickup = 0x4,

        Group_Enemy = 0x9,
        Enemy_Group_Body = 0x9,
        Enemy_Group_Tread_Passive = 0xA,

        Group_Item = 0x11,
        Item_Group_Body = 0x11,
        Item_Group_Pickup_Small = 0x12,
        Item_Group_Pickup_Large = 0x13,

        Group_Stage = 0x15,
        Stage_Group_Gimmick_Normal = 0x15,
        Stage_Group_Gimmick_Ladder = 0x16,
        Stage_Group_Gimmick_Elevator = 0x17,

        Stage_Group_Space_Normal = 0x19,
        Stage_Group_Space_Wind = 0x1A,
        Stage_Group_Space_Movement_Area = 0x1B,
        Stage_Group_Space_Water = 0x1C,

        Group_Weapon = 0x1D,
        Weapon_Group_Body = 0x1D,
        Weapon_Group_Abnormal = 0x1E,
        Weapon_Group_Spring = 0x1F,
    };

    int m_0x0;
    int m_0x4;
    int m_checkTargetGroup;
    int m_checkTargetLocalGroup;
    u32 m_id;
    u32 m_teamId;
    ShapeType m_shapeType;
    // HYPOTHESIS: flag meanings from the constructor, debug drawing and registration.
    u8 m_isDrawFill : 1;
    u8 m_unk1C_6 : 1;
    u8 m_isNoDraw : 1;
    u8 m_isRegist : 1;
    u8 m_unk1C_lo : 4;
    u8 m_group;
    char _0x1E[2];
    Vec2f m_pos;
    gfAreaEventReciver* m_eventReciver;
    clAABBox2D m_aabBox;
    int m_ownerId;
    GXColor m_debugColor;

    // HYPOTHESIS: parameter types from the callee's register use; callers pass 0 or 1.
    gfArea(const Vec2f& pos, bool isRegist);
    ~gfArea();
    void regist();
    void unRegist();
    void setEventReciver(gfAreaEventReciver* reciver);
    void test(gfArea* other);
    bool testIntersect(gfArea* other);

    virtual bool testIntersectShape(gfArea* other);
    virtual void debugDrawImp();
    virtual void notifyIntersect(gfArea* other);
    virtual void update() { }
    virtual bool testIncludePos(Vec2f* pos) = 0;
};
static_assert(sizeof(gfArea) == 0x48, "Class is wrong size!");

class gfAreaRectangle : public gfArea {
public:
    Vec2f m_vertices[4];

    gfAreaRectangle(const Vec2f& pos, bool isRegist) : gfArea(pos, isRegist) { m_shapeType = Shape_Rectangle; }
    // HYPOTHESIS: centre and full size, from the half-size corner setup.
    void setShape(const Vec2f& center, const Vec2f& size);

    virtual bool testIntersectShape(gfArea* other);
    virtual void debugDrawImp();
    virtual void update();
    virtual bool testIncludePos(Vec2f* pos);
};
static_assert(sizeof(gfAreaRectangle) == 0x68, "Class is wrong size!");

class gfAreaCircle : public gfArea {
public:
    clCircle2D m_circle;

    gfAreaCircle(const Vec2f& pos, bool isRegist) : gfArea(pos, isRegist) { m_shapeType = Shape_Circle; }
    void setShape(const Vec2f& center, float radius);

    virtual bool testIntersectShape(gfArea* other);
    virtual void debugDrawImp();
    virtual void update();
    virtual bool testIncludePos(Vec2f* pos);
};
static_assert(sizeof(gfAreaCircle) == 0x54, "Class is wrong size!");

class gfAreaTriangle : public gfArea {
public:
    clTriangle2D m_triangle;

    gfAreaTriangle(const Vec2f& pos, bool isRegist) : gfArea(pos, isRegist) { m_shapeType = Shape_Triangle; }

    virtual bool testIntersectShape(gfArea* other);
    virtual void debugDrawImp();
    virtual void update();
    virtual bool testIncludePos(Vec2f* pos);
};
static_assert(sizeof(gfAreaTriangle) == 0x60, "Class is wrong size!");

class gfAreaEventReciver {
public:
    virtual void reciveEvent(gfArea*, gfArea*);
};

typedef gfAreaEventReciver gfAreaEventReceiver;

class gfAreaEventReciverNull : public gfAreaEventReciver {
public:
    virtual void reciveEvent(gfArea*, gfArea*) { }
};
