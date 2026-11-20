#include <nw4r/g3d.h>
#include <nw4r/math.h>

namespace nw4r {
namespace g3d {

void ResNode::PatchChrAnmResult(ChrAnmResult* pResult) const {
    if (!IsValid()) {
        return;
    }

    const ResNodeData& r = ref();
    u32 flags = pResult->flags;

    if (flags & ChrAnmResult::FLAG_PATCH_SCALE) {
        if (r.flags & ResNodeData::FLAG_SCALE_ONE) {
            flags |=
                ChrAnmResult::FLAG_SCALE_ONE | ChrAnmResult::FLAG_SCALE_UNIFORM;

            pResult->s.z = 1.0f;
            pResult->s.y = 1.0f;
            pResult->s.x = 1.0f;
        } else {
            u32 uniform = flags & ~ChrAnmResult::FLAG_SCALE_UNIFORM;

            if (r.flags & ResNodeData::FLAG_SCALE_UNIFORM) {
                uniform = flags | ChrAnmResult::FLAG_SCALE_UNIFORM;
            }

            flags = uniform & ~(ChrAnmResult::FLAG_MTX_IDENT |
                                ChrAnmResult::FLAG_SCALE_ONE);

            pResult->s.x = r.scale.x;
            pResult->s.y = r.scale.y;
            pResult->s.z = r.scale.z;
        }
    }

    if (flags & ChrAnmResult::FLAG_PATCH_ROT) {
        math::VEC3 trans(pResult->rt._03, pResult->rt._13, pResult->rt._23);

        if (r.flags & ResNodeData::FLAG_ROT_ZERO) {
            math::MTX34Identity(&pResult->rt);
            flags |= ChrAnmResult::FLAG_ROT_ZERO;
        } else {
            math::MTX34RotXYZDeg(&pResult->rt, r.rot.x, r.rot.y, r.rot.z);
            flags &= ~(ChrAnmResult::FLAG_ROT_ZERO |
                       ChrAnmResult::FLAG_ROT_TRANS_ZERO |
                       ChrAnmResult::FLAG_MTX_IDENT);
        }

        flags |= ChrAnmResult::FLAG_ROT_RAW_FMT;

        pResult->rt._03 = trans.x;
        pResult->rt._13 = trans.y;
        pResult->rt._23 = trans.z;
    }

    if (flags & ChrAnmResult::FLAG_PATCH_TRANS) {
        if (r.flags & ResNodeData::FLAG_TRANS_ZERO) {
            flags |= ChrAnmResult::FLAG_TRANS_ZERO;

            pResult->rt._23 = 0.0f;
            pResult->rt._13 = 0.0f;
            pResult->rt._03 = 0.0f;
        } else {
            flags &= ~(ChrAnmResult::FLAG_TRANS_ZERO |
                       ChrAnmResult::FLAG_ROT_TRANS_ZERO |
                       ChrAnmResult::FLAG_MTX_IDENT);

            pResult->rt._03 = r.translate.x;
            pResult->rt._13 = r.translate.y;
            pResult->rt._23 = r.translate.z;
        }
    }

    if (flags & ChrAnmResult::FLAG_ROT_ZERO) {
        if (flags & ChrAnmResult::FLAG_TRANS_ZERO) {
            flags |= ChrAnmResult::FLAG_ROT_TRANS_ZERO;

            if (flags & ChrAnmResult::FLAG_SCALE_ONE) {
                flags |= ChrAnmResult::FLAG_MTX_IDENT;
            }
        }
    }

    pResult->flags = flags & ~(ChrAnmResult::FLAG_PATCH_SCALE |
                               ChrAnmResult::FLAG_PATCH_ROT |
                               ChrAnmResult::FLAG_PATCH_TRANS);
}

void ResNode::CalcChrAnmResult(ChrAnmResult* pResult) const {
    if (!IsValid()) {
        return;
    }

    const ResNodeData& r = ref();
    u32 flags = 0;

    if (r.flags & ResNodeData::FLAG_SCALE_ONE) {
        flags |=
            ChrAnmResult::FLAG_SCALE_ONE | ChrAnmResult::FLAG_SCALE_UNIFORM;

        pResult->s.z = 1.0f;
        pResult->s.y = 1.0f;
        pResult->s.x = 1.0f;
    } else {
        if (r.flags & ResNodeData::FLAG_SCALE_UNIFORM) {
            flags |= ChrAnmResult::FLAG_SCALE_UNIFORM;
        }

        pResult->s = static_cast<const math::VEC3&>(r.scale);
    }

    if (r.flags & ResNodeData::FLAG_ROT_ZERO) {
        PSMTXIdentity(pResult->rt);
        flags |= ChrAnmResult::FLAG_ROT_ZERO;
    } else {
        pResult->rawR = math::VEC3(r.rot);
        math::MTX34RotXYZDeg(&pResult->rt, r.rot.x, r.rot.y, r.rot.z);
    }

    if (r.flags & ResNodeData::FLAG_TRANS_ZERO) {
        flags |= ChrAnmResult::FLAG_TRANS_ZERO;
    } else {
        pResult->rt._03 = r.translate.x;
        pResult->rt._13 = r.translate.y;
        pResult->rt._23 = r.translate.z;
    }

    if (flags & ChrAnmResult::FLAG_ROT_ZERO) {
        if (flags & ChrAnmResult::FLAG_TRANS_ZERO) {
            flags |= ChrAnmResult::FLAG_ROT_TRANS_ZERO;

            if (flags & ChrAnmResult::FLAG_SCALE_ONE) {
                flags |= ChrAnmResult::FLAG_MTX_IDENT;
            }
        }
    }

    flags |= ChrAnmResult::FLAG_ROT_RAW_FMT;
    flags |= ChrAnmResult::FLAG_ANM_EXISTS;

    if (r.flags & ResNodeData::FLAG_SSC_APPLY) {
        flags |= ChrAnmResult::FLAG_SSC_APPLY;
    }

    if (r.flags & ResNodeData::FLAG_SSC_PARENT) {
        flags |= ChrAnmResult::FLAG_SSC_PARENT;
    }

    pResult->flags = flags;
}

void ResNode::SetScale(f32 x, f32 y, f32 z) {
    if (IsValid()) {
        ResNodeData& r = ref();

        if (x == y && y == z) {
            r.flags |= ResNodeData::FLAG_SCALE_UNIFORM;

            if (x == 1.0f) {
                r.flags |= ResNodeData::FLAG_SCALE_ONE;
            } else {
                r.flags &= ~ResNodeData::FLAG_SCALE_ONE;
            }
        } else {
            r.flags &= ~(ResNodeData::FLAG_SCALE_UNIFORM |
                         ResNodeData::FLAG_SCALE_ONE);
        }

        if ((r.flags & ResNodeData::FLAG_TRANS_ZERO) &&
            (r.flags & ResNodeData::FLAG_ROT_ZERO) &&
            (r.flags & ResNodeData::FLAG_SCALE_ONE)) {
            r.flags |= ResNodeData::FLAG_IDENTITY;
        } else {
            r.flags &= ~ResNodeData::FLAG_IDENTITY;
        }

        r.scale.x = x;
        r.scale.y = y;
        r.scale.z = z;
    }
}

void ResNode::SetTranslate(f32 x, f32 y, f32 z) {
    if (IsValid()) {
        ResNodeData& r = ref();

        if (x == 0.0f && y == 0.0f && z == 0.0f) {
            r.flags |= ResNodeData::FLAG_TRANS_ZERO;
        } else {
            r.flags &= ~ResNodeData::FLAG_TRANS_ZERO;
        }

        if ((r.flags & ResNodeData::FLAG_TRANS_ZERO) &&
            (r.flags & ResNodeData::FLAG_ROT_ZERO) &&
            (r.flags & ResNodeData::FLAG_SCALE_ONE)) {
            r.flags |= ResNodeData::FLAG_IDENTITY;
        } else {
            r.flags &= ~ResNodeData::FLAG_IDENTITY;
        }

        r.translate.x = x;
        r.translate.y = y;
        r.translate.z = z;
    }
}

void ResNode::SetRotate(f32 x, f32 y, f32 z) {
    if (IsValid()) {
        ResNodeData& r = ref();

        if (x == 0.0f && y == 0.0f && z == 0.0f) {
            r.flags |= ResNodeData::FLAG_ROT_ZERO;
        } else {
            r.flags &= ~ResNodeData::FLAG_ROT_ZERO;
        }

        if ((r.flags & ResNodeData::FLAG_TRANS_ZERO) &&
            (r.flags & ResNodeData::FLAG_ROT_ZERO) &&
            (r.flags & ResNodeData::FLAG_SCALE_ONE)) {
            r.flags |= ResNodeData::FLAG_IDENTITY;
        } else {
            r.flags &= ~ResNodeData::FLAG_IDENTITY;
        }

        r.rot.x = x;
        r.rot.y = y;
        r.rot.z = z;
    }
}

} // namespace g3d
} // namespace nw4r

