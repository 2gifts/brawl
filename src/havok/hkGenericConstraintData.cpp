// Havok translation unit hkGenericConstraintData.o (main.dol 0x802DBC80-0x802DD570).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DBC80    52  finishLoadedObjecthkGenericConstraintData   [map: hkGenericConstraintData__finishLoadedObjecthkGenericConstraintData]
//   0x802DBCB4    20  cleanupLoadedObjecthkGenericConstraintData   [map: hkGenericConstraintData__cleanupLoadedObjecthkGenericConstraintData]
//   0x802DBCC8    72  getVtablehkGenericConstraintData   [map: hkGenericConstraintData__getVtablehkGenericConstraintData]
//   0x802DBD10    88  __ct   [map: hkGenericConstraintData____ct]
//   0x802DBD68   416  __dt   [map: hkGenericConstraintData____dt]
//   0x802DBF08    76  getConstraintInfo   [map: hkGenericConstraintData__getConstraintInfo]
//   0x802DBF54    20  getRuntimeInfo   [map: hkGenericConstraintData__getRuntimeInfo]
//   0x802DBF68   104  buildJacobian   [map: hkGenericConstraintData__buildJacobian]
//   0x802DBFD0   260  constrainAllLinearW   [map: hkGenericConstraintData__constrainAllLinearW]
//   0x802DC0D4   112  calcDeltaAngleAroundAxis   [map: hkGenericConstraintDataParameters__calcDeltaAngleAroundAxis]
//   0x802DC144   248  setLinearFrictionW   [map: hkGenericConstraintData__setLinearFrictionW]
//   0x802DC23C  1212  hatchScheme   [map: hkGenericConstraintData__hatchScheme]
//   0x802DC6F8   108  _setRotatedDir   [map: hkVector4___setRotatedDir]
//   0x802DC764   208  constrainLinearW   [map: hkGenericConstraintData__constrainLinearW]
//   0x802DC834   100  __as   [map: hkRotation____as]
//   0x802DC898   404  constrainToAngularW   [map: hkGenericConstraintData__constrainToAngularW]
//   0x802DCA2C   428  constrainAllAngularW   [map: hkGenericConstraintData__constrainAllAngularW]
//   0x802DCBD8   240  setLinearLimitW   [map: hkGenericConstraintData__setLinearLimitW]
//   0x802DCCC8   344  setAngularLimitW   [map: hkGenericConstraintData__setAngularLimitW]
//   0x802DCE20   452  setConeLimitW   [map: hkGenericConstraintData__setConeLimitW]
//   0x802DCFE4   228  setTwistLimitW   [map: hkGenericConstraintData__setTwistLimitW]
//   0x802DD0C8   512  setAngularMotorW   [map: hkGenericConstraintData__setAngularMotorW]
//   0x802DD2C8   404  setLinearMotorW   [map: hkGenericConstraintData__setLinearMotorW]
//   0x802DD45C   152  setAngularFrictionW   [map: hkGenericConstraintData__setAngularFrictionW]
//   0x802DD4F4     4  __ct   [map: hkGenericConstraintDataParameters____ct]
//   0x802DD4F8     8  begin   [map: hkArray_i___begin]
//   0x802DD500     8  begin   [map: hkArray_9hkVector4___begin]
//   0x802DD508     8  begin   [map: hkArray_P20hkConstraintModifier___begin]
//   0x802DD510     8  isValid   [map: hkGenericConstraintData__isValid]
//   0x802DD518     8  getType   [map: hkGenericConstraintData__getType]
//   0x802DD520    80  __sinit_\hkGenericConstraintData_cpp   [map: hkGenericConstraintDatacpp____sinit_]

#include <havok/hkGenericConstraintData.h>
#include <havok/hkRegistry.h>
#include <havok/hkMath.h>

// Stand-in for a helper in another TU (not yet named).
extern "C" void fn_80288B40(void* a, void* b, u32 c, u32 d);

void hkGenericConstraintData::finishLoadedObjecthkGenericConstraintData(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkGenericConstraintData(flag);
}

void hkGenericConstraintData::cleanupLoadedObjecthkGenericConstraintData(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

#pragma push
#pragma auto_inline off
const void* hkGenericConstraintData::getVtablehkGenericConstraintData() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x58] __attribute__((aligned(16)));
    ::new (buf) hkGenericConstraintData(flag);
    return *(const void**)buf;
}
#pragma pop

hkGenericConstraintData::hkGenericConstraintData(hkFinishLoadedObjectFlag flag)
    : hkConstraintData(flag), m_scheme(flag) {
    m_atoms.m_bridgeAtom.init(m_atoms.m_bridgeAtom.m_constraintData);
    m_atoms.m_bridgeAtom.init(this);
}

// Releases the motors (reference counted), then the member arrays are destroyed in reverse order.
hkGenericConstraintData::~hkGenericConstraintData() {
    for (int i = 0; i < m_scheme.m_motors.m_size; i++) {
        ((hkReferencedObject**)m_scheme.m_motors.m_data)[i]->removeReference();
    }
}

