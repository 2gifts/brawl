#include <types.h>
#include <string.h>

// BYTES: original emits out-of-line local calls and stmw/lmw prologues
#pragma auto_inline off
#pragma use_lmw_stmw on

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
typedef struct TMCC_UsrParam TMCC_UsrParam;
typedef struct TMCC_ImageInfo TMCC_ImageInfo;
typedef struct TMCC_McuBuf TMCC_McuBuf;
typedef int (*TMCC_CompFn)(int, int, TMCC_ImageInfo*);
typedef int (*TMCC_EdgeFn)(int, int, int, TMCC_ImageInfo*);
typedef void (*TMCC_PutFn)(TMCC_Bitstream*, int, int);
typedef int (*TMCC_CvtFn)(TMCC_McuBuf*, TMCC_UsrParam*, int, int);

// HYPOTHESIS: huffman bit-count / encoding table for a component
typedef struct TMCC_CompHuff {
    int dcMax;
    int* dcCode;
    int* dcBits;
    int acMax;
    int* acCode;
    int* acBits;
} TMCC_CompHuff;

// HYPOTHESIS: component info inside image info
typedef struct TMCC_CompInfo {
    int unk0;
    int stride;
    int dcPred;
    int unkC;
    TMCC_CompHuff* unk10;
    int* unk14;
    int* unk18;
    TMCC_Bitstream* unk1C;
    TMCC_PutFn putFn; u8 pad24[4]; u16 nextW; u16 nextH;
    u8 unk2C;
    u8 unk2D;
    u8 nextBlocks; u8 nextEdgeW; u8 nextEdgeH; u8 nextFmt; u8 pad32[2];
} TMCC_CompInfo;

// HYPOTHESIS: image conversion info at offset 0x1580 of TMCC_Encoder
struct TMCC_ImageInfo {
    u16 width;
    u16 height;
    u16 unk4;
    u16 unk6;
    u8 unk8;
    u8 unk9;
    u8 padA[6]; u16 yW; u16 yH;
    u8 unk14;
    u8 unk15;
    u8 yBlocks; u8 yEdgeW; u8 yEdgeH; u8 yFmt; u8 pad1A[2];
    TMCC_CompInfo compY;
    TMCC_CompInfo compCb;
    int crUnk0;
    int crStride;
    int crDcPred;
    int crUnkC; TMCC_CompHuff* crUnk10; int* crUnk14; const int* crUnk18; TMCC_Bitstream* crUnk1C; TMCC_PutFn crPutFn;
    TMCC_UsrParam* usr;
    TMCC_CvtFn cvtFn;
    TMCC_CvtFn cvtEdgeFn;
};

// HYPOTHESIS: encoder state, only the fields these functions touch
struct TMCC_Encoder {
    u8 format;
    u8 pad1;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
    u16 unkA;
    u16 unkC;
    u8 unkE;
    u8 unkF;
    int unk10;
    int unk14;
    int restartCount;
    union {
        TMCC_Bitstream* stream;
        u8 unk1C;
    } u1C;
    union {
        TMCC_PutFn putFn;
        int unk20;
    } u20;
    TMCC_CompFn compFn;
    TMCC_EdgeFn edgeFn;
    const u8* huffSrc[8];
    u8 huffBuf[0x114c - 0x4C];
    struct {
        int unk0;
        void* unk4;
        void* unk8;
    } huffTbl[4];
    int qLum[64]; int qLumRecip[64]; int qChr[64]; int qChrRecip[64]; const int* zigzag;
    TMCC_ImageInfo img;
    TMCC_Bitstream outStream;
};

// HYPOTHESIS: user parameter block filled from the encoder state
struct TMCC_UsrParam {
    TMCC_Encoder* unk0;
    u16 unk4;
    u16 unk6;
    int unk8;
    u16 unkC;
    u16 unkE;
    int unk10;
    TMCC_PutFn unk14;
    int unk18;
    u16 unk1C;
    u16 unk1E;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    u32 unk24;
    u32 unk28;
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
    u32 unk48;
    int unk4C;
    int unk50;
    int unk54;
    u8* unk58;
    u8* unk5C;
    u8* unk60;
    u16* unk64;
    u8 pad68[0x6FC - 0x68];
};

// HYPOTHESIS: init parameters
typedef struct TMCC_InitParam {
    TMCC_Encoder* enc;
    int unk4;
    u16 unk8;
    u16 unkA;
    u8 unkC;
    u8 padD;
    u16 unkE;
    u8 unk10;
    u8 pad11[3];
    int unk14;
    u32 unk18; u8 pad1C[0x9C - 0x1C];
    int bufSize;
} TMCC_InitParam;

