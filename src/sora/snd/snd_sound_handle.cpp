#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A12C8[0x8] __attribute__((aligned(4))) = { 0 };

#include <snd/snd_doubutsugo.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>

// HYPOTHESIS: these data records hold callbacks tested before starting speech.
extern void (Doubutsugo::*lbl_804937B8)();
extern void (Doubutsugo::*lbl_804937C4)();

void Doubutsugo::Init() {
    unk3C = 0;
    unk40 = 3;
    unk44 = 0;
    if (m_source != 0) {
        m_source->vf18(0);
    }
    unk48 = 0x12;
    unk4C = 0;
    unk54 = 0;
    unk58 = 0;
    unk5C = 0;
    unk60 = 0;
    unk64 = 0;
    unk68 = 0x216;
    unk6C = 0;
    unk70 = 0;
    unk39 = 0;
    unk74 = 0;
    unk78 = 0;
    unk7C = 0;
    unk38 = 0;
    unkC4 = 0.7f;
    unk14 = 1.0f;
    unk28 = 1.0f;
}

void Doubutsugo::SetVolume(f32 volume) {
    if (volume < 0.0f) {
        unkC4 = 0.0f;
    } else if (volume > 1.0f) {
        unkC4 = 1.0f;
    } else {
        unkC4 = volume;
    }
}

bool Doubutsugo::Setup(nw4r::snd::SoundArchivePlayer* player) {
    bool available = false;
    if (player != 0 && player->IsAvailable()) {
        Init();
        unk0 = player;
        available = true;
    }
    return available;
}

void Doubutsugo::Start(const void* script) {
    if (unk0 != 0) {
        if (lbl_804937C4) {
            unk3A = 0;
        }
        unk80 = static_cast<const s16*>(script) + 4;
        unk39 = 1;
        unk4C = 0;
        unk40 = 3;
        unk44 = 0;
        unk74 = 0;
        unk78 = 0;
        unk7C = 0;
        unk6C = 0;
        unk18 = 1.0f;
        unk2C = 1.0f;
        unk64 = 0;
        unk1C = 1.0f;
        unk30 = 1.0f;
        unk38 = 0;
    }
}

void Doubutsugo::StartDirect(const s16* words) {
    if (unk0 != 0) {
        for (int count = 0; count < 32; count++) {
            unk84[count] = *words;
            if (*words == 0xD3) {
                unk84[count] = 0xCB;
                unk84[count + 1] = 0xCB;
                unk84[count + 2] = 0xCB;
                unk84[count + 3] = 0xD3;
                break;
            }
            if (count == 31) {
                return;
            }
            words++;
        }
        if (lbl_804937B8) {
            unk3A = 0;
        }
        unk80 = unk84;
        unk39 = 1;
        unk4C = 0;
        unk40 = 3;
        unk44 = 0;
        unk74 = 0;
        unk78 = 0;
        unk7C = 0;
        unk6C = 0;
        unk18 = 1.0f;
        unk2C = 1.0f;
        unk64 = 0;
        unk1C = 1.0f;
        unk30 = 1.0f;
        unk38 = 0;
    }
}

bool Doubutsugo::IsAvailable() {
    return unk0 != 0;
}

// TODO: clean up, give variables real names
extern "C" unsigned char fn_803EF1C8(void* self) {
    return *(unsigned char*)((unsigned char*)self + 0x3a);
}

// TODO: clean up, give variables real names
namespace jj_803EF038 {
extern "C" void fn_803EF038(int *arg0) {
    *arg0 = 0;
}
}

namespace jj_803EF1D0 {
extern void fn_801CBFB0(void*, void*, int, int, int, int);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_803EF1D0(int* a) {
    fn_801CBFB0((void*)(*a + 0xC), a + 2, 0x69, 0, 0, 0);
}
}

namespace jj_803EF188 {
extern void fn_801C92E4(int);
extern void fn_803EF640(void);

// TODO: clean up, give variables real names
extern "C" void fn_803EF188(int *arg0) {
    if (*arg0 != 0) {
        fn_803EF640();
        fn_801C92E4(*arg0);
    }
}
}

