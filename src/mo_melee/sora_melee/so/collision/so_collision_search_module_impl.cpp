#include <gf/gf_camera.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/collision/so_collision_search_part.h>
#include <types.h>

// HYPOTHESIS: the part layout as the module reads it; ft/builder/ft_dol_types.h keeps an opaque copy for the fighter RELs.
class soCollisionSearchPart {
public:
    int m_status; // HYPOTHESIS: nonzero while the part searches
    soCollisionSearchData m_data;
    soArrayVector<clTarget, 6> m_targets;
    int unk58;
    int unk5C;

    void reset();
    void debugDisplay(nw4r::math::_MTX34* view, void* collision);
};
static_assert(sizeof(soCollisionSearchPart) == 0x60, "Class is wrong size!");

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soStatusDataView {
    char _0[0xc];
    u32 : 23;
    u32 m_keepSearch : 1;
};

soCollisionSearchModule::~soCollisionSearchModule() { }

void soCollisionSearchModuleImpl::begin() {
    m_nodeIndex = -1;
    unk104 = 0;
}

void soCollisionSearchModuleImpl::clearAll() {
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        m_collisionSearchPartArray->at(i).m_status = 0;
    }
}

void soCollisionSearchModuleImpl::clear(u32 partIndex) {
    soCollisionSearchPart& part = m_collisionSearchPartArray->at(partIndex);
    part.m_status = 0;
}

bool soCollisionSearchModuleImpl::isSearch(u32 partIndex) {
    return m_collisionSearchPartArray->at(partIndex).m_status != 0;
}

float soCollisionSearchModuleImpl::getSize(u32 partIndex) {
    return m_collisionSearchPartArray->at(partIndex).m_data.m_size;
}

void soCollisionSearchModuleImpl::setSize(u32 partIndex, float size) {
    m_collisionSearchPartArray->at(partIndex).m_data.m_size = size;
}

bool soCollisionSearchModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

void soCollisionSearchModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (!((soStatusDataView*)statusData)->m_keepSearch) {
        clearAll();
    }
}

void soCollisionSearchModuleImpl::setCheckInterval(int interval) {
    unk100 = interval;
    m_checkInterval = interval;
}

int soCollisionSearchModuleImpl::isInflict() {
    return m_isInflict;
}

void soCollisionSearchModuleImpl::activate(float param, int taskId) {
    m_collision.m_taskId = taskId;
    m_48 = param;
    unk100 = m_checkInterval;
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        m_collisionSearchPartArray->at(i).reset();
    }
    m_isInflict = 0;
}

void soCollisionSearchModuleImpl::deactivate() {
    if (unk105 == 0) {
        m_collision.clean();
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
    m_collision.m_taskId = -1;
}

void soCollisionSearchModuleImpl::setPos(int nodeIndex, Vec3f* startOffsetPos, Vec3f* endOffsetPos) {
    float ez, ey, ex, sz, sy, sx;
    sx = startOffsetPos->m_x;
    sy = startOffsetPos->m_y;
    sz = startOffsetPos->m_z;
    ex = endOffsetPos->m_x;
    ey = endOffsetPos->m_y;
    ez = endOffsetPos->m_z;
    m_startOffsetPos.m_x = sx;
    m_startOffsetPos.m_y = sy;
    m_startOffsetPos.m_z = sz;
    m_endOffsetPos.m_x = ex;
    m_endOffsetPos.m_y = ey;
    m_endOffsetPos.m_z = ez;
    m_nodeIndex = nodeIndex;
}

void soCollisionSearchModuleImpl::renderDebug() {
    int groupIndex;
    nw4r::math::_MTX34 view = gfCameraManager::getManager()->m_cameras[0].m_viewMatrix;
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        if (m_collisionSearchPartArray->at(i).m_status != 0) {
            if (m_checkInterval == 0 || unk100 == m_checkInterval) {
                groupIndex = m_collisionSearchPartArray->at(i).unk58;
                soCollisionSearchPart& part = m_collisionSearchPartArray->at(i);
                part.debugDisplay(&view, m_collision.getCollision(i, groupIndex));
            }
        }
    }
}
