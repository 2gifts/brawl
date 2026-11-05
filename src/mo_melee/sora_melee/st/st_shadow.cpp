#include <gf/gf_camera.h>
#include <st/st_shadow.h>
#include <st/stage.h>
#include <types.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_test_types.h>

stShadow::ShadowObject::ShadowObject() : unk1c(0xFFFF) { }

void stShadow::addShadowObject(u32 id, const Vec3f* pos, float size, u32 arg3, float height, u32 arg5) {
    gfCamera::ClipSphere sphere;
    sphere.center.set(pos->m_x, pos->m_y - height, pos->m_z);
    sphere.radius = 1.5f * size;
    if (gfCameraManager::getManager()->getCamera(0)->isClipIn(&sphere) && unk40 && !unk41) {
        ShadowObject* object = static_cast<ShadowObject*>(unk90.getHead());
        if (object) {
            unk90.remove(object);
            object->unk8 = id;
            object->unkC = pos->m_x;
            object->unk10 = pos->m_y;
            object->unk14 = pos->m_z;
            object->unk18 = size;
            object->unk20 = NULL;
            object->unk24 = 0;
            object->unk1c = 0xFFFF;
            object->unk28 = arg3;
            object->unk2C = height;
            object->unk30 = arg5;
            unk84.addTail(object);
            ++unk7c;
        }
    }
}

void stShadow::advDrawShadowProc(nw4r::g3d::ScnProc*, bool opa) {
    if (!opa) {
        stShadow* shadow = g_Stage->m_shadow;
        if (shadow && shadow->unk43_0 == 1) {
            shadow->drawADVShadow(0);
        }
    }
}

void stShadow::advDrawShadowProc_gr2(nw4r::g3d::ScnProc*, bool opa) {
    if (!opa) {
        stShadow* shadow = g_Stage->m_shadow;
        if (shadow && shadow->unk43_0 == 1) {
            shadow->drawADVShadow(1);
        }
    }
}

void stShadow::calcAdvShadowPoints(grCollisionLineWork* line, float left, float right, Vec2f* points, u32* count) {
    Vec2f leftPoints[8];
    Vec2f rightPoints[8];
    grCollSegmentEx segment;
    Vec2f* cursor;
    u32 rightCount;
    u32 leftCount = 0;
    grCollisionLineWork current;
    grCollisionLineWork adjacent;
    Vec2f position;
    Vec2f projection;
    cursor = leftPoints;
    current = *line;
    do {
        current.m_collision->getSegment(&segment, current.m_collisionLine, 0);
        if (left < segment.m_segment.m_p0Pos.m_x) {
            *cursor++ = segment.m_segment.m_p0Pos;
            ++leftCount;
        } else {
            position.m_x = left;
            position.m_y = 0.0f;
            segment.m_segment.projectionX(position, projection);
            leftPoints[leftCount].m_x = projection.m_x;
            leftPoints[leftCount].m_y = projection.m_y;
            ++leftCount;
            break;
        }
        if (!current.getPrev(&adjacent) || !adjacent.m_collisionLine->m_isUnder) break;
        current = adjacent;
    } while (true);
    for (u32 index = 0; index < leftCount; ++index) {
        points[index] = leftPoints[leftCount - 1 - index];
    }
    current = *line;
    rightCount = 0;
    cursor = rightPoints;
    do {
        current.m_collision->getSegment(&segment, current.m_collisionLine, 0);
        if (segment.m_segment.m_p1Pos.m_x < right) {
            *cursor++ = segment.m_segment.m_p1Pos;
            ++rightCount;
        } else {
            position.m_x = right;
            position.m_y = 0.0f;
            segment.m_segment.projectionX(position, projection);
            rightPoints[rightCount].m_x = projection.m_x;
            rightPoints[rightCount].m_y = projection.m_y;
            ++rightCount;
            break;
        }
        current.getNext(&adjacent);
        if (!current.getNext(&adjacent) || !adjacent.m_collisionLine->m_isUnder) break;
        current = adjacent;
    } while (true);
    u32 outputIndex = leftCount;
    for (u32 index = 0; index < rightCount; ++index) {
        points[outputIndex] = rightPoints[index];
        ++outputIndex;
    }
    *count = leftCount + rightCount;
}
