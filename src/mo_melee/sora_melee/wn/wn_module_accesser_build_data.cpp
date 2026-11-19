#include <types.h>
#include <wn/wn_module_accesser_build_data.h>
#include <so/so_module_accesser.h>

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

bool wnGroundConditionCheckerImpl::isConditionNoCliffStop(soModuleAccesser* moduleAccesser)
{
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2));
    return speed.lengthSq() > 0.0f;
}

// TODO: clean up, give variables real names
namespace jj_1DC67C {
extern "C" void* getAnimCmdData__25wnModuleAccesserBuildDataCFUll(void* self, s32 idx, s32 flag) {
    if (flag == 0) {
        switch (idx) {
        case 0:
            return *(void**)(*(u8**)((u8*)self + 0x8) + 0x14);
        case 1:
            return *(void**)(*(u8**)((u8*)self + 0x8) + 0x18);
        case 2:
            return *(void**)(*(u8**)((u8*)self + 0x8) + 0x1c);
        case 3:
            return *(void**)(*(u8**)((u8*)self + 0x8) + 0x20);
        case 5:
            return *(void**)((u8*)self + 0x38);
        }
    }
    return nullptr;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC6F4 {
extern "C" int fn_27_1DC6F4() {
    return 0xFFFF;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC700 {
extern "C" u32 fn_27_1DC700(void)
{
    return 0xFFFF;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC70C {
extern "C" u8 fn_27_1DC70C(void* self) {
    return *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0xc);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC714 {
extern "C" int fn_27_1DC714(const int* a)
{
    return ((const int*)a[2])[4];
}
}

// TODO: clean up, give variables real names
namespace jj_1DC720 {
extern "C" int fn_27_1DC720(int *p) {
    return *(int *)(*(int **)(p + 2) + 3);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC72C {
extern "C" {
struct Obj2 {
    int y0;
    int y4;
    int y8;
    int yC;
    int y10;
    int y14;
    int y18;
    int y1C;
    int y20;
    int y24;
};

struct Obj {
    int x0;
    int x4;
    Obj2 *x8;
};

int fn_27_1DC72C(Obj *obj) {
    return obj->x8->y24;
}
}
}

// TODO: clean up, give variables real names
namespace jj_1DC738 {
extern "C" u32 fn_27_1DC738(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x14);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC740 {
extern "C" s32 fn_27_1DC740(void) {
    return 0x4;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC748 {
struct FnObj27 {
    u8 pad[0x18];
    void *field18;
};

extern "C" void *fn_27_1DC748(FnObj27 *p)
{
    void *v = p->field18;
    if (v != 0) {
        return v;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC75C {
extern "C" u32 fn_27_1DC75C(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x1c);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC764 {
extern "C" u32 fn_27_1DC764(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x2c);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC76C {
extern "C" u32 fn_27_1DC76C(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x30);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC774 {
extern "C" {
extern unsigned int lbl_27_bss_509C;

unsigned int* fn_27_1DC774()
{
    return &lbl_27_bss_509C;
}
}
}

// TODO: clean up, give variables real names
namespace jj_1DC780 {
extern "C" u32 fn_27_1DC780(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x34);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC788 {
extern "C" u32 fn_27_1DC788(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x3c);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC848 {
extern "C" u32 getNWMotionInstanceHeap__25wnModuleAccesserBuildDataCFv(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x28);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC850 {
extern "C" u32 getNWModelInstanceHeap__25wnModuleAccesserBuildDataCFv(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x24);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC858 {
extern "C" u32 getInstanceHeap__25wnModuleAccesserBuildDataCFv(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x20);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC860 {
extern "C" s32 getItemNodeData__25wnModuleAccesserBuildDataCFv(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC868 {
extern "C" u32 getTeam__25wnModuleAccesserBuildDataCFv(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_1DC870 {
extern "C" {
void __dl__FPv(void*);

void* __dt__25wnModuleAccesserBuildDataFv(void* ptr, int flag) {
    if (ptr != 0 && flag > 0) {
        __dl__FPv(ptr);
    }
    return ptr;
}
}
}

