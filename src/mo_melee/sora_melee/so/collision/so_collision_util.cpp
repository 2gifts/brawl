#include <types.h>
#include <so/collision/so_collision_util.h>
#include <so/model/so_model_module_simple.h>
#include <cl/cl_collision.h>

void soCollisionUtil::renewCollision(soModelModule* model, newCollision* collision,
                                    int shape, u32 nodeId, const Vec3f* offset0,
                                    const Vec3f* offset1) {
    Matrix matrix(true);
    Vec3f position0;
    Vec3f position1;
    if (offset0) {
        Vec3f offset = *offset0;
        position0 = model->getNodeGlobalPosition(nodeId, &offset, false, false);
    } else {
        position0 = model->getNodeGlobalPosition(nodeId, false);
    }
    switch (shape) {
    case 1:
        if (offset1) {
            Vec3f offset = *offset1;
            position1 = model->getNodeGlobalPosition(nodeId, &offset, false, false);
        } else {
            position1 = model->getNodeGlobalPosition(nodeId, false);
        }
        model->getNodeGlobalMtx(nodeId, &matrix, false);
        collision->renew(&position0, &position1, &matrix);
        break;
    case 0:
        position1.m_x = position1.m_y = position1.m_z = 0.0f;
        collision->renew(&position0, &position1, 0);
        break;
    }
}

// TODO: clean up, give variables real names
namespace jj_55BB8 {
struct Vec3 { float x,y,z; };
struct Object_vptr_30_base { u32 d0,d1,d2; u8 pad[0x24]; };
class Object_vptr_30 : public Object_vptr_30_base {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2(Vec3*,Vec3*,u32,float);
};
inline void copy(Vec3* dst,const Vec3* src) {
float x,y,z;
x=src->x; y=src->y; z=src->z;
dst->x=x; dst->y=y; dst->z=z;
}
extern "C" void fn_27_55BB8(Object_vptr_30* self,s32 id,u32 flag,Vec3* __restrict a,Vec3* b, float unused) {
float z,y,x;
x=a->x; y=a->y; z=a->z;
Vec3 v1,v2;
v1.x=x; v1.y=y; v1.z=z;
switch(id) {
case 1: copy(&v2,b); break;
case 0:
if(flag==1) { v2.x=x; v2.y=y; v2.z=z; }
else { struct Bits {u32 x,y,z;}; Bits t=*(Bits*)self; copy(&v2,(Vec3*)&t); }
break;
}
self->slot2(&v1,&v2,0,unused);
}
}