void TMCC_JPEGENC_set_usr_parameters(TMCC_Encoder*, TMCC_UsrParam*);
void TMCC_JPEGENC_PutStream(TMCC_Bitstream*, int, int);
void TMCC_JPEGENC_PutStreamWithCheck(TMCC_Bitstream*, int, int);
void TMCC_JPEGENC_PutMaker(TMCC_Bitstream*, u16);
int TMCC_JPEGENC_encoder_y411edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y420edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y422edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y411(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y420(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y422(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y411edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y420edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y422edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y411(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y420(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y422(int, int, TMCC_ImageInfo*);

int fn_8026A62C(TMCC_InitParam*, TMCC_Encoder*);
void fn_8026C0AC(TMCC_Encoder*);
void fn_8026C45C(TMCC_Encoder*);
void fn_8026A7D0(TMCC_Encoder*);
int fn_8026A3B8(TMCC_Encoder*);
void fn_8026BBF0(s8*, int*, int);
void fn_8026BDE8(s8*, int*, int);
int fn_802698AC(int*, TMCC_CompInfo*);
int fn_8026CE80(int*, TMCC_CompInfo*);
int fn_80269C98(TMCC_Encoder*, TMCC_UsrParam*, int);
int fn_8026D53C(TMCC_Encoder*, TMCC_UsrParam*, int, int);
void fn_80269508(TMCC_Bitstream*);
void fn_8026D514(TMCC_Bitstream*, int*);

int fn_8026DCEC(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026DF60(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E104(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E2B4(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E4AC(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E6A8(TMCC_McuBuf*, TMCC_UsrParam*, int, int);

int fn_8026E9B4(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026EBE4(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026ED68(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026EEF8(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026F0E0(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026F2CC(TMCC_McuBuf*, TMCC_UsrParam*, int, int);

struct TMCC_McuBuf {
    int unk0;
    s8 data[384];
};

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
    enc->img.compY.dcPred = 0;
    enc->img.compCb.dcPred = 0;
    enc->img.crDcPred = 0;
}

void TMCC_JPEGENC_CheckPutStream(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = enc->u1C.stream;
    if ((u32)(bs->cur - bs->start) >= (u32)(bs->capacity - 0x100)) {
        enc->u20.putFn = TMCC_JPEGENC_PutStreamWithCheck;
    } else {
        enc->u20.putFn = TMCC_JPEGENC_PutStream;
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

void TMCC_JPEGENC_init_precomp(TMCC_Encoder* enc, int mode) {
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
    enc->img.compY.dcPred = 0;
    enc->img.compCb.dcPred = 0;
    enc->img.crDcPred = 0;
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
    usr->unk14 = enc->u20.putFn;
    usr->unk1C = enc->img.width;
    usr->unk1E = enc->img.height;
    usr->unk18 = usr->unk1C * usr->unk1E;
    usr->unk20 = enc->img.unk14;
    usr->unk21 = enc->img.unk15;
    usr->unk22 = enc->img.unk8;
    usr->unk23 = enc->img.unk9;
    if (enc->img.unk8 == 0) {
        edge = (usr->unk1C + 1) * usr->unk20;
    } else {
        edge = (usr->unk1C - 1) * usr->unk20;
    }
    usr->unk24 = edge;
    if (enc->img.unk9 == 0) {
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
    usr->unk40 = enc->img.compY.unk2C * usr->unk1C;
    usr->unk44 = enc->img.compY.unk2D * usr->unk1E;
}

void fn_802692BC(TMCC_Bitstream* bs, u8 byte) {
    u8* p = bs->cur;
    *p = byte;
    p++;
    bs->cur = p;
    if ((u32)(p - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += p - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur = 0;
}

static const u8 sDefaultDcLumBits[0x14] = {
    0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultDcLumVals[0x0C] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
};
static const u8 sDefaultDcChrBits[0x14] = {
    0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultDcChrVals[0x0C] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
};
static const u8 sDefaultAcLumBits[0x14] = {
    0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultAcLumVals[0xA4] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA, 0x00, 0x00
};
static const u8 sDefaultAcChrBits[0x14] = {
    0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultAcChrVals[0xA4] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
    0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
    0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
    0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
    0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
    0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA, 0x00, 0x00
};

static const u8 sBitMasks[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

void fn_8026D514(TMCC_Bitstream* bs, int* out) {
    u32 bytes = bs->total >> 3;
    *out += bytes;
    bs->total -= bytes << 3;
}

int fn_8026BB2C(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = &enc->outStream;
    TMCC_JPEGENC_PutWord(bs, 0xFFDA);
    TMCC_JPEGENC_PutWord(bs, 12);
    fn_802692BC(bs, 3);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 1);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 2);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 0x3F);
    fn_802692BC(bs, 0);
    return 0;
}

void fn_80269508(TMCC_Bitstream* bs) {
    if (bs->unk8 != 8) {
        TMCC_JPEGENC_PutStreamWithCheck(bs, sBitMasks[bs->unk8], bs->unk8);
    }
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = 0xFF;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = 0xD9;
}

void TMCC_JPEGENC_PutMaker(TMCC_Bitstream* bs, u16 marker) {
    if (bs->unk8 != 8) {
        TMCC_JPEGENC_PutStreamWithCheck(bs, sBitMasks[bs->unk8], bs->unk8);
    }
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = marker >> 8;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = marker;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur = 0;
}


void fn_8026BFFC(TMCC_Encoder* enc) {
    enc->huffSrc[0] = sDefaultDcLumBits;
    enc->huffSrc[1] = sDefaultDcLumVals;
    enc->huffSrc[2] = sDefaultDcChrBits;
    enc->huffSrc[3] = sDefaultDcChrVals;
    enc->huffSrc[4] = sDefaultAcLumBits;
    enc->huffSrc[5] = sDefaultAcLumVals;
    enc->huffSrc[6] = sDefaultAcChrBits;
    enc->huffSrc[7] = sDefaultAcChrVals;

    enc->huffTbl[0].unk0 = 0;
    enc->huffTbl[0].unk4 = &enc->huffBuf[0x0];
    enc->huffTbl[0].unk8 = &enc->huffBuf[0x40];

    enc->huffTbl[1].unk0 = 0;
    enc->huffTbl[1].unk4 = &enc->huffBuf[0x100];
    enc->huffTbl[1].unk8 = &enc->huffBuf[0x500];

    enc->huffTbl[2].unk0 = 0;
    enc->huffTbl[2].unk4 = &enc->huffBuf[0x80];
    enc->huffTbl[2].unk8 = &enc->huffBuf[0xC0];

    enc->huffTbl[3].unk0 = 0;
    enc->huffTbl[3].unk4 = &enc->huffBuf[0x900];
    enc->huffTbl[3].unk8 = &enc->huffBuf[0xD00];
}



int TMCC_JPEGENC_encoder_y422(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x80, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0xC0, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y422(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x80, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0xC0, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y422edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x80, block, 8);
    ret = fn_802698AC(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0xC0, block, 8);
    ret = fn_802698AC(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y422edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x80, block, 8);
    ret = fn_8026CE80(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0xC0, block, 8);
    ret = fn_8026CE80(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y411(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y411(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y420(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y420(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y411edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_802698AC(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_802698AC(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y411edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_8026CE80(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_8026CE80(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y420edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_802698AC(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_802698AC(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y420edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_8026CE80(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_8026CE80(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int fn_8026A28C(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = &enc->outStream;
    u16 h = enc->unk4;
    u16 w = enc->unk2;
    int v = 1 << enc->unkF;
    int u = 1 << enc->unkE;

    TMCC_JPEGENC_PutWord(bs, 0xFFC0);
    TMCC_JPEGENC_PutWord(bs, 17);
    fn_802692BC(bs, 8);
    TMCC_JPEGENC_PutWord(bs, h);
    TMCC_JPEGENC_PutWord(bs, w);
    fn_802692BC(bs, 3);
    fn_802692BC(bs, 0);
    TMCC_JPEGENC_PutStreamWithCheck(bs, u, 4);
    TMCC_JPEGENC_PutStreamWithCheck(bs, v, 4);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 1);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 1);
    fn_802692BC(bs, 2);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 1);
    if (fn_8026BB2C(enc) < 0) {
        return -2;
    }
    return 0;
}

int fn_8026C7D4(TMCC_UsrParam* usr, int unused, TMCC_InitParam* param) {
    TMCC_Encoder* enc = param->enc;
    int ret;
    if (enc == NULL) {
        return -1;
    }
    memset(usr, 0, sizeof(TMCC_UsrParam));
    memset(enc, 0, sizeof(TMCC_Encoder));
    usr->unk0 = enc;
    enc->img.usr = usr;
    ret = fn_8026A62C(param, enc);
    if (ret < 0) {
        return ret;
    }
    fn_8026BFFC(enc);
    fn_8026C0AC(enc);
    fn_8026C45C(enc);
    fn_8026A7D0(enc);
    TMCC_JPEGENC_Init_bitstream(&enc->outStream, 0, param->bufSize);
    TMCC_JPEGENC_init_precomp(enc, enc->format);
    TMCC_JPEGENC_init_precomp_edge(enc, enc->format);
    TMCC_JPEGENC_set_usr_parameters(enc, usr);
    switch (usr->unk8) {
    case 4:
        ret = fn_8026E8C0(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    case 6:
        ret = fn_8026DBF8(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    }
    if (enc->unk10 != 0) {
        usr->unk10 = 0x263;
    } else {
        usr->unk10 = 0x25D;
    }
    return usr->unk18;
}

int fn_8026B3D8(TMCC_UsrParam* usr, u8* outBuf, int unused, TMCC_InitParam* param) {
    TMCC_Encoder* enc = param->enc;
    int ret;
    if (enc == NULL) {
        return -1;
    }
    memset(usr, 0, sizeof(TMCC_UsrParam));
    memset(enc, 0, sizeof(TMCC_Encoder));
    usr->unk0 = enc;
    enc->img.usr = usr;
    ret = fn_8026A62C(param, enc);
    if (ret < 0) {
        return ret;
    }
    fn_8026BFFC(enc);
    fn_8026C0AC(enc);
    fn_8026C45C(enc);
    fn_8026A7D0(enc);
    TMCC_JPEGENC_Init_bitstream(&enc->outStream, outBuf, param->bufSize);
    TMCC_JPEGENC_init_comp(enc, enc->format);
    TMCC_JPEGENC_init_comp_edge(enc, enc->format);
    if (fn_8026A3B8(enc) < 0) {
        return -2;
    }
    TMCC_JPEGENC_set_usr_parameters(enc, usr);
    switch (usr->unk8) {
    case 4:
        ret = fn_8026E8C0(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    case 6:
        ret = fn_8026DBF8(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    }
    TMCC_JPEGENC_Get_bitstream(&enc->outStream, &usr->unk10);
    return usr->unk18;
}













void TMCC_JPEGENC_PutStream(TMCC_Bitstream* bs, int bits, int len) {
    const u8* masks = sBitMasks;
    int val = *bs->cur;
    int rem = bs->unk8;
    while (len > rem) {
        val |= (bits >> (len - rem)) & masks[rem];
        *bs->cur++ = val;
        if (val == 0xFF) {
            *bs->cur++ = 0;
        }
        len -= rem;
        rem = 8;
        val = 0;
    }
    if (len == rem) {
        val |= bits & sBitMasks[len];
        bs->unk8 = 8;
        *bs->cur++ = val;
        if (val == 0xFF) {
            *bs->cur++ = 0;
        }
        *bs->cur = 0;
    } else {
        rem -= len;
        val |= (bits & sBitMasks[len]) << rem;
        *bs->cur = val;
        bs->unk8 = rem;
    }
}

int fn_8026A62C(TMCC_InitParam* param, TMCC_Encoder* enc) {
    if (param->unk8 == 0) return -1;
    if (param->unkA == 0) return -1;
    enc->unk2 = param->unk8;
    enc->unk4 = param->unkA;
    enc->unk10 = param->unkE;
    enc->u1C.unk1C = param->unk10;
    switch (enc->u1C.unk1C) {
    case 1:
        if (param->unk14 == 0 || (u32)param->unk14 > 500) return -1;
        enc->u20.unk20 = param->unk14;
        break;
    default:
        return -1;
    }
    switch (param->unkC) {
    case 0:
        enc->format = 0;
        enc->unkE = 2;
        enc->unkF = 0;
        enc->unk6 = (enc->unk2 + 3) / 4;
        enc->unk8 = enc->unk4;
        enc->unkA = (enc->unk2 + 3) / 4;
        enc->unkC = enc->unk4;
        break;
    case 1:
        enc->format = 1;
        enc->unkE = 1;
        enc->unkF = 0;
        enc->unk6 = (enc->unk2 + 1) / 2;
        enc->unk8 = enc->unk4;
        enc->unkA = (enc->unk2 + 1) / 2;
        enc->unkC = enc->unk4;
        break;
    case 2:
        enc->format = 2;
        enc->unkE = 1;
        enc->unkF = 1;
        enc->unk6 = (enc->unk2 + 1) / 2;
        enc->unk8 = (enc->unk4 + 1) / 2;
        enc->unkA = (enc->unk2 + 1) / 2;
        enc->unkC = (enc->unk4 + 1) / 2;
        break;
    default:
        return -1;
    }
    if (param->unk4 != 4 && param->unk4 != 6) return -4;
    // BYTES: the input format is copied as an unsigned word after validation.
    enc->img.usr->unk8 = *(u32*)&param->unk4;
    return 0;
}

void TMCC_JPEGENC_PutStreamWithCheck(TMCC_Bitstream* bs, int bits, int len) {
    const u8* masks = sBitMasks;
    int val = *bs->cur;
    int rem = bs->unk8;
    while (len > rem) {
        val |= (bits >> (len - rem)) & masks[rem];
        *bs->cur++ = val;
        if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
            bs->overflow = 1;
            bs->total += bs->cur - bs->start;
            bs->cur = bs->start;
        }
        if (val == 0xFF) {
            *bs->cur++ = 0;
            if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
                bs->overflow = 1;
                bs->total += bs->cur - bs->start;
                bs->cur = bs->start;
            }
        }
        len -= rem;
        rem = 8;
        val = 0;
    }
    if (len == rem) {
        val |= bits & sBitMasks[len];
        bs->unk8 = 8;
        *bs->cur++ = val;
        if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
            bs->overflow = 1;
            bs->total += bs->cur - bs->start;
            bs->cur = bs->start;
        }
        if (val == 0xFF) {
            *bs->cur++ = 0;
            if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
                bs->overflow = 1;
                bs->total += bs->cur - bs->start;
                bs->cur = bs->start;
            }
        }
        *bs->cur = 0;
    } else {
        rem -= len;
        *bs->cur = val | ((bits & sBitMasks[len]) << rem);
        bs->unk8 = rem;
    }
}

int fn_8026C930(TMCC_UsrParam* usr, int count, u8* pixels, int limit) {
    TMCC_Encoder* enc = usr->unk0;
    TMCC_ImageInfo* img = &enc->img;
    int x, y, w, h, total;
    int skip = 0;
    int done = 0;
    int rst, ret;

    usr->unk58 = pixels;
    usr->unk64 = (u16*)pixels;
    usr->unk5C = pixels + usr->unk48 * usr->unk4C;
    usr->unk60 = pixels + usr->unk50 * usr->unk54;
    if (img->unk9 != 0 || img->unk8 != 0) {
        return fn_8026D53C(enc, usr, count, limit);
    }
    w = img->width;
    h = img->height;
    total = w * h;
    x = usr->unkC;
    y = usr->unkE;
    rst = enc->unk14;
    while (y < h && done < count) {
        while (x < w && done < count) {
            if (++skip >= limit) {
                ret = enc->compFn(x, y, img);
                if (ret < 0) return ret;
                skip = 0;
                fn_8026D514(&enc->outStream, &usr->unk10);
            }
            x++;
            done++;
            if (--rst == 0) {
                TMCC_JPEGENC_PutRstMarkerRC(enc);
                rst = enc->unk10;
            }
        }
        if (x >= w) {
            x = 0;
            y++;
        }
    }
    if (y == h && x == 0) {
        TMCC_JPEGENC_CountMarker(enc);
        if (enc->outStream.overflow == 1) {
            fn_8026D514(&enc->outStream, &usr->unk10);
            return -3;
        }
    }
    img->unk4 = x;
    usr->unkC = x;
    img->unk6 = y;
    usr->unkE = y;
    fn_8026D514(&enc->outStream, &usr->unk10);
    enc->unk14 = rst;
    return total - x - y * w;
}

int fn_8026B544(TMCC_UsrParam* usr, int count, u8* pixels) {
    TMCC_Encoder* enc = usr->unk0;
    TMCC_ImageInfo* img = &enc->img;
    int x, y, w, h, total;
    int done = 0;
    int rst, ret;

    usr->unk58 = pixels;
    usr->unk64 = (u16*)pixels;
    usr->unk5C = pixels + usr->unk48 * usr->unk4C;
    usr->unk60 = pixels + usr->unk50 * usr->unk54;
    if (img->unk9 != 0 || img->unk8 != 0) {
        return fn_80269C98(enc, usr, count);
    }
    w = img->width;
    h = img->height;
    total = w * h;
    x = usr->unkC;
    y = usr->unkE;
    rst = enc->unk14;
    while (y < h && done < count) {
        while (x < w && done < count) {
            ret = enc->compFn(x, y, img);
            if (ret < 0) return ret;
            x++;
            done++;
            if (--rst == 0) {
                TMCC_JPEGENC_PutMaker(&enc->outStream, (enc->restartCount & 7) + 0xFFD0);
                enc->restartCount++;
                enc->img.compY.dcPred = 0;
                enc->img.compCb.dcPred = 0;
                enc->img.crDcPred = 0;
                rst = enc->unk10;
            }
        }
        if (x >= w) {
            x = 0;
            y++;
        }
    }
    if (y == h && x == 0) {
        fn_80269508(&enc->outStream);
        if (enc->outStream.overflow == 1) {
            TMCC_JPEGENC_Get_bitstream(&enc->outStream, &usr->unk10);
            return -3;
        }
    }
    img->unk4 = x;
    usr->unkC = x;
    img->unk6 = y;
    usr->unkE = y;
    TMCC_JPEGENC_Get_bitstream(&enc->outStream, &usr->unk10);
    enc->unk14 = rst;
    return total - x - y * w;
}

int fn_80269C98(TMCC_Encoder* enc, TMCC_UsrParam* usr, int count) {
    TMCC_ImageInfo* img = &enc->img;
    int x;
    int y;
    int w;
    int h;
    int total;
    int done = 0;
    int rst;
    int ret;

    w = img->width - 1;
    rst = enc->unk14;
    h = img->height - 1;
    y = img->unk6;
    total = img->width * img->height;
    x = img->unk4;

    while (y < h && done < count) {
        while (x < w && done < count) {
            ret = enc->compFn(x, y, img);
            if (ret < 0) {
                return ret;
            }
            done--;
            x++;
            if (--rst == 0) {
                TMCC_JPEGENC_enc_restart_interval(enc);
                rst = enc->unk10;
            }
        }
        if (done < count) {
            ret = enc->edgeFn(x, y, 0, img);
            if (ret < 0) {
                return ret;
            }
            x = 0;
            done--;
            y++;
            if (--rst == 0) {
                TMCC_JPEGENC_enc_restart_interval(enc);
                rst = enc->unk10;
            }
        }
    }
    while (x < w && done < count) {
        ret = enc->edgeFn(x, y, 1, img);
        if (ret < 0) {
            return ret;
        }
        done--;
        x++;
        if (--rst == 0) {
            TMCC_JPEGENC_enc_restart_interval(enc);
            rst = enc->unk10;
        }
    }
    if (done < count) {
        ret = enc->edgeFn(x, y, 2, img);
        if (ret < 0) {
            return ret;
        }
        x = 0;
        y++;
        if (--rst == 0) {
            TMCC_JPEGENC_enc_restart_interval(enc);
            rst = enc->unk10;
        }
        fn_80269508(&enc->outStream);
        if (enc->outStream.overflow != 0) {
            TMCC_JPEGENC_Get_bitstream(&enc->outStream, &usr->unk10);
            return -3;
        }
    }
    img->unk4 = x;
    usr->unkC = x;
    img->unk6 = y;
    usr->unkE = y;
    enc->unk14 = rst;
    TMCC_JPEGENC_Get_bitstream(&enc->outStream, &usr->unk10);
    return total - x - y * img->width;
}

int fn_8026D53C(TMCC_Encoder* enc, TMCC_UsrParam* usr, int count, int limit) {
    TMCC_ImageInfo* img = &enc->img;
    int x;
    int y;
    int w;
    int h;
    int total;
    int done = 0;
    int rst;
    int skip = 0;
    int ret;

    w = img->width - 1;
    rst = enc->unk14;
    h = img->height - 1;
    y = img->unk6;
    total = img->width * img->height;
    x = img->unk4;

    while (y < h && done < count) {
        while (x < w && done < count) {
            if (++skip >= limit) {
                ret = enc->compFn(x, y, img);
                if (ret < 0) {
                    return ret;
                }
                skip = 0;
                fn_8026D514(&enc->outStream, &usr->unk10);
            }
            done--;
            x++;
            if (--rst == 0) {
                TMCC_JPEGENC_PutRstMarkerRC(enc);
                rst = enc->unk10;
            }
        }
        if (done < count) {
            if (++skip >= limit) {
                ret = enc->edgeFn(x, y, 0, img);
                if (ret < 0) {
                    return ret;
                }
                skip = 0;
                fn_8026D514(&enc->outStream, &usr->unk10);
            }
            x = 0;
            done--;
            y++;
            if (--rst == 0) {
                TMCC_JPEGENC_PutRstMarkerRC(enc);
                rst = enc->unk10;
            }
        }
    }
    while (x < w && done < count) {
        if (++skip >= limit) {
            ret = enc->edgeFn(x, y, 1, img);
            if (ret < 0) {
                return ret;
            }
            skip = 0;
            fn_8026D514(&enc->outStream, &usr->unk10);
        }
        done--;
        x++;
        if (--rst == 0) {
            TMCC_JPEGENC_PutRstMarkerRC(enc);
            rst = enc->unk10;
        }
    }
    if (done < count) {
        if (++skip >= limit) {
            ret = enc->edgeFn(x, y, 2, img);
            if (ret < 0) {
                return ret;
            }
            skip = 0;
            fn_8026D514(&enc->outStream, &usr->unk10);
        }
        x = 0;
        y++;
        if (--rst == 0) {
            TMCC_JPEGENC_PutRstMarkerRC(enc);
            rst = enc->unk10;
        }
        TMCC_JPEGENC_CountMarker(enc);
        if (enc->outStream.overflow != 0) {
            fn_8026D514(&enc->outStream, &usr->unk10);
            return -3;
        }
    }
    img->unk4 = x;
    usr->unkC = x;
    img->unk6 = y;
    usr->unkE = y;
    enc->unk14 = rst;
    fn_8026D514(&enc->outStream, &usr->unk10);
    return total - x - y * img->width;
}

int fn_8026DBF8(TMCC_ImageInfo* img, TMCC_UsrParam* usr) {
    switch (usr->unk2E) {
    case 0:
        img->cvtFn = fn_8026DCEC;
        img->cvtEdgeFn = fn_8026E2B4;
        break;
    case 1:
        img->cvtFn = fn_8026DF60;
        img->cvtEdgeFn = fn_8026E4AC;
        break;
    case 2:
        img->cvtFn = fn_8026E104;
        img->cvtEdgeFn = fn_8026E6A8;
        break;
    default:
        return -0x70;
    }
    usr->unk48 = (((u32)usr->unk30 >> 2) + ((usr->unk30 % 4) != 0)) << 2;
    usr->unk4C = (((u32)usr->unk32 >> 2) + ((usr->unk32 % 4) != 0)) << 2;
    return 0;
}

int fn_8026E8C0(TMCC_ImageInfo* img, TMCC_UsrParam* usr) {
    switch (usr->unk2E) {
    case 0:
        img->cvtFn = fn_8026E9B4;
        img->cvtEdgeFn = fn_8026EEF8;
        break;
    case 1:
        img->cvtFn = fn_8026EBE4;
        img->cvtEdgeFn = fn_8026F0E0;
        break;
    case 2:
        img->cvtFn = fn_8026ED68;
        img->cvtEdgeFn = fn_8026F2CC;
        break;
    default:
        return -0x70;
    }
    usr->unk48 = (((u32)usr->unk30 >> 2) + ((usr->unk30 % 4) != 0)) << 2;
    usr->unk4C = (((u32)usr->unk32 >> 2) + ((usr->unk32 % 4) != 0)) << 2;
    return 0;
}

int fn_8026EBE4(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    int rowTile;
    u16* row;
    int r, g, b;
    u16 pix;
    u8* pY;
    u8* pCb;
    u8* pCr;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int ySkip, cSkip;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x80;
    pCr = (u8*)mcu->data + 0xC0;
    w = 16 / usr->unk2C;
    h = 8 / usr->unk2C;
    ySkip = 16 - w;
    cSkip = ySkip >> 1;
    xEnd = x + w;
    yEnd = y + h;
    src = usr->unk64;
    tileStride = usr->unk48 >> 2;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i += 2) {
            pix = row[(i & 3) + (((i >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[0] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            *pCb++ = ((b - pY[0]) * 0x90 + 0x80) >> 8;
            *pCr++ = ((r - pY[0]) * 0xB7 + 0x80) >> 8;

            pix = row[((i + 1) & 3) + ((((i + 1) >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[1] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            pY += 2;
        }
        pY += ySkip;
        pCb += cSkip;
        pCr += cSkip;
    }
    return 0;
}

int fn_8026ED68(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    int rowTile;
    u16* row;
    int r, g, b;
    u16 pix;
    u8* pY;
    u8* pCb;
    u8* pCr;
    u16* src;
    int i, j;
    int w, xEnd, yEnd;
    int ySkip, cSkip;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    w = 16 / usr->unk2C;
    ySkip = 16 - w;
    cSkip = ySkip >> 1;
    xEnd = x + w;
    yEnd = y + w;
    src = usr->unk64;
    tileStride = usr->unk48 >> 2;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i += 2) {
            pix = row[(i & 3) + (((i >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[0] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((j & 1) == 0) {
                *pCb++ = ((b - pY[0]) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - pY[0]) * 0xB7 + 0x80) >> 8;
            }

            pix = row[((i + 1) & 3) + ((((i + 1) >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[1] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            pY += 2;
        }
        pY += ySkip;
        if ((j & 1) == 0) {
            pCb += cSkip;
            pCr += cSkip;
        }
    }
    return 0;
}

int fn_8026E9B4(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    int rowTile;
    u16* row;
    int r, g, b;
    u16 pix;
    u8* pY;
    u8* pCb;
    u8* pCr;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int ySkip, cSkip;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    w = 32 / usr->unk2C;
    h = 8 / usr->unk2C;
    ySkip = 32 - w;
    cSkip = ySkip >> 2;
    xEnd = x + w;
    yEnd = y + h;
    src = usr->unk64;
    tileStride = usr->unk48 >> 2;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i += 4) {
            pix = row[(i & 3) + (((i >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[0] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            *pCb++ = ((b - pY[0]) * 0x90 + 0x80) >> 8;
            *pCr++ = ((r - pY[0]) * 0xB7 + 0x80) >> 8;

            pix = row[((i + 1) & 3) + ((((i + 1) >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[1] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;

            pix = row[((i + 2) & 3) + ((((i + 2) >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[2] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;

            pix = row[((i + 3) & 3) + ((((i + 3) >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            pY[3] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            pY += 4;
        }
        pY += ySkip;
        pCb += cSkip;
        pCr += cSkip;
    }
    return 0;
}

int fn_8026EEF8(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    u8* pY;
    u8* pCb;
    u8* pCr;
    int rowTile;
    u16* row;
    int r, g, b;
    u16 pix;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    src = usr->unk64;
    tileStride = usr->unk48 >> 2;

    if (usr->unk24 == x) {
        w = usr->unk22;
    } else {
        w = 32 / usr->unk2C;
    }
    if (usr->unk28 == y) {
        h = usr->unk23;
    } else {
        h = 8 / usr->unk2C;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i++) {
            pix = row[(i & 3) + (((i >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            *pY = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((i & 3) == 0) {
                *pCb++ = ((b - *pY) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - *pY) * 0xB7 + 0x80) >> 8;
            }
            pY++;
        }
        for (i = w; i < 32; i++) {
            *pY = pY[-1];
            pY++;
            if ((i & 3) == 0) {
                *pCb = pCb[-1];
                pCb++;
                *pCr = pCr[-1];
                pCr++;
            }
        }
    }
    for (; h < 8; h++) {
        memcpy(pY, pY - 32, 32);
        pY += 32;
        memcpy(pCb, pCb - 8, 8);
        memcpy(pCr, pCr - 8, 8);
        pCb += 8;
        pCr += 8;
    }
    return 0;
}

int fn_8026F0E0(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    u8* pY;
    u8* pCb;
    u8* pCr;
    int rowTile;
    u16* row;
    int r, g, b;
    u16 pix;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x80;
    pCr = (u8*)mcu->data + 0xC0;
    src = usr->unk64;
    tileStride = usr->unk48 >> 2;

    if (usr->unk24 == x) {
        w = usr->unk22;
    } else {
        w = 16 / usr->unk2C;
    }
    if (usr->unk28 == y) {
        h = usr->unk23;
    } else {
        h = 8 / usr->unk2C;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i++) {
            pix = row[(i & 3) + (((i >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            *pY = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((i & 1) == 0) {
                *pCb++ = ((b - *pY) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - *pY) * 0xB7 + 0x80) >> 8;
            }
            pY++;
        }
        for (i = w; i < 16; i++) {
            *pY = pY[-1];
            pY++;
            if ((i & 1) == 0) {
                *pCb = pCb[-1];
                pCb++;
                *pCr = pCr[-1];
                pCr++;
            }
        }
    }
    for (; h < 8; h++) {
        memcpy(pY, pY - 16, 16);
        pY += 16;
        memcpy(pCb, pCb - 8, 8);
        memcpy(pCr, pCr - 8, 8);
        pCb += 8;
        pCr += 8;
    }
    return 0;
}

int fn_8026F2CC(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    u8* pY;
    u8* pCb;
    u8* pCr;
    int rowTile;
    u16* row;
    int yOdd;
    u16* src;
    int r, g, b;
    u16 pix;
    int i, j;
    int w, h, xEnd, yEnd;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    src = usr->unk64;
    tileStride = usr->unk48 >> 2;

    if (usr->unk24 == x) {
        w = usr->unk22;
    } else {
        w = 16 / usr->unk2C;
    }
    if (usr->unk28 == y) {
        h = usr->unk23;
    } else {
        h = 16 / usr->unk2C;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        yOdd = j & 1;
        for (i = x; i < xEnd; i++) {
            pix = row[(i & 3) + (((i >> 2) + rowTile) << 4)];
            r = (s8)(pix >> 8) & 0xF8;
            g = (s8)(pix >> 3) & 0xFC;
            b = (s8)(pix << 3) & 0xF8;
            *pY = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((i & 1) == 0 && yOdd == 0) {
                *pCb++ = ((b - *pY) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - *pY) * 0xB7 + 0x80) >> 8;
            }
            pY++;
        }
        for (i = w; i < 16; i++) {
            *pY = pY[-1];
            pY++;
            if ((i & 1) == 0 && yOdd == 0) {
                *pCb = pCb[-1];
                pCb++;
                *pCr = pCr[-1];
                pCr++;
            }
        }
    }
    for (; h < 16; h++) {
        memcpy(pY, pY - 16, 16);
        pY += 16;
        if ((h & 1) == 0) {
            memcpy(pCb, pCb - 8, 8);
            memcpy(pCr, pCr - 8, 8);
            pCb += 8;
            pCr += 8;
        }
    }
    return 0;
}

int fn_8026DF60(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    int rowTile;
    u16* row;
    int r, g, b;
    u8* pY;
    u8* pCb;
    u8* pCr;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int ySkip, cSkip;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x80;
    pCr = (u8*)mcu->data + 0xC0;
    w = 16 / usr->unk2C;
    h = 8 / usr->unk2C;
    ySkip = 16 - w;
    cSkip = ySkip >> 1;
    xEnd = x + w;
    yEnd = y + h;
    src = usr->unk64;
    tileStride = (usr->unk48 >> 2) << 1;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i += 2) {
            g = row[(i & 3) + (((((i >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[(i & 3) + ((((i >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[0] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            *pCb++ = ((b - pY[0]) * 0x90 + 0x80) >> 8;
            *pCr++ = ((r - pY[0]) * 0xB7 + 0x80) >> 8;

            g = row[((i + 1) & 3) + ((((((i + 1) >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[((i + 1) & 3) + (((((i + 1) >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[1] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            pY += 2;
        }
        pY += ySkip;
        pCb += cSkip;
        pCr += cSkip;
    }
    return 0;
}

int fn_8026E104(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    int rowTile;
    u16* row;
    int r, g, b;
    u8* pY;
    u8* pCb;
    u8* pCr;
    u16* src;
    int i, j;
    int w, xEnd, yEnd;
    int ySkip, cSkip;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    w = 16 / usr->unk2C;
    ySkip = 16 - w;
    cSkip = ySkip >> 1;
    xEnd = x + w;
    yEnd = y + w;
    src = usr->unk64;
    tileStride = (usr->unk48 >> 2) << 1;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i += 2) {
            g = row[(i & 3) + (((((i >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[(i & 3) + ((((i >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[0] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((j & 1) == 0) {
                *pCb++ = ((b - pY[0]) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - pY[0]) * 0xB7 + 0x80) >> 8;
            }

            g = row[((i + 1) & 3) + ((((((i + 1) >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[((i + 1) & 3) + (((((i + 1) >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[1] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            pY += 2;
        }
        pY += ySkip;
        if ((j & 1) == 0) {
            pCb += cSkip;
            pCr += cSkip;
        }
    }
    return 0;
}

int fn_8026DCEC(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    int rowTile;
    u16* row;
    int r, g, b;
    u8* pY;
    u8* pCb;
    u8* pCr;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int ySkip, cSkip;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    w = 32 / usr->unk2C;
    h = 8 / usr->unk2C;
    ySkip = 32 - w;
    cSkip = ySkip >> 2;
    xEnd = x + w;
    yEnd = y + h;
    src = usr->unk64;
    tileStride = (usr->unk48 >> 2) << 1;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i += 4) {
            g = row[(i & 3) + (((((i >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[(i & 3) + ((((i >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[0] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            *pCb++ = ((b - pY[0]) * 0x90 + 0x80) >> 8;
            *pCr++ = ((r - pY[0]) * 0xB7 + 0x80) >> 8;

            g = row[((i + 1) & 3) + ((((((i + 1) >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[((i + 1) & 3) + (((((i + 1) >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[1] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;

            g = row[((i + 2) & 3) + ((((((i + 2) >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[((i + 2) & 3) + (((((i + 2) >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[2] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;

            g = row[((i + 3) & 3) + ((((((i + 3) >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[((i + 3) & 3) + (((((i + 3) >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            pY[3] = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            pY += 4;
        }
        pY += ySkip;
        pCb += cSkip;
        pCr += cSkip;
    }
    return 0;
}

int fn_8026E2B4(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    u8* pY;
    u8* pCb;
    u8* pCr;
    int rowTile;
    u16* row;
    int r, g, b;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    src = usr->unk64;
    tileStride = (usr->unk48 >> 2) << 1;

    if (usr->unk24 == x) {
        w = usr->unk22;
    } else {
        w = 32 / usr->unk2C;
    }
    if (usr->unk28 == y) {
        h = usr->unk23;
    } else {
        h = 8 / usr->unk2C;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i++) {
            g = row[(i & 3) + (((((i >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[(i & 3) + ((((i >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            *pY = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((i & 3) == 0) {
                *pCb++ = ((b - *pY) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - *pY) * 0xB7 + 0x80) >> 8;
            }
            pY++;
        }
        for (i = w; i < 32; i++) {
            *pY = pY[-1];
            pY++;
            if ((i & 3) == 0) {
                *pCb = pCb[-1];
                pCb++;
                *pCr = pCr[-1];
                pCr++;
            }
        }
    }
    for (; h < 8; h++) {
        memcpy(pY, pY - 32, 32);
        pY += 32;
        memcpy(pCb, pCb - 8, 8);
        memcpy(pCr, pCr - 8, 8);
        pCb += 8;
        pCr += 8;
    }
    return 0;
}

int fn_8026E4AC(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    u8* pY;
    u8* pCb;
    u8* pCr;
    int rowTile;
    u16* row;
    int r, g, b;
    u16* src;
    int i, j;
    int w, h, xEnd, yEnd;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x80;
    pCr = (u8*)mcu->data + 0xC0;
    src = usr->unk64;
    tileStride = (usr->unk48 >> 2) << 1;

    if (usr->unk24 == x) {
        w = usr->unk22;
    } else {
        w = 16 / usr->unk2C;
    }
    if (usr->unk28 == y) {
        h = usr->unk23;
    } else {
        h = 8 / usr->unk2C;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        for (i = x; i < xEnd; i++) {
            g = row[(i & 3) + (((((i >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[(i & 3) + ((((i >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            *pY = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((i & 1) == 0) {
                *pCb++ = ((b - *pY) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - *pY) * 0xB7 + 0x80) >> 8;
            }
            pY++;
        }
        for (i = w; i < 16; i++) {
            *pY = pY[-1];
            pY++;
            if ((i & 1) == 0) {
                *pCb = pCb[-1];
                pCb++;
                *pCr = pCr[-1];
                pCr++;
            }
        }
    }
    for (; h < 8; h++) {
        memcpy(pY, pY - 16, 16);
        pY += 16;
        memcpy(pCb, pCb - 8, 8);
        memcpy(pCr, pCr - 8, 8);
        pCb += 8;
        pCr += 8;
    }
    return 0;
}

int fn_8026E6A8(TMCC_McuBuf* mcu, TMCC_UsrParam* usr, int x, int y) {
    u8* pY;
    u8* pCb;
    u8* pCr;
    int rowTile;
    u16* row;
    int yOdd;
    u16* src;
    int r, g, b;
    int i, j;
    int w, h, xEnd, yEnd;
    int tileStride;

    pY = (u8*)mcu->data;
    pCb = (u8*)mcu->data + 0x100;
    pCr = (u8*)mcu->data + 0x140;
    src = usr->unk64;
    tileStride = (usr->unk48 >> 2) << 1;

    if (usr->unk24 == x) {
        w = usr->unk22;
    } else {
        w = 16 / usr->unk2C;
    }
    if (usr->unk28 == y) {
        h = usr->unk23;
    } else {
        h = 16 / usr->unk2C;
    }
    xEnd = x + w;
    yEnd = y + h;

    for (j = y; j < yEnd; j++) {
        rowTile = (j >> 2) * tileStride;
        row = src + ((j & 3) << 2);
        yOdd = j & 1;
        for (i = x; i < xEnd; i++) {
            g = row[(i & 3) + (((((i >> 2) << 1) + rowTile) + 1) << 4)];
            r = row[(i & 3) + ((((i >> 2) << 1) + rowTile) << 4)] & 0xFF;
            b = g & 0xFF;
            g >>= 8;
            *pY = (r * 0x4D + g * 0x96 + b * 0x1D + 0x80) >> 8;
            if ((i & 1) == 0 && yOdd == 0) {
                *pCb++ = ((b - *pY) * 0x90 + 0x80) >> 8;
                *pCr++ = ((r - *pY) * 0xB7 + 0x80) >> 8;
            }
            pY++;
        }
        for (i = w; i < 16; i++) {
            *pY = pY[-1];
            pY++;
            if ((i & 1) == 0 && yOdd == 0) {
                *pCb = pCb[-1];
                pCb++;
                *pCr = pCr[-1];
                pCr++;
            }
        }
    }
    for (; h < 16; h++) {
        memcpy(pY, pY - 16, 16);
        pY += 16;
        if ((h & 1) == 0) {
            memcpy(pCb, pCb - 8, 8);
            memcpy(pCr, pCr - 8, 8);
            pCb += 8;
            pCr += 8;
        }
    }
    return 0;
}

void fn_8026BBF0(s8* src, int* dst, int stride) {
    int tmp[64];
    u8* in = (u8*)src;
    int* out = tmp;
    int i;
    int t0, t1, t2, t3, t4, t5, t6, t7;
    int t10, t11, t12, t13;
    int s0, s1, s2;
    int z1, z2, z3, z4, z5, z11, z13;
    int* s;
    int* d;

    for (i = 0; i < 8; i++) {
        t0 = in[0] + in[7];
        t7 = in[0] - in[7];
        t1 = in[1] + in[6];
        t6 = in[1] - in[6];
        t2 = in[2] + in[5];
        t5 = in[2] - in[5];
        t3 = in[3] + in[4];
        t4 = in[3] - in[4];

        s2 = t6 + t7;
        s0 = t4 + t5;
        s1 = t5 + t6;

        t10 = t0 + t3;
        t11 = t1 + t2;
        t12 = t1 - t2;
        t13 = t0 - t3;

        z1 = ((t13 + t12) * 0x2D41) >> 14;
        out[0] = t10 + t11 - 1024;
        out[32] = t10 - t11;
        out[16] = t13 + z1;
        out[48] = t13 - z1;

        z2 = (s0 * 0x3B21 - s2 * 0x187E) >> 14;
        z4 = (s2 * 0x3B21 + s0 * 0x187E) >> 14;
        z3 = (s1 * 0x2D41) >> 14;

        z11 = t7 + z3;
        z13 = t7 - z3;

        out[8] = z11 + z4;
        out[56] = z11 - z4;
        out[24] = z13 - z2;
        out[40] = z13 + z2;

        in += stride;
        out++;
    }

    s = tmp;
    d = dst;
    for (i = 0; i < 8; i++) {
        t0 = s[0] + s[7];
        t7 = s[0] - s[7];
        t1 = s[1] + s[6];
        t6 = s[1] - s[6];
        t2 = s[2] + s[5];
        t5 = s[2] - s[5];
        t3 = s[3] + s[4];
        t4 = s[3] - s[4];

        s2 = t6 + t7;
        s0 = t4 + t5;
        s1 = t5 + t6;

        t13 = t0 - t3;
        t12 = t1 - t2;
        t10 = t0 + t3;
        t11 = t1 + t2;

        z1 = ((t13 + t12) * 0x2D41) >> 14;
        d[0] = t10 + t11;
        d[32] = t10 - t11;
        d[16] = t13 + z1;
        d[48] = t13 - z1;

        z5 = (s0 - s2) * 0x187E;
        z2 = (s0 * 0x22A3 + z5) >> 14;
        z4 = (s2 * 0x539F + z5) >> 14;
        z3 = (s1 * 0x2D41) >> 14;

        z13 = t7 - z3;
        z11 = t7 + z3;

        d[40] = z13 + z2;
        d[24] = z13 - z2;
        d[8] = z11 + z4;
        d[56] = z11 - z4;

        s += 8;
        d++;
    }
}

void fn_8026BDE8(s8* src, int* dst, int stride) {
    int tmp[64];
    int t0, t1, t2, t3, t4, t5, t6, t7;
    int t10, t11, t12, t13;
    int s0, s1, s2;
    int z1, z2, z3, z4, z5, z11, z13;
    s8* in = src;
    int* out = tmp;
    int i;
    int* s;
    int* d;

    for (i = 0; i < 8; i++) {
        t0 = in[0] + in[7];
        t7 = in[0] - in[7];
        t3 = in[3] + in[4];
        t4 = in[3] - in[4];
        t2 = in[2] + in[5];
        t5 = in[2] - in[5];
        t1 = in[1] + in[6];
        t6 = in[1] - in[6];

        s0 = t4 + t5;
        t13 = t0 - t3;
        t12 = t1 - t2;
        s2 = t6 + t7;
        t10 = t0 + t3;
        s1 = t5 + t6;
        t11 = t1 + t2;

        z1 = ((t13 + t12) * 0x2D41) >> 14;
        out[0] = t10 + t11;
        out[32] = t10 - t11;
        out[16] = t13 + z1;
        out[48] = t13 - z1;

        z2 = (s0 * 0x3B21 - s2 * 0x187E) >> 14;
        z4 = (s2 * 0x3B21 + s0 * 0x187E) >> 14;
        z3 = (s1 * 0x2D41) >> 14;

        z11 = t7 + z3;
        z13 = t7 - z3;

        out[8] = z11 + z4;
        out[56] = z11 - z4;
        out[24] = z13 - z2;
        out[40] = z13 + z2;

        in += stride;
        out++;
    }

    s = tmp;
    d = dst;
    for (i = 0; i < 8; i++) {
        t0 = s[0] + s[7];
        t7 = s[0] - s[7];
        t1 = s[1] + s[6];
        t6 = s[1] - s[6];
        t2 = s[2] + s[5];
        t5 = s[2] - s[5];
        t3 = s[3] + s[4];
        t4 = s[3] - s[4];

        s2 = t6 + t7;
        s0 = t4 + t5;
        s1 = t5 + t6;

        t13 = t0 - t3;
        t12 = t1 - t2;
        t10 = t0 + t3;
        t11 = t1 + t2;

        z1 = ((t13 + t12) * 0x2D41) >> 14;
        d[0] = t10 + t11;
        d[32] = t10 - t11;
        d[16] = t13 + z1;
        d[48] = t13 - z1;

        z5 = (s0 - s2) * 0x187E;
        z2 = (s0 * 0x22A3 + z5) >> 14;
        z4 = (s2 * 0x539F + z5) >> 14;
        z3 = (s1 * 0x2D41) >> 14;

        z13 = t7 - z3;
        z11 = t7 + z3;

        d[40] = z13 + z2;
        d[24] = z13 - z2;
        d[8] = z11 + z4;
        d[56] = z11 - z4;

        s += 8;
        d++;
    }
}

int fn_8026CE80(int* block, TMCC_CompInfo* comp) {
    TMCC_CompHuff* huff;
    int i, run;
    int* qtbl;
    int* zz;
    int dc, val, diff, temp, nbits, totalBits;

    dc = block[0];
    zz = comp->unk18 + 1;
    qtbl = comp->unk14;
    huff = comp->unk10;
    if (dc < 0) {
        dc = -dc;
        val = -((qtbl[0] * dc + 0x8000) >> 16);
    } else {
        val = (qtbl[0] * dc + 0x8000) >> 16;
    }

    diff = val - comp->dcPred;
    comp->dcPred = val;
    if (diff < 0) {
        diff = -diff;
    }
    nbits = 0;
    while (diff > 0) {
        diff >>= 1;
        nbits++;
    }
    if (nbits >= huff->dcMax) {
        return -2;
    }
    totalBits = huff->dcBits[nbits];
    totalBits += nbits;

    run = 0;
    for (i = 1; i < 64; i++) {
        int idx = *zz++;
        int ac = block[idx];
        int q = qtbl[idx];
        if (ac <= 0) {
            val = -((-ac * q + 0x8000) >> 16);
        } else {
            val = (ac * q + 0x8000) >> 16;
        }
        if (val == 0) {
            run++;
        } else {
            while (run > 15) {
                totalBits += huff->acBits[0xF0];
                run -= 16;
            }
            temp = val;
            if (val < 0) {
                temp = -val;
            }
            nbits = 0;
            do {
                nbits++;
            } while ((temp >> nbits) != 0);
            if (nbits >= huff->acMax) {
                return -2;
            }
            totalBits += huff->acBits[(run << 4) + nbits] + nbits;
            run = 0;
        }
    }

    if (run > 0) {
        totalBits += huff->acBits[0];
    }
    comp->unk1C->total += totalBits;
    return 0;
}

int fn_8026A3B8(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = &enc->outStream;
    int* qChr = enc->qChr;
    const int* zz = enc->zigzag;
    int* qLum;
    int i;

    TMCC_JPEGENC_PutWord(bs, 0xFFD8);
    TMCC_JPEGENC_PutWord(bs, 0xFFDB);
    TMCC_JPEGENC_PutWord(bs, 0x43);
    fn_802692BC(bs, 0);
    qLum = enc->qLum;
    for (i = 0; i < 64; i++) {
        fn_802692BC(bs, qLum[zz[i]]);
    }

    TMCC_JPEGENC_PutWord(bs, 0xFFDB);
    TMCC_JPEGENC_PutWord(bs, 0x43);
    fn_802692BC(bs, 1);
    for (i = 0; i < 64; i++) {
        fn_802692BC(bs, qChr[zz[i]]);
    }

    TMCC_JPEGENC_PutWord(bs, 0xFFC4);
    TMCC_JPEGENC_PutWord(bs, 0x1F);
    fn_802692BC(bs, 0);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[0], 16);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[1], 12);

    TMCC_JPEGENC_PutWord(bs, 0xFFC4);
    TMCC_JPEGENC_PutWord(bs, 0x1F);
    fn_802692BC(bs, 1);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[2], 16);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[3], 12);

    TMCC_JPEGENC_PutWord(bs, 0xFFC4);
    TMCC_JPEGENC_PutWord(bs, 0xB5);
    fn_802692BC(bs, 0x10);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[4], 16);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[5], 0xA2);

    TMCC_JPEGENC_PutWord(bs, 0xFFC4);
    TMCC_JPEGENC_PutWord(bs, 0xB5);
    fn_802692BC(bs, 0x11);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[6], 16);
    TMCC_JPEGENC_PutString(bs, enc->huffSrc[7], 0xA2);

    if (enc->unk10 != 0) {
        TMCC_JPEGENC_PutWord(bs, 0xFFDD);
        TMCC_JPEGENC_PutWord(bs, 4);
        TMCC_JPEGENC_PutWord(bs, enc->unk10);
    }

    if (fn_8026A28C(enc) < 0) {
        return -2;
    }
    return 0;
}

int fn_802698AC(int* block, TMCC_CompInfo* comp) {
    TMCC_CompHuff* huff;
    int val, nbits, i, run;
    int* qtbl;
    TMCC_Bitstream* bs;
    int* zz;
    TMCC_PutFn putFn;
    u16 masks[17] = {
        0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F,
        0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF,
        0xFFFF
    };
    int dcVal, diff, temp, dcBits;
    int freeBits, buf;

    zz = comp->unk18 + 1;
    bs = comp->unk1C;
    TMCC_JPEGENC_CheckPutStream((TMCC_Encoder*)comp);
    putFn = comp->putFn;
    qtbl = comp->unk14;
    huff = comp->unk10;

    dcVal = block[0];
    if (dcVal < 0) {
        dcVal = -dcVal;
        dcVal = -((qtbl[0] * dcVal + 0x8000) >> 16);
    } else {
        dcVal = (qtbl[0] * dcVal + 0x8000) >> 16;
    }

    diff = dcVal - comp->dcPred;
    comp->dcPred = dcVal;
    temp = diff;
    if (diff < 0) {
        temp = -diff;
        diff--;
    }
    dcBits = 0;
    while (temp > 0) {
        temp >>= 1;
        dcBits++;
    }
    if (dcBits >= huff->dcMax) {
        return -2;
    }
    freeBits = 32;
    buf = huff->dcCode[dcBits];
    freeBits -= huff->dcBits[dcBits];
    buf <<= dcBits;
    freeBits -= dcBits;
    buf |= diff & masks[dcBits];
    if (freeBits < 16) {
        putFn(bs, buf, 32 - freeBits);
        buf = 0;
        freeBits = 32;
    }

    run = 0;
    for (i = 1; i < 64; i++) {
        int idx = *zz++;
        int ac = block[idx];
        int q = qtbl[idx];
        if (ac <= 0) {
            val = -((-ac * q + 0x8000) >> 16);
        } else {
            val = (ac * q + 0x8000) >> 16;
        }
        if (val == 0) {
            run++;
        } else {
            while (run > 15) {
                run -= 16;
                freeBits -= huff->acBits[0xF0];
                buf <<= huff->acBits[0xF0];
                buf += huff->acCode[0xF0];
                if (freeBits < 16) {
                    putFn(bs, buf, 32 - freeBits);
                    buf = 0;
                    freeBits = 32;
                }
            }
            temp = val;
            if (val < 0) {
                temp = -val;
                val--;
            }
            nbits = 0;
            do {
                nbits++;
            } while ((temp >> nbits) != 0);
            if (nbits >= huff->acMax) {
                return -2;
            }
            {
                int codeIdx = (run << 4) + nbits;
                run = 0;
                freeBits -= huff->acBits[codeIdx];
                buf <<= huff->acBits[codeIdx];
                buf += huff->acCode[codeIdx];
                if (freeBits < 16) {
                    putFn(bs, buf, 32 - freeBits);
                    buf = 0;
                    freeBits = 32;
                }
            }
            freeBits -= nbits;
            buf = (buf << nbits) | (val & masks[nbits]);
            if (freeBits < 16) {
                putFn(bs, buf, 32 - freeBits);
                buf = 0;
                freeBits = 32;
            }
        }
    }

    if (run > 0) {
        buf <<= huff->acBits[0];
        buf += huff->acCode[0];
        freeBits -= huff->acBits[0];
        putFn(bs, buf, 32 - freeBits);
    } else if (freeBits != 32) {
        putFn(bs, buf, 32 - freeBits);
    }
    return 0;
}

int fn_8026CAE0(TMCC_UsrParam* usr, u8* pixels, TMCC_InitParam* param) {
    u32 target = (u32)(0.9 * (double)param->unk18);
    u32 size200, size50, size100, size1, size500;
    int mcuCount;
    u32 q;

    param->unk14 = 100;
    mcuCount = fn_8026C7D4(usr, (int)pixels, param);
    if (mcuCount >= 0) {
        if (fn_8026C930(usr, mcuCount, pixels, 5) < 0) {
            return -2;
        }
        size100 = usr->unk10 * 5;
    } else {
        return -2;
    }

    if (size100 > target) {
        param->unk14 = 50;
        mcuCount = fn_8026C7D4(usr, (int)pixels, param);
        if (mcuCount >= 0) {
            if (fn_8026C930(usr, mcuCount, pixels, 5) < 0) {
                return -2;
            }
            size50 = usr->unk10 * 5;
        } else {
            return -2;
        }

        if (size50 > target) {
            param->unk14 = 1;
            mcuCount = fn_8026C7D4(usr, (int)pixels, param);
            if (mcuCount >= 0) {
                if (fn_8026C930(usr, mcuCount, pixels, 5) < 0) {
                    return -2;
                }
                size1 = usr->unk10 * 5;
            } else {
                return -2;
            }

            if (size1 >= target) {
                param->unk14 = 1;
            } else {
                q = (target - size1) / ((size50 - size1 + 49) / 50U);
                param->unk14 = q;
                if (q > 49) {
                    param->unk14 = 49;
                }
            }
        } else if (size50 == target) {
            param->unk14 = 50;
        } else {
            q = (target - size50) / ((size100 - size50 + 49) / 50U) + 50;
            param->unk14 = q;
            if (q > 99) {
                param->unk14 = 99;
            }
        }
    } else {
        param->unk14 = 200;
        mcuCount = fn_8026C7D4(usr, (int)pixels, param);
        if (mcuCount >= 0) {
            if (fn_8026C930(usr, mcuCount, pixels, 5) < 0) {
                return -2;
            }
            size200 = usr->unk10 * 5;
        } else {
            return -2;
        }

        if (size100 == target) {
            param->unk14 = 100;
        } else if (size200 > target) {
            q = (target - size100) / ((size200 - size100 + 99) / 100U) + 100;
            param->unk14 = q;
            if (q > 199) {
                param->unk14 = 199;
            }
        } else {
            param->unk14 = 500;
            mcuCount = fn_8026C7D4(usr, (int)pixels, param);
            if (mcuCount >= 0) {
                if (fn_8026C930(usr, mcuCount, pixels, 5) < 0) {
                    return -2;
                }
                size500 = usr->unk10 * 5;
            } else {
                return -2;
            }

            if (size200 == target) {
                param->unk14 = 200;
            } else if (size500 > target) {
                q = (target - size200) / ((size500 - size200 + 299) / 300U) + 200;
                param->unk14 = q;
                if (q > 499) {
                    param->unk14 = 499;
                }
            } else {
                param->unk14 = 500;
            }
        }
    }

    if ((u32)param->unk14 < 1) {
        param->unk14 = 1;
    } else if ((u32)param->unk14 > 500) {
        param->unk14 = 500;
    }
    return param->unk14;
}

static const int sZigZag[64] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};
static const int sScaleTbl1[64] = {
    16384, 11812, 12540, 13933, 16384, 20853, 30274, 59384,
    11812, 8516, 9041, 10045, 11812, 15034, 21826, 42813,
    12540, 9041, 9598, 10664, 12540, 15960, 23170, 45451,
    13933, 10045, 10664, 11849, 13933, 17734, 25746, 50502,
    16384, 11812, 12540, 13933, 16384, 20853, 30274, 59384,
    20853, 15034, 15960, 17734, 20853, 26541, 38531, 75581,
    30274, 21826, 23170, 25746, 30274, 38531, 55938, 109727,
    59384, 42813, 45451, 50502, 59384, 75581, 109727, 215238
};
static const int sScaleTbl2[64] = {
    8192, 5906, 6270, 6967, 8192, 10426, 15137, 29692,
    5906, 4258, 4520, 5023, 5906, 7517, 10913, 21407,
    6270, 4520, 4799, 5332, 6270, 7980, 11585, 22725,
    6967, 5023, 5332, 5925, 6967, 8867, 12873, 25251,
    8192, 5906, 6270, 6967, 8192, 10426, 15137, 29692,
    10426, 7517, 7980, 8867, 10426, 13270, 19266, 37791,
    15137, 10913, 11585, 12873, 15137, 19266, 27969, 54864,
    29692, 21407, 22725, 25251, 29692, 37791, 54864, 107619
};
static const int sDefaultLumRecip[64] = {
    512, 536, 627, 435, 341, 260, 296, 486,
    492, 354, 322, 264, 227, 129, 181, 389,
    447, 347, 299, 222, 156, 140, 167, 405,
    497, 295, 242, 204, 136, 101, 160, 407,
    455, 268, 169, 124, 120, 95, 146, 385,
    434, 214, 145, 138, 128, 127, 170, 410,
    308, 170, 148, 147, 146, 159, 233, 542,
    412, 232, 239, 257, 265, 377, 532, 1087
};
static const int sDefaultLumQuant[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};
static const int sDefaultChrRecip[64] = {
    481, 328, 261, 148, 82, 105, 152, 299,
    328, 202, 173, 76, 59, 75, 110, 216,
    261, 173, 85, 53, 63, 80, 117, 229,
    148, 76, 53, 59, 70, 89, 130, 255,
    82, 59, 63, 70, 82, 105, 152, 299,
    105, 75, 80, 89, 105, 134, 194, 381,
    152, 110, 117, 130, 152, 194, 282, 554,
    299, 216, 229, 255, 299, 381, 554, 1087
};
static const int sDefaultChrQuant[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

void fn_8026C45C(TMCC_Encoder* enc) {
    int i;
    int* qLum = enc->qLum;
    int* qChr = enc->qChr;
    int* qLumRecip = enc->qLumRecip;
    int* qChrRecip = enc->qChrRecip;
    u8 mode = enc->u1C.unk1C;

    enc->zigzag = sZigZag;
    if (mode < 2) {
        const int* lumTbl = sDefaultLumQuant;
        const int* chrTbl = sDefaultChrQuant;
        const int* scaleTbl = sScaleTbl2;
        u32 scale = enc->u20.unk20;
        for (i = 0; i < 64; i++) {
            int q1 = (lumTbl[i] * 50) / scale;
            int q2 = (chrTbl[i] * 50) / scale;
            qLum[i] = ((q1 < 256 && q1 > 0) != 0) ? q1 : ((q1 > 0) ? 255 : 1);
            qChr[i] = ((q2 < 256 && q2 > 0) != 0) ? q2 : ((q2 > 0) ? 255 : 1);
            qLumRecip[i] = scaleTbl[i] / qLum[i];
            qChrRecip[i] = scaleTbl[i] / qChr[i];
        }
    } else if (mode == 2) {
        const int* scaleTbl = sScaleTbl2;
        for (i = 0; i < 64; i++) {
            qLumRecip[i] = scaleTbl[i] / qLum[i];
            qChrRecip[i] = scaleTbl[i] / qChr[i];
        }
    } else {
        const int* lumTbl = sDefaultLumQuant;
        const int* chrTbl = sDefaultChrQuant;
        const int* lumRecipTbl = sDefaultLumRecip;
        const int* chrRecipTbl = sDefaultChrRecip;
        for (i = 0; i < 64; i++) {
            qLum[i] = lumTbl[i];
            qChr[i] = chrTbl[i];
            qLumRecip[i] = lumRecipTbl[i];
            qChrRecip[i] = chrRecipTbl[i];
        }
    }
}

void fn_8026C0AC(TMCC_Encoder* enc) {
    int huffSize[256];
    int huffCode[256];
    const u8* bits;
    const u8* vals;
    struct {
        int unk0;
        void* unk4;
        void* unk8;
    }* tbl;
    int tblIdx;
    int i, j, k;
    u8 lastK;
    u16 code;
    u8 si;

    for (tblIdx = 0; tblIdx < 4; tblIdx++) {
        switch (tblIdx) {
        case 0:
            bits = sDefaultDcLumBits;
            vals = sDefaultDcLumVals;
            tbl = (void*)&enc->huffTbl[0];
            break;
        case 1:
            bits = sDefaultDcChrBits;
            vals = sDefaultDcChrVals;
            tbl = (void*)&enc->huffTbl[2];
            break;
        case 2:
            bits = sDefaultAcLumBits;
            vals = sDefaultAcLumVals;
            tbl = (void*)&enc->huffTbl[1];
            break;
        case 3:
            bits = sDefaultAcChrBits;
            vals = sDefaultAcChrVals;
            tbl = (void*)&enc->huffTbl[3];
            break;
        }

        k = 0;
        i = 1;
        j = 1;
        while (1) {
            if (j > bits[i]) {
                i++;
                j = 1;
                if (i > 16) {
                    huffSize[k] = 0;
                    lastK = (u8)k;
                    break;
                }
            } else {
                huffSize[k] = i + 1;
                k++;
                j++;
            }
        }

        code = 0;
        si = (u8)huffSize[0];
        k = 0;
        while (1) {
            do {
                huffCode[k] = code;
                code++;
                k++;
            } while (huffSize[k] == si);
            if (huffSize[k] == 0) {
                break;
            }
            do {
                code <<= 1;
                si++;
            } while (huffSize[k] != si);
        }

        for (k = 0; k < lastK; k++) {
            ((int*)tbl->unk4)[vals[k]] = huffCode[k];
            ((int*)tbl->unk8)[vals[k]] = huffSize[k];
        }
        tbl->unk0 = lastK;
    }
}

void fn_8026A7D0(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs;
    u16 wY, hY, wCb, hCb, wCr, hCr;
    int* qLum;
    int* qChr;
    const int* zz;
    int minQ, i, q;
    int remW, remH;

    bs = &enc->outStream;
    qLum = enc->qLumRecip;
    qChr = enc->qChrRecip;
    enc->restartCount = 0;
    zz = enc->zigzag;
    enc->unk14 = enc->unk10;
    wY = enc->unk2;
    hY = enc->unk4;
    wCb = enc->unk6;
    hCb = enc->unk8;
    wCr = enc->unkA;
    hCr = enc->unkC;

    switch (enc->format) {
    case 0:
        enc->img.width = (wY + 31) >> 5;
        enc->img.height = (hY + 7) >> 3;
        enc->img.unk4 = 0;
        enc->img.unk6 = 0;
        remW = wY % 32;
        remH = hY % 8;
        enc->img.unk8 = remW;
        enc->img.unk9 = remH;
        enc->img.yW = wY;
        enc->img.yH = hY;
        enc->img.yEdgeW = remW ? remW : 32;
        enc->img.yEdgeH = remH ? remH : 8;
        enc->img.unk14 = 32;
        enc->img.unk15 = 8;
        enc->img.yBlocks = 4;
        enc->img.yFmt = 0;
        enc->img.compY.stride = 32;
        enc->img.compY.dcPred = 0;
        minQ = 0x10000;
        for (i = 0; i < 64; i++) {
            q = 0x10000 / qLum[i];
            if (minQ > q) minQ = q;
        }
        enc->img.compY.unkC = minQ;
        enc->img.compY.unk10 = (TMCC_CompHuff*)&enc->huffTbl[0];
        enc->img.compY.unk14 = qLum;
        enc->img.compY.unk18 = (int*)zz;
        enc->img.compY.unk1C = bs;

        enc->img.compY.nextW = wCb;
        enc->img.compY.nextH = hCb;
        enc->img.compY.nextEdgeW = (wCb % 8) ? (wCb % 8) : 8;
        enc->img.compY.nextEdgeH = (hCb % 8) ? (hCb % 8) : 8;
        enc->img.compY.unk2C = 8;
        enc->img.compY.unk2D = 8;
        enc->img.compY.nextBlocks = 1;
        enc->img.compY.nextFmt = 0;
        enc->img.compCb.stride = 8;
        enc->img.compCb.dcPred = 0;
        minQ = 0x10000;
        for (i = 0; i < 64; i++) {
            q = 0x10000 / qChr[i];
            if (minQ > q) minQ = q;
        }
        enc->img.compCb.unkC = minQ;
        enc->img.compCb.unk10 = (TMCC_CompHuff*)&enc->huffTbl[2];
        enc->img.compCb.unk14 = qChr;
        enc->img.compCb.unk18 = (int*)zz;
        enc->img.compCb.unk1C = bs;

        enc->img.compCb.nextW = wCr;
        enc->img.compCb.nextH = hCr;
        enc->img.compCb.nextEdgeW = (wCr % 8) ? (wCr % 8) : 8;
        enc->img.compCb.nextEdgeH = (hCr % 8) ? (hCr % 8) : 8;
        enc->img.compCb.unk2C = 8;
        enc->img.compCb.unk2D = 8;
        enc->img.compCb.nextBlocks = 1;
        enc->img.compCb.nextFmt = 0;
        enc->img.crStride = 8;
        enc->img.crDcPred = 0;
        enc->img.crUnkC = enc->img.compCb.unkC;
        enc->img.crUnk10 = (TMCC_CompHuff*)&enc->huffTbl[2];
        enc->img.crUnk14 = qChr;
        enc->img.crUnk18 = zz;
        enc->img.crUnk1C = bs;
        break;
    case 2:
        enc->img.width = (wY + 15) >> 4;
        enc->img.height = (hY + 15) >> 4;
        enc->img.unk4 = 0;
        enc->img.unk6 = 0;
        remW = wY % 16;
        remH = hY % 16;
        enc->img.unk8 = remW;
        enc->img.unk9 = remH;
        enc->img.yW = wY;
        enc->img.yH = hY;
        enc->img.yEdgeW = remW ? remW : 16;
        enc->img.yEdgeH = remH ? remH : 16;
        enc->img.unk14 = 16;
        enc->img.unk15 = 16;
        enc->img.yBlocks = 4;
        enc->img.yFmt = 2;
        enc->img.compY.stride = 16;
        enc->img.compY.dcPred = 0;
        minQ = 0x10000;
        for (i = 0; i < 64; i++) {
            q = 0x10000 / qLum[i];
            if (minQ > q) minQ = q;
        }
        enc->img.compY.unkC = minQ;
        enc->img.compY.unk10 = (TMCC_CompHuff*)&enc->huffTbl[0];
        enc->img.compY.unk14 = qLum;
        enc->img.compY.unk18 = (int*)zz;
        enc->img.compY.unk1C = bs;

        enc->img.compY.nextW = wCb;
        enc->img.compY.nextH = hCb;
        enc->img.compY.nextEdgeW = (wCb % 8) ? (wCb % 8) : 8;
        enc->img.compY.nextEdgeH = (hCb % 8) ? (hCb % 8) : 8;
        enc->img.compY.unk2C = 8;
        enc->img.compY.unk2D = 8;
        enc->img.compY.nextBlocks = 1;
        enc->img.compY.nextFmt = 2;
        enc->img.compCb.stride = 8;
        enc->img.compCb.dcPred = 0;
        minQ = 0x10000;
        for (i = 0; i < 64; i++) {
            q = 0x10000 / qChr[i];
            if (minQ > q) minQ = q;
        }
        enc->img.compCb.unkC = minQ;
        enc->img.compCb.unk10 = (TMCC_CompHuff*)&enc->huffTbl[2];
        enc->img.compCb.unk14 = qChr;
        enc->img.compCb.unk18 = (int*)zz;
        enc->img.compCb.unk1C = bs;

        enc->img.compCb.nextW = wCr;
        enc->img.compCb.nextH = hCr;
        enc->img.compCb.nextEdgeW = (wCr % 8) ? (wCr % 8) : 8;
        enc->img.compCb.nextEdgeH = (hCr % 8) ? (hCr % 8) : 8;
        enc->img.compCb.unk2C = 8;
        enc->img.compCb.unk2D = 8;
        enc->img.compCb.nextBlocks = 1;
        enc->img.compCb.nextFmt = 2;
        enc->img.crStride = 8;
        enc->img.crDcPred = 0;
        enc->img.crUnkC = enc->img.compCb.unkC;
        enc->img.crUnk10 = (TMCC_CompHuff*)&enc->huffTbl[2];
        enc->img.crUnk14 = qChr;
        enc->img.crUnk18 = zz;
        enc->img.crUnk1C = bs;
        break;
    case 1:
        enc->img.width = (wY + 15) >> 4;
        enc->img.height = (hY + 7) >> 3;
        enc->img.unk4 = 0;
        enc->img.unk6 = 0;
        remW = wY % 16;
        remH = hY % 8;
        enc->img.unk8 = remW;
        enc->img.unk9 = remH;
        enc->img.yW = wY;
        enc->img.yH = hY;
        enc->img.yEdgeW = remW ? remW : 16;
        enc->img.yEdgeH = remH ? remH : 8;
        enc->img.unk14 = 16;
        enc->img.unk15 = 8;
        enc->img.yBlocks = 2;
        enc->img.yFmt = 1;
        enc->img.compY.stride = 16;
        enc->img.compY.dcPred = 0;
        minQ = 0x10000;
        for (i = 0; i < 64; i++) {
            q = 0x10000 / qLum[i];
            if (minQ > q) minQ = q;
        }
        enc->img.compY.unkC = minQ;
        enc->img.compY.unk10 = (TMCC_CompHuff*)&enc->huffTbl[0];
        enc->img.compY.unk14 = qLum;
        enc->img.compY.unk18 = (int*)zz;
        enc->img.compY.unk1C = bs;

        enc->img.compY.nextW = wCb;
        enc->img.compY.nextH = hCb;
        enc->img.compY.nextEdgeW = (wCb % 8) ? (wCb % 8) : 8;
        enc->img.compY.nextEdgeH = (hCb % 8) ? (hCb % 8) : 8;
        enc->img.compY.unk2C = 8;
        enc->img.compY.unk2D = 8;
        enc->img.compY.nextBlocks = 1;
        enc->img.compY.nextFmt = 1;
        enc->img.compCb.stride = 8;
        enc->img.compCb.dcPred = 0;
        minQ = 0x10000;
        for (i = 0; i < 64; i++) {
            q = 0x10000 / qChr[i];
            if (minQ > q) minQ = q;
        }
        enc->img.compCb.unkC = minQ;
        enc->img.compCb.unk10 = (TMCC_CompHuff*)&enc->huffTbl[2];
        enc->img.compCb.unk14 = qChr;
        enc->img.compCb.unk18 = (int*)zz;
        enc->img.compCb.unk1C = bs;

        enc->img.compCb.nextW = wCr;
        enc->img.compCb.nextH = hCr;
        enc->img.compCb.nextEdgeW = (wCr % 8) ? (wCr % 8) : 8;
        enc->img.compCb.nextEdgeH = (hCr % 8) ? (hCr % 8) : 8;
        enc->img.compCb.unk2C = 8;
        enc->img.compCb.unk2D = 8;
        enc->img.compCb.nextBlocks = 1;
        enc->img.compCb.nextFmt = 1;
        enc->img.crStride = 8;
        enc->img.crDcPred = 0;
        enc->img.crUnkC = enc->img.compCb.unkC;
        enc->img.crUnk10 = (TMCC_CompHuff*)&enc->huffTbl[2];
        enc->img.crUnk14 = qChr;
        enc->img.crUnk18 = zz;
        enc->img.crUnk1C = bs;
        break;
    }
}
