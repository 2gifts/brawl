#include <types.h>
#include <cm/cm_subject.h>

extern "C" void Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(void *list, void *node);
extern "C" u8 lbl_8049DEDC[];

cmSubject::~cmSubject() {
    if ((*(u32*)&_0[8] >> 0xDU & 1) == 1) {
        Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(lbl_8049DEDC, this);
        *(u32*)&_0[8] &= 0xFFFFDFFF;
    }
}
