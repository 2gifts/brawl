#include <types.h>

// TODO: clean up, give variables real names
namespace jj_8002F4B4 {
extern "C" {
void* getInstance__13gfFrameBufferFv();
unsigned short* getCurrentDispBuffer__12gfXFBManagerFv(void*);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
void fn_8002F188(int, const void*, void*);
void DCStoreRange(void*, unsigned int);
void GXInitTexObj(void*, void*, unsigned short, unsigned short, int, int, int, unsigned char);
void GXInitTexObjLOD(void*, int, int, int, int, int, float, float, float);
extern float lbl_805A1598;

// TODO: clean up, give variables real names
void fn_8002F4B4(int heap, void* texture, void** out) {
    unsigned short* src = getCurrentDispBuffer__12gfXFBManagerFv(getInstance__13gfFrameBufferFv());
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

    DCStoreRange(buffer, 0x12C000);
    GXInitTexObj(texture, buffer, 640, 480, 6, 0, 0, 0);
    GXInitTexObjLOD(texture, 1, 1, 0, 0, 0,
                lbl_805A1598, lbl_805A1598, lbl_805A1598);
}
}
}

