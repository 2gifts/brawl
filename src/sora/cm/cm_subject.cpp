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

// TODO: clean up, give variables real names
namespace jj_8009ED40 {
struct LinkNodeBits {
    u32 hi : 3;
    u32 mid : 16;
    u32 lo : 13;
};

struct LinkNodeInit {
    s32 unk0;
    s32 unk4;
    union {
        u32 unk8;
        LinkNodeBits bits;
    } f8;
    s16 unkC;
    s16 unkE;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    f32 unk38;
    f32 unk3C;
    f32 unk40;
    f32 unk44;
    f32 unk48;
    f32 unk4C;
    f32 unk50;
    f32 unk54;
    f32 unk58;
    f32 unk5C;
    f32 unk60;
    f32 unk64;
    f32 unk68;
    f32 unk6C;
    f32 unk70;
    f32 unk74;
    f32 unk78;
    f32 unk7C;
    f32 unk80;
};

extern "C" {
extern char lbl_8049DEDC[];
extern const float lbl_805A1E30;
extern const float lbl_805A1E38;
extern const float lbl_805A1E3C;
extern const float lbl_805A1E40;
extern const float lbl_805A1E44;
extern const float lbl_805A1E48;
extern const float lbl_805A1E4C;
extern const float lbl_805A1E50;
extern const float lbl_805A1E54;

void Insert__Q44nw4r2ut6detail12LinkListImplFQ54nw4r2ut6detail12LinkListImpl8IteratorPQ34nw4r2ut12LinkListNode(char* list, void** it, void* node);

struct LinkNodeInit* __ct__9cmSubjectFii(struct LinkNodeInit* self, s32 arg1, u32 arg2) {
    u32 flags;
    void* it;

    self->f8.bits.hi = arg1;
    self->f8.bits.mid = 0;
    flags = self->f8.unk8;
    self->unk0 = 0;
    self->unk4 = 0;
    self->f8.unk8 = flags;
    self->unkC = 0;
    self->unkE = 0;
    self->unk10 = lbl_805A1E30;
    self->unk14 = lbl_805A1E30;
    self->unk18 = lbl_805A1E30;
    self->unk1C = lbl_805A1E30;
    self->unk20 = lbl_805A1E30;
    self->unk24 = lbl_805A1E30;
    self->unk28 = lbl_805A1E30;
    self->unk60 = lbl_805A1E30;
    self->unk64 = lbl_805A1E30;
    self->unk68 = lbl_805A1E30;
    self->unk2C = lbl_805A1E38;
    self->unk30 = lbl_805A1E3C;
    self->unk34 = lbl_805A1E3C;
    self->unk38 = lbl_805A1E38;
    self->unk4C = lbl_805A1E30;
    self->unk50 = lbl_805A1E30;
    self->unk54 = lbl_805A1E30;
    self->unk58 = lbl_805A1E30;
    self->unk3C = lbl_805A1E38;
    self->unk40 = lbl_805A1E3C;
    self->unk44 = lbl_805A1E3C;
    self->unk48 = lbl_805A1E38;
    self->unk5C = lbl_805A1E30;
    self->unk6C = lbl_805A1E40;
    self->unk70 = lbl_805A1E44;
    self->unk74 = lbl_805A1E48;
    self->unk78 = lbl_805A1E4C;
    self->unk7C = lbl_805A1E50;
    self->unk80 = lbl_805A1E54;
    if (arg2 == 1U && !((flags >> 13) & 1)) {
        it = lbl_8049DEDC + 4;
        Insert__Q44nw4r2ut6detail12LinkListImplFQ54nw4r2ut6detail12LinkListImpl8IteratorPQ34nw4r2ut12LinkListNode(lbl_8049DEDC, &it, self);
        self->f8.unk8 |= 0x2000;
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_8009EF8C {
struct Vec3 {
    float x;
    float y;
    float z;
};

struct Obj {
    unsigned char pad[0x10];
    Vec3 v;
};

extern "C" void setPos__9cmSubjectFP5Vec3f(Obj* dst, const Vec3* src) {
    dst->v = *src;
}
}