void hkGenericConstraintData::getConstraintInfo(hkConstraintInfo* info) {
    ((hkConstraintInfoWithAtoms*)info)->unk10 = &m_atoms.m_bridgeAtom;
    ((hkConstraintInfoWithAtoms*)info)->unk14 = (u32)((u8*)&m_scheme - (u8*)&m_atoms.m_bridgeAtom);
    info->m_maxSizeOfJacobians = 0;
    info->m_sizeOfJacobians = 0;
    info->m_sizeOfSchemas = 0;
    info->m_numSolverResults = 0;
    info->m_maxSizeOfJacobians = m_scheme.m_info.m_maxSizeOfJacobians;
    info->m_sizeOfJacobians = m_scheme.m_info.m_sizeOfJacobians;
    info->m_sizeOfSchemas = m_scheme.m_info.m_sizeOfSchemas;
    info->m_numSolverResults = m_scheme.m_info.m_numSolverResults;
}

void hkGenericConstraintData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) {
    u32 n = m_scheme.m_info.m_numSolverResults;
    out->unk04 = n;
    out->unk00 = n * 8;
}

void hkGenericConstraintData::buildJacobian(void* a, void* b) {
    fn_80288B40(a, b, *(u32*)((u8*)a + 0x44), 8);
    hatchScheme(&m_scheme, a, b);
}

// this = R * v (rotation columns are 0x10 apart; the w lane is the constant 0). Same shape as hkSolver's _setMul3.
void hkVector4::setRotatedDir(const hkRotation& r, const hkVector4& v) {
    const hkVector4* c = (const hkVector4*)&r;
    set(c[0].x * v.x + c[1].x * v.y + c[2].x * v.z, c[0].y * v.x + c[1].y * v.y + c[2].y * v.z,
        c[0].z * v.x + c[1].z * v.y + c[2].z * v.z, 0.0f);
}

// HYPOTHESIS: the two axis indices come from a lookup table (values unknown); the result is atan2 of two dot products.
float hkGenericConstraintDataParameters::calcDeltaAngleAroundAxis(int axis, const u8* base) {
    static const int s_axisTable[] = {1, 2, 0, 1, 2};
    const u8* e1 = base + s_axisTable[axis + 1] * 0x10;
    const u8* e2 = base + s_axisTable[axis + 2] * 0x10;
    float a = *(float*)(e2 + 0x84) * *(float*)(e1 + 0x54) + *(float*)(e2 + 0x80) * *(float*)(e1 + 0x50) +
              *(float*)(e2 + 0x88) * *(float*)(e1 + 0x58);
    float b = *(float*)(e1 + 0x84) * *(float*)(e1 + 0x54) + *(float*)(e1 + 0x80) * *(float*)(e1 + 0x50) +
              *(float*)(e1 + 0x88) * *(float*)(e1 + 0x58);
    return hkMathTypes::hkMath_atan2fApproximation(a, b);
}

hkGenericConstraintDataParameters::hkGenericConstraintDataParameters() {
}

hkBool hkGenericConstraintData::isValid() const {
    return hkBool(true);
}

int hkGenericConstraintData::getType() const {
    return 10;
}

static hkStaticTypeInfo hkGenericConstraintDataTypeInfo("hkGenericConstraintData", hkGenericConstraintData::finishLoadedObjecthkGenericConstraintData, hkGenericConstraintData::cleanupLoadedObjecthkGenericConstraintData,
                                      hkGenericConstraintData::getVtablehkGenericConstraintData());

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkArray<int>::begin, hkArray<hkVector4>::begin, hkArray<hkConstraintModifier*>::begin
extern "C" void* fn_802DD4F8(void* const* p) { return *p; }
extern "C" void* fn_802DD500(void* const* p) { return *p; }
extern "C" void* fn_802DD508(void* const* p) { return *p; }

// Shape from https://github.com/jj-link/brawl src/havok/fn_802DC834.cpp.
// hkRotation::operator=
extern "C" void fn_802DC834(float* d, const float* s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
    d[4] = s[4];
    d[5] = s[5];
    d[6] = s[6];
    d[7] = s[7];
    d[8] = s[8];
    d[9] = s[9];
    d[10] = s[10];
    d[11] = s[11];
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_802DC6F8.cpp.
// hkVector4::_setRotatedDir(const hkRotation&, const hkVector4&)
extern "C" void fn_802DC6F8(float* out, const float* m, const float* v) {
    float a, b, c;
    float y = v[1];
    float z = v[2];
    float x = v[0];
    a = (z * m[8]) + ((x * m[0]) + (y * m[4]));
    b = (z * m[9]) + ((x * m[1]) + (y * m[5]));
    c = (z * m[10]) + ((x * m[2]) + (y * m[6]));
    out[0] = a;
    out[1] = b;
    out[2] = c;
    out[3] = 0.0f;
}
