#include <types.h>
#include <cm/cm_subject.h>
#include <nw4r/ut.h>

extern "C" void Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(void *list, void *node);
extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

cmSubject::~cmSubject() {
    if ((*(u32*)&_0[8] >> 0xDU & 1) == 1) {
        Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(&lbl_8049DEDC, this);
        *(u32*)&_0[8] &= 0xFFFFDFFF;
    }
}

cmSubject* cmSubjectList::getSubjectByPlayerNo(u32 playerNo) {
    u32 bits;
    cmSubject* found = NULL;
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
         it != lbl_8049DEDC.GetEndIter(); ++it) {
        // MATCH-ONLY: the header's bitfields at +0x8 are laid out differently; the raw word reads the original's fields.
        bits = *(u32*)&it->_0[8];
        if ((u8)(((bits >> 0x1A) & 7) + 0xFE) <= 1U && playerNo == ((bits >> 0x14) & 7)) {
            found = &*it;
            break;
        }
    }
    return found;
}
