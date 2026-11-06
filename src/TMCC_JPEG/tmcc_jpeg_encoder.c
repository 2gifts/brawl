#include <types.h>

// HYPOTHESIS: bitstream writer state
typedef struct TMCC_Bitstream {
    u8* start;
    u8* cur;
    int unk8;
    u8 overflow;
    int capacity;
    u32 total;
} TMCC_Bitstream;

typedef struct TMCC_Encoder TMCC_Encoder;
typedef void (*TMCC_EncFn)(TMCC_Encoder*);

// HYPOTHESIS: encoder state, only the fields these functions touch
struct TMCC_Encoder {
    u8 format;
    u8 pad1;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
    u8 padA[0x18 - 0xA];
    int restartCount;
    TMCC_Bitstream* stream;
    TMCC_EncFn putFn;
    TMCC_EncFn compFn;
    TMCC_EncFn edgeFn;
    u8 pad2C[0x1580 - 0x2C];
    u16 width;
    u16 height;
    u8 pad1584[4];
    u8 unk1588;
    u8 unk1589;
    u8 pad158A[0x1594 - 0x158A];
    u8 unk1594;
    u8 unk1595;
    u8 pad1596[0x15a4 - 0x1596];
    int unk15A4;
    u8 pad15A8[0x15c8 - 0x15A8];
    u8 unk15C8;
    u8 unk15C9;
    u8 pad15CA[0x15d8 - 0x15CA];
    int unk15D8;
    u8 pad15DC[0x160c - 0x15DC];
    int unk160C;
    u8 pad1610[0x1634 - 0x1610];
    TMCC_Bitstream outStream;
};

// HYPOTHESIS: user parameter block filled from the encoder state
typedef struct TMCC_UsrParam {
    u8 pad0[0xC];
    u16 unkC;
    u16 unkE;
    int unk10;
    TMCC_EncFn unk14;
    int unk18;
    u16 unk1C;
    u16 unk1E;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    int unk24;
    int unk28;
    u8 unk2C;
    u8 pad2D;
    u8 unk2E;
    u8 pad2F;
    u16 unk30;
    u16 unk32;
    u16 unk34;
    u16 unk36;
    int unk38;
    int unk3C;
    int unk40;
    int unk44;
} TMCC_UsrParam;

void TMCC_JPEGENC_set_usr_parameters(TMCC_Encoder*, TMCC_UsrParam*);
void TMCC_JPEGENC_PutStream(TMCC_Encoder*);
void TMCC_JPEGENC_PutStreamWithCheck(TMCC_Encoder*);
void TMCC_JPEGENC_PutMaker(TMCC_Bitstream*, u16);
void TMCC_JPEGENC_encoder_y411edge(TMCC_Encoder*);
void TMCC_JPEGENC_encoder_y420edge(TMCC_Encoder*);
void TMCC_JPEGENC_encoder_y422edge(TMCC_Encoder*);
void TMCC_JPEGENC_encoder_y411(TMCC_Encoder*);
void TMCC_JPEGENC_encoder_y420(TMCC_Encoder*);
void TMCC_JPEGENC_encoder_y422(TMCC_Encoder*);
void TMCC_JPEGENC_counter_y411edge(TMCC_Encoder*);
void TMCC_JPEGENC_counter_y420edge(TMCC_Encoder*);
void TMCC_JPEGENC_counter_y422edge(TMCC_Encoder*);
void TMCC_JPEGENC_counter_y411(TMCC_Encoder*);
void TMCC_JPEGENC_counter_y420(TMCC_Encoder*);
void TMCC_JPEGENC_counter_y422(TMCC_Encoder*);

void TMCC_JPEGENC_Init_bitstream(TMCC_Bitstream* bs, u8* buf, int size) {
    bs->start = buf;
    bs->cur = buf;
    bs->unk8 = 8;
    bs->capacity = size;
    bs->total = 0;
    bs->overflow = 0;
}

void TMCC_JPEGENC_Get_bitstream(TMCC_Bitstream* bs, int* out) {
    *out = bs->total + (bs->cur - bs->start);
}

int TMCC_JPEGENC_CountMarker(TMCC_Encoder* enc) {
    enc->outStream.total = (((enc->outStream.total + 7) >> 3) + 2) << 3;
    return 0;
}

void TMCC_JPEGENC_PutRstMarkerRC(TMCC_Encoder* enc) {
    enc->outStream.total = (((enc->outStream.total + 7) >> 3) + 2) << 3;
    enc->restartCount++;
    enc->unk15A4 = 0;
    enc->unk15D8 = 0;
    enc->unk160C = 0;
}

