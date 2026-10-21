// Local BrawlHeaders shadow: verified camera storage and recovered operations.
#pragma once

#include <StaticAssert.h>
#include <stddef.h>
namespace nw4r { namespace g3d { class Camera; } }
#include <mt/mt_vector.h>
#include <types.h>

class gfCamera {
public:
    struct OrthoParam {
        float m_top;
        float m_bottom;
        float m_left;
        float m_right;
    };

    enum ProjectionType {
        Projection_Orthogonal = 0x0,
        Projection_Perspective = 0x1,
    };

    union TransformFlag {
        u16 m_mask;
        struct {
            bool m_flag15 : 1;      // 0x8000
            bool m_flag14 : 1;      // 0x4000
            bool m_flag13 : 1;      // 0x2000
            bool m_flag12 : 1;      // 0x1000
            bool m_flag11 : 1;      // 0x800
            bool m_flag10 : 1;      // 0x400
            bool m_flag9 : 1;       // 0x200
            bool m_flag8 : 1;       // 0x100
            bool m_flag7 : 1;       // 0x80
            bool m_rotate : 1;       // 0x40
            bool m_flag5 : 1;       // 0x20
            bool m_flag4 : 1;       // 0x10
            bool m_flag3 : 1;       // 0x8
            bool m_flag2 : 1;       // 0x4
            bool m_flag1 : 1;       // 0x2
            bool m_flag0 : 1;       // 0x1
        };
    };

    // HYPOTHESIS: local field names; extents verified by view construction and consumers.
    float m_view[3][4];
    float m_inverseView[3][4];
    Vec3f m_targetPos;
    Vec3f m_centerPos;
    Vec3f m_direction;
    Vec3f m_up;
    Vec3f m_0x90;
    Vec3f m_0x9C;
    Vec3f m_targetOffset;
    Vec3f m_rotationOffset;
    Vec3f m_rot;
    float unkCC;
    float unkD0;
    float m_fovScale;
    float m_fovY;
    float m_near;
    float m_far;
    float m_aspect;
    OrthoParam m_orthoParam;
    TransformFlag m_transformFlag;
    TransformFlag unkFA;

    s32 unkFC; // HYPOTHESIS: 0 applies world offsets; 1 applies an eye-space translation.
    ProjectionType m_projectionType;
    float m_tanHalfFovX, m_tanHalfFovY;
    float m_secHalfFovX, m_secHalfFovY;
    float m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight;
    float m_viewportNear, m_viewportFar;
    u16 m_scissorX, m_scissorY, m_scissorWidth, m_scissorHeight;

    // HYPOTHESIS: source type name/qualifiers are unknown; callers pass XYZ followed by radius.
    struct ClipSphere { Vec3f center; float radius; };
    void init();
    // HYPOTHESIS: NW4R handle passing/const qualifiers are not recovered from the map.
    void setNwCamera(nw4r::g3d::Camera*);
    void setFromNwCamera(nw4r::g3d::Camera*);
    void setFromNwCameraAllParam(nw4r::g3d::Camera*);
    bool isClipIn(const ClipSphere*);
    bool isClipInAll(const ClipSphere*);
    u8 getClipOutStatus(const ClipSphere*);
    void getScreenPosition2WorldVector(Vec3f*, float, float);

    void calc();
    void setGX();
    void calcProjection3Dto2D(Vec3f*, Vec2f*);
    void renderDebug();
};
static_assert(sizeof(gfCamera) == 308, "Class is wrong size!");

class gfCameraManager {
public:
    gfCamera m_cameras[6];
    // HYPOTHESIS: aspect correction from gfVideo and the selected 4:3/16:9 ratio.
    float m_aspectRatio, m_aspectCorrection;
    gfCameraManager();
    static gfCameraManager* create();
    void setAspectRatio(int);

    inline gfCamera* getCamera(u32 index) { return &m_cameras[index]; }

    static gfCameraManager* getManager();
};
static_assert(sizeof(gfCameraManager) == 0x740, "Class is wrong size!");

static_assert(offsetof(gfCamera, m_targetPos) == 0x60, "camera target");
static_assert(offsetof(gfCamera, m_projectionType) == 0x100, "camera projection");
static_assert(offsetof(gfCamera, m_viewportX) == 0x114, "camera viewport");
static_assert(offsetof(gfCameraManager, m_aspectRatio) == 0x738, "camera manager aspect");
static_assert(sizeof(gfCamera::ClipSphere) == 0x10, "camera sphere view");
