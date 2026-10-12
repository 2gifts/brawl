#include <sora/ms_message.h>
#include <types.h>

extern "C" void __destroy_arr(void*, void (*)(void*), u32, u32);
extern "C" void fn_8006CE1C(void*);
// HYPOTHESIS: fn_8006E7F0 tears down a sub-object at this, with a delete-flag argument passed as 0.
extern "C" void fn_8006E7F0(void*, s32);

MsgTagProc::~MsgTagProc() {
    __destroy_arr(&m_data[0x90], fn_8006CE1C, 0x10, 8);
    __destroy_arr(&m_data[0xC], fn_8006CE1C, 0x10, 8);
    fn_8006E7F0(this, 0);
}


extern "C" void __construct_array(void*, void (*)(void*), void (*)(void*), u32, u32);
extern "C" void fn_8006CE10(void*);
extern "C" void fn_8006CE1C(void*);
extern "C" void fn_8006E7E0();
extern "C" u8 lbl_80454360[];

MsgTagProc::MsgTagProc() {
    fn_8006E7E0();
    *reinterpret_cast<void**>(m_data) = lbl_80454360;
    __construct_array(&m_data[0xC], fn_8006CE10, fn_8006CE1C, 0x10, 8);
    __construct_array(&m_data[0x90], fn_8006CE10, fn_8006CE1C, 0x10, 8);
}
