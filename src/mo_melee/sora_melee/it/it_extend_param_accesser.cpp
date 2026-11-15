#include <it/it_extend_param_accesser.h>
#include <it/it_resource_module_impl.h>
#include <so/stageobject.h>
#include <types.h>

// HYPOTHESIS: the per-variation parameter table of the item resource (the words the accesser reads).
struct itExtendParamTable {
    u8 unk0[8];
    s32* m_ints;     // 0x8
    u32 m_intNum;    // 0xC
    float* m_floats; // 0x10
    u32 m_floatNum;  // 0x14
};

// HYPOTHESIS: the item parameter block of the resource that points at the table (itResourceModuleImpl::unk34).
struct itExtendParamHolder {
    u8 unk0[0xEC];
    itExtendParamTable* m_table; // 0xEC
};

itExtendParamAccesser::itExtendParamAccesser() { }

// The instance it_value_accesser.cpp hands the extended ids to.
itExtendParamAccesser lbl_27_bss_71DC;

float itExtendParamAccesser::getParamFloat(soModuleAccesser* moduleAccesser, s32 id, s32 index) {
    if (id <= 3999) {
        return soValueAccesser::getConstantFloat(moduleAccesser, id, index);
    }
    itResourceModuleImpl& resource = dynamic_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule());
    itExtendParamHolder* params = reinterpret_cast<itExtendParamHolder*>(resource.unk34);
    if (params->m_table != NULL) {
        itExtendParamTable* table;
        int i = index + (id - 4000 - moduleAccesser->getStageObject().soGetSubKind() * 50);
        table = params->m_table;
        if (i >= 0 && (u32)i < table->m_floatNum) {
            return table->m_floats[i];
        }
    }
    return 0.0f;
}

s32 itExtendParamAccesser::getParamInt(soModuleAccesser* moduleAccesser, s32 id, s32 index) {
    if (id <= 23999) {
        return soValueAccesser::getConstantInt(moduleAccesser, id, index);
    }
    itResourceModuleImpl& resource = dynamic_cast<itResourceModuleImpl&>(moduleAccesser->getResourceModule());
    itExtendParamHolder* params = reinterpret_cast<itExtendParamHolder*>(resource.unk34);
    if (params->m_table != NULL) {
        itExtendParamTable* table;
        int i = index + (id - 24000 - moduleAccesser->getStageObject().soGetSubKind() * 50);
        table = params->m_table;
        if (i >= 0 && (u32)i < table->m_intNum) {
            return table->m_ints[i];
        }
    }
    return 0;
}
