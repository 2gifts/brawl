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
    u8 pad1[0x18 - 1];
    int restartCount;
    TMCC_Bitstream* stream;
    TMCC_EncFn putFn;
    TMCC_EncFn compFn;
    TMCC_EncFn edgeFn;
    u8 pad2C[0x15a4 - 0x2C];
    int unk15A4;
    u8 pad15A8[0x15d8 - 0x15A8];
    int unk15D8;
    u8 pad15DC[0x160c - 0x15DC];
    int unk160C;
    u8 pad1610[0x1634 - 0x1610];
    TMCC_Bitstream outStream;
};

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
