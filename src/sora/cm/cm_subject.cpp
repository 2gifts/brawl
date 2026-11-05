#include <types.h>
#include <cm/cm_subject.h>
#include <nw4r/ut.h>
#include <cm/cm_camera_controller.h>
#include <sr/sr_common.h>

extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;
extern const float lbl_805A1E5C;
extern const float lbl_805A1E60;

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
            it->updateDispStatus();
            if (it->m_isLoupeTarget) {
                it->shiftRange(scale);
            }
        }
    }
}

void cmSubject::unRegist() {
    if (m_isRegist == 1) {
        lbl_8049DEDC.Erase(this);
        m_isRegist = 0;
    }
}

cmSubject* cmSubjectGetStruct(HeapType heap, int kind, int flag) {
    return new (heap) cmSubject(kind, flag);
}

void cmSubject::updateDispStatus() {
    if (m_state != 1 && !unk8Bit13) {
        if (m_state == 2) {
            if (unkC != 0) {
                --unkC;
                m_isLoupeTarget = 0;
                return;
            }
            CameraController* controller = CameraController::getInstance();
            float width = controller->m_stageCameraBounds.m_right - controller->m_stageCameraBounds.m_left;
            float height = controller->m_stageCameraBounds.m_up - controller->m_stageCameraBounds.m_down;
            if (width < 0.001f) {
                width = 0.001f;
            }
            if (height < 0.001f) {
                height = 0.001f;
            }
            float x = (m_pos.m_x - controller->m_stageCameraBounds.m_left) / width;
            float y = (m_pos.m_y - controller->m_stageCameraBounds.m_down) / height;
            if (x > lbl_805A1E5C || x < lbl_805A1E60 || y > lbl_805A1E5C || y < lbl_805A1E60) {
                if (unk8Bit14) {
                    unk8Bit14 = 0;
                    unkC = 600;
                }
                m_isLoupeTarget = 0;
                return;
            }
            unkC = 0;
        }
        unk8Bit14 = 1;
        m_isLoupeTarget = 1;
    } else {
        m_isLoupeTarget = 0;
    }
}
