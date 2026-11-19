#include <gf/gf_camera.h>
#include <st/st_shadow.h>
#include <st/stage.h>
#include <types.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_test_types.h>

stShadow::ShadowObject::ShadowObject() : unk1c(0xFFFF) { }

void stShadow::eraseShadowObject(nw4r::g3d::ScnMdl* model) {
    ShadowObject* object = static_cast<ShadowObject*>(unk84.getHead());
    while (object) {
        ShadowObject* next = static_cast<ShadowObject*>(object->m_next);
        if (object->unk8 == reinterpret_cast<u32>(model)) {
            unk84.remove(object);
            unk90.addTail(object);
            break;
        }
        object = next;
    }
}

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

#include <nw4r/g3d/g3d_resmat.h>
// TODO: clean up, give variables real names
namespace jj_2283F0 {
struct Object {
    char pad[0x48];
    unsigned int field48;
};

struct SceneRoot {
    char pad[0x474];
    unsigned int field474;
};

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" Object* fn_27_228454(Object*, void*, void*);
extern "C" SceneRoot* g_gfSceneRoot;

extern "C" Object* create__8stShadowFii(void* a, void* b) {
    Object* p = (Object*)__nw__FUlQ25Heaps8HeapType(0x28C, 15);
    if (p)
        p = fn_27_228454(p, a, b);
    g_gfSceneRoot->field474 = p->field48;
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_22891C {
typedef unsigned char u8;

extern "C" {
extern u8 lbl_27_data_4A770[];
extern void* g_gfSceneRoot;
void __dla__FPv(void*);
void __dl__FPv(void*);
void __dt__6gfTaskFv(void*, int);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void remove__11gfSceneRootFQ211gfSceneRoot9LayerTypePQ34nw4r3g3d6ScnMdl(void*, int, void*);
}

extern "C" void* fn_27_22891C(u8* self, int deleting)
{
    if (self) {
        *(void**)(self + 0x3C) = lbl_27_data_4A770;
        int p80 = *(int*)(self + 0x80);
        if (p80) {
            if (p80) {
                __dla__FPv((void*)(p80 - 0x10));
            }
        }

        for (unsigned int i = 0; i < *(unsigned short*)(self + 0x44); i++) {
            void* ptr = *(void**)(*(char**)(self + 0x48) + i * 0x10 + 0xC);
            if (ptr) {
                __dla__FPv(ptr);
            }
        }

        void* arr = *(void**)(self + 0x48);
        if (arr) {
            __dla__FPv(arr);
        }

        *(int*)((char*)g_gfSceneRoot + 0x474) = 0;
        *(int*)((char*)g_gfSceneRoot + 0x478) = 0;

        remove__11gfSceneRootFQ211gfSceneRoot9LayerTypePQ34nw4r3g3d6ScnMdl(g_gfSceneRoot, 0, *(void**)(self + 0x74));
        Destroy__Q34nw4r3g3d6G3dObjFv(*(void**)(self + 0x74));
        *(void**)(self + 0x74) = 0;

        remove__11gfSceneRootFQ211gfSceneRoot9LayerTypePQ34nw4r3g3d6ScnMdl(g_gfSceneRoot, 0, *(void**)(self + 0x78));
        Destroy__Q34nw4r3g3d6G3dObjFv(*(void**)(self + 0x78));
        *(void**)(self + 0x78) = 0;

        __dt__6gfTaskFv(self, 0);

        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_228A44 {
extern "C" void addTail__6utListFP10utListNode(void*, void*);

struct ShadowObject {
    unsigned char data[0x34];
};

struct Shadow {
    unsigned char pad[0x46];
    unsigned short count;
    unsigned char pad48[0x34];
    unsigned int activeCount;
    ShadowObject* objects;
    unsigned int list84;
    unsigned int list88;
    unsigned int list8C;
    unsigned int list90;
    unsigned int list94;
    unsigned int list98;
};

extern "C" void clearShadowObject__8stShadowFv(Shadow* self) {
    self->list88 = 0;
    self->list84 = 0;
    self->list8C = 0;
    self->list94 = 0;
    self->list90 = 0;
    self->list98 = 0;

    for (unsigned int i = 0; i < self->count; ++i) {
        addTail__6utListFP10utListNode(&self->list90, &self->objects[i]);
    }

    self->activeCount = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_228DF4 {
extern "C" void* GetResMat__Q34nw4r3g3d6ResMdlCFi(s32*, u8, void*);
extern "C" u32 GetResTex__Q34nw4r3g3d14ResTexPlttInfoCFv(void**);
struct Slot { u8 flags; u8 a; u8 b; u8 c; u8 d; void* data; };
extern "C" s32 fn_27_228DF4(void* arg0, void* arg1)
{
            u8 flags;
    u8* a1 = (u8*)arg1;
    u8* base = (u8*)arg0;
    s32 result = 0;
    u8 flag = 0;
    Slot* r28 = 0;
    if ((a1[0] >> 7) & 1) flag = 1;
    s32 p = *(s32*)(a1 + 8);
    s32 q = *(s32*)(p + 0x44);
    s32 r = *(s32*)(q + a1[1] * 4);
    s32 resource = *(s32*)(r + 0xE8);
    void* matResult = GetResMat__Q34nw4r3g3d6ResMdlCFi(&resource, a1[2], (void*)r);
    u8* matPtr = (u8*)matResult;
    s32 offset30 = *(s32*)(matPtr + 0x30);
    void* spC;
    if (offset30 != 0) spC = (void*)(matPtr + offset30);
    else spC = 0;
    u32 id = GetResTex__Q34nw4r3g3d14ResTexPlttInfoCFv(&spC);
    nw4r::g3d::ResTexObj tex(matPtr + 0x3c);
    void* texResult = tex.GetTexObj((GXTexMapID)a1[3]);
    s32 idx = 0;
    Slot* ptr = (Slot*)(base + 0x9c);
    for (s32 i = 0; i < 36; i++) {
        if (!((ptr->flags >> 6) & 1)) {
            result = 1;
            flags = a1[0];
            ptr->flags = flags;
            ptr->a = a1[1];
            ptr->b = a1[2];
            ptr->c = a1[3];
            ptr->d = a1[4];
            ptr->data = *(void**)(a1 + 8);
            ptr->flags = flags | 0x40;
            r28 = (Slot*)(base + 0x9c) + idx;
            break;
        } else {
            ptr++;
            idx++;
        }
    }
    if (result != 0) {
        u8* ptr2 = base + 0x24c;
        s32 found = 0;
        for (s32 i = 0; i < 4; i++) {
            if (ptr2[0] != 0 && *(u32*)(ptr2 + 4) == id) {
                found = 1;
                ptr2[0xd] += flag;
                ptr2[0xc] += 1;
                r28->d = (u8)i;
                break;
            }
            ptr2 += 16;
        }
        if (found == 0) {
            u8* ptr3 = base + 0x24c;
            for (s32 i = 0; i < 4; i++) {
                if (ptr3[0] == 0) {
                    ptr3[0] = 1;
                    ptr3[0xc] = 1;
                    ptr3[0xd] = flag;
                    r28->d = (u8)i;
                    *(u32*)(ptr3 + 4) = id;
                    *(s32*)(ptr3 + 8) = (s32)texResult;
                    break;
                }
                ptr3 += 16;
            }
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_228FD8 {
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct ModelEntry {
    u8 pad[0xE8];
    void* resource;
};

struct Model {
    u8 pad[0x44];
    ModelEntry** entries;
};

struct Slot {
    u8 unused : 1;
    u8 active : 1;
    u8 flags : 6;
    u8 modelIndex;
    u8 materialIndex;
    u8 pad[5];
    Model* model;
};

struct Ref {
    u8 active;
    u8 pad[3];
    u32 id;
    u32 unk8;
    u8 count;
    u8 padD[3];
};

struct State {
    u8 pad[0x9C];
    Slot slots[36];
    Ref refs[4];
};

struct Material {
    u8 pad[0x30];
    s32 offset;
};

extern "C" Material* GetResMat__Q34nw4r3g3d6ResMdlCFi(void*, u32);
extern "C" u32 GetResTex__Q34nw4r3g3d14ResTexPlttInfoCFv(void**);

extern "C" void fn_27_228FD8(State* self, Model* model)
{
    Slot* slot = self->slots;
    for (u32 i = 0; i < 36; ++i, ++slot) {
        if (slot->active && slot->model == model) {
            slot->active = 0;
            void* resource = slot->model->entries[slot->modelIndex]->resource;
            Material* mat = GetResMat__Q34nw4r3g3d6ResMdlCFi(&resource, slot->materialIndex);
            void* data = mat->offset ? (u8*)mat + mat->offset : 0;
            u32 id = GetResTex__Q34nw4r3g3d14ResTexPlttInfoCFv(&data);
            Ref* ref = self->refs;
            while (i < 4) {
                if (ref->active && ref->id == id) {
                    if (--ref->count == 0)
                        ref->active = 0;
                    break;
                }
                ++ref;
            }
        }
    }
}
}

