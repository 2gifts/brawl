// hkMathUtil.o (main.dol 0x802821DC-0x802829C4)
// only decompose4x4ColTransform is in this build: splits a 4x4 matrix into translation, rotation, scale and skew.
// pretty much the havok 5.1 version, the calls in the asm line up with it one for one
//   0x802821DC  2024  decompose4x4ColTransform
#include <havok/hkMathUtil.h>
#include <havok/hkMatrix3.h>

// can't pull in hkTransform.h here (see hkMathUtil.h), so this is just the bit we need of it
struct hkTransform {
    hkRotation m_rotation;
    hkVector4 m_translation;

    void set4x4ColumnMajor(const hkReal* p);
};

// the identity transform sitting in .data (hkTransform::getIdentity), used for the skew check at the end
extern hkMatrix3 lbl_804864F0;

static inline hkMatrix3& asMat(hkMatrix3Storage& s) { return *reinterpret_cast<hkMatrix3*>(&s); }
static inline hkVector4& colOf(hkMatrix3& m, int c) { return *reinterpret_cast<hkVector4*>(&m.elements[c * 4]); }

void hkMathUtil::decompose4x4ColTransform(const hkReal* matrixIn, Decomposition& out) {
    // this is an iteration so give up after 30 rounds (5.1 says it usually takes about 7)
    const int maxNumIterations = 30;
    const hkReal tolerance = 1.1920929e-07f;

    hkTransform t;
    t.set4x4ColumnMajor(matrixIn);

    out.m_translation = t.m_translation;

    // basis: keep averaging the matrix with its inverse transpose until it stops changing, that converges
    // on the closest pure rotation (polar decomposition)
    {
        hkRotation qCur, qNext, qInvT;

        int numIterations = 0;

        qNext = t.m_rotation;
        do {
            qCur = qNext;

            qInvT = qCur;
            qInvT.invert(tolerance);
            qInvT.transpose();

            hkMatrix3 temp;
            temp = qCur;
            temp.add(qInvT);
            qNext.setMul(0.5f, temp);

            numIterations++;
        } while (numIterations < maxNumIterations && !qNext.isApproximatelyEqual(tolerance, qCur));

        asMat(out.m_basis) = qNext;
    }

    // rotation quaternion + whether the basis mirrors things
    {
        hkMatrix3& basis = asMat(out.m_basis);
        hkVector4 c1 = colOf(basis, 0);
        c1.normalize3();
        hkVector4 c2 = colOf(basis, 1);
        c2.normalize3();
        hkVector4 c3 = colOf(basis, 2);
        c3.normalize3();

        hkVector4 r0;
        r0.setCross(c2, c3);
        hkReal determinant = c1.dot3(r0);

        if (determinant < 0.0f) {
            // handedness got flipped, so flip one column back. the scale will undo it
            c1.mul4(-1.0f);
        }

        out.m_flips = (determinant < 0.0f);

        {
            hkRotation rot;
            colOf(rot, 0) = c1;
            colOf(rot, 1) = c2;
            colOf(rot, 2) = c3;
            out.m_rotation.set(rot);
            out.m_rotation.normalize();
        }
    }

    // scale and skew: undo the rotation and what's left over is both
    {
        hkQuaternion rotInvQ;
        rotInvQ.m_vec.set(-out.m_rotation.m_vec.x, -out.m_rotation.m_vec.y, -out.m_rotation.m_vec.z, out.m_rotation.m_vec.w);
        hkRotation rotInvM;
        rotInvM.set(rotInvQ);

        asMat(out.m_scaleAndSkew).setMul(rotInvM, t.m_rotation);
    }

    // scale is just the diagonal
    {
        hkMatrix3& ss = asMat(out.m_scaleAndSkew);
        out.m_scale.set(ss.elements[0], ss.elements[5], ss.elements[10], 0.0f);

        // HYPOTHESIS: 0.001 is the tolerance for "is the scale 1". 5.1 uses equals3's default for this
        out.m_hasScale = !(fabsf(out.m_scale.x - 1.0f) < 0.001f && fabsf(out.m_scale.y - 1.0f) < 0.001f &&
                           fabsf(out.m_scale.z - 1.0f) < 0.001f);
    }

    // skew: scaleAndSkew = scale * skew, so skew = inverse(scale) * scaleAndSkew. if that isn't the identity there's skew
    {
        hkMatrix3 scaleInv;
        for (int i = 0; i < 12; i++) {
            scaleInv.elements[i] = 0.0f;
        }
        scaleInv.elements[0] = 1.0f / out.m_scale.x;
        scaleInv.elements[5] = 1.0f / out.m_scale.y;
        scaleInv.elements[10] = 1.0f / out.m_scale.z;
        asMat(out.m_skew).setMul(scaleInv, asMat(out.m_scaleAndSkew));

        out.m_hasSkew = !asMat(out.m_skew).isApproximatelyEqual(0.001f, lbl_804864F0);
    }
}
