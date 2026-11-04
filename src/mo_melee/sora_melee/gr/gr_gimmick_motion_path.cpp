#include <types.h>
#include <gr/gr_gimmick_motion_path.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale);
extern const float lbl_27_rodata_43C0[2];

void grGimmickMotionPath::setTRS() {
    nw4r::g3d::ScnMdl* model = m_sceneModels[unk178];
    if (model) {
        Matrix matrix(true);
        if (model->GetScnMtxPos(&matrix, nw4r::g3d::ScnObj::MTX_WORLD, m_nodeIndex)) {
            Vec3f position = matrix.getTranslation();
            Vec3f rotation;
            Vec3f scale;
            matrix.getRotate(&rotation);
            getScalingFactor(&matrix, &scale);
            float rotationX, rotationY, rotationZ;
            rotationX = rotation.m_x;
            rotationY = rotation.m_y;
            rotationX = lbl_27_rodata_43C0[0] * rotationX;
            rotationZ = rotation.m_z;
            rotationY = lbl_27_rodata_43C0[0] * rotationY;
            rotationZ = lbl_27_rodata_43C0[0] * rotationZ;
            rotation.m_x = rotationX;
            rotation.m_y = rotationY;
            rotation.m_z = rotationZ;
            setPos(&position);
            setRot(&rotation);
            setScale(&scale);
        }
    }
}

void grGimmickMotionPath::playDefaultSE() {
    if (m_soundEffects) {
        for (int index = 0; index < 3; ++index) {
            startGimmickSE(index);
        }
    }
    if (m_effects) {
        for (int index = 0; index < 3; ++index) {
            startGimmickEffect(index);
        }
    }
}
