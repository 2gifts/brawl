#include <types.h>
#include <cm/cm_subject.h>
#include <nw4r/ut.h>

extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

// cmSubject::updateDispStatus (the map name); so_camera_subject.cpp calls it by this stand-in name.
extern "C" void fn_8009EFCC(cmSubject* subject);

cmSubject::~cmSubject() {
    if (m_isRegist == 1) {
        lbl_8049DEDC.Erase(this);
        m_isRegist = 0;
    }
}

cmSubject* cmSubjectList::getSubjectByPlayerNo(u32 playerNo) {
    cmSubject* found = NULL;
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
         it != lbl_8049DEDC.GetEndIter(); ++it) {
        u8 kind = it->m_stateB;
        if ((kind == 2 || kind == 3) && playerNo == it->m_playerNo) {
            found = &*it;
            break;
        }
    }
    return found;
}

void cmSubject::setRange(Rect2D* range) {
    float down, up, right, left;
    left = range->m_left;
    right = range->m_right;
    up = range->m_up;
    down = range->m_down;
    m_range.m_left = left;
    m_range.m_right = right;
    m_range.m_up = up;
    m_range.m_down = down;
}

void cmSubject::regist() {
    if (m_isRegist == 0) {
        lbl_8049DEDC.PushBack(this);
        m_isRegist = 1;
    }
}

cmSubjectList::~cmSubjectList() {
}

void cmSubjectList::update(float scale) {
    if (scale > 0.0f) {
        for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
             it != lbl_8049DEDC.GetEndIter(); ++it) {
            fn_8009EFCC(&*it);
            if (it->m_isLoupeTarget) {
                it->shiftRange(scale);
            }
        }
    }
}
