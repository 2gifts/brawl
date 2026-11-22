#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_raycheck.h>
#include <mt/mt_coll2d.h>
#include <st/st_collision_attr_param.h>
#include <st/st_data_container.h>
#include <st/st_utility.h>
#include <types.h>
#include <memory.h>

// TODO: clean up - the two raycheck members are called through their main.dol stand-in names (grCollisionRaycheck::raycheck and
// ::raycheck1 sit at 0x80138348 and 0x8013848C, which have no names in the symbol map), and the ray is a local view with the same
// layout as clRay2D (its constructor stays out of line in the header; here the original inlines it).
struct stRayView {
    Vec2f m_pos;
    Vec2f m_vec;
    stRayView(const Vec2f& pos, const Vec2f& vec) : m_pos(pos), m_vec(vec) { }
};
extern "C" {
bool raycheck__19grCollisionRaycheckFP7clRay2D(grCollisionRaycheck* check, stRayView* ray);
bool fn_8013848C(grCollisionRaycheck* check, stRayView* ray, grCollision* collision);
}

// Builds the ray from a start and a vector (planar part only) and casts it, against one collision or against all enabled ones.
static inline bool castRay(grCollisionRaycheck& check, Vec3f* pos, Vec3f* dir, bool unk41, grCollision* collision, bool unk40, u8 targetMask) {
    Vec2f vec;
    vec.m_x = dir->m_x;
    vec.m_y = dir->m_y;
    Vec2f start;
    start.m_x = pos->m_x;
    start.m_y = pos->m_y;
    stRayView ray(start, vec);
    check.m_unk12 = targetMask;
    check.m_unk40 = unk40;
    check.m_unk41 = unk41;
    if (collision != NULL) {
        return fn_8013848C(&check, &ray, collision);
    }
    return raycheck__19grCollisionRaycheckFP7clRay2D(&check, &ray);
}

bool stRayCheck(Vec3f* pos, Vec3f* dir, bool unk41, grCollision* collision, bool unk40, u8 targetMask) {
    grCollisionRaycheck check;
    return castRay(check, pos, dir, unk41, collision, unk40, targetMask);
}

bool stRayCheck(Vec3f* pos, Vec3f* dir, Vec3f* outHitPos, Vec3f* outCollNormalVec, bool unk41, grCollision* collision, bool unk40, u8 targetMask) {
    grCollisionRaycheck check;
    bool hit = castRay(check, pos, dir, unk41, collision, unk40, targetMask);
    if (hit) {
        float outHitPosY = check.m_hitPos.m_y;
        float outHitPosX = check.m_hitPos.m_x;
        outHitPos->m_x = outHitPosX;
        outHitPos->m_y = outHitPosY;
        outHitPos->m_z = 0.0f;
        float outCollNormalVecY = check.m_hitNormal.m_y;
        float outCollNormalVecX = check.m_hitNormal.m_x;
        outCollNormalVec->m_x = outCollNormalVecX;
        outCollNormalVec->m_y = outCollNormalVecY;
        outCollNormalVec->m_z = 0.0f;
    }
    return hit;
}

bool stRayCheck(Vec3f* pos, Vec3f* dir, int* lineId, Vec3f* outHitPos, Vec3f* outCollNormalVec, bool unk41, bool unk40, u8 targetMask) {
    grCollisionRaycheck check;
    bool hit = castRay(check, pos, dir, unk41, NULL, unk40, targetMask);
    if (hit) {
        *lineId = check.m_unk3C;
        float outHitPosY = check.m_hitPos.m_y;
        float outHitPosX = check.m_hitPos.m_x;
        outHitPos->m_x = outHitPosX;
        outHitPos->m_y = outHitPosY;
        outHitPos->m_z = 0.0f;
        float outCollNormalVecY = check.m_hitNormal.m_y;
        float outCollNormalVecX = check.m_hitNormal.m_x;
        outCollNormalVec->m_x = outCollNormalVecX;
        outCollNormalVec->m_y = outCollNormalVecY;
        outCollNormalVec->m_z = 0.0f;
    }
    return hit;
}

// TODO: clean up - the 'SCLA' tag the collision attribute block must carry (a loose word pair in the unit's data).
static u32 sCollisionAttrMagic[2] = {0x53434C41, 0};

stCollisionAttrParam::stCollisionAttrParam() : m_container(NULL), m_attackData(NULL), m_attackRate(1) { }

stCollisionAttrParam::~stCollisionAttrParam() {
    if (m_container != NULL) {
        delete m_container;
    }
    m_container = NULL;
    if (m_attackData != NULL) {
        delete[] m_attackData;
    }
    m_attackData = NULL;
}

stCollisionAttrParamSet* stCollisionAttrParam::getCollisionAttrParamSet(int materialType) {
    if (m_container != NULL && m_container->m_filedata->m_MAGIC == sCollisionAttrMagic[0]) {
        for (u32 i = 0; i < m_container->m_filedata->m_numFiles; i++) {
            stCollisionAttrParamSet* set = reinterpret_cast<stCollisionAttrParamSet*>(m_container->getData(i));
            if (set->m_materialType == materialType) {
                return set;
            }
        }
    }
    return NULL;
}

void stCollisionAttrParam::setCollisionAttrParamSet(stDataContainerData* data) {
    if (data != NULL) {
        if (m_container != NULL) {
            delete m_container;
            m_container = NULL;
        }
        m_container = stDataMultiContainer::create(data, Heaps::StageInstance);
    }
}

void stCollisionAttrParam::setCollisionAttackData(grGimmickDamageFloor* attackData) {
    if (m_attackData == NULL) {
        m_attackData = new (Heaps::StageInstance) grGimmickDamageFloor[3];
        memset(m_attackData, 0, sizeof(grGimmickDamageFloor) * 3);
        m_attackData[0].m_index = 1;
        m_attackData[1].m_index = 2;
        m_attackData[2].m_index = 3;
    }
    int slot;
    bool found;
    if (attackData->m_index == m_attackData[0].m_index) {
        slot = 0;
        found = true;
    } else if (attackData->m_index == m_attackData[1].m_index) {
        slot = 1;
        found = true;
    } else if (attackData->m_index == m_attackData[2].m_index) {
        slot = 2;
        found = true;
    } else {
        found = false;
    }
    if (found == true) {
        // TODO: clean up - a plain struct assignment; the original copies the first 0x5C bytes as one block, then the index and team.
        __memcpy(&m_attackData[slot], attackData, 0x5c);
        m_attackData[slot].m_index = attackData->m_index;
        m_attackData[slot].m_teamNo = attackData->m_teamNo;
    }
}
