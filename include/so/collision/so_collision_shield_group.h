#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <nw4r/math/math_types.h>
#include <so/so_array.h>
#include <types.h>

// NOTE: shadows the BrawlHeaders copy to model the layout of soCollisionShieldData.
struct soCollisionShieldData {
    // MATCH-ONLY: stored as plain words (a struct copy uses lwz/stw); read as two Vec3f.
    u32 m_offset0, m_offset1, m_offset2, m_offset3, m_offset4, m_offset5;
    // (struct copy tested with various member layouts)
    float m_size;
    u32 m_nodeIndex : 9;
    u32 m_shapeType : 1; // HYPOTHESIS: same meaning as the bit after the node index in soCollisionHitData
    u32 _1c_rest : 22;

    float offset(int i) { return ((float*)&m_offset0)[i]; }
    // The six words are two Vec3f-style triples (start, end).
    void setOffset(int i, float value) { ((float*)&m_offset0)[i] = value; }
};
static_assert(sizeof(soCollisionShieldData) == 0x20, "Class is wrong size!");

// HYPOTHESIS: layout read by soCollisionShieldGroup::setReflectorData; the first 8 bytes are not read there.
struct soCollisionReflectorData {
    char _0[0x8];
    float m_attackMul;
    float m_speedMul;
    float m_attackLimit;
    float m_lifeMul;
    u32 m_isNoMBall : 1;
    u32 _18_rest : 31;
};
static_assert(sizeof(soCollisionReflectorData) == 0x1C, "Class is wrong size!");

struct soCollisionShieldGroupData {
    soSet<soCollisionShieldData> m_shieldDataSet;
    // Four high flag bits are copied by soCollisionShieldGroup::add.
    u32 m_flags : 4;
    u32 _8_rest : 28;
};

static_assert(sizeof(soCollisionShieldGroupData) == 0xC, "Shield group descriptor layout");

struct soCollisionReflectorGroupData {
    soSet<soCollisionShieldData> m_shieldDataSet;
    soCollisionReflectorData* m_reflectorData;
};

class soCollisionShieldPart;
class soCollision;
class soModelModule;

class soCollisionShieldGroup {
public:
	~soCollisionShieldGroup();
	void init();
    // HYPOTHESIS: add returns the number of parts consumed; indices are unsigned.
    int add(soCollision* collision, soArray<soCollisionShieldPart>* parts, soCollisionShieldGroupData* data, u32 groupIndex, u32 partStart);
    int add(soCollision* collision, soArray<soCollisionShieldPart>* parts, soCollisionReflectorGroupData* data, u32 groupIndex, u32 partStart);
    // HYPOTHESIS: position, scale and facing are forwarded as doubles with the model module.
    bool update(soCollision* collision, soArray<soCollisionShieldPart>* parts, soModelModule* model, double posX, double scale, double lr);
	void set(soArray<soCollisionShieldPart>* parts, u32 index, soCollisionShieldData* data);
	void setReflectorData(const soCollisionReflectorData* data);
	bool isShield(soArray<soCollisionShieldPart>* parts, u32 index) const;
	void setStatus(soArray<soCollisionShieldPart>* parts, u32 index, u32 status);
	void clearAll(soArray<soCollisionShieldPart>* parts);
	void setTargetProperty(soArray<soCollisionShieldPart>* parts, u32 property);
	// HYPOTHESIS: draws the group's shields with the camera view matrix.
	void renderDebug(nw4r::math::_MTX34* view, soCollision* collision, soArray<soCollisionShieldPart>* parts);
	Vec3f getCenterPos(soCollision* collision, u32 index);

	int unk0;
	s16 m_partStart; // HYPOTHESIS: first index of the group's parts in the module's part array
	s16 m_partSize;
	float unk8;
	float unkC;
	u8 _10[0x80];
	float m_posX;
	u32 _94;
	float m_Lr;
	float m_hopAngle;
	float m_attackMul;
	float m_speedMul;
	float m_attackLimit;
	float m_lifeMul;
	bool m_isFront;
	bool m_isHop;
	bool m_isTurn;
	u8 m_isNoMBall;
	bool m_isNoHop;
	u8 _b5[0x3];
};
static_assert(sizeof(soCollisionShieldGroup) == 0xB8, "Class is wrong size!");