// TODO: clean up, give variables real names
namespace jj_80192ECC {
extern "C" float lbl_805A31A0;
struct Inner { char pad[0x14]; unsigned int flags; char pad2[8]; float x,y,z; };
extern "C" void ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(Inner** pp, float x, float y, float z) {
 Inner* p = *pp;
 if (!p) return;
 if (x == y && y == z) {
  p->flags |= 0x10;
  if (1.0f == x) p->flags |= 8;
  else p->flags &= ~8u;
 } else p->flags &= ~0x18u;
 unsigned int flags = p->flags;
 if ((flags & 2) && (flags & 4) && (flags & 8))
  p->flags = flags | 1;
 else p->flags &= ~1u;
 p->x = x;
 p->y = y;
 p->z = z;
}
}

// TODO: clean up, give variables real names
namespace jj_80192FF8 {
extern "C" f32 lbl_805A31A8;

struct Obj {
    u8 pad0[0x14];
    u32 flags;
    u8 pad1[0x2c - 0x18];
    f32 m_x;
    f32 m_y;
    f32 m_z;
};

extern "C" {

void ResNode_SetScale__FPQ34nw4r3g3d7ResNodefff(Obj** arg0, f32 farg0, f32 farg1, f32 farg2) {
    Obj* obj = *arg0;
    u32 flags;
    if (obj == 0) {
        return;
    }
    if (lbl_805A31A8 == farg0 && lbl_805A31A8 == farg1 && lbl_805A31A8 == farg2) {
        obj->flags |= 4;
    } else {
        obj->flags &= ~4u;
    }
    flags = obj->flags;
    if ((flags & 2) && (flags & 4) && (flags & 8)) {
        obj->flags = flags | 1;
    } else {
        obj->flags &= ~1u;
    }
    obj->m_x = farg0;
    obj->m_y = farg1;
    obj->m_z = farg2;
}

}
}

