#include <sora/ms_message.h>
#include <types.h>

u8* floatToBytes(u8* out, float f) {
    union {
        float value;
        u8 bytes[4];
    } u;

    u.value = f;
    out[0] = u.bytes[0];
    out[1] = u.bytes[1];
    out[2] = u.bytes[2];
    out[3] = u.bytes[3];
    return out;
}
