#include <so/visibility/so_visibility_module_impl.h>
#include <types.h>

// HYPOTHESIS: status metadata words followed by transition preservation flags; soStatusData itself stays empty because
// the enemy's emStatusData derives from it.
struct soVisibilityStatusData : public soStatusData {
    unsigned int unk0;
    unsigned int unk4;
    unsigned int unk8;
    unsigned int unkC;
};

soVisibilityModuleImpl::~soVisibilityModuleImpl() {
    delete[] m_setData;
}

void soVisibilityModuleImpl::setStatusDefault(u32 index, int value, bool apply) {
    m_setData[index * 3] = value;
    unk2C = true;
    if (apply == true) {
        set(index, m_setData[index * 3]);
    }
}

void soVisibilityModuleImpl::set(u32 index, int value) {
    s8* setData = &m_setData[index * 3];
    if (value != setData[1]) {
        setData[1] = value;
        unk2C = true;
    }
}

int soVisibilityModuleImpl::get(u32 index) {
    return m_setData[index * 3 + 1];
}

void soVisibilityModuleImpl::setData(soVisibilityData* data) {
    m_data = data;
}

void soVisibilityModuleImpl::activate() {
    int count = m_data->unkC;
    for (int i = 0; i < count; ++i) {
        setStatusDefault(m_data->unk8[i].unk0, m_data->unk8[i].unk4, false);
    }
    setDefaultAll();
    soVisibilityModuleSimple::activate();
}

void soVisibilityModuleImpl::setDefaultAll() {
    if (unk2C == true) {
        for (int i = 0; i < m_data->unk4; ++i) {
            set(i, m_setData[i * 3]);
        }
        unk2C = false;
    }
}

void soVisibilityModuleImpl::hideAll() {
    for (int i = 0; i < m_data->unk4; ++i) {
        set(i, -1);
    }
}

void soVisibilityModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (!((static_cast<soVisibilityStatusData*>(statusData)->unkC >> 6) & 1)) {
        setDefaultAll();
    }
}
