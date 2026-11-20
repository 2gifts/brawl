#define MT_VEC3F_IMPLICIT_ASSIGN // the search offset copies move the components as words
#define MT_VEC3F_SCALE_XY_IN_PLACE
#include <so/collision/so_collision_search_part.h>
#include <so/collision/so_collision_shield_part.h>
#include <cl/cl_collision.h>
#include <revolution/gx.h>
#include <types.h>

class soModelModule;
namespace nw4r { namespace math { struct _MTX34; } }

// HYPOTHESIS: the part layout as soCollisionSearchModuleImpl reads it (that unit keeps its own copy of this class).
class soCollisionSearchPart {
public:
    int m_status; // HYPOTHESIS: 0 idle, 1 just set, 2 updated
    soCollisionSearchData m_data;
    soArrayVector<clTarget, 6> m_targets;
    int unk58;
    int unk5C;

    soCollisionSearchPart(soCollision::Category category);
    ~soCollisionSearchPart();
    void reset();
    void set(soCollisionSearchData* data, newCollision* collision);
    // HYPOTHESIS: name and parameters (the linker map leaves this one unnamed). With explicit positions the shape is
    // placed at them, otherwise at the model node.
    bool update(float scale, soModuleAccesser* moduleAccesser, soCollisionShieldPartUnit* unit, Vec3f* startPos, Vec3f* endPos);
    void initPos(float scale, soModelModule* modelModule, newCollision* collision);
    void debugDisplay(nw4r::math::_MTX34* view, void* collision);
};
static_assert(sizeof(soCollisionSearchPart) == 0x60, "Class is wrong size!");

// TODO: clean up: these soCollisionUtil helpers are still unnamed (see so_collision_util.cpp).
extern "C" void fn_27_55BB8(soCollisionShieldPartUnit* unit, u32 shapeType, bool isFirst, Vec3f* startPos, Vec3f* endPos,
                            float size);
extern "C" void fn_27_55E8C(soModelModule* modelModule, newCollision* collision, bool isFirst, u32 shapeType, u32 nodeIndex,
                            Vec3f* offset, float size, float scale);

soCollisionSearchPart::soCollisionSearchPart(soCollision::Category category) : m_status(0), m_targets(6, 0) {
    {
        clTarget& target = m_targets.at(0);
        target.m_0 = 0x20;
    }
    {
        clTarget& target = m_targets.at(1);
        target.m_0 = 1 << category;
    }
}

soCollisionSearchPart::~soCollisionSearchPart() { }

void soCollisionSearchPart::reset() {
    m_status = 0;
}

// NONMATCHING: the original loads the whole search data before storing it (registers r9, r8, r7, f0, r6, r5).
void soCollisionSearchPart::set(soCollisionSearchData* data, newCollision* collision) {
    m_data = *data;
    m_status = 1;
    {
        clTarget& target = m_targets.at(0);
        target.m_4 = m_data.m_unk10a;
    }
    {
        clTarget& target = m_targets.at(1);
        target.m_4 = m_data.m_unk10b;
    }
    {
        clTarget& target = m_targets.at(2);
        target.m_4 = m_data.m_unk10c;
    }
    {
        clTarget& target = m_targets.at(3);
        target.m_4 = m_data.m_unk14a;
    }
    {
        clTarget& target = m_targets.at(4);
        target.m_4 = m_data.m_unk14b;
    }
    collision->m_targets = &m_targets;
}

bool soCollisionSearchPart::update(float scale, soModuleAccesser* moduleAccesser, soCollisionShieldPartUnit* unit, Vec3f* startPos, Vec3f* endPos) {
    if (m_status != 0) {
        float size = m_data.m_size;
        if (!m_data.m_unk14e && !m_data.m_unk14c) {
            size *= scale;
        }
        if (startPos != NULL) {
            fn_27_55BB8(unit, m_data.m_unk14e, m_status == 1, startPos, endPos, size);
        } else {
            Vec3f offset = m_data.m_offset;
            if (!m_data.m_unk14e && !m_data.m_unk14c) {
                Vec3fScale(&offset, &offset, scale);
            }
            soCollisionUtil::updateCollision(moduleAccesser, unit, m_data.m_unk14e, m_status == 1, m_data.m_node, &offset, NULL, 0,
                                             true, size, 1.0f);
        }
        unit->m_isUpdated = true;
        m_status = 2;
        return true;
    }
    return false;
}

void soCollisionSearchPart::initPos(float scale, soModelModule* modelModule, newCollision* collision) {
    if (m_status != 0) {
        fn_27_55E8C(modelModule, collision, m_status == 1, m_data.m_unk14e, m_data.m_node, &m_data.m_offset, m_data.m_size,
                    !m_data.m_unk14c ? scale : 1.0f);
    }
}

// TODO: clean up: newCollision::display (vtable slot at 0x24) takes the view and two colors here, but cl_collision.h
// declares it without parameters (the clCapsule/clSphere overrides are empty).
struct soCollisionSearchPartDebugBase { // MATCH-ONLY
    u8 _0[0x30];
};
class soCollisionSearchPartDebugView : public soCollisionSearchPartDebugBase { // MATCH-ONLY
public:
    virtual ~soCollisionSearchPartDebugView();
    virtual void v0C();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1C();
    virtual void v20();
    virtual void display(nw4r::math::_MTX34* view, const GXColor* color0, const GXColor* color1);
};

void soCollisionSearchPart::debugDisplay(nw4r::math::_MTX34* view, void* collision) {
    if (m_status != 1) {
        GXColor color0 = {0xFF, 0xFF, 0xFF, 0x80};
        GXColor color1 = {0x40, 0x40, 0x40, 0x80};
        ((soCollisionSearchPartDebugView*)collision)->display(view, &color0, &color1);
    }
}
