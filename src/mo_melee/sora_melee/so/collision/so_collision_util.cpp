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