namespace jj_803EEFCC {
extern void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
extern void fn_803EF038(void*);
extern void fn_80072FD4(void*, int);
extern int lbl_805A12C8;

typedef struct Obj {
    u32 unk0;
    u32 unk4;
    u8 arr[0x31];
    u8 unk39;
    u8 unk3A;
} Obj;

// TODO: clean up, give variables real names
extern "C" void* fn_803EEFCC(void* arg0) {
    Obj* self = (Obj*)arg0;
    self->unk4 = 0;
    __construct_array(self->arr, (void (*)(void*))fn_803EF038, (void (*)(void*))fn_80072FD4, 4, 2);
    self->unk39 = 0;
    self->unk3A = 0;
    lbl_805A12C8 = (int)self;
    self->unk0 = 0;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_803EF044 {
typedef struct Owner {
    char pad[0x30];
    u32 count;
} Owner;

typedef struct Object {
    Owner* owner;
    char rest[12];
} Object;

extern int fn_801C8B20(Owner*);
extern void* fn_801C96A8(Owner*, int);
extern void fn_801CAD90(void*, int);
extern void fn_801CA7E8(void*);
extern void* fn_80072FD4(void*, int);
extern void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
extern void __dl__FPv(void*);
extern int lbl_805A12C8;

// TODO: clean up, give variables real names
extern "C" Object* fn_803EF044(Object* self, int flag) {
    u32 i;
    void* p;
    if (self) {
        if (fn_801C8B20(self->owner)) {
            for (i = 0; i < self->owner->count; i++) {
                fn_801CAD90(fn_801C96A8(self->owner, i), 0);
            }
        }
        self->owner = 0;
        lbl_805A12C8 = 0;
        __destroy_arr((char*)self + 8, (void (*)(void*, int))fn_80072FD4, 4, 2);
        p = (char*)self + 4;
        if (p) {
            fn_801CA7E8(p);
        }
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_803EF4A8 {
typedef unsigned char u8;

extern float lbl_805A4B04;
extern float lbl_805A4B0C;
extern float lbl_805A4B10;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_803EF4A8(u8 *arg0, int arg1) {
    float f;

    switch (arg1) {
    case 0xD0:
        *(int *)(arg0 + 0x7C) = 0;
        *(int *)(arg0 + 0x78) = *(int *)(arg0 + 0x78) + 1;
        break;
    case 0xD1:
        break;
    case 0xD2:
        arg0[0x3A] = 1;
        arg0[0x38] = 0;
        *(int *)(arg0 + 0x78) = 0;
        *(int *)(arg0 + 0x7C) = 0;
        break;
    case 0xD4:
        break;
    case 0xDC:
        *(int *)(arg0 + 0x3C) = 0;
        break;
    case 0xDD:
        *(int *)(arg0 + 0x3C) = 0;
        break;
    case 0xDE:
        *(int *)(arg0 + 0x3C) = 0;
        break;
    case 0xFA:
        *(int *)(arg0 + 0x60) = 0xFA;
        break;
    case 0xFB:
        *(int *)(arg0 + 0x60) = 0xFB;
        break;
    case 0xFC:
        *(int *)(arg0 + 0x60) = 0xFC;
        break;
    case 0xFD:
        *(int *)(arg0 + 0x60) = 0xFD;
        return;
    case 0x104:
        f = lbl_805A4B04;
        *(float *)(arg0 + 0x14) = f;
        *(float *)(arg0 + 0x28) = f;
        break;
    case 0x105:
        f = lbl_805A4B0C;
        *(float *)(arg0 + 0x14) = f;
        *(float *)(arg0 + 0x28) = f;
        break;
    case 0x106:
        f = lbl_805A4B10;
        *(float *)(arg0 + 0x14) = f;
        *(float *)(arg0 + 0x28) = f;
        break;
    case 0x12B:
        break;
    }

    if (arg1 >= 0xE6 && arg1 <= 0xF1) {
        *(int *)(arg0 + 0x40) = arg1 - 0xE5;
        *(int *)(arg0 + 0x44) = 1;
    }
    if (arg1 >= 0x12C && arg1 <= 0x16B) {
        *(int *)(arg0 + 0x48) = arg1 - 0x12C;
    }
}
}

namespace jj_803EFEA4 {
extern int fn_803EFA54(void *, int, int);
// TODO: clean up, give variables real names
extern "C" int fn_803EFEA4(void *a, int b, int c, int d) {
 int r;
 switch(b) {
 case 0xBE: case 0xCB: case 0xCC: case 0xD1: r=fn_803EFA54(a,c,d); break;
 default: r=fn_803EFA54(a,b,c); break;
 }
 if(r == -1) {
 r=fn_803EFA54(a,c,d);
 if(r == -1) {
 r=0xD1;
 switch(b) {
 case 0x72: r=0; break;
 case 0x73: r=0x3C; break;
 case 0x74: r=7; break;
 case 0x75: r=0x39; break;
 case 0x76: r=3; break;
 case 0x77: r=0x1B; break;
 case 0x78: r=0x2F; break;
 case 0x79: r=0x1B; break;
 case 0x7A: r=1; break;
 case 0x7B: r=0x33; break;
 case 0x7C: r=7; break;
 case 0x7D: r=0x28; break;
 case 0x7E: r=0x20; break;
 case 0x7F: r=0x2C; break;
 case 0x80: r=4; break;
 case 0x81: r=0x41; break;
 case 0x83: r=0x28; break;
 case 0x84: r=0xC; break;
 case 0x85: r=0x11; break;
 case 0x86: r=2; break;
 case 0x87: r=0x3C; break;
 case 0x88: r=2; break;
 case 0x89: r=7; break;
 case 0x8A: r=1; break;
 case 0x8B: r=0x34; break;
 }
 }
 }
 return r;
}
}

// TODO: clean up, give variables real names
namespace jj_803F004C {
typedef struct {
    char pad00[0x10];
    float x10;
    char pad14[4];
    float x18;
    float x1c;
    char pad20[4];
    float x24;
    char pad28[4];
    float x2c;
    float x30;
    char pad34[0x28];
    int x5c;
    int x60;
    int x64;
    unsigned int x68;
    int x6c;
    int x70;
} Object;

extern int fn_803EFEA4(void *, int, int);

extern const float lbl_805A4B04;
extern const float lbl_805A4B10;
extern const float lbl_805A4B14;
extern const float lbl_805A4B18;
extern const float lbl_805A4B1C;
extern const float lbl_805A4B20;
extern const float lbl_805A4B24;
extern const float lbl_805A4B28;
extern const float lbl_805A4B2C;
extern const float lbl_805A4B30;
extern const float lbl_805A4B34;
extern const float lbl_805A4B38;
extern const float lbl_805A4B3C;
extern const float lbl_805A4B40;
extern const float lbl_805A4B44;
extern const float lbl_805A4B48;
extern const float lbl_805A4B4C;
extern const float lbl_805A4B50;
extern const float lbl_805A4B54;
extern const float lbl_805A4B58;

// TODO: clean up, give variables real names
extern "C" int fn_803F004C(Object *obj, int arg1, int arg2, int arg3, int arg4)
{
    int result;
    int count;
    unsigned int seed;
    float value;

    if ((arg1 >= 0 && arg1 <= 0xcf) || arg1 >= 0x12c) {
        result = fn_803EFEA4(obj, arg1, arg2);
    } else {
        switch (arg1) {
        case 0xbe:
            result = obj->x5c;
            break;
        default:
            result = arg1;
            break;
        }
    }

    if (arg1 >= 0 && arg1 <= 0xc7 && arg1 == arg2 && arg1 != 0xc9) {
        obj->x68 = obj->x68 * 0x19660d + 0x3c6ef35f;
        result = (obj->x68 >> 2) & 0x3f;
    }

    switch (arg1) {
    case 0xcb:
    case 0xcc:
    case 0xcd:
    case 0xce:
    case 0xcf:
    case 0xd3:
        obj->x6c = 0;
        obj->x18 = lbl_805A4B04;
        obj->x2c = lbl_805A4B04;
        break;
    }

    if (arg1 == 0xc9) {
        obj->x18 = lbl_805A4B04;
        obj->x2c = lbl_805A4B04;
    } else {
        switch (obj->x6c) {
        default:
            obj->x18 = lbl_805A4B04;
            obj->x2c = lbl_805A4B04;
            break;
        case 1:
            obj->x18 = lbl_805A4B10;
            obj->x2c = lbl_805A4B10;
            break;
        case 2:
            obj->x18 = lbl_805A4B14;
            obj->x2c = lbl_805A4B14;
            break;
        case 3:
            obj->x18 = lbl_805A4B18;
            obj->x2c = lbl_805A4B18;
            break;
        case 4:
            obj->x18 = lbl_805A4B1C;
            obj->x2c = lbl_805A4B1C;
            break;
        case 5:
            obj->x18 = lbl_805A4B20;
            obj->x2c = lbl_805A4B20;
            break;
        case 6:
            obj->x18 = lbl_805A4B24;
            obj->x2c = lbl_805A4B24;
            break;
        }
        obj->x6c++;
    }

    switch (arg1) {
    case 0xcd:
    case 0xce:
    case 0xcf:
    case 0xd3:
        obj->x64 = 0;
        obj->x1c = lbl_805A4B04;
        obj->x30 = lbl_805A4B04;
        break;
    }

    if (arg1 == 0xc9) {
        obj->x1c = lbl_805A4B04;
        obj->x30 = lbl_805A4B04;
    } else {
        switch (obj->x60) {
        case 0xfd:
            count = obj->x64;
            if (count == 0) {
                seed = obj->x68 * 0x19660d + 0x3c6ef35f;
                obj->x68 = seed;
                obj->x64 = ((int)(unsigned char)(seed >> 16) >> 6) + 10;
            } else {
                obj->x64 = count - 1;
            }
            switch (obj->x64) {
            case 0:
            case 1:
            case 2:
                obj->x1c = lbl_805A4B28;
                obj->x30 = lbl_805A4B28;
                obj->x10 = lbl_805A4B2C;
                obj->x24 = lbl_805A4B2C;
                break;
            case 3:
            case 4:
                obj->x1c = lbl_805A4B30;
                obj->x30 = lbl_805A4B30;
                obj->x10 = lbl_805A4B10;
                obj->x24 = lbl_805A4B10;
                break;
            case 9:
            case 10:
                obj->x1c = lbl_805A4B10;
                obj->x30 = lbl_805A4B10;
                obj->x10 = lbl_805A4B2C;
                obj->x24 = lbl_805A4B2C;
                break;
            default:
                obj->x1c = lbl_805A4B24;
                obj->x30 = lbl_805A4B24;
                obj->x10 = lbl_805A4B34;
                obj->x24 = lbl_805A4B34;
                break;
            }
            break;

        case 0xfc:
            count = obj->x64;
            if (count == 0) {
                obj->x68 = obj->x68 * 0x19660d + 0x3c6ef35f;
                obj->x64 = 1;
            } else if (count == 0) {
                obj->x64 = 0;
                if (obj->x70 == 0)
                    obj->x70 = 1;
                else
                    obj->x70 = 0;
            } else if (count > 15) {
                obj->x64 = 1;
            } else {
                obj->x64 = count + 1;
            }

            if (obj->x70 == 0)
                value = lbl_805A4B04 - lbl_805A4B38 * (float)obj->x64;
            else
                value = lbl_805A4B24 - lbl_805A4B38 * (float)obj->x64;
            if (value < lbl_805A4B3C)
                value = lbl_805A4B3C;
            obj->x1c = value;
            obj->x30 = value;

            if (obj->x70 == 0)
                value = lbl_805A4B40 - lbl_805A4B44 * (float)obj->x64;
            else
                value = lbl_805A4B04 - lbl_805A4B44 * (float)obj->x64;
            if (value < lbl_805A4B48)
                value = lbl_805A4B48;
            obj->x10 = value;
            obj->x24 = value;
            break;

        case 0xfb:
            count = obj->x64;
            if (count == 0) {
                seed = obj->x68 * 0x19660d + 0x3c6ef35f;
                obj->x68 = seed;
                obj->x64 = ((int)(unsigned char)(seed >> 16) >> 6) + 10;
            } else {
                obj->x64 = count - 1;
            }
            switch (obj->x64) {
            case 0:
            case 1:
                obj->x1c = lbl_805A4B34;
                obj->x30 = lbl_805A4B34;
                obj->x10 = lbl_805A4B34;
                obj->x24 = lbl_805A4B34;
                break;
            case 2:
                obj->x1c = lbl_805A4B40;
                obj->x30 = lbl_805A4B40;
                obj->x10 = lbl_805A4B30;
                obj->x24 = lbl_805A4B30;
                break;
            default:
                obj->x1c = lbl_805A4B30;
                obj->x30 = lbl_805A4B30;
                obj->x10 = lbl_805A4B34;
                obj->x24 = lbl_805A4B34;
                break;
            }
            break;

        default:
            obj->x1c = lbl_805A4B04;
            obj->x30 = lbl_805A4B04;
            obj->x10 = lbl_805A4B04;
            obj->x24 = lbl_805A4B04;
            break;
        }
    }

    if (arg4 != 0x20) {
        if (arg4 > 0x20) {
            value = obj->x10 + lbl_805A4B4C * (float)(arg4 - 0x20);
            if (value > lbl_805A4B50)
                value = lbl_805A4B50;
            obj->x10 = value;
            obj->x24 = value;
        } else {
            arg4 = 0x1f - arg4;
            value = obj->x10 - lbl_805A4B54 * (float)arg4;
            if (value < lbl_805A4B58)
                value = lbl_805A4B58;
            obj->x10 = value;
            obj->x24 = value;
        }
    }

    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_803EF640 {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12(float, int);
    virtual void slot13(float);
};
struct NetObj {
    void* field0;
    char pad04[4];
    Object_vptr_0* ptr1;
    Object_vptr_0* ptr2;
    float f10, f14, f18, f1C, f20, f24, f28, f2C, f30, f34;
    unsigned char b38, b39, b3A, b3B;
    int i3C, i40, i44, i48, i4C;
    char pad50[4];
    int i54, i58;
    char pad5C[4];
    int i60;
    char pad64[0x10];
    int i74;
    char pad78[4];
    int i7C;
    short* ptr80;
    char pad84[0x40];
    float fC4;
};
extern "C" void fn_803EF4A8(void*, int);
extern "C" int fn_803F004C(void*, int, int, int, int, int);
extern "C" void detail_StartSound__Q34nw4r3snd14SoundStartableFPQ34nw4r3snd11SoundHandleUlPQ54nw4r3snd6detail10BasicSound14AmbientArgInfoPQ44nw4r3snd6detail19ExternalSoundPlayerPCQ44nw4r3snd14SoundStartable9StartInfo(void*, void*, int, int, int, int);
extern float lbl_805A4B04;
inline void setVolume(Object_vptr_0* p, float v) { if (p) p->slot12(v, 0); }
inline void setPitch(Object_vptr_0* p, float v) { if (p) p->slot13(v); }
extern "C" void fn_803EF640(NetObj* obj) {
    int val1, val2, val3, ret, offset, flag, var_r3;
    if (obj->b39 == 1 && obj->b3A == 0) {
        val1 = obj->ptr80[obj->i4C];
        val2 = obj->ptr80[obj->i4C + 1];
        val3 = obj->ptr80[obj->i4C + 2];
        if (val1 == 0xD3 || val2 == 0xD3) {
            obj->i4C = 0; obj->i48 = 0x12; obj->b39 = 0; obj->i60 = 0;
        }
        while (val1 >= 0xD0 && val1 <= 0x18F && obj->b3A == 0 && obj->b39 == 1) {
            fn_803EF4A8(obj, val1);
            obj->i4C++;
            val1 = obj->ptr80[obj->i4C];
        }
        if (obj->b39 == 0) return;
        {
            if (obj->i44 == 0 || obj->b38 == 1) {
                if ((val1 >= 0 && val1 <= 0xCF) || val1 >= 0x12C) obj->i74 = val1;
                if ((val1 >= 0 && val1 <= 0xCF) || val1 >= 0x12C) obj->i7C++;
                obj->i44 = 1;
                ret = fn_803F004C(obj, val1, val2, val3, obj->i48, 0);
                if (val1 == 0xCC && val2 == 0xCC) ret = 0xC9;
                if (obj->i54 == 0) { obj->i54 = 1; var_r3 = 0; }
                else { obj->i54 = 0; var_r3 = 1; }
                switch (ret) {
                case 0x12B: offset = -1; flag = 1; break;
                case 0xC9: offset = 0x68; flag = 1; break;
                default: offset = ret + obj->i3C; flag = 0; break;
                }
                if (ret == 0xC9 && obj->b38 == 1) offset = -1;
                if (offset != -1)
                    detail_StartSound__Q34nw4r3snd14SoundStartableFPQ34nw4r3snd11SoundHandleUlPQ54nw4r3snd6detail10BasicSound14AmbientArgInfoPQ44nw4r3snd6detail19ExternalSoundPlayerPCQ44nw4r3snd14SoundStartable9StartInfo((char*)obj->field0 + 0xC, &obj->ptr1 + var_r3, offset, 0, 0, 0);
                obj->f20 = obj->f1C * (obj->f14 * obj->f18);
                obj->f34 = obj->f30 * (obj->f28 * obj->f2C);
                if (offset >= 0 && offset <= 0xCF && flag == 0) {
                    setVolume(obj->ptr1, obj->f10 * obj->fC4);
                    setVolume(obj->ptr2, obj->f24 * obj->fC4);
                    setPitch(obj->ptr1, obj->f20);
                    setPitch(obj->ptr2, obj->f34);
                } else {
                    setVolume(obj->ptr1, obj->fC4);
                    setVolume(obj->ptr2, obj->fC4);
                    if (obj->ptr1) obj->ptr1->slot13(lbl_805A4B04);
                    if (obj->ptr2) obj->ptr2->slot13(lbl_805A4B04);
                }
                obj->i4C++;
                if (ret != 0xC9 && val2 < 0xD0) obj->i4C++;
            } else if (obj->i44 == obj->i40) obj->i44 = 0;
            else obj->i44++;
        }
    }
    if (obj->i58 != 0) {
        fn_803F004C(obj, obj->i58, 0, 0, obj->i48, 2);
        obj->i58 = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_803EFA54 {
extern "C" int fn_803EFA54(void* self, int arg1, int arg2)
{
    int ret = -1;

#define INNER(a, b, c, d, e) \
    switch (arg2) { \
    case 0x72: ret = a; break; \
    case 0x7A: ret = b; break; \
    case 0x86: ret = c; break; \
    case 0x76: ret = d; break; \
    case 0x80: ret = e; break; \
    } \
    break;

    switch (arg1) {
    case 0x7C: INNER(5, 6, 7, 8, 9)
    case 0x84: INNER(0xA, 0xB, 0xC, 0xD, 0xE)
    case 0x85: INNER(0xF, 0x10, 0x11, 0x12, 0x13)
    case 0x7F: INNER(0x14, 0x15, 0x16, 0x17, 0x18)
    case 0x79: INNER(0x19, 0x1A, 0x1B, 0x1C, 0x1D)
    case 0x7E: INNER(0x1E, 0x1F, 0x20, 0x21, 0x22)
    case 0x8A: INNER(0x23, 1, 0x24, 3, 0x25)
    case 0x83: INNER(0x26, 0x27, 0x28, 0x29, 0x2A)
    case 0x88: INNER(0x2B, 1, 2, 3, 4)
    case 0x78: INNER(0x2D, 0x2E, 0x2F, 0x30, 0x31)
    case 0x8B: INNER(0x32, 0x33, 0x34, 0x35, 0x36)
    case 0x75: INNER(0x37, 0x33, 0x34, 0x38, 0x39)
    case 0x73: INNER(0x3A, 0x3B, 0x3C, 0x3D, 0x3E)
    case 0x81: INNER(0x3F, 0x40, 0x41, 0x42, 0x43)
    }
    return ret;
}
}

