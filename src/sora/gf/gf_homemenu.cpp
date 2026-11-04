#include <gf/gf_homemenu.h>
#include <gf/gf_trigger_callback.h>
#include <mt/mt_vector.h>
#include <nw4r/snd/snd_RemoteSpeaker.h>
#include <nw4r/snd/snd_RemoteSpeakerManager.h>
#include <revolution/MEM/mem_allocator.h>
#include <types.h>

extern "C" void HBMStartBlackOut();

gfTriggerCallback::~gfTriggerCallback() { }

void gfHomeMenu::userProc() {
    unkB1_20 = 1;
    if (unkB1_10 && unkB0.unkB0Hi == 2) {
        HBMStartBlackOut();
    }
}

bool gfHomeMenu::returnStatus() {
    bool result = false;
    if (unkB0.unkB0Mode != 1 || !unkB1_10 || unkB0.unkB0Hi != 2) {
        result = true;
    }
    return result;
}

void SpeakerSetupCallback(long, long);
void SpeakerShutdownCallback(long, long);

void ConnectCallback(long channel, long status) {
    if (status == 0) {
        nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(channel).Setup(SpeakerSetupCallback);
    } else if (status == -1) {
        nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(channel).Shutdown(SpeakerShutdownCallback);
    }
}

class Allocator : public MEMAllocator {
public:
    // HYPOTHESIS: allocator cursor and end follow two unclassified words.
    u32 unk10[2];
    u8* unk18;
    u8* unk1C;
    static void* funcAlloc(MEMAllocator*, unsigned long);
};

void* Allocator::funcAlloc(MEMAllocator* allocator, unsigned long size) {
    Allocator* self = static_cast<Allocator*>(allocator);
    u8* result = self->unk18;
    self->unk18 += (size + 7) & ~7UL;
    if (self->unk18 > self->unk1C) {
        return 0;
    }
    return result;
}

// HYPOTHESIS: cursor coordinates use the existing two-component vector type.
bool calcDigitalCursorPos(u32 buttons, Vec2f* pos) {
    switch (buttons & 0xC003) {
    case 1: pos->m_y -= 0.03515625f; break;
    case 2: pos->m_x -= 0.03515625f; break;
    case 0x4000: pos->m_y += 0.03515625f; break;
    case 0x8000: pos->m_x += 0.03515625f; break;
    case 3:
        pos->m_y -= 0.024858983233571053f;
        pos->m_x -= 0.024858983233571053f;
        break;
    case 0x8001:
        pos->m_y -= 0.024858983233571053f;
        pos->m_x += 0.024858983233571053f;
        break;
    case 0x4002:
        pos->m_y += 0.024858983233571053f;
        pos->m_x -= 0.024858983233571053f;
        break;
    case 0xC000:
        pos->m_y += 0.024858983233571053f;
        pos->m_x += 0.024858983233571053f;
        break;
    default: return false;
    }
    pos->m_x = pos->m_x > 1.0f ? 1.0f : pos->m_x < -1.0f ? -1.0f : pos->m_x;
    pos->m_y = pos->m_y > 1.0f ? 1.0f : pos->m_y < -1.0f ? -1.0f : pos->m_y;
    return true;
}
extern "C" void fn_80228154(s32 p1);
extern "C" void fn_80228138(s32 p1);

struct gfHomeMenuFields {
    char unk0[0x9C];
    s32 unk9C;
    char unkA0[0x10];
    union {
        struct {
            u32 unkB0Hi : 4;
            u32 unkB0Mode : 4;
            u32 unkB0Lo : 24;
        } unkB0;
        u8 unkB0Bytes[4];
    };
};

void gfHomeMenu::start() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);
    s32 var_r4 = 0;

    if ((((u8) self->unkB0Bytes[1] >> 4U) & 1) || (self->unk9C != 0)) {
        var_r4 = 1;
    }
    if (var_r4 != 0) {
        fn_80228154(0);
        fn_80228154(1);
        fn_80228154(2);
        fn_80228154(3);
    }
    self->unkB0.unkB0Mode = 1;
}

void gfHomeMenu::kill() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);

    fn_80228138(0);
    fn_80228138(1);
    fn_80228138(2);
    fn_80228138(3);
    self->unkB0.unkB0Mode = 2;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804953D0[0x48] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80495418[0xc0] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804954D8[0x8] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804954E0[0xf4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804955D4[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_804955D5[0x1] = { 0 };
unsigned char lbl_804955D8[0x2100] __attribute__((aligned(8))) = { 0 };
