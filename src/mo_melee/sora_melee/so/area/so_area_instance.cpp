#include <gf/gf_area.h>
#include <memory.h>
#include <so/area/so_area_instance.h>
#include <so/area/so_area_module_impl.h>
#include <st/st_area_manager.h>
#include <types.h>

extern stAreaManager* lbl_27_bss_5780;

void soAreaInstance::setOwnerId(int ownerId) {
    m_area->m_ownerId = ownerId;
}

void soAreaInstance::deactivate() {
    if (m_area->m_isRegist == true) {
        m_area->unRegist();
    }
}

bool soAreaInstance::isFlags(u16 mask) {
    return (m_data->m_shapeFlag.m_mask & mask) != 0;
}

u32 soAreaInstance::getId() {
    if (m_isCreated) {
        return m_area->m_id;
    }
    return 0;
}

void soAreaInstance::enable(bool isEnable, int frame) {
    if (isEnable) {
        m_area->m_isDrawFill = true;
    } else {
        m_area->m_isDrawFill = false;
    }
    m_isEnable = isEnable;
    m_enableFrame = frame;
}

void soAreaInstance::setWhole(bool isWhole) {
    if (!isWhole) {
        m_area->m_isDrawFill = false;
    }
}

void soAreaInstance::setCheckTargetGroup(int group) {
    m_area->m_checkTargetGroup = group;
}

void soAreaInstance::setCheckTargetLocalGroup(int group) {
    m_area->m_checkTargetLocalGroup = group;
}

void soAreaInstance::setTeamId(u32 teamId) {
    m_area->m_teamId = teamId;
}

u32 soAreaInstance::getTeamId() {
    return m_area->m_teamId;
}

void soAreaInstance::setShapeType(ShapeType shapeType) {
    m_shapeType = shapeType;
}

void soAreaInstance::destroy() {
    if (m_area != NULL) {
        if (m_isWindSet == true) {
            if (lbl_27_bss_5780 == NULL) {
                lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
            }
            lbl_27_bss_5780->erase(m_area->m_id);
            m_isWindSet = false;
        }
        delete m_area;
        m_area = NULL;
    }
}

void soAreaInstance::setShape(const Vec2f& center, const Vec2f& size) {
    dynamic_cast<gfAreaRectangle&>(*m_area).setShape(center, size);
    unkC = center.m_x;
    unk10 = center.m_y;
    unk14 = size.m_x;
    unk18 = size.m_y;
}

void soAreaInstance::setShape(const Vec2f& center, float radius) {
    dynamic_cast<gfAreaCircle&>(*m_area).setShape(center, radius);
    unkC = center.m_x;
    unk10 = center.m_y;
    unk14 = radius;
}

void soAreaInstance::setWind(const stMovementData& data) {
    stMovementAreaInfoData info;
    info.m_areaId = m_area->m_id;
    info.m_data = data;
    if (lbl_27_bss_5780 == NULL) {
        lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
    }
    lbl_27_bss_5780->setWind(&info);
    m_isWindSet = true;
}

void soAreaInstance::unsetWind() {
    if (m_isWindSet == true) {
        if (lbl_27_bss_5780 == NULL) {
            lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
        }
        lbl_27_bss_5780->erase(m_area->m_id);
        m_isWindSet = false;
    }
}
