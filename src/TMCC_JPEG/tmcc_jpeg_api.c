#include <types.h>

// TODO: clean up, give variables real names
/* jj_80260730 */
typedef struct {
    void *obj;
    unsigned char type;
    char pad[3];
    unsigned int size;
    char pad2[16];
    unsigned char mode;
    unsigned char depth;
    char pad3[2];
} Config;
typedef struct {
    char pad[8];
    int type;
    char pad2[0x24];
    unsigned short width, height;
    char pad3[0x14];
    int a, b;
    char rest[0x6ac];
} State;
typedef struct { unsigned int width, height, size; } Result;
extern int fn_80264074(unsigned int *, unsigned int *, void *, Config *);
extern int fn_802642F0(State *, void *, Config *);
extern int fn_80262218(State *, void *, Config *);
// TODO: clean up, give variables real names
int fn_80260730(void *obj, void *data, unsigned int size, unsigned char type, unsigned char depth, unsigned int mode, Result *out) {
    unsigned int offset, length;
    Config cfg;
    State state;
    int r, n;
    out->width = 0;
    out->height = 0;
    out->size = 0;
    cfg.depth = depth;
    cfg.type = type;
    cfg.obj = obj;
    cfg.size = size;
    if (mode == 1) {
        cfg.mode = 1;
        r = fn_80264074(&offset, &length, data, &cfg);
        if (r < 0) {
            if (r == -0x45 || r == -2) return -0xa0;
            return r;
        }
        if (offset + length > size) return -0xa0;
        r = fn_802642F0(&state, (char *)data + offset, &cfg);
        if (r < 0) {
            n = -0xa0;
            if (r != -0x45) n = r;
            return n;
        }
    } else if (mode == 0) {
        cfg.mode = 0;
        r = fn_80262218(&state, data, &cfg);
        if (r < 0) return r;
    } else return -1;
    out->width = state.width;
    out->height = state.height;
    switch (state.type) {
    case 4: n = state.b * (state.a * 2); break;
    case 6: n = state.b * (state.a * 4); break;
    case 5:
    default: n = 0; break;
    }
    out->size = n;
    return 0;
}