void TMCC_JPEGENC_CheckPutStream(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = enc->stream;
    if ((u32)(bs->cur - bs->start) >= (u32)(bs->capacity - 0x100)) {
        enc->putFn = TMCC_JPEGENC_PutStreamWithCheck;
    } else {
        enc->putFn = TMCC_JPEGENC_PutStream;
    }
}

void TMCC_JPEGENC_init_comp_edge(TMCC_Encoder* enc, int mode) {
    switch (mode) {
    case 0:
        enc->edgeFn = TMCC_JPEGENC_encoder_y411edge;
        break;
    case 2:
        enc->edgeFn = TMCC_JPEGENC_encoder_y420edge;
        break;
    case 1:
        enc->edgeFn = TMCC_JPEGENC_encoder_y422edge;
        break;
    }
}

void TMCC_JPEGENC_init_comp(TMCC_Encoder* enc, int mode) {
    switch (mode) {
    case 0:
        enc->compFn = TMCC_JPEGENC_encoder_y411;
        break;
    case 2:
        enc->compFn = TMCC_JPEGENC_encoder_y420;
        break;
    case 1:
        enc->compFn = TMCC_JPEGENC_encoder_y422;
        break;
    }
}

void TMCC_JPEGENC_init_precomp_edge(TMCC_Encoder* enc, int mode) {
    switch (mode) {
    case 0:
        enc->edgeFn = TMCC_JPEGENC_counter_y411edge;
        break;
    case 2:
        enc->edgeFn = TMCC_JPEGENC_counter_y420edge;
        break;
    case 1:
        enc->edgeFn = TMCC_JPEGENC_counter_y422edge;
        break;
    }
}

void TMCC_JPEGENC_init_precomp(TMCC_Encoder* enc) {
    switch (enc->format) {
    case 0:
        enc->compFn = TMCC_JPEGENC_counter_y411;
        break;
    case 2:
        enc->compFn = TMCC_JPEGENC_counter_y420;
        break;
    case 1:
        enc->compFn = TMCC_JPEGENC_counter_y422;
        break;
    }
}

void TMCC_JPEGENC_enc_restart_interval(TMCC_Encoder* enc) {
    TMCC_JPEGENC_PutMaker(&enc->outStream, (enc->restartCount & 7) + 0xFFD0);
    enc->restartCount++;
    enc->unk15A4 = 0;
    enc->unk15D8 = 0;
    enc->unk160C = 0;
}

void TMCC_JPEGENC_PutWord(TMCC_Bitstream* bs, u16 word) {
    *bs->cur++ = word >> 8;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = word;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur = 0;
}

void TMCC_JPEGENC_PutString(TMCC_Bitstream* bs, const u8* str, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
            bs->overflow = 1;
            bs->total += bs->cur - bs->start;
            bs->cur = bs->start;
        }
        *bs->cur = *str++;
        bs->cur++;
    }
    *bs->cur = 0;
}

void TMCC_JPEGENC_set_usr_parameters(TMCC_Encoder* enc, TMCC_UsrParam* usr) {
    int edge;
    usr->unkC = 0;
    usr->unkE = 0;
    usr->unk10 = 0;
    usr->unk14 = enc->putFn;
    usr->unk1C = enc->width;
    usr->unk1E = enc->height;
    usr->unk18 = usr->unk1C * usr->unk1E;
    usr->unk20 = enc->unk1594;
    usr->unk21 = enc->unk1595;
    usr->unk22 = enc->unk1588;
    usr->unk23 = enc->unk1589;
    if (enc->unk1588 == 0) {
        edge = (usr->unk1C + 1) * usr->unk20;
    } else {
        edge = (usr->unk1C - 1) * usr->unk20;
    }
    usr->unk24 = edge;
    if (enc->unk1589 == 0) {
        edge = (usr->unk1E + 1) * usr->unk21;
    } else {
        edge = (usr->unk1E - 1) * usr->unk21;
    }
    usr->unk28 = edge;
    usr->unk2C = 1;
    usr->unk2E = enc->format;
    usr->unk30 = enc->unk2;
    usr->unk32 = enc->unk4;
    usr->unk34 = enc->unk6;
    usr->unk36 = enc->unk8;
    usr->unk38 = usr->unk20 * usr->unk1C;
    usr->unk3C = usr->unk21 * usr->unk1E;
    usr->unk40 = enc->unk15C8 * usr->unk1C;
    usr->unk44 = enc->unk15C9 * usr->unk1E;
}
