#include <types.h>
#include <wn/wn_module_accesser_build_data.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnModuleAccesserBuildData::wnModuleAccesserBuildData(s32 kind, const void* data, Heaps::HeapType instHeap,
                                                     Heaps::HeapType nwModelInstHeap, Heaps::HeapType nwMotionInstHeap,
                                                     u8 resGroupNo, s32 unk10, soParamAccesser* paramAccesser, s32 unk18,
                                                     s32 unk1C, s32 unk2C, s32 unk30, s32 unk34, s32 unk38, s32 unk3C)
{
    m_kind = kind;
    m_data = data;
    m_resGroupNo = resGroupNo;
    this->unk10 = unk10;
    m_paramAccesser = paramAccesser;
    this->unk18 = unk18;
    this->unk1C = unk1C;
    m_instanceHeap = instHeap;
    m_nwModelInstanceHeap = nwModelInstHeap;
    m_nwMotionInstanceHeap = nwMotionInstHeap;
    this->unk2C = unk2C;
    this->unk30 = unk30;
    this->unk34 = unk34;
    this->unk38 = unk38;
    this->unk3C = unk3C;
}
