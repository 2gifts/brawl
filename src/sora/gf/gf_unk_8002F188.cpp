#include <types.h>

// TODO: clean up, give variables real names
namespace jj_8002F4B4 {
extern "C" {
void* fn_80023AC8();
unsigned short* fn_80023914(void*);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
void fn_8002F188(int, const void*, void*);
void fn_801D7718(void*, unsigned int);
void fn_801F28F0(void*, void*, unsigned short, unsigned short, int, int, int, unsigned char);
void fn_801F2B44(void*, int, int, int, int, int, float, float, float);
extern float lbl_805A1598;

// TODO: clean up, give variables real names
void fn_8002F4B4(int heap, void* texture, void** out) {
    unsigned short* src = fn_80023914(fn_80023AC8());
    unsigned int* dst;
    void* buffer = alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(heap, 0x12C000);
    *out = buffer;
    dst = (unsigned int*)buffer;

    for (unsigned int y = 0; y < 120; ++y) {
        for (unsigned int x = 0; x < 160; ++x) {
            fn_8002F188(0x500, src + y * 0xA00 + x * 4, dst);
            dst += 16;
        }
    }

    fn_801D7718(buffer, 0x12C000);
    fn_801F28F0(texture, buffer, 640, 480, 6, 0, 0, 0);
    fn_801F2B44(texture, 1, 1, 0, 0, 0,
                lbl_805A1598, lbl_805A1598, lbl_805A1598);
}
}
}

