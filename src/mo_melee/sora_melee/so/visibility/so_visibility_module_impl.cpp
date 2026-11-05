#include <so/visibility/so_visibility_module_impl.h>
#include <types.h>
#include <ac/ac_anim_cmd_impl.h>

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

bool soVisibilityModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 4) {
        return false;
    }
    switch (acmd->getType()) {
    case 0: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        const acCmdArg& index = acGetCmdArg(args, 0);
        acShiftCmdArg(args);
        const acCmdArg& value = acGetCmdArg(args, 0);
        set(index.getIntData(), value.getIntData());
        return true;
    }
    case 1: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        const acCmdArg& index = acGetCmdArg(args, 0);
        acShiftCmdArg(args);
        const acCmdArg& value = acGetCmdArg(args, 0);
        setStatusDefault(index.getIntData(), value.getIntData(), true);
        return true;
    }
    case 3: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        const acCmdArg& index = acGetCmdArg(args, 0);
        u32 setIndex = index.getIntData();
        set(setIndex, m_setData[setIndex * 3]);
        return true;
    }
    }
    return soVisibilityModuleSimple::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}
