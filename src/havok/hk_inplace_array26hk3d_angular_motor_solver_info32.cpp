// Havok translation unit hk_inplace_array26hk3d_angular_motor_solver_info32.o (main.dol 0x802DB9BC-0x802DBA6C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DB9BC    28  __ct   [map: hkInplaceArray_26hk3dAngularMotorSolverInfo_32_____ct]
//   0x802DB9D8   148  __dt   [map: hkInplaceArray_26hk3dAngularMotorSolverInfo_32_____dt]

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkInplaceArray<hk3dAngularMotorSolverInfo, 32>::hkInplaceArray(int size)
struct hkInplaceArrayHeader_802DB9BC {
    void* m_data;
    int m_size;
    int m_capacityAndFlags;
};
extern "C" void fn_802DB9BC(hkInplaceArrayHeader_802DB9BC* a, int size) {
    a->m_data = (char*)a + 0xC;
    a->m_size = size;
    a->m_capacityAndFlags = 0x80000020;
}
