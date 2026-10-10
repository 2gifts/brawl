#include "revolution.h"
#include "revolution/thp.h"
#include <cstring>

// THP (movie) jpeg frame decoder. the original source is full of hand-scheduled paired-single asm,
// this is written to follow the dol closely rather than to be pretty.
// HYPOTHESIS: all the THP_* constants below are reverse engineered from immediates in the asm,
// the original names live in a private header i don't have.

#define forever for (;;)

#define THP_OK 0
#define THP_ZERO 0
#define THP_DCTWIDTH 8
#define THP_DCTSIZE 64
#define THP_NUM_COMPONENTS 3
#define THP_MAX_BLOCKS_IN_MCU 6
#define THP_MAX_HUFFMAN_TABS 2
#define THP_MAX_HUFFMAN_CODES 256
#define THP_MAX_QUANT_TABS 3
#define THP_DC_TABLE 0
#define THP_AC_TABLE 1

#define THP_MCU_Y_OUT 0
#define THP_MCU_U_OUT 1
#define THP_MCU_V_OUT 2
#define THP_MCU_YUV_LAST_BUFFER 3

#define THP_MCU512_Y_SIZE 0x2000
#define THP_MCU512_UV_SIZE 0x800
#define THP_MCU640_Y_SIZE 0x2800
#define THP_MCU640_UV_SIZE 0xA00
#define THP_MCU672_Y_SIZE 0x2A00
#define THP_MCU672_UV_SIZE 0xA80
#define THP_MCUN_Y_SIZE(x) (((x) >> 4) << 8)
#define THP_MCUN_UV_SIZE(x) (((x) >> 4) << 6)

// jpeg markers
#define THP_M_MARKER 0xff
#define THP_M_SOF0 0xc0
#define THP_M_DHT 0xc4
#define THP_M_SOI 0xd8
#define THP_M_SOS 0xda
#define THP_M_DQT 0xdb
#define THP_M_DRI 0xdd
#define THP_M_APP0 0xe0
#define THP_M_APP15 0xef
#define THP_M_COM 0xfe

#define THP_READ_UBYTE(p) (*(p)++)
#define THP_SKIP_UBYTE(p) ((p)++)

// error codes (values are guesses, only THP_OK = 0 is certain)
#define THP_NO_INPUT_FILE 0x19
#define THP_NO_OUTPUT_BUFFER 0x1b
#define THP_NO_WORK_AREA 0x1a
#define THP_UNSUPPORTED_MARKER 0xb
#define THP_RESOURCE_NOT_FOUND 5
#define THP_NO_MEMORY 6
#define THP_BAD_SYNTAX 3
#define THP_LC_NOT_ENABLED 0x1c
#define THP_NOT_INITIALIZED 0x1d
#define THP_UNSUPPORTED_PRECISION 0xa
#define THP_UNSUPPORTED_NUM_COMP 0xc
#define THP_UNSUPPORTED_COMPS 0x13
#define THP_BAD_SCAN_HEADER 0xe
#define THP_INVALID_HUFFTAB 0xf
#define THP_UNSUPPORTED_QUANTIZATION 15
#define THP_UNSUPPORTED_NUM_HUFF 16

#define THP_GQR_U8_DIV8_DEF 0x3D04

static const char *__THPVersion = "<< RVL_SDK - THP \trelease build: May  8 2007 12:30:45 (0x4199_60831) >>";

#define HID2_LCE 0x10000000
#define OS_GQR_S16 0x0007

#define LCGetBase() ((u8 *)0xE0000000)
#define THP_JDIV_ROUNDUP(a, b) (((a) + (b) - 1) / (b))


#ifdef _DEBUG
#define DEBUGSET(a) if (_DEBUG) a
#else
#define DEBUGSET(a)
#endif

static THPHuffmanTab   *Ydchuff ATTRIBUTE_ALIGN(32);
static THPHuffmanTab   *Udchuff ATTRIBUTE_ALIGN(32);
static THPHuffmanTab   *Vdchuff ATTRIBUTE_ALIGN(32);

static THPHuffmanTab   *Yachuff ATTRIBUTE_ALIGN(32);
static THPHuffmanTab   *Uachuff ATTRIBUTE_ALIGN(32);
static THPHuffmanTab   *Vachuff ATTRIBUTE_ALIGN(32);

static f32              __THPIDCTWorkspace[THP_DCTSIZE] ATTRIBUTE_ALIGN(32);

static u8              *__THPHuffmanBits;
static u8              *__THPHuffmanSizeTab;
static u16             *__THPHuffmanCodeTab;
#ifdef _DEBUG
static s32              __THPHuffmanLastK;
#endif

static THPSample       *Gbase ATTRIBUTE_ALIGN(32);
static u32              Gwid  ATTRIBUTE_ALIGN(32);
static f32             *Gq    ATTRIBUTE_ALIGN(32);

static u8              *__THPLCWork512[THP_MCU_YUV_LAST_BUFFER];
static u8              *__THPLCWork672[THP_MCU_YUV_LAST_BUFFER];

static u32              __THPOldGQR5;
static u32              __THPOldGQR6;

static u8              *__THPWorkArea;
static THPCoeff        *__THPMCUBuffer[THP_MAX_BLOCKS_IN_MCU];
static THPFileInfo     *__THPInfo;

static BOOL             __THPInitFlag = FALSE;

#ifdef _DEBUG
static u32 TABSTATE;

#define DONE                         0
#define FAILEDCHECKENOUGHBITS        1
#define NOTENOUGHBITS                2
#define FAILEDCHECKNOBITS0           3
#define FAILEDCHECKNOBITS1           4
#define ONEBITLEFT                   5
#define READ4                        6
#define GETFULLWORD                  7
#define FAILEDCHECKENOUGHBITSUPDATED 8
#endif

#ifdef _DEBUG
static s32
__THPHuffDecodeTab
#else
static inline s32
__THPHuffDecodeTab
#endif
(
    register THPFileInfo     *info,
    register THPHuffmanTab   *h
)
{
    register s32    code;
    register u32    cnt;
    register s32    cb;
    register u32    increment;
    register s32    tmp;

#define cnt4 code
    asm
    {
        lwz     cnt, info->cnt;

        addi    increment, h, 32;
        lwz     cb, info->currByte;
        addi    cnt4, cnt, 4;
        cmpwi   cnt, 28;

        rlwnm   tmp, cb, cnt4, 27, 31;
        bgt     _notEnoughBits;

        lbzx    code, h, tmp;
        lbzx    increment, increment, tmp;
        cmpwi   code, 0xFF;

        beq     _FailedCheckEnoughBits;

        add     cnt, cnt, increment;
        stw     cnt, info->cnt;
    }
  _done:
    DEBUGSET(TABSTATE = DONE);
    ASSERT (info->cnt <=33);
    return code;

    {
        register u32 maxcodebase;

        register u32 tmp2;

  _FailedCheckEnoughBits:
        maxcodebase = (u32)&(h->maxCode);
        cnt += 5;

        asm
        {
            li          tmp2, sizeof(s32)*(5);
            li          code, 5;
            add         maxcodebase, maxcodebase, tmp2;
          __WHILE_START:
            cmpwi       cnt, 33;
            slwi        tmp, tmp, 1

            beq         _FCEB_faster;
            rlwnm       increment, cb, cnt, 31, 31;
            lwzu        tmp2, 4(maxcodebase);
            or          tmp, tmp, increment
            addi        cnt, cnt, 1;
            b __WHILE_CHECK;

          _FCEB_faster:

            lwz     increment, info->c;
            li      cnt, 1;
            lwzu    cb, 4(increment);

            lwzu    tmp2, 4(maxcodebase);

            stw     increment, info->c;
            rlwimi  tmp, cb, 1,31,31;
            stw     cb, info->currByte;
            b __FL_WHILE_CHECK;

          __FL_WHILE_START:
            slwi    tmp, tmp, 1;

            rlwnm   increment, cb, cnt, 31, 31;
            lwzu    tmp2, 4(maxcodebase);
            or      tmp, tmp, increment;

          __FL_WHILE_CHECK:
            cmpw    tmp,tmp2
            addi    cnt, cnt, 1;
            addi        code, code, 1
            bgt     __FL_WHILE_START;
            b _FCEB_Done;

          __WHILE_CHECK:
            cmpw    tmp,tmp2
            addi        code, code, 1
            bgt     __WHILE_START;
        }
    }
  _FCEB_Done:
    info->cnt = cnt;
    DEBUGSET(TABSTATE = FAILEDCHECKENOUGHBITS);

    return (h->Vij[(s32)(tmp + h->valPtr[code])]);

    asm
    {
      _notEnoughBits:
        cmpwi   cnt, 33;
        lwz     tmp, info->c;
        beq     _getfullword;

        cmpwi   cnt, 32;
        rlwnm   code, cb, cnt4, 27, 31
        beq     _1bitleft;

        lbzx    tmp, h, code;
        lbzx    increment, increment, code;

        cmpwi   tmp, 0xFF;
        add     code, cnt, increment;
        beq _FailedCheckNoBits0;

        cmpwi   code, 33;
        stw     code, info->cnt;
        bgt     _FailedCheckNoBits1;

    }
    ASSERT (info->cnt <=33);
    DEBUGSET(TABSTATE = NOTENOUGHBITS);
    return tmp;

    asm
    {
      _1bitleft:
        lwzu    cb, 4(tmp);

        stw     tmp, info->c;
        rlwimi  code, cb, 4, 28, 31;
        lbzx    tmp, h, code;
        lbzx    increment, increment, code
        stw     cb, info->currByte;
        cmpwi   tmp, 0xFF
        stw     increment, info->cnt;
        beq     _DammitRead4;

    }
    ASSERT (info->cnt <=33);
    DEBUGSET(TABSTATE = ONEBITLEFT);
    return tmp;

  _DammitRead4:

    {
        register u32 maxcodebase = (u32)&(h->maxCode);
        register u32 tmp2;

        asm
        {
            li      cnt, sizeof(s32)*5;
            add     maxcodebase, maxcodebase, cnt;

            slwi    tmp, code, 32-5;
            li      cnt,5;
            rlwimi  tmp, cb, 32-1, 1,31;

          __DR4_WHILE_START:

            subfic  cb, cnt, 31;
            lwzu    tmp2, 4(maxcodebase);
            srw     code, tmp, cb;
          __DR4_WHILE_CHECK:
            cmpw    code, tmp2
            addi    cnt, cnt, 1
            bgt     __DR4_WHILE_START;

        }
    }

    info->cnt = cnt;
    ASSERT (info->cnt <=33);
    DEBUGSET(TABSTATE = READ4);
#ifdef ANALCHECK
    if (cnt > 16)
    {
        return h->Vij1;
    }
#endif
  __CODE_PLUS_VP_CNT:
    return (h->Vij[(s32)(code + h->valPtr[cnt])]);

  _getfullword:
    asm
    {
        lwzu    cb, 4(tmp);

        rlwinm  code, cb, 5, 27, 31
        stw     tmp, info->c;
        lbzx    cnt, h, code;
        lbzx    increment, increment, code;
        cmpwi   cnt, 0xFF
        stw     cb, info->currByte;
        addi    increment, increment, 1
        beq     _FailedCheckEnoughbits_Updated;

        stw     increment, info->cnt;
    }
    DEBUGSET(TABSTATE = GETFULLWORD);
    ASSERT (info->cnt <=33);
    return (s32)cnt;

  _FailedCheckEnoughbits_Updated:

    cnt = 5;
    do
    {

        asm
        {
            subfic  tmp, cnt, 31;
            addi    cnt, cnt, 1;
            srw     code, cb, tmp;
        }
    }     while (code > h->maxCode[cnt]);

    info->cnt = cnt+1;
    ASSERT (info->cnt <=33);
#ifdef ANALCHECK
    if (cnt > 16)
    {
        return h->Vij1;
    }
#endif
    DEBUGSET(TABSTATE = FAILEDCHECKENOUGHBITSUPDATED);
    goto __CODE_PLUS_VP_CNT;

#undef cnt4

  _FailedCheckNoBits0:
    DEBUGSET(TABSTATE = FAILEDCHECKNOBITS0);
  _FailedCheckNoBits1:
    DEBUGSET(TABSTATE = FAILEDCHECKNOBITS1);

    {
        register u32 mask = 0xFFFFFFFF << (33 - cnt);
        register u32 tmp2;

        code = (s32)(cb & (~mask));
        mask = (u32)&(h->maxCode);

        asm
        {
            lwz     tmp, info->c;
            subfic  tmp2, cnt, 33;
            addi    cnt, tmp2, 1;
            slwi    tmp2, tmp2, 2;
            lwzu    cb, 4(tmp);
            add     mask,mask, tmp2;
            stw     tmp, info->c;
            slwi    code, code, 1;
            stw     cb, info->currByte;
            rlwimi  code, cb, 1, 31, 31;
            lwzu    tmp2, 4(mask);
            li      tmp, 2;

            b       __FCNB1_WHILE_CHECK;

          __FCNB1_WHILE_START:
            slwi    code, code, 1;

            addi    cnt, cnt, 1;
            lwzu    tmp2, 4(mask);
            add     code, code, increment;
            addi    tmp, tmp, 1;

          __FCNB1_WHILE_CHECK:
            cmpw    code, tmp2;
            rlwnm   increment, cb, tmp, 31, 31;
            bgt     __FCNB1_WHILE_START;

        }
    }

    info->cnt = (u32)tmp;
    ASSERT (info->cnt <=33);
#ifdef ANALCHECK
    if (cnt > 16)
    {
        return h->Vij1;
    }
#endif
    return (h->Vij[(s32)(code + h->valPtr[cnt])]);

}

#ifndef _DEBUG

static inline void __THPInverseDCTY8(
    register THPCoeff *in,
    register u32       xPos
)
{
    register f32 *q, *ws;

    register f32 tmp0,  tmp1,  tmp2,  tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
    register f32 tmp10, tmp11, tmp12, tmp13;
    register f32 tmp20, tmp21, tmp22, tmp23;

    register f32 cc4    = 1.414213562F;
    register f32 cc2    = 1.847759065F;
    register f32 cc2c6s = 1.082392200F;
    register f32 cc2c6a = -2.613125930F;
    register f32 bias   = 1024.0F;

    q  = Gq;
    ws = &__THPIDCTWorkspace[0] - 2;

    {
        register u32 itmp0, itmp1, itmp2, itmp3;

        asm
        {

            li          itmp2, 8
            mtctr       itmp2

        _loopHead0:

            psq_l       tmp10, 0(in), 0, 5

            psq_l       tmp11, 0(q), 0, 0

            lwz         itmp0, 12(in)
            lwz         itmp3, 8(in)

            ps_mul      tmp10, tmp10, tmp11
            lwz         itmp1, 4(in)
            lhz         itmp2, 2(in)
            or          itmp0, itmp0, itmp3

        _loopHead1:
            cmpwi       itmp0, 0
            bne         _regularIDCT
            ps_merge00  tmp0, tmp10, tmp10
            cmpwi       itmp1, 0
            psq_st      tmp0, 8(ws), 0, 0
            bne         _halfIDCT
            psq_st      tmp0, 16(ws), 0, 0
            cmpwi       itmp2, 0
            psq_st      tmp0, 24(ws), 0, 0
            bne         _quarterIDCT

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            psq_stu     tmp0, 32(ws), 0, 0

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)

            bdnz        _loopHead0
            b           _loopEnd

        _quarterIDCT:

            ps_msub     tmp2, tmp10, cc2, tmp10

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)

            ps_merge00  tmp9, tmp10, tmp10

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            ps_sub      tmp1, cc2, cc2c6s

            lwz         itmp1, 4(in)

            ps_msub     tmp3, tmp10, cc4, tmp2

            lhz         itmp2, 2(in)

            ps_merge11  tmp5, tmp10, tmp2

            psq_l       tmp11, 0(q), 0, 0

            ps_nmsub    tmp4, tmp10, tmp1, tmp3

            ps_add      tmp7, tmp9, tmp5

            psq_l       tmp10, 0(in), 0, 5

            ps_merge11  tmp6, tmp3, tmp4

            ps_sub      tmp5, tmp9, tmp5

            lwz         itmp0, 12(in)

            ps_add      tmp8, tmp9, tmp6

            lwz         itmp3, 8(in)

            ps_sub      tmp6, tmp9, tmp6

            psq_stu     tmp7, 8(ws), 0, 0

            ps_merge10  tmp6, tmp6, tmp6

            psq_stu     tmp8, 8(ws), 0, 0

            ps_merge10  tmp5, tmp5, tmp5

            or          itmp0, itmp0, itmp3

            psq_stu     tmp6, 8(ws), 0, 0

            ps_mul      tmp10, tmp10, tmp11

            psq_stu     tmp5, 8(ws), 0, 0

            bdnz        _loopHead1
            b           _loopEnd

        _halfIDCT:

            psq_l       tmp1, 4(in), 0, 5
            psq_l       tmp9, 8(q), 0, 0

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)
            ps_mul      tmp1, tmp1, tmp9

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            ps_sub      tmp3, tmp10, tmp1

            ps_add      tmp2, tmp10, tmp1

            lwz         itmp0, 12(in)

            ps_madd     tmp4, tmp1, cc4, tmp3

            ps_nmsub    tmp5, tmp1, cc4, tmp2

            ps_mul      tmp8, tmp3, cc2

            ps_merge00  tmp4, tmp2, tmp4

            lwz         itmp3, 8(in)

            ps_nmsub    tmp6, tmp1, cc2c6a, tmp8

            ps_merge00  tmp5, tmp5, tmp3

            lwz         itmp1, 4(in)

            ps_sub      tmp6, tmp6, tmp2

            ps_nmsub    tmp7, tmp10, cc2c6s, tmp8

            lhz         itmp2, 2(in)

            ps_merge11  tmp2, tmp2, tmp6

            ps_msub     tmp8, tmp3, cc4, tmp6

            psq_l       tmp10, 0(in), 0, 5

            ps_add      tmp9, tmp4, tmp2

            ps_sub      tmp7, tmp7, tmp8

            psq_l       tmp11, 0(q), 0, 0

            ps_merge11  tmp3, tmp8, tmp7

            ps_sub      tmp4, tmp4, tmp2

            psq_stu     tmp9, 8(ws), 0, 0

            ps_add      tmp0, tmp5, tmp3

            ps_sub      tmp1, tmp5, tmp3

            or          itmp0, itmp0, itmp3

            psq_stu     tmp0, 8(ws), 0, 0

            ps_merge10  tmp1, tmp1, tmp1

            ps_merge10  tmp4, tmp4, tmp4

            psq_stu     tmp1, 8(ws), 0, 0

            ps_mul      tmp10, tmp10, tmp11

            psq_stu     tmp4, 8(ws), 0, 0

            bdnz        _loopHead1
            b           _loopEnd

        _regularIDCT:

            psq_l       tmp9, 4(in), 0, 5

            psq_l       tmp5, 8(q), 0, 0

            ps_mul      tmp9, tmp9, tmp5

            psq_l       tmp2, 8(in), 0, 5

            psq_l       tmp6, 16(q), 0, 0

            ps_merge01  tmp0, tmp10, tmp9

            psq_l       tmp3, 12(in), 0, 5

            ps_merge01  tmp1, tmp9, tmp10

            psq_l       tmp7, 24(q), 0, 0

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)

            ps_madd     tmp4, tmp2, tmp6, tmp0

            ps_nmsub    tmp5, tmp2, tmp6, tmp0

            ps_madd     tmp6, tmp3, tmp7, tmp1

            ps_nmsub    tmp7, tmp3, tmp7, tmp1

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            ps_add      tmp0, tmp4, tmp6

            ps_sub      tmp3, tmp4, tmp6

            ps_msub     tmp2, tmp7, cc4, tmp6

            lwz         itmp0, 12(in)

            ps_sub      tmp8, tmp7, tmp5

            ps_add      tmp1, tmp5, tmp2

            ps_sub      tmp2, tmp5, tmp2

            ps_mul      tmp8, tmp8, cc2

            lwz         itmp3, 8(in)

            ps_merge00  tmp1, tmp0, tmp1

            ps_nmsub    tmp6, tmp5, cc2c6a, tmp8

            ps_msub     tmp4, tmp7, cc2c6s, tmp8

            lwz         itmp1, 4(in)

            ps_sub      tmp6, tmp6, tmp0

            ps_merge00  tmp2, tmp2, tmp3

            lhz         itmp2, 2(in)

            ps_madd     tmp5, tmp3, cc4, tmp6

            ps_merge11  tmp7, tmp0, tmp6

            psq_l       tmp10, 0(in), 0, 5

            ps_sub      tmp4, tmp4, tmp5

            ps_add      tmp3, tmp1, tmp7

            psq_l       tmp11, 0(q), 0, 0

            ps_merge11  tmp4, tmp5, tmp4

            ps_sub      tmp0, tmp1, tmp7

            ps_mul      tmp10, tmp10, tmp11

            ps_add      tmp5, tmp2, tmp4

            ps_sub      tmp6, tmp2, tmp4

            ps_merge10  tmp5, tmp5, tmp5

            psq_stu     tmp3, 8(ws), 0, 0

            ps_merge10  tmp0, tmp0, tmp0

            psq_stu     tmp6, 8(ws), 0, 0

            psq_stu     tmp5, 8(ws), 0, 0

            or          itmp0, itmp0, itmp3

            psq_stu     tmp0, 8(ws), 0, 0

            bdnz        _loopHead1

        _loopEnd:

        }

    }

    ws = &__THPIDCTWorkspace[0];

    {
        register THPSample *obase = Gbase;
        register u32 wid = Gwid;

        register u32 itmp0, off0, off1;
        register THPSample *out0, *out1;

        asm
        {

            psq_l       tmp10, THP_DCTWIDTH*0*sizeof(f32)(ws), 0, 0

            slwi off0, wid, 3;

            psq_l       tmp11, THP_DCTWIDTH*4*sizeof(f32)(ws), 0, 0

            slwi        xPos, xPos, 2

            psq_l       tmp12, THP_DCTWIDTH*2*sizeof(f32)(ws), 0, 0

            slwi        off1, wid, 2

            ps_add      tmp6, tmp10, tmp11

            add         off0, off0, xPos

            psq_l       tmp13, THP_DCTWIDTH*6*sizeof(f32)(ws), 0, 0

            ps_sub      tmp8, tmp10, tmp11

            add         off1, off0, off1

            ps_add      tmp6, tmp6, bias

            li          itmp0, 3

            ps_add      tmp7, tmp12, tmp13

            add         out0, obase, off0

            ps_sub      tmp9, tmp12, tmp13

            ps_add      tmp0, tmp6, tmp7

            add         out1, obase, off1

            ps_add      tmp8, tmp8, bias

            mtctr       itmp0

        _loopHead10:

            psq_l       tmp4, THP_DCTWIDTH*1*sizeof(f32)(ws), 0, 0

            ps_msub     tmp9, tmp9, cc4, tmp7

            psq_l       tmp5, THP_DCTWIDTH*3*sizeof(f32)(ws), 0, 0

            ps_sub      tmp3, tmp6, tmp7

            ps_add      tmp1, tmp8, tmp9

            psq_l       tmp6, THP_DCTWIDTH*5*sizeof(f32)(ws), 0, 0

            ps_sub      tmp2, tmp8, tmp9

            psq_l       tmp7, THP_DCTWIDTH*7*sizeof(f32)(ws), 0, 0

            ps_add      tmp8, tmp6, tmp5

            ps_sub      tmp6, tmp6, tmp5

            addi        ws, ws, 2*sizeof(f32)

            ps_add      tmp9, tmp4, tmp7

            ps_sub      tmp4, tmp4, tmp7

            psq_l       tmp10, THP_DCTWIDTH*0*sizeof(f32)(ws), 0, 0

            ps_add      tmp7, tmp9, tmp8

            ps_sub      tmp5, tmp9, tmp8

            ps_add      tmp8, tmp6, tmp4

            psq_l       tmp11, THP_DCTWIDTH*4*sizeof(f32)(ws), 0, 0

            ps_add      tmp9, tmp0, tmp7

            ps_mul      tmp8, tmp8, cc2

            psq_l       tmp12, THP_DCTWIDTH*2*sizeof(f32)(ws), 0, 0

            ps_sub      tmp23, tmp0, tmp7

            ps_madd     tmp6, tmp6, cc2c6a, tmp8

            psq_l       tmp13, THP_DCTWIDTH*6*sizeof(f32)(ws), 0, 0

            ps_sub      tmp6, tmp6, tmp7

            addi        off0, off0, 2*sizeof(THPSample)

            psq_st      tmp9, 0(out0), 0, 6

            ps_msub     tmp4, tmp4, cc2c6s, tmp8

            ps_add      tmp9, tmp1, tmp6

            ps_msub     tmp5, tmp5, cc4, tmp6

            ps_sub      tmp22, tmp1, tmp6

            psq_st      tmp9, 8(out0), 0, 6

            ps_add      tmp8, tmp2, tmp5

            ps_add      tmp4, tmp4, tmp5

            psq_st      tmp8, 16(out0), 0, 6

            addi        off1, off1, 2*sizeof(THPSample)

            ps_sub      tmp9, tmp3, tmp4

            ps_add      tmp20, tmp3, tmp4

            psq_st      tmp9, 24(out0), 0, 6

            ps_sub      tmp21, tmp2, tmp5

            ps_add      tmp6, tmp10, tmp11

            psq_st      tmp20, 0(out1), 0, 6

            ps_sub      tmp8, tmp10, tmp11

            ps_add      tmp6, tmp6, bias

            psq_st      tmp21, 8(out1), 0, 6

            ps_add      tmp7, tmp12, tmp13

            ps_sub      tmp9, tmp12, tmp13

            psq_st      tmp22, 16(out1), 0, 6

            add         out0, obase, off0

            ps_add      tmp0, tmp6, tmp7

            psq_st      tmp23, 24(out1), 0, 6

            ps_add      tmp8, tmp8, bias

            add         out1, obase, off1

            bdnz        _loopHead10

            psq_l       tmp4, THP_DCTWIDTH*1*sizeof(f32)(ws), 0, 0

            ps_msub     tmp9, tmp9, cc4, tmp7

            psq_l       tmp5, THP_DCTWIDTH*3*sizeof(f32)(ws), 0, 0

            ps_sub      tmp3, tmp6, tmp7

            ps_add      tmp1, tmp8, tmp9

            psq_l       tmp6, THP_DCTWIDTH*5*sizeof(f32)(ws), 0, 0

            ps_sub      tmp2, tmp8, tmp9

            psq_l       tmp7, THP_DCTWIDTH*7*sizeof(f32)(ws), 0, 0

            ps_add      tmp8, tmp6, tmp5

            ps_sub      tmp6, tmp6, tmp5

            ps_add      tmp9, tmp4, tmp7

            ps_sub      tmp4, tmp4, tmp7

            ps_add      tmp7, tmp9, tmp8

            ps_sub      tmp5, tmp9, tmp8

            ps_add      tmp8, tmp6, tmp4

            ps_add      tmp9, tmp0, tmp7

            ps_mul      tmp8, tmp8, cc2

            ps_sub      tmp23, tmp0, tmp7

            ps_madd     tmp6, tmp6, cc2c6a, tmp8

            psq_st      tmp9, 0(out0), 0, 6

            ps_sub      tmp6, tmp6, tmp7

            ps_msub     tmp4, tmp4, cc2c6s, tmp8

            psq_st      tmp23, 24(out1), 0, 6

            ps_add      tmp9, tmp1, tmp6

            ps_msub     tmp5, tmp5, cc4, tmp6

            ps_sub      tmp22, tmp1, tmp6

            psq_st      tmp9, 8(out0), 0, 6

            ps_add      tmp8, tmp2, tmp5

            ps_add      tmp4, tmp4, tmp5

            psq_st      tmp8, 16(out0), 0, 6

            ps_sub      tmp9, tmp3, tmp4

            psq_st      tmp22, 16(out1), 0, 6

            ps_add      tmp20, tmp3, tmp4

            psq_st      tmp9, 24(out0), 0, 6

            ps_sub      tmp21, tmp2, tmp5

            psq_st      tmp20, 0(out1), 0, 6
            psq_st      tmp21, 8(out1), 0, 6

        }
    }
}
#else
void __THPInverseDCTY8(
    THPCoeff *in,
    u32       xPos
)
{
    u32          y, off0, off1;
    f32         *q;
    f32         *ws;
    THPSample   *out;

    f32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp10, tmp11, tmp12, tmp13;
    f32 z5, z10, z11, z12, z13;

    q  = Gq;
    ws = &__THPIDCTWorkspace[0];

    for (y = THP_DCTWIDTH; y > 0; y--)
    {

        if (in[THP_DCTWIDTH * 1] == 0 &&
            in[THP_DCTWIDTH * 2] == 0 &&
            in[THP_DCTWIDTH * 3] == 0 &&
            in[THP_DCTWIDTH * 4] == 0 &&
            in[THP_DCTWIDTH * 5] == 0 &&
            in[THP_DCTWIDTH * 6] == 0 &&
            in[THP_DCTWIDTH * 7] == 0)
        {

            f32 dcval = (f32)in[THP_DCTWIDTH * 0] * q[THP_DCTWIDTH * 0];

            ws[THP_DCTWIDTH * 0]
              = ws[THP_DCTWIDTH * 1]
              = ws[THP_DCTWIDTH * 2]
              = ws[THP_DCTWIDTH * 3]
              = ws[THP_DCTWIDTH * 4]
              = ws[THP_DCTWIDTH * 5]
              = ws[THP_DCTWIDTH * 6]
              = ws[THP_DCTWIDTH * 7] = dcval;

            in++;
            q++;
            ws++;
            continue;
        }

        tmp0 = (f32)in[THP_DCTWIDTH * 0] * q[THP_DCTWIDTH * 0];
        tmp1 = (f32)in[THP_DCTWIDTH * 2] * q[THP_DCTWIDTH * 2];
        tmp2 = (f32)in[THP_DCTWIDTH * 4] * q[THP_DCTWIDTH * 4];
        tmp3 = (f32)in[THP_DCTWIDTH * 6] * q[THP_DCTWIDTH * 6];

        tmp10 = tmp0 + tmp2;
        tmp11 = tmp0 - tmp2;

        tmp13 = tmp1 + tmp3;
        tmp12 = (tmp1 - tmp3) * 1.414213562f - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        tmp4 = (f32)in[THP_DCTWIDTH * 1] * q[THP_DCTWIDTH * 1];
        tmp5 = (f32)in[THP_DCTWIDTH * 3] * q[THP_DCTWIDTH * 3];
        tmp6 = (f32)in[THP_DCTWIDTH * 5] * q[THP_DCTWIDTH * 5];
        tmp7 = (f32)in[THP_DCTWIDTH * 7] * q[THP_DCTWIDTH * 7];

        z13 = tmp6 + tmp5;
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7 = z11 + z13;
        tmp11 = (z11 - z13) * 1.414213562f;

        z5 = (z10 + z12) * 1.847759065f;
        tmp10 = 1.082392200f * z12 - z5;
        tmp12 = -2.613125930f * z10 + z5;

        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        ws[THP_DCTWIDTH * 0] = tmp0 + tmp7;
        ws[THP_DCTWIDTH * 7] = tmp0 - tmp7;
        ws[THP_DCTWIDTH * 1] = tmp1 + tmp6;
        ws[THP_DCTWIDTH * 6] = tmp1 - tmp6;
        ws[THP_DCTWIDTH * 2] = tmp2 + tmp5;
        ws[THP_DCTWIDTH * 5] = tmp2 - tmp5;
        ws[THP_DCTWIDTH * 4] = tmp3 + tmp4;
        ws[THP_DCTWIDTH * 3] = tmp3 - tmp4;

        in++;
        q++;
        ws++;
    }

    ws = &__THPIDCTWorkspace[0];

    off0 = Gwid * 8;
    off0 = off0 + xPos * 4;
    off1 = off0 + Gwid * 4;

    out = Gbase + off0;

    for (y = 0; y < THP_DCTWIDTH; y++)
    {

        tmp10 = ws[0] + ws[4];
        tmp11 = ws[0] - ws[4];

        tmp13 = ws[2] + ws[6];
        tmp12 = (ws[2] - ws[6]) * 1.414213562f - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        z13 = ws[5] + ws[3];
        z10 = ws[5] - ws[3];
        z11 = ws[1] + ws[7];
        z12 = ws[1] - ws[7];

        tmp7 = z11 + z13;
        tmp11 = (z11 - z13) * 1.414213562f;

        z5 = (z10 + z12) * 1.847759065f;
        tmp10 = 1.082392200f * z12 - z5;
        tmp12 = -2.613125930f * z10 + z5;

        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        {
          register f32 rtmp0, rtmp1, rtmp2, rtmp3, rtmp4, rtmp5, rtmp6, rtmp7;
          register u32 outptr;

          outptr = (u32)out;

          rtmp0 = tmp0 + tmp7 + 1024.0f;
          rtmp1 = tmp0 - tmp7 + 1024.0f;
          rtmp2 = tmp1 + tmp6 + 1024.0f;
          rtmp3 = tmp1 - tmp6 + 1024.0f;
          rtmp4 = tmp2 + tmp5 + 1024.0f;
          rtmp5 = tmp2 - tmp5 + 1024.0f;
          rtmp6 = tmp3 + tmp4 + 1024.0f;
          rtmp7 = tmp3 - tmp4 + 1024.0f;

          asm
          {
              psq_st      rtmp0, 0(outptr), 0, 6
              psq_st      rtmp1, 7(outptr), 0, 6
              psq_st      rtmp2, 1(outptr), 0, 6
              psq_st      rtmp3, 6(outptr), 0, 6
              psq_st      rtmp4, 2(outptr), 0, 6
              psq_st      rtmp5, 5(outptr), 0, 6
              psq_st      rtmp6, 4(outptr), 0, 6
              psq_st      rtmp7, 3(outptr), 0, 6
          }
        }

        out += THP_DCTWIDTH;
        ws  += THP_DCTWIDTH;

        if (y == 3)
        {
            out = Gbase + off1;
        }
    }

}
#endif

#ifndef _DEBUG
static inline void __THPInverseDCTNoYPos(
    register THPCoeff *in,
    register u32       xPos
)
{

    register f32 *q, *ws;

    register f32 tmp0,  tmp1,  tmp2,  tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
    register f32 tmp10, tmp11, tmp12, tmp13;
    register f32 tmp20, tmp21, tmp22, tmp23;

    register f32 cc4    = 1.414213562F;
    register f32 cc2    = 1.847759065F;
    register f32 cc2c6s = 1.082392200F;
    register f32 cc2c6a = -2.613125930F;
    register f32 bias   = 1024.0F;

    q  = Gq;
    ws = &__THPIDCTWorkspace[0] - 2;

    {
        register u32 itmp0, itmp1, itmp2, itmp3;

        asm
        {

            li          itmp2, 8
            mtctr       itmp2

        _loopHead0:

            psq_l       tmp10, 0(in), 0, 5

            psq_l       tmp11, 0(q), 0, 0

            lwz         itmp0, 12(in)
            lwz         itmp3, 8(in)

            ps_mul      tmp10, tmp10, tmp11
            lwz         itmp1, 4(in)
            lhz         itmp2, 2(in)
            or.         itmp0, itmp0, itmp3

        _loopHead1:
            cmpwi       itmp0, 0
            bne         _regularIDCT
            ps_merge00  tmp0, tmp10, tmp10
            cmpwi       itmp1, 0
            psq_st      tmp0, 8(ws), 0, 0
            bne         _halfIDCT
            psq_st      tmp0, 16(ws), 0, 0
            cmpwi       itmp2, 0
            psq_st      tmp0, 24(ws), 0, 0
            bne         _quarterIDCT

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            psq_stu     tmp0, 32(ws), 0, 0

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)

            bdnz        _loopHead0
            b           _loopEnd

        _quarterIDCT:

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)

            ps_msub     tmp2, tmp10, cc2, tmp10

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            ps_merge00  tmp9, tmp10, tmp10

            lwz         itmp1, 4(in)

            ps_sub      tmp1, cc2, cc2c6s

            ps_msub     tmp3, tmp10, cc4, tmp2

            lhz         itmp2, 2(in)

            ps_merge11  tmp5, tmp10, tmp2

            psq_l       tmp11, 0(q), 0, 0

            ps_nmsub    tmp4, tmp10, tmp1, tmp3

            ps_add      tmp7, tmp9, tmp5

            psq_l       tmp10, 0(in), 0, 5

            ps_merge11  tmp6, tmp3, tmp4

            ps_sub      tmp5, tmp9, tmp5

            lwz         itmp0, 12(in)

            ps_add      tmp8, tmp9, tmp6

            lwz         itmp3, 8(in)

            ps_sub      tmp6, tmp9, tmp6

            psq_stu     tmp7, 8(ws), 0, 0

            ps_merge10  tmp6, tmp6, tmp6

            psq_stu     tmp8, 8(ws), 0, 0

            ps_merge10  tmp5, tmp5, tmp5

            or          itmp0, itmp0, itmp3

            psq_stu     tmp6, 8(ws), 0, 0

            ps_mul      tmp10, tmp10, tmp11

            psq_stu     tmp5, 8(ws), 0, 0

            bdnz        _loopHead1
            b           _loopEnd

        _halfIDCT:

            psq_l       tmp1, 4(in), 0, 5
            psq_l       tmp9, 8(q), 0, 0

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)
            ps_mul      tmp1, tmp1, tmp9

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            ps_sub      tmp3, tmp10, tmp1

            ps_add      tmp2, tmp10, tmp1

            lwz         itmp0, 12(in)

            ps_madd     tmp4, tmp1, cc4, tmp3

            ps_nmsub    tmp5, tmp1, cc4, tmp2

            ps_mul      tmp8, tmp3, cc2

            ps_merge00  tmp4, tmp2, tmp4

            lwz         itmp3, 8(in)

            ps_nmsub    tmp6, tmp1, cc2c6a, tmp8

            ps_merge00  tmp5, tmp5, tmp3

            lwz         itmp1, 4(in)

            ps_sub      tmp6, tmp6, tmp2

            ps_nmsub    tmp7, tmp10, cc2c6s, tmp8

            lhz         itmp2, 2(in)

            ps_merge11  tmp2, tmp2, tmp6

            ps_msub     tmp8, tmp3, cc4, tmp6

            psq_l       tmp10, 0(in), 0, 5

            ps_add      tmp9, tmp4, tmp2

            ps_sub      tmp7, tmp7, tmp8

            psq_l       tmp11, 0(q), 0, 0

            ps_merge11  tmp3, tmp8, tmp7

            ps_sub      tmp4, tmp4, tmp2

            psq_stu     tmp9, 8(ws), 0, 0

            ps_add      tmp0, tmp5, tmp3

            ps_sub      tmp1, tmp5, tmp3

            or          itmp0, itmp0, itmp3

            psq_stu     tmp0, 8(ws), 0, 0

            ps_merge10  tmp1, tmp1, tmp1

            ps_merge10  tmp4, tmp4, tmp4

            psq_stu     tmp1, 8(ws), 0, 0

            ps_mul      tmp10, tmp10, tmp11

            psq_stu     tmp4, 8(ws), 0, 0

            bdnz        _loopHead1
            b           _loopEnd

        _regularIDCT:

            psq_l       tmp9, 4(in), 0, 5

            psq_l       tmp5, 8(q), 0, 0

            ps_mul      tmp9, tmp9, tmp5

            psq_l       tmp2, 8(in), 0, 5

            psq_l       tmp6, 16(q), 0, 0

            ps_merge01  tmp0, tmp10, tmp9

            psq_l       tmp3, 12(in), 0, 5

            ps_merge01  tmp1, tmp9, tmp10

            psq_l       tmp7, 24(q), 0, 0

            addi        in, in, THP_DCTWIDTH*sizeof(THPCoeff)

            ps_madd     tmp4, tmp2, tmp6, tmp0

            ps_nmsub    tmp5, tmp2, tmp6, tmp0

            ps_madd     tmp6, tmp3, tmp7, tmp1

            ps_nmsub    tmp7, tmp3, tmp7, tmp1

            addi        q, q, THP_DCTWIDTH*sizeof(f32)

            ps_add      tmp0, tmp4, tmp6

            ps_sub      tmp3, tmp4, tmp6

            ps_msub     tmp2, tmp7, cc4, tmp6

            lwz         itmp0, 12(in)

            ps_sub      tmp8, tmp7, tmp5

            ps_add      tmp1, tmp5, tmp2

            ps_sub      tmp2, tmp5, tmp2

            ps_mul      tmp8, tmp8, cc2

            lwz         itmp3, 8(in)

            ps_merge00  tmp1, tmp0, tmp1

            ps_nmsub    tmp6, tmp5, cc2c6a, tmp8

            ps_msub     tmp4, tmp7, cc2c6s, tmp8

            lwz         itmp1, 4(in)

            ps_sub      tmp6, tmp6, tmp0

            ps_merge00  tmp2, tmp2, tmp3

            lhz         itmp2, 2(in)

            ps_madd     tmp5, tmp3, cc4, tmp6

            ps_merge11  tmp7, tmp0, tmp6

            psq_l       tmp10, 0(in), 0, 5

            ps_sub      tmp4, tmp4, tmp5

            ps_add      tmp3, tmp1, tmp7

            psq_l       tmp11, 0(q), 0, 0

            ps_merge11  tmp4, tmp5, tmp4

            ps_sub      tmp0, tmp1, tmp7

            ps_mul      tmp10, tmp10, tmp11

            ps_add      tmp5, tmp2, tmp4

            ps_sub      tmp6, tmp2, tmp4

            ps_merge10  tmp5, tmp5, tmp5

            psq_stu     tmp3, 8(ws), 0, 0

            ps_merge10  tmp0, tmp0, tmp0

            psq_stu     tmp6, 8(ws), 0, 0

            psq_stu     tmp5, 8(ws), 0, 0

            or          itmp0, itmp0, itmp3

            psq_stu     tmp0, 8(ws), 0, 0

            bdnz        _loopHead1

        _loopEnd:

        }
    }

    ws = &__THPIDCTWorkspace[0];

    {
        register THPSample *obase = Gbase;
        register u32 wid = Gwid;

        register u32    itmp0, off0, off1;
        register THPSample *out0, *out1;

        asm
        {

            psq_l       tmp10, THP_DCTWIDTH*0*sizeof(f32)(ws), 0, 0

            slwi        xPos, xPos, 2

            psq_l       tmp11, THP_DCTWIDTH*4*sizeof(f32)(ws), 0, 0

            slwi        off1, wid, 2

            psq_l       tmp12, THP_DCTWIDTH*2*sizeof(f32)(ws), 0, 0

            mr         off0, xPos

            ps_add      tmp6, tmp10, tmp11

            psq_l       tmp13, THP_DCTWIDTH*6*sizeof(f32)(ws), 0, 0

            ps_sub      tmp8, tmp10, tmp11

            add         off1, off0, off1

            ps_add      tmp6, tmp6, bias

            li      itmp0, 3

            ps_add      tmp7, tmp12, tmp13

            add         out0, obase, off0

            ps_sub      tmp9, tmp12, tmp13

            ps_add      tmp0, tmp6, tmp7

            add         out1, obase, off1

            ps_add      tmp8, tmp8, bias

            mtctr   itmp0

        _loopHead10:

            psq_l       tmp4, THP_DCTWIDTH*1*sizeof(f32)(ws), 0, 0

            ps_msub     tmp9, tmp9, cc4, tmp7

            psq_l       tmp5, THP_DCTWIDTH*3*sizeof(f32)(ws), 0, 0

            ps_sub      tmp3, tmp6, tmp7

            ps_add      tmp1, tmp8, tmp9

            psq_l       tmp6, THP_DCTWIDTH*5*sizeof(f32)(ws), 0, 0

            ps_sub      tmp2, tmp8, tmp9

            psq_l       tmp7, THP_DCTWIDTH*7*sizeof(f32)(ws), 0, 0

            ps_add      tmp8, tmp6, tmp5

            ps_sub      tmp6, tmp6, tmp5

            addi        ws, ws, 2*sizeof(f32)

            ps_add      tmp9, tmp4, tmp7

            ps_sub      tmp4, tmp4, tmp7

            psq_l       tmp10, THP_DCTWIDTH*0*sizeof(f32)(ws), 0, 0

            ps_add      tmp7, tmp9, tmp8

            ps_sub      tmp5, tmp9, tmp8

            ps_add      tmp8, tmp6, tmp4

            psq_l       tmp11, THP_DCTWIDTH*4*sizeof(f32)(ws), 0, 0

            ps_add      tmp9, tmp0, tmp7

            ps_mul      tmp8, tmp8, cc2

            psq_l       tmp12, THP_DCTWIDTH*2*sizeof(f32)(ws), 0, 0

            ps_sub      tmp23, tmp0, tmp7

            ps_madd     tmp6, tmp6, cc2c6a, tmp8

            psq_l       tmp13, THP_DCTWIDTH*6*sizeof(f32)(ws), 0, 0

            ps_sub      tmp6, tmp6, tmp7

            addi        off0, off0, 2*sizeof(THPSample)

            psq_st      tmp9, 0(out0), 0, 6

            ps_msub     tmp4, tmp4, cc2c6s, tmp8

            ps_add      tmp9, tmp1, tmp6

            ps_msub     tmp5, tmp5, cc4, tmp6

            ps_sub      tmp22, tmp1, tmp6

            psq_st      tmp9, 8(out0), 0, 6

            ps_add      tmp8, tmp2, tmp5

            ps_add      tmp4, tmp4, tmp5

            psq_st      tmp8, 16(out0), 0, 6

            addi        off1, off1, 2*sizeof(THPSample)

            ps_sub      tmp9, tmp3, tmp4

            ps_add      tmp20, tmp3, tmp4

            psq_st      tmp9, 24(out0), 0, 6

            ps_sub      tmp21, tmp2, tmp5

            ps_add      tmp6, tmp10, tmp11

            psq_st      tmp20, 0(out1), 0, 6

            ps_sub      tmp8, tmp10, tmp11

            ps_add      tmp6, tmp6, bias

            psq_st      tmp21, 8(out1), 0, 6

            ps_add      tmp7, tmp12, tmp13

            ps_sub      tmp9, tmp12, tmp13

            psq_st      tmp22, 16(out1), 0, 6

            add         out0, obase, off0

            ps_add      tmp0, tmp6, tmp7

            psq_st      tmp23, 24(out1), 0, 6

            ps_add      tmp8, tmp8, bias

            add         out1, obase, off1

            bdnz        _loopHead10

            psq_l       tmp4, THP_DCTWIDTH*1*sizeof(f32)(ws), 0, 0

            ps_msub     tmp9, tmp9, cc4, tmp7

            psq_l       tmp5, THP_DCTWIDTH*3*sizeof(f32)(ws), 0, 0

            ps_sub      tmp3, tmp6, tmp7

            ps_add      tmp1, tmp8, tmp9

            psq_l       tmp6, THP_DCTWIDTH*5*sizeof(f32)(ws), 0, 0

            ps_sub      tmp2, tmp8, tmp9

            psq_l       tmp7, THP_DCTWIDTH*7*sizeof(f32)(ws), 0, 0

            ps_add      tmp8, tmp6, tmp5

            ps_sub      tmp6, tmp6, tmp5

            ps_add      tmp9, tmp4, tmp7

            ps_sub      tmp4, tmp4, tmp7

            ps_add      tmp7, tmp9, tmp8

            ps_sub      tmp5, tmp9, tmp8

            ps_add      tmp8, tmp6, tmp4

            ps_add      tmp9, tmp0, tmp7

            ps_mul      tmp8, tmp8, cc2

            ps_sub      tmp23, tmp0, tmp7

            ps_madd     tmp6, tmp6, cc2c6a, tmp8

            psq_st      tmp9, 0(out0), 0, 6

            ps_sub      tmp6, tmp6, tmp7

            ps_msub     tmp4, tmp4, cc2c6s, tmp8

            psq_st      tmp23, 24(out1), 0, 6

            ps_add      tmp9, tmp1, tmp6

            ps_msub     tmp5, tmp5, cc4, tmp6

            ps_sub      tmp22, tmp1, tmp6

            psq_st      tmp9, 8(out0), 0, 6

            ps_add      tmp8, tmp2, tmp5

            ps_add      tmp4, tmp4, tmp5

            psq_st      tmp22, 16(out1), 0, 6
            psq_st      tmp8, 16(out0), 0, 6

            ps_sub      tmp9, tmp3, tmp4

            ps_add      tmp20, tmp3, tmp4

            psq_st      tmp9, 24(out0), 0, 6

            ps_sub      tmp21, tmp2, tmp5

            psq_st      tmp20, 0(out1), 0, 6
            psq_st      tmp21, 8(out1), 0, 6

        }
    }
}

#else
void __THPInverseDCTNoYPos(
    THPCoeff *in,
    u32       xPos
)
{
    u32          y, off0, off1;
    f32         *q;
    f32         *ws;
    THPSample   *out;

    f32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp10, tmp11, tmp12, tmp13;
    f32 z5, z10, z11, z12, z13;

    q  = Gq;
    ws = &__THPIDCTWorkspace[0];

    for (y = THP_DCTWIDTH; y > 0; y--)
    {

        if (in[THP_DCTWIDTH * 1] == 0 &&
            in[THP_DCTWIDTH * 2] == 0 &&
            in[THP_DCTWIDTH * 3] == 0 &&
            in[THP_DCTWIDTH * 4] == 0 &&
            in[THP_DCTWIDTH * 5] == 0 &&
            in[THP_DCTWIDTH * 6] == 0 &&
            in[THP_DCTWIDTH * 7] == 0)
        {

            f32 dcval = (f32)in[THP_DCTWIDTH * 0] * q[THP_DCTWIDTH * 0];

            ws[THP_DCTWIDTH * 0]
              = ws[THP_DCTWIDTH * 1]
              = ws[THP_DCTWIDTH * 2]
              = ws[THP_DCTWIDTH * 3]
              = ws[THP_DCTWIDTH * 4]
              = ws[THP_DCTWIDTH * 5]
              = ws[THP_DCTWIDTH * 6]
              = ws[THP_DCTWIDTH * 7] = dcval;

            in++;
            q++;
            ws++;
            continue;
        }

        tmp0 = (f32)in[THP_DCTWIDTH * 0] * q[THP_DCTWIDTH * 0];
        tmp1 = (f32)in[THP_DCTWIDTH * 2] * q[THP_DCTWIDTH * 2];
        tmp2 = (f32)in[THP_DCTWIDTH * 4] * q[THP_DCTWIDTH * 4];
        tmp3 = (f32)in[THP_DCTWIDTH * 6] * q[THP_DCTWIDTH * 6];

        tmp10 = tmp0 + tmp2;
        tmp11 = tmp0 - tmp2;

        tmp13 = tmp1 + tmp3;
        tmp12 = (tmp1 - tmp3) * 1.414213562f - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        tmp4 = (f32)in[THP_DCTWIDTH * 1] * q[THP_DCTWIDTH * 1];
        tmp5 = (f32)in[THP_DCTWIDTH * 3] * q[THP_DCTWIDTH * 3];
        tmp6 = (f32)in[THP_DCTWIDTH * 5] * q[THP_DCTWIDTH * 5];
        tmp7 = (f32)in[THP_DCTWIDTH * 7] * q[THP_DCTWIDTH * 7];

        z13 = tmp6 + tmp5;
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7 = z11 + z13;
        tmp11 = (z11 - z13) * 1.414213562f;

        z5 = (z10 + z12) * 1.847759065f;
        tmp10 = 1.082392200f * z12 - z5;
        tmp12 = -2.613125930f * z10 + z5;

        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        ws[THP_DCTWIDTH * 0] = tmp0 + tmp7;
        ws[THP_DCTWIDTH * 7] = tmp0 - tmp7;
        ws[THP_DCTWIDTH * 1] = tmp1 + tmp6;
        ws[THP_DCTWIDTH * 6] = tmp1 - tmp6;
        ws[THP_DCTWIDTH * 2] = tmp2 + tmp5;
        ws[THP_DCTWIDTH * 5] = tmp2 - tmp5;
        ws[THP_DCTWIDTH * 4] = tmp3 + tmp4;
        ws[THP_DCTWIDTH * 3] = tmp3 - tmp4;

        in++;
        q++;
        ws++;
    }

    ws = &__THPIDCTWorkspace[0];

    off0 = xPos * 4;
    off1 = off0 + Gwid * 4;

    out = Gbase + off0;

    for (y = 0; y < THP_DCTWIDTH; y++)
    {

        tmp10 = ws[0] + ws[4];
        tmp11 = ws[0] - ws[4];

        tmp13 = ws[2] + ws[6];
        tmp12 = (ws[2] - ws[6]) * 1.414213562f - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        z13 = ws[5] + ws[3];
        z10 = ws[5] - ws[3];
        z11 = ws[1] + ws[7];
        z12 = ws[1] - ws[7];

        tmp7 = z11 + z13;
        tmp11 = (z11 - z13) * 1.414213562f;

        z5 = (z10 + z12) * 1.847759065f;
        tmp10 = 1.082392200f * z12 - z5;
        tmp12 = -2.613125930f * z10 + z5;

        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        {
          register f32 rtmp0, rtmp1, rtmp2, rtmp3, rtmp4, rtmp5, rtmp6, rtmp7;
          register u32 outptr;

          outptr = (u32)out;

          rtmp0 = tmp0 + tmp7 + 1024.0f;
          rtmp1 = tmp0 - tmp7 + 1024.0f;
          rtmp2 = tmp1 + tmp6 + 1024.0f;
          rtmp3 = tmp1 - tmp6 + 1024.0f;
          rtmp4 = tmp2 + tmp5 + 1024.0f;
          rtmp5 = tmp2 - tmp5 + 1024.0f;
          rtmp6 = tmp3 + tmp4 + 1024.0f;
          rtmp7 = tmp3 - tmp4 + 1024.0f;

          asm
          {
              psq_st      rtmp0, 0(outptr), 0, 6
              psq_st      rtmp1, 7(outptr), 0, 6
              psq_st      rtmp2, 1(outptr), 0, 6
              psq_st      rtmp3, 6(outptr), 0, 6
              psq_st      rtmp4, 2(outptr), 0, 6
              psq_st      rtmp5, 5(outptr), 0, 6
              psq_st      rtmp6, 4(outptr), 0, 6
              psq_st      rtmp7, 3(outptr), 0, 6
          }
        }

        out += THP_DCTWIDTH;
        ws  += THP_DCTWIDTH;

        if (y == 3)
        {
            out = Gbase + off1;
        }
    }

}
#endif

s32 THPVideoDecode(void *file, void *tileY, void *tileU, void *tileV, void *work)
{
    u8                  all_done;
    u8                  status;
    s32                 errorCode;

    if (!file)
    {
        goto _err_no_input;
    }

    if (tileY == NULL || tileU == NULL || tileV == NULL)
    {
        goto _err_no_output;
    }

    if (!work)
    {
        goto _err_no_work;
    }

    if (!(PPCMfhid2() & HID2_LCE))
    {
        goto _err_lc_not_enabled;
    }

    if (__THPInitFlag == FALSE)
    {
        goto _err_not_initialized;
    }

    __THPWorkArea = (u8 *)work;

    __THPInfo = (THPFileInfo *)OSRoundUp32B(__THPWorkArea);
    __THPWorkArea = (u8 *)OSRoundUp32B(__THPWorkArea) + sizeof(THPFileInfo);

    DCZeroRange(__THPInfo, sizeof(THPFileInfo));

    __THPInfo->cnt            = 33;
    __THPInfo->decompressedY  = THP_ZERO;
    __THPInfo->c              = (u8 *)file;

    all_done = FALSE;

    forever
    {

        if (THP_READ_UBYTE(__THPInfo->c) != THP_M_MARKER)
        {
            goto _err_bad_syntax;
        }

        while (*__THPInfo->c == THP_M_MARKER)
        {
            THP_SKIP_UBYTE(__THPInfo->c);
        }

        status = THP_READ_UBYTE(__THPInfo->c);

        if(status <= 0xd7)
        {
            if      (status == THP_M_DHT)
            {
                status = __THPReadHuffmanTableSpecification();
                if (status != THP_OK)
                {
                    goto _err_bad_status;
                }
            }

            else if (status == THP_M_SOF0)
            {
                status = __THPReadFrameHeader();
                if (status != THP_OK)
                {
                    goto _err_bad_status;
                }
            }

            else
            {
                goto _err_unsupported_marker;
            }
        }

        else if(0xd8 <= status && status <= 0xdf)
        {
            if      (status == THP_M_DRI)
            {
                __THPRestartDefinition();
            }

            else if (status == THP_M_DQT)
            {
                status = __THPReadQuantizationTable();
                if (status != THP_OK)
                {
                    goto _err_bad_status;
                }
            }

            else if (status == THP_M_SOS)
            {
                status = __THPReadScaneHeader();
                if (status != THP_OK)
                {
                    goto _err_bad_status;
                }
                all_done = TRUE;
            }

            else if (status == THP_M_SOI)
            {

            }

            else
            {
                goto _err_unsupported_marker;
            }
        }

        else  if(0xe0 <= status)
        {
            if ( ( THP_M_APP0 <= status && status <= THP_M_APP15 ) || status == THP_M_COM )
            {
                __THPInfo->c += (__THPInfo->c)[0] << 8 | (__THPInfo->c)[1];
            }

            else
            {
                goto _err_unsupported_marker;
            }
        }

        if (all_done)
        {
            break;
        }
    }

    __THPSetupBuffers();

    __THPDecompressYUV(tileY, tileU, tileV);

    return (THP_OK);

_err_no_input:
    errorCode = THP_NO_INPUT_FILE;
    goto _err_exit;

_err_no_output:
    errorCode = THP_NO_OUTPUT_BUFFER;
    goto _err_exit;

_err_no_work:
    errorCode = THP_NO_WORK_AREA;
    goto _err_exit;

_err_unsupported_marker:
    errorCode = THP_UNSUPPORTED_MARKER;
    goto _err_exit;

_err_bad_resource:
    errorCode = THP_RESOURCE_NOT_FOUND;
    goto _err_exit;

_err_no_mem:
    errorCode = THP_NO_MEMORY;
    goto _err_exit;

_err_bad_syntax:
    errorCode = THP_BAD_SYNTAX;
    goto _err_exit;

_err_bad_status:
    errorCode = status;
    goto _err_exit;

_err_lc_not_enabled:
    errorCode = THP_LC_NOT_ENABLED;
    goto _err_exit;

_err_not_initialized:
    errorCode = THP_NOT_INITIALIZED;
    goto _err_exit;

_err_exit:
#ifdef _DEBUG
    OSReport("THP: THPVideDecode failed: '%s'...\n", __THPErrorStrings[errorCode]);
#endif

    return (errorCode);
}

static void __THPSetupBuffers(void)
{
    u8         i;
    THPCoeff  *buffer;

    buffer = (THPCoeff *)OSRoundUp32B(__THPWorkArea);

    for(i = 0; i < THP_MAX_BLOCKS_IN_MCU; i++)
    {
        __THPMCUBuffer[i] = &buffer[i * THP_DCTSIZE];
    }
}

u8 __THPReadFrameHeader(void)
{
    u8      i, utmp8;

    __THPInfo->c += 2;

    utmp8 = THP_READ_UBYTE(__THPInfo->c);
    if (utmp8 != 8)
        return (THP_UNSUPPORTED_PRECISION);

    __THPInfo->yPixelSize = (u16)((__THPInfo->c)[0] << 8 | (__THPInfo->c)[1]);
    __THPInfo->c += 2;
    __THPInfo->xPixelSize = (u16)((__THPInfo->c)[0] << 8 | (__THPInfo->c)[1]);
    __THPInfo->c += 2;

    utmp8 = THP_READ_UBYTE(__THPInfo->c);
    if (utmp8 != THP_NUM_COMPONENTS)
        return (THP_UNSUPPORTED_NUM_COMP);

    for (i = 0; i < THP_NUM_COMPONENTS; i++)
    {

        utmp8 = THP_READ_UBYTE(__THPInfo->c);
#ifdef _DEBUG
        if (utmp8 != i + 1)
        {
            return (THP_UNSUPPORTED_COMPS);
        }
#endif

        utmp8 = THP_READ_UBYTE(__THPInfo->c);
        if ((i == 0 && utmp8 != 0x22) || (i > 0 && utmp8 != 0x11))
        {
            return (THP_UNSUPPORTED_COMPS);
        }

        __THPInfo->components[i].quantizationTableSelector = THP_READ_UBYTE(__THPInfo->c);
    }

    return (THP_OK);
}

u8 __THPReadScaneHeader(void)
{
    u8      i, utmp8;
#ifdef _DEBUG
    u8      t1, t2, t3;
#endif

    __THPInfo->c += 2;

    utmp8 = THP_READ_UBYTE(__THPInfo->c);
    if (utmp8 != THP_NUM_COMPONENTS)
        return (THP_UNSUPPORTED_NUM_COMP);

    for (i = 0; i < THP_NUM_COMPONENTS; i++)
    {

        utmp8 = THP_READ_UBYTE(__THPInfo->c);
#ifdef _DEBUG
        if (utmp8 != i + 1)
        {
            return (THP_BAD_SCAN_HEADER);
        }
#endif

        utmp8 = THP_READ_UBYTE(__THPInfo->c);
        __THPInfo->components[i].DCTableSelector = (u8)(utmp8 >> 4);
        __THPInfo->components[i].ACTableSelector = (u8)(utmp8 & 15);
        if ((__THPInfo->validHuffmanTabs & (1 << ((utmp8 >> 4) + THP_DC_TABLE))) == THP_ZERO)
        {
            return (THP_INVALID_HUFFTAB);
        }
        if ((__THPInfo->validHuffmanTabs & (1 << ((utmp8 & 15) + THP_AC_TABLE))) == THP_ZERO)
        {
            return (THP_INVALID_HUFFTAB);
        }
    }

#ifdef _DEBUG
    t1 = THP_READ_UBYTE(__THPInfo->c);
    t2 = THP_READ_UBYTE(__THPInfo->c);
    t3 = THP_READ_UBYTE(__THPInfo->c);

    if (t1 != 0 || t2 != 63 || t3 != 0)
    {
        return (THP_BAD_SCAN_HEADER);
    }
#else
    __THPInfo->c += 3;
#endif

    __THPInfo->MCUsPerRow = (u16)THP_JDIV_ROUNDUP(__THPInfo->xPixelSize, 2 * THP_DCTWIDTH);

    __THPInfo->components[0].predDC = THP_ZERO;
    __THPInfo->components[1].predDC = THP_ZERO;
    __THPInfo->components[2].predDC = THP_ZERO;

    return (THP_OK);
}

u8 __THPReadQuantizationTable(void)
{
    u16     length, id, i, row, col;
    f32     q_temp[THP_DCTSIZE];

    length        = (u16)((__THPInfo->c)[0] << 8 | (__THPInfo->c)[1]);
    __THPInfo->c += 2 ;
    length       -= 2;

    forever
    {
        id = THP_READ_UBYTE(__THPInfo->c);
#ifdef _DEBUG
        if (id >= THP_MAX_QUANT_TABS)
        {
            return (THP_UNSUPPORTED_QUANTIZATION);
        }
#endif

        for (i = 0; i < 64; i++)
        {
            q_temp[__THPJpegNaturalOrder[i]] = (f32)THP_READ_UBYTE(__THPInfo->c);
        }

        i = THP_ZERO;
        for (row = 0; row < THP_DCTWIDTH; row++)
        {
            for (col = 0; col < THP_DCTWIDTH; col++)
            {
                __THPInfo->quantTabs[id][i] = (f32)((f64)q_temp[i] * __THPAANScaleFactor[row] * __THPAANScaleFactor[col]);
                i++;
            }
        }

        length -= (THP_DCTSIZE + 1);
        if (!length)
        {
            break;
        }
    }

    return (THP_OK);
}

u8 __THPReadHuffmanTableSpecification(void)
{
    u8      t_class, id, i, tab_index;
    u16     length, num_Vij;

    __THPHuffmanSizeTab = __THPWorkArea;
    __THPHuffmanCodeTab = (u16 *)((u32)__THPWorkArea + THP_MAX_HUFFMAN_CODES + 1);

    length        = (u16)((__THPInfo->c)[0] << 8 | (__THPInfo->c)[1]);
    __THPInfo->c += 2 ;
    length       -= 2;

    forever
    {
        i       = THP_READ_UBYTE(__THPInfo->c);
        id      = (u8)(i & 15);
        t_class = (u8)(i >> 4);

        __THPHuffmanBits = __THPInfo->c;

#ifdef _DEBUG
        if (t_class > THP_AC_TABLE || id >= THP_MAX_HUFFMAN_TABS)
        {
            return (THP_UNSUPPORTED_NUM_HUFF);
        }
#endif
        tab_index = (u8)((id << 1) + t_class);

        num_Vij = THP_ZERO;
        for (i = 0; i < 16; i++)
        {
            num_Vij += THP_READ_UBYTE(__THPInfo->c);
        }

        __THPInfo->huffmanTabs[tab_index].Vij = __THPInfo->c;
        __THPInfo->c += num_Vij;

        __THPHuffGenerateSizeTable();
        __THPHuffGenerateCodeTable();

        __THPHuffGenerateDecoderTables(tab_index);

        __THPInfo->validHuffmanTabs |= 1 << tab_index;

        length -= 17 + num_Vij;
        if (length == THP_ZERO) break;
    }

    return (THP_OK);
}

static void __THPHuffGenerateSizeTable(void)
{
    s32 p, l, i;

    p = THP_ZERO;
    for (l = 1; l <= 16; l++)
    {
        i = (s32)__THPHuffmanBits[l - 1];
        while (i--)
        {
            __THPHuffmanSizeTab[p++] = (u8)l;
        }
    }

    __THPHuffmanSizeTab[p] = 0;
#ifdef _DEBUG
    __THPHuffmanLastK = p;
#endif
}

static void __THPHuffGenerateCodeTable(void)
{
    u8      si;
    u16     p, code;

    p    = 0;
    code = 0;
    si   = __THPHuffmanSizeTab[0];

    while (__THPHuffmanSizeTab[p])
    {
        while (__THPHuffmanSizeTab[p] == si)
        {
            __THPHuffmanCodeTab[p++] = code;
            code++;
        }
        code <<= 1;
        si++;
    }
}

static void __THPHuffGenerateDecoderTables(u8 tabIndex)
{
    s32              p, l;
    THPHuffmanTab   *h;

    p = 0;
    h = &__THPInfo->huffmanTabs[tabIndex];
    for (l = 1; l <= 16; l++)
    {
        if (__THPHuffmanBits[l - 1])
        {
            h->valPtr[l]  = p - __THPHuffmanCodeTab[p];
            p            += __THPHuffmanBits[l - 1];
            h->maxCode[l] = __THPHuffmanCodeTab[p-1];
        }
        else
        {
            h->maxCode[l] = -1;
            h->valPtr[l]  = -1;
        }
    }
    h->maxCode[17] = 0xfffffL;
}

static void __THPRestartDefinition(void)
{
    __THPInfo->RST = TRUE;

    __THPInfo->c    += 2;
    __THPInfo->nMCU  = (u16)((__THPInfo->c)[0] << 8 | (__THPInfo->c)[1]);
    __THPInfo->c    += 2;

    __THPInfo->currMCU = __THPInfo->nMCU;
}

#define THP_GQR_U8_DIV8    0x3D04
static inline void __THPGQRSetup(void)
{
    register u32 tmp1, tmp2;

    asm
    {
        mfspr   tmp1, GQR5;
        mfspr   tmp2, GQR6;
    }

    __THPOldGQR5 = tmp1;
    __THPOldGQR6 = tmp2;

    asm
    {

        li      r3, OS_GQR_S16
        oris    r3, r3, OS_GQR_S16
        mtspr   GQR5, r3

        li      r3, THP_GQR_U8_DIV8
        oris    r3, r3, THP_GQR_U8_DIV8
        mtspr   GQR6, r3
    }
}

static inline void __THPGQRRestore(void)
{
    register u32 tmp1, tmp2;

    tmp1 = __THPOldGQR5;
    tmp2 = __THPOldGQR6;

    asm
    {
        mtspr   GQR5, tmp1;
        mtspr   GQR6, tmp2;
    }
}

void __THPPrepBitStream(void)
{

    u32 * ptr    ;
    u32   offset ;
    u32   i, j, k;

    ptr       = (u32*) ((u32)__THPInfo->c & 0xFFFFFFFC);
    offset    = (u32)__THPInfo->c  & 3;

    ASSERT (__THPInfo->cnt <= 33);
    if (__THPInfo->cnt != 33)
    {

        __THPInfo->cnt -= (3-offset)* 8;
    }
    else
    {

        __THPInfo->cnt = (offset*8)+1;
    }
    ASSERT (__THPInfo->cnt <= 33);

    __THPInfo->c = (u8*)ptr;

    __THPInfo->currByte = *ptr;

    for ( i = 0; i < THP_MAX_HUFFMAN_TABS*2; i++)
    {
        if ( __THPInfo->validHuffmanTabs & (1 << i))
        {
#ifdef ANALCHECK
            __THPInfo->huffmanTabs[i].Vij1 = *(__THPInfo->huffmanTabs[i].Vij + __THPInfo->huffmanTabs[i].valPtr[1]);
#endif

            for (j = 0; j < 32; j++)
            {
                __THPInfo->huffmanTabs[i].quick[j] = 0xFF;

                for (k = 0; k < 5; k++)
                {
                    s32 code = (s32)(j >> (5-k-1));

                    if ( code <= __THPInfo->huffmanTabs[i].maxCode[k+1])
                    {

                        __THPInfo->huffmanTabs[i].quick[j] = __THPInfo->huffmanTabs[i].Vij[ (s32)(code + __THPInfo->huffmanTabs[i].valPtr[k+1])];
                        __THPInfo->huffmanTabs[i].increment[j] = (u8)(k+1);
                        k = 99;
                    }
                    else
                    {

                    }
                }
            }
        }
    }

    {
        s32 YdcTab;
        s32 UdcTab;
        s32 VdcTab;

        s32 YacTab;
        s32 UacTab;
        s32 VacTab;

        YdcTab = (__THPInfo->components[0].DCTableSelector << 1) + THP_DC_TABLE;
        UdcTab = (__THPInfo->components[1].DCTableSelector << 1) + THP_DC_TABLE;
        VdcTab = (__THPInfo->components[2].DCTableSelector << 1) + THP_DC_TABLE;

        YacTab = (__THPInfo->components[0].ACTableSelector << 1) + THP_AC_TABLE;
        UacTab = (__THPInfo->components[1].ACTableSelector << 1) + THP_AC_TABLE;
        VacTab = (__THPInfo->components[2].ACTableSelector << 1) + THP_AC_TABLE;

        Ydchuff = &__THPInfo->huffmanTabs[YdcTab];
        Udchuff = &__THPInfo->huffmanTabs[UdcTab];
        Vdchuff = &__THPInfo->huffmanTabs[VdcTab];

        Yachuff = &__THPInfo->huffmanTabs[YacTab];
        Uachuff = &__THPInfo->huffmanTabs[UacTab];
        Vachuff = &__THPInfo->huffmanTabs[VacTab];
    }
}

static void __THPDecompressYUV(void *tileY, void *tileU, void *tileV)
{
    u16 currentY, targetY;

    __THPInfo->dLC[0] = tileY;
    __THPInfo->dLC[1] = tileU;
    __THPInfo->dLC[2] = tileV;

    currentY = __THPInfo->decompressedY;
    targetY  = __THPInfo->yPixelSize;

    __THPGQRSetup();
    __THPPrepBitStream();

    if (__THPInfo->xPixelSize == 512 && targetY == 448)
    {
        while (currentY < targetY)
        {
            __THPDecompressiMCURow512x448();
            currentY += 2 * THP_DCTWIDTH;
        }
    }
    else if (__THPInfo->xPixelSize == 640 && targetY == 480)
    {
        while (currentY < targetY)
        {
            __THPDecompressiMCURow640x480();
            currentY += 2 * THP_DCTWIDTH;
        }
    }
    else
    {
        while (currentY < targetY)
        {
            __THPDecompressiMCURowNxN();
            currentY += 2 * THP_DCTWIDTH;
        }
    }

    __THPGQRRestore();

}

void __THPDecompressiMCURow512x448(void)
{
    u8              cl_num;
    u32             x_pos;
    THPComponent   *comp;

    LCQueueWait(3);

    {
        for (cl_num = 0; cl_num < __THPInfo->MCUsPerRow; cl_num++)
        {

            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[0]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[1]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[2]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[3]);
            __THPHuffDecodeDCTCompU(__THPInfo, __THPMCUBuffer[4]);
            __THPHuffDecodeDCTCompV(__THPInfo, __THPMCUBuffer[5]);

            comp    = &__THPInfo->components[0];
            Gbase   = __THPLCWork512[0];
            Gwid    = 512;
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            x_pos   = (u32)(cl_num * 16);

            __THPInverseDCTNoYPos(__THPMCUBuffer[0], x_pos);
            __THPInverseDCTNoYPos(__THPMCUBuffer[1], x_pos+THP_DCTWIDTH);
            __THPInverseDCTY8    (__THPMCUBuffer[2], x_pos);
            __THPInverseDCTY8    (__THPMCUBuffer[3], x_pos+THP_DCTWIDTH);

            comp    = &__THPInfo->components[1];
            Gbase   = __THPLCWork512[1];
            Gwid    = 256;
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            x_pos  /= 2;
            __THPInverseDCTNoYPos(__THPMCUBuffer[4], x_pos);

            comp    = &__THPInfo->components[2];
            Gbase   = __THPLCWork512[2];
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            __THPInverseDCTNoYPos(__THPMCUBuffer[5], x_pos);

            if (__THPInfo->RST != 0)
            {

                if ((--__THPInfo->currMCU) == 0)
                {
                    __THPInfo->currMCU = __THPInfo->nMCU ;

                    __THPInfo->cnt =  1 + ((__THPInfo->cnt + 6) & 0xFFFFFFF8);

                    if (__THPInfo->cnt > 33)
                    {
                        __THPInfo->cnt = 33;
                    }

                    __THPInfo->components[0].predDC = 0 ;
                    __THPInfo->components[1].predDC = 0 ;
                    __THPInfo->components[2].predDC = 0 ;
                }
            }
        }
    }

    LCStoreData(__THPInfo->dLC[0], __THPLCWork512[0], THP_MCU512_Y_SIZE);
    LCStoreData(__THPInfo->dLC[1], __THPLCWork512[1], THP_MCU512_UV_SIZE);
    LCStoreData(__THPInfo->dLC[2], __THPLCWork512[2], THP_MCU512_UV_SIZE);

    __THPInfo->dLC[0] += THP_MCU512_Y_SIZE;
    __THPInfo->dLC[1] += THP_MCU512_UV_SIZE;
    __THPInfo->dLC[2] += THP_MCU512_UV_SIZE;
}

void __THPDecompressiMCURow640x480(void)
{
    u8              cl_num;
    u32             x_pos;
    THPComponent   *comp;

    LCQueueWait(3);

    {
        for (cl_num = 0; cl_num < __THPInfo->MCUsPerRow; cl_num++)
        {

            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[0]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[1]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[2]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[3]);
            __THPHuffDecodeDCTCompU(__THPInfo, __THPMCUBuffer[4]);
            __THPHuffDecodeDCTCompV(__THPInfo, __THPMCUBuffer[5]);

            comp    = &__THPInfo->components[0];
            Gbase   = __THPLCWork672[0];
            Gwid    = 640;
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            x_pos   = (u32)(cl_num * 16);
            __THPInverseDCTNoYPos(__THPMCUBuffer[0], x_pos);
            __THPInverseDCTNoYPos(__THPMCUBuffer[1], x_pos+THP_DCTWIDTH);
            __THPInverseDCTY8    (__THPMCUBuffer[2], x_pos);
            __THPInverseDCTY8    (__THPMCUBuffer[3], x_pos+THP_DCTWIDTH);

            comp    = &__THPInfo->components[1];
            Gbase   = __THPLCWork672[1];
            Gwid    = 320;
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            x_pos  /= 2;
            __THPInverseDCTNoYPos(__THPMCUBuffer[4], x_pos);

            comp    = &__THPInfo->components[2];
            Gbase   = __THPLCWork672[2];
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            __THPInverseDCTNoYPos(__THPMCUBuffer[5], x_pos);

            if (__THPInfo->RST != 0)
            {
                __THPInfo->currMCU -- ;
                if (__THPInfo->currMCU == 0)
                {

                    __THPInfo->currMCU = __THPInfo->nMCU ;

                    __THPInfo->cnt =  1 + ((__THPInfo->cnt + 6) & 0xFFFFFFF8);

                    if (__THPInfo->cnt > 32)
                    {
                        __THPInfo->cnt = 33;
                    }

                    __THPInfo->components[0].predDC = 0 ;
                    __THPInfo->components[1].predDC = 0 ;
                    __THPInfo->components[2].predDC = 0 ;
                }
            }
        }
    }

    LCStoreData(__THPInfo->dLC[0], __THPLCWork672[0], THP_MCU640_Y_SIZE);
    LCStoreData(__THPInfo->dLC[1], __THPLCWork672[1], THP_MCU640_UV_SIZE);
    LCStoreData(__THPInfo->dLC[2], __THPLCWork672[2], THP_MCU640_UV_SIZE);

    __THPInfo->dLC[0] += THP_MCU640_Y_SIZE;
    __THPInfo->dLC[1] += THP_MCU640_UV_SIZE;
    __THPInfo->dLC[2] += THP_MCU640_UV_SIZE;
}

void __THPDecompressiMCURowNxN(void)
{
    u8              cl_num;
    u32             x_pos;
    u32             x;
    THPComponent   *comp;

    x = __THPInfo->xPixelSize;

    LCQueueWait(3);

    {
        for (cl_num = 0; cl_num < __THPInfo->MCUsPerRow; cl_num++)
        {

            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[0]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[1]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[2]);
            __THPHuffDecodeDCTCompY(__THPInfo, __THPMCUBuffer[3]);
            __THPHuffDecodeDCTCompU(__THPInfo, __THPMCUBuffer[4]);
            __THPHuffDecodeDCTCompV(__THPInfo, __THPMCUBuffer[5]);

            comp    = &__THPInfo->components[0];
            Gbase   = __THPLCWork672[0];
            Gwid    = x;
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            x_pos   = (u32)(cl_num * 16);
            __THPInverseDCTNoYPos(__THPMCUBuffer[0], x_pos);
            __THPInverseDCTNoYPos(__THPMCUBuffer[1], x_pos+THP_DCTWIDTH);
            __THPInverseDCTY8    (__THPMCUBuffer[2], x_pos);
            __THPInverseDCTY8    (__THPMCUBuffer[3], x_pos+THP_DCTWIDTH);

            comp    = &__THPInfo->components[1];
            Gbase   = __THPLCWork672[1];
            Gwid    = x/2;
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            x_pos  /= 2;
            __THPInverseDCTNoYPos(__THPMCUBuffer[4], x_pos);

            comp    = &__THPInfo->components[2];
            Gbase   = __THPLCWork672[2];
            Gq      = __THPInfo->quantTabs[comp->quantizationTableSelector];
            __THPInverseDCTNoYPos(__THPMCUBuffer[5], x_pos);

            if (__THPInfo->RST != 0)
            {
                __THPInfo->currMCU -- ;
                if (__THPInfo->currMCU == 0)
                {

                    __THPInfo->currMCU = __THPInfo->nMCU ;

                    __THPInfo->cnt =  1 + ((__THPInfo->cnt + 6) & 0xFFFFFFF8);

                    if (__THPInfo->cnt > 32)
                    {
                        __THPInfo->cnt = 33;
                    }

                    __THPInfo->components[0].predDC = 0 ;
                    __THPInfo->components[1].predDC = 0 ;
                    __THPInfo->components[2].predDC = 0 ;
                }
            }
        }
    }

    LCStoreData(__THPInfo->dLC[0], __THPLCWork672[0], THP_MCUN_Y_SIZE(x));
    LCStoreData(__THPInfo->dLC[1], __THPLCWork672[1], THP_MCUN_UV_SIZE(x));
    LCStoreData(__THPInfo->dLC[2], __THPLCWork672[2], THP_MCUN_UV_SIZE(x));

    __THPInfo->dLC[0] += THP_MCUN_Y_SIZE(x);
    __THPInfo->dLC[1] += THP_MCUN_UV_SIZE(x);
    __THPInfo->dLC[2] += THP_MCUN_UV_SIZE(x);
}

#ifndef _DEBUG
static inline void
__THPHuffDecodeDCTCompY(
    register THPFileInfo       *info,
    THPCoeff                   *block
)
{
#ifdef DEBUG_INLINE_DECODE
    u8 *csave;
#endif
    {
        register s32         t;
        THPCoeff             dc;
        register THPCoeff    diff;

        __dcbz((void*)block, 0);

        t = __THPHuffDecodeTab(info, Ydchuff);
        __dcbz((void*)block, 32);

        diff  = THP_ZERO;
        __dcbz((void*)block, 64);

        if (t)
        {
                {
                    register s32    v;
                    register u32    cb;
                    register u32    cnt;
                    register u32    cnt33;
                    register u32 tmp;
                    register u32    cnt1;
                    register u32 tmp1;
                    asm
                    {
                        lwz      cnt,info->cnt;
                        subfic   cnt33,cnt,33;
                        lwz      cb,info->currByte;

                        subfc. tmp, cnt33, t;
                        subi     cnt1,cnt,1;

                        bgt      _notEnoughBitsDIFF;
                        add      v,cnt,t;

                        slw      cnt,cb,cnt1;
                        stw      v,info->cnt;
                        subfic   v,t,32;
                        srw      diff,cnt,v;
                    }

                    ASSERT (info->cnt <=33);
                    asm
                    {

                        b _DoneDIFF;
                      _notEnoughBitsDIFF:

                        lwz     tmp1, info->c;
                        slw     v, cb, cnt1;

                        lwzu    cb, 4(tmp1);
                        addi    tmp, tmp, 1;
                        stw     cb, info->currByte;
                        srw     cb, cb, cnt33;
                        stw     tmp1, info->c;

                        add     v, cb, v;

                        stw     tmp, info->cnt;

                        subfic  tmp, t,32;
                        srw     diff, v, tmp;
                  _DoneDIFF:
                    }
                    ASSERT (info->cnt <=33);

                }
            if (__cntlzw((u32)diff) > 32 - t)
            {
                diff += ((0xFFFFFFFF << t) + 1);
            }
        };
        __dcbz((void*)block, 96);

        dc       = (s16)(info->components[0].predDC + diff);
        block[0] = info->components[0].predDC = dc;
    }

    {
        register s32            k;
        register s32            code;
        register u32            cnt;
        register u32            cb;
        register u32            increment;
        register s32            tmp;
        register THPHuffmanTab *h = Yachuff;
#ifdef DEBUG_INLINE_DECODE
        u32 incsave;
#endif

#define cnt4 code
        asm
        {
            lwz     cnt, info->cnt;

            addi    increment, h, 32;
            lwz     cb, info->currByte;
        }
#ifdef DEBUG_INLINE_DECODE
        incsave = increment;
#endif

        for( k = 1; k < THP_DCTSIZE; k++)
        {

            register s32 ssss;
            register s32 rrrr;
#ifdef DEBUG_INLINE_DECODE
            csave = info->c;
#endif

            asm
            {
                addi    cnt4, cnt, 4;
                cmpwi   cnt, 28;
                rlwnm   tmp, cb, cnt4, 27, 31;
                bgt     _notEnoughBits;

                lbzx    ssss, h, tmp;
                lbzx    code, increment, tmp;
                cmpwi   ssss, 0xFF;

                beq     _FailedCheckEnoughBits;
                add     cnt, cnt, code;
                b       _DoneDecodeTab;
            }
            {
                register u32 maxcodebase;
                register u32 tmp2;

              _FailedCheckEnoughBits:
#ifdef DEBUG_INLINE_DECODE
                TABSTATE = FAILEDCHECKENOUGHBITS;
#endif
                cnt += 5;
                maxcodebase = (u32)&(h->maxCode);
                asm
                {
                    li          tmp2, sizeof(s32)*(5);
                    li          code, 5;
                    add         maxcodebase, maxcodebase, tmp2;
                  __WHILE_START:
                    cmpwi       cnt, 33;
                    slwi        tmp, tmp, 1

                    beq         _FCEB_faster;
                    rlwnm       ssss, cb, cnt, 31, 31;
                    lwzu        tmp2, 4(maxcodebase);
                    or          tmp, tmp, ssss
                    addi        cnt, cnt, 1;
                    b __WHILE_CHECK;

                  _FCEB_faster:

                    lwz     ssss, info->c;
                    li      cnt, 1;
                    lwzu    cb, 4(ssss);

                    lwzu    tmp2, 4(maxcodebase);

                    stw     ssss, info->c;
                    rlwimi  tmp, cb, 1,31,31;
                    b __FL_WHILE_CHECK;

                  __FL_WHILE_START:
                    slwi    tmp, tmp, 1;

                    rlwnm   ssss, cb, cnt, 31, 31;
                    lwzu    tmp2, 4(maxcodebase);
                    or      tmp, tmp, ssss;

                  __FL_WHILE_CHECK:
                    cmpw    tmp,tmp2
                    addi    cnt, cnt, 1;
                    addi    code, code, 1
                    bgt     __FL_WHILE_START;
                    b _FCEB_Done;

                  __WHILE_CHECK:
                    cmpw    tmp,tmp2
                    addi    code, code, 1
                    bgt     __WHILE_START;
                }
            }
          _FCEB_Done:
            ssss= (h->Vij[(s32)(tmp + h->valPtr[code])]);
            goto _DoneDecodeTab;

          _notEnoughBits:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = NOTENOUGHBITS;
#endif
            asm
            {
                cmpwi   cnt, 33;
                lwz     tmp, info->c;
                beq     _getfullword;

                cmpwi   cnt, 32;
                rlwnm   code, cb, cnt4, 27, 31
                beq     _1bitleft;

                lbzx    ssss, h, code;
                lbzx    rrrr, increment, code;

                cmpwi   ssss, 0xFF;
                add     code, cnt, rrrr;
                beq _FailedCheckNoBits0;

                cmpwi   code, 33;
                bgt     _FailedCheckNoBits1;
            }
            cnt = (u32)code;
            goto _DoneDecodeTab;

          _getfullword:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = GETFULLWORD;
#endif
            {

                asm
                {
                    lwzu    cb, 4(tmp);

                    rlwinm  code, cb, 5, 27, 31
                    stw     tmp, info->c;
                    lbzx    ssss, h, code;
                    lbzx    tmp, increment, code;
                    cmpwi   ssss, 0xFF
                    addi    cnt, tmp, 1
                    beq     _FailedCheckEnoughbits_Updated;

                }
            }
            goto _DoneDecodeTab;

          _FailedCheckEnoughbits_Updated:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = FAILEDCHECKENOUGHBITSUPDATED;
#endif
             ssss = 5;
            do
            {

                asm
                {
                    subfic  tmp, ssss, 31;
                    addi    ssss, ssss, 1;
                    srw     code, cb, tmp;
                }
            }     while (code > h->maxCode[ssss]);
            cnt = (u32)(ssss+1);
            ssss = (h->Vij[(s32)(code + h->valPtr[ssss])]);

            goto _DoneDecodeTab;

          _1bitleft:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = ONEBITLEFT;
#endif
            asm
            {
                lwzu    cb, 4(tmp);

                stw     tmp, info->c;
                rlwimi  code, cb, 4, 28, 31;
                lbzx    ssss, h, code;
                lbzx    cnt, increment, code
                cmpwi   ssss, 0xFF

                beq     _DammitRead4;

            }

            goto _DoneDecodeTab;

          _DammitRead4:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = READ4;
#endif

            {
                register u32 maxcodebase = (u32)&(h->maxCode);
                register u32 tmp2;

                asm
                {
                    li  cnt, sizeof(s32)*5;
                    add     maxcodebase, maxcodebase, cnt;

                    slwi    tmp, code, 32-5;
                    li      cnt,5;
                    rlwimi  tmp, cb, 32-1, 1,31;

                  __DR4_WHILE_START:

                    subfic  ssss, cnt, 31;
                    lwzu    tmp2, 4(maxcodebase);
                    srw     code, tmp, ssss;
                  __DR4_WHILE_CHECK:
                    cmpw    code, tmp2
                    addi    cnt, cnt, 1
                    bgt     __DR4_WHILE_START;

                }
            }
            ssss =  (h->Vij[(s32)(code + h->valPtr[cnt])]);
            goto _DoneDecodeTab;

          _FailedCheckNoBits0:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = FAILEDCHECKNOBITS0;
            goto           _REALFAILEDCHECKNOBITS;
#endif
          _FailedCheckNoBits1:
#ifdef DEBUG_INLINE_DECODE
            TABSTATE = FAILEDCHECKNOBITS1;
#endif
          _REALFAILEDCHECKNOBITS:
            {
                register u32 mask = 0xFFFFFFFF << (33 - cnt);
                register u32 tmp2;
                register u32 tmp3;

                code = (s32)(cb & (~mask));
                mask = (u32)&(h->maxCode);

                asm
                {
                    lwz     tmp, info->c;
                    subfic  tmp2, cnt, 33;
                    addi    tmp3, tmp2, 1;
                    slwi    tmp2, tmp2, 2;
                    lwzu    cb, 4(tmp);
                    add     mask,mask, tmp2;
                    stw     tmp, info->c;
                    slwi    code, code, 1;
                    rlwimi  code, cb, 1, 31, 31;
                    lwzu    tmp2, 4(mask);
                    li      cnt, 2;

                    b       __FCNB1_WHILE_CHECK;

                  __FCNB1_WHILE_START:
                    slwi    code, code, 1;

                    addi    tmp3, tmp3, 1;
                    lwzu    tmp2, 4(mask);
                    add     code, code, rrrr;
                    addi    cnt, cnt, 1;

                  __FCNB1_WHILE_CHECK:
                    cmpw    code, tmp2;
                    rlwnm   rrrr, cb, cnt, 31, 31;
                    bgt     __FCNB1_WHILE_START;

                }
                ssss= (h->Vij[(s32)(code + h->valPtr[tmp3])]);
            }

            goto _DoneDecodeTab;

          _DoneDecodeTab:
#ifdef DEBUG_INLINE_DECODE
            info->c = csave;

            if (ssss != (code =__THPHuffDecodeTab(info,h)))
            {
                OSReport("ssss %d code %d cnt %d their cnt %d\n",
                         ssss, code, cnt, info->cnt);
            }
            else if (cnt != info->cnt)
            {
                OSReport("\ncnt %d info->cnt %d\n", cnt, info->cnt);
            }

            if (incsave != increment)
            {
                OSReport("increment trampled\n");
            }

            OSReport("<%d>", TABSTATE);

            info->cnt = cnt;
            info->currByte = cb;
#endif

            asm
            {
                andi.   rrrr, ssss, 15;
                srawi   ssss, ssss, 4;
                beq     _RECV_SSSS_ZERO;
            }

            {
                k   += ssss;
                {
                    register s32    v;

                    #define cnt33 code

                    register u32    cnt1;
                    register u32 tmp1;
                    asm
                    {

                        subfic   cnt33,cnt,33;

                        subfc. tmp, cnt33, rrrr;
                        subi     cnt1,cnt,1;

                        bgt      _RECVnotEnoughBits;
                        add      cnt,cnt,rrrr;

                        slw      tmp1,cb,cnt1;

                        subfic   v,rrrr,32;
                        srw      ssss,tmp1,v;
                    }
                    asm
                    {

                        b _RECVDone;
                      _RECVnotEnoughBits:

                        lwz     tmp1, info->c;
                        slw     v, cb, cnt1;

                        lwzu    cb, 4(tmp1);
                        addi    cnt, tmp, 1;

                        stw     tmp1, info->c;
                        srw     tmp1, cb, cnt33;

                        add     v, tmp1, v;

                        subfic  tmp, rrrr,32;
                        srw     ssss, v, tmp;
                  _RECVDone:
                    }
                }

                #undef cnt33

                if (__cntlzw((u32)ssss) > 32 - rrrr)
                {
                    ssss += ((0xFFFFFFFF << rrrr) + 1);
                }
                block[__THPJpegNaturalOrder[k]] = (s16)ssss;
                goto _RECV_END;

            }

            {
              _RECV_SSSS_ZERO:
                if (ssss != 15) break;
                k += 15;
            };
#ifdef SNSYS
          _RECV_END:
#else
            asm
            {
              _RECV_END:
            }
#endif

#ifdef DEBUG_INLINE_DECODE
            info->cnt = cnt;
            info->currByte = cb;
#endif

        }
        info->cnt      = cnt;
        info->currByte = cb;
    }
#undef cnt4

}

#else
static void
__THPHuffDecodeDCTCompY(
    register THPFileInfo       *info,
    THPCoeff                   *block
)
{
    register s32       t;
    register THPCoeff  diff;
    THPCoeff           dc;

    register s32       v;
    register u32       cb;
    register u32       cnt;
    register u32       cnt33;
    register u32       tmp;
    register u32       cnt1;
    register u32       tmp1;

    register s32       k;
    register s32       ssss;
    register s32       rrrr;

    __dcbz((void*)block, 0);

    t = __THPHuffDecodeTab(info, Ydchuff);
    __dcbz((void*)block, 32);

    diff  = THP_ZERO;
    __dcbz((void*)block, 64);

    if (t)
    {
        ASSERT (info->cnt <=33);

        asm
        {
            lwz      cnt,info->cnt;
            subfic   cnt33,cnt,33;
            lwz      cb,info->currByte;

            subfc. tmp, cnt33, t;
            subi     cnt1,cnt,1;

            bgt      _notEnoughBitsDIFF;
            add      v,cnt,t;

            slw      cnt,cb,cnt1;
            stw      v,info->cnt;
            subfic   v,t,32;
            srw      diff,cnt,v;
        }

        ASSERT (info->cnt <=33);

        asm
        {
            b _DoneDIFF;
        _notEnoughBitsDIFF:

            lwz     tmp1, info->c;
            slw     v, cb, cnt1;

            lwzu    cb, 4(tmp1);
            addi    tmp, tmp, 1;
            stw     cb, info->currByte;
            srw     cb, cb, cnt33;
            stw     tmp1, info->c;

            add     v, cb, v;

            stw     tmp, info->cnt;

            subfic  tmp, t,32;
            srw     diff, v, tmp;
        _DoneDIFF:
        }

        ASSERT (info->cnt <=33);

        if (__cntlzw((u32)diff) > 32 - t)
        {
            diff += ((0xFFFFFFFF << t) + 1);
        }
    }

    __dcbz((void*)block, 96);

    dc       = (s16)(info->components[0].predDC + diff);
    block[0] = info->components[0].predDC = dc;

    for (k = 1; k < THP_DCTSIZE; k++)
    {
        ssss = __THPHuffDecodeTab(info, Yachuff);
        rrrr = ssss >> 4;
        ssss &= 15;

        if (ssss)
        {
            k += rrrr;
            asm
            {
                lwz      cnt,info->cnt;
                subfic   cnt33,cnt,33;
                lwz      cb,info->currByte;

                subf. tmp, cnt33, ssss;
                subi     cnt1,cnt,1;

                bgt      _notEnoughBits;
                add      v,cnt,ssss;

                slw      cnt,cb,cnt1;
                stw      v,info->cnt;
                subfic   v,ssss,32;
                srw      rrrr,cnt,v;
            }

            ASSERT (info->cnt <=33);

            asm
            {
                b _Done;
            _notEnoughBits:

                lwz     tmp1, info->c;
                slw     v, cb, cnt1;

                lwzu    cb, 4(tmp1);
                addi    tmp, tmp, 1;
                stw     cb, info->currByte;
                srw     cb, cb, cnt33;
                stw     tmp1, info->c;

                add     v, cb, v;

                stw     tmp, info->cnt;

                subfic  tmp, ssss,32;
                srw     rrrr, v, tmp;
            _Done:
            }

            ASSERT (info->cnt <=33);

            if (__cntlzw((u32)rrrr) > 32 - ssss)
            {
                rrrr += ((0xFFFFFFFF << ssss) + 1);
            }

            block[__THPJpegNaturalOrder[k]] = (s16)rrrr;
        }

        else
        {
            if (rrrr != 15) break;
            k += 15;
        }
    }
}
#endif

#ifndef _DEBUG
static inline void
__THPHuffDecodeDCTCompU(
    register THPFileInfo     *info,
    THPCoeff                 *block
)
#else
static void
__THPHuffDecodeDCTCompU(
    register THPFileInfo     *info,
    THPCoeff                 *block
)
#endif
{
    register s32       t;
    register THPCoeff  diff;
    THPCoeff           dc;

    register s32       v;
    register u32       cb;
    register u32       cnt;
    register u32       cnt33;
    register u32       tmp;
    register u32       cnt1;
    register u32       tmp1;

    register s32       k;
    register s32       ssss;
    register s32       rrrr;

    __dcbz((void*)block, 0);

    t = __THPHuffDecodeTab(info, Udchuff);
    __dcbz((void*)block, 32);

    diff  = THP_ZERO;
    __dcbz((void*)block, 64);

    if (t)
    {
        ASSERT (info->cnt <=33);

        asm
        {
            lwz      cnt,info->cnt;
            subfic   cnt33,cnt,33;
            lwz      cb,info->currByte;

            subfc. tmp, cnt33, t;
            subi     cnt1,cnt,1;

            bgt      _notEnoughBitsDIFF;
            add      v,cnt,t;

            slw      cnt,cb,cnt1;
            stw      v,info->cnt;
            subfic   v,t,32;
            srw      diff,cnt,v;
        }

        ASSERT (info->cnt <=33);

        asm
        {
            b _DoneDIFF;
        _notEnoughBitsDIFF:

            lwz     tmp1, info->c;
            slw     v, cb, cnt1;

            lwzu    cb, 4(tmp1);
            addi    tmp, tmp, 1;
            stw     cb, info->currByte;
            srw     cb, cb, cnt33;
            stw     tmp1, info->c;

            add     v, cb, v;

            stw     tmp, info->cnt;

            subfic  tmp, t,32;
            srw     diff, v, tmp;
        _DoneDIFF:
        }

        ASSERT (info->cnt <=33);

        if (__cntlzw((u32)diff) > 32 - t)
        {
            diff += ((0xFFFFFFFF << t) + 1);
        }
    }

    __dcbz((void*)block, 96);

    dc       = (s16)(info->components[1].predDC + diff);
    block[0] = info->components[1].predDC = dc;

    for (k = 1; k < THP_DCTSIZE; k++)
    {
        ssss = __THPHuffDecodeTab(info, Uachuff);
        rrrr = ssss >> 4;
        ssss &= 15;

        if (ssss)
        {
            k += rrrr;
            asm
            {
                lwz      cnt,info->cnt;
                subfic   cnt33,cnt,33;
                lwz      cb,info->currByte;

                subf. tmp, cnt33, ssss;
                subi     cnt1,cnt,1;

                bgt      _notEnoughBits;
                add      v,cnt,ssss;

                slw      cnt,cb,cnt1;
                stw      v,info->cnt;
                subfic   v,ssss,32;
                srw      rrrr,cnt,v;
            }

            ASSERT (info->cnt <=33);

            asm
            {
                b _Done;
            _notEnoughBits:

                lwz     tmp1, info->c;
                slw     v, cb, cnt1;

                lwzu    cb, 4(tmp1);
                addi    tmp, tmp, 1;
                stw     cb, info->currByte;
                srw     cb, cb, cnt33;
                stw     tmp1, info->c;

                add     v, cb, v;

                stw     tmp, info->cnt;

                subfic  tmp, ssss,32;
                srw     rrrr, v, tmp;
            _Done:
            }

            ASSERT (info->cnt <=33);

            if (__cntlzw((u32)rrrr) > 32 - ssss)
            {
                rrrr += ((0xFFFFFFFF << ssss) + 1);
            }

            block[__THPJpegNaturalOrder[k]] = (s16)rrrr;
        }

        else
        {
            if (rrrr != 15) break;
            k += 15;
        }
    }
}

#ifndef _DEBUG
static inline void
__THPHuffDecodeDCTCompV(
    register THPFileInfo  *info,
    THPCoeff              *block
)
#else
static void
__THPHuffDecodeDCTCompV(
    register THPFileInfo  *info,
    THPCoeff              *block
)
#endif
{
    register s32       t;
    register THPCoeff  diff;
    THPCoeff           dc;

    register s32       v;
    register u32       cb;
    register u32       cnt;
    register u32       cnt33;
    register u32       tmp;
    register u32       cnt1;
    register u32       tmp1;

    register s32       k;
    register s32       ssss;
    register s32       rrrr;

    __dcbz((void*)block, 0);

    t = __THPHuffDecodeTab(info, Vdchuff);
    __dcbz((void*)block, 32);

    diff  = THP_ZERO;
    __dcbz((void*)block, 64);

    if (t)
    {
        ASSERT (info->cnt <=33);

        asm
        {
            lwz      cnt,info->cnt;
            subfic   cnt33,cnt,33;
            lwz      cb,info->currByte;

            subf. tmp, cnt33, t;
            subi     cnt1,cnt,1;

            bgt      _notEnoughBitsDIFF;
            add      v,cnt,t;

            slw      cnt,cb,cnt1;
            stw      v,info->cnt;
            subfic   v,t,32;
            srw      diff,cnt,v;
        }

        ASSERT (info->cnt <=33);

        asm
        {
            b _DoneDIFF;
        _notEnoughBitsDIFF:

            lwz     tmp1, info->c;
            slw     v, cb, cnt1;

            lwzu    cb, 4(tmp1);
            addi    tmp, tmp, 1;
            stw     cb, info->currByte;
            srw     cb, cb, cnt33;
            stw     tmp1, info->c;

            add     v, cb, v;

            stw     tmp, info->cnt;

            subfic  tmp, t,32;
            srw     diff, v, tmp;
        _DoneDIFF:
        }

        ASSERT (info->cnt <=33);

        if (__cntlzw((u32)diff) > 32 - t)
        {
            diff += ((0xFFFFFFFF << t) + 1);
        }
    }

    __dcbz((void*)block, 96);

    dc       = (s16)(info->components[2].predDC + diff);
    block[0] = info->components[2].predDC = dc;

    for (k = 1; k < THP_DCTSIZE; k++)
    {
        ssss = __THPHuffDecodeTab(info, Vachuff);
        rrrr = ssss >> 4;
        ssss &= 15;

        if (ssss)
        {
            k += rrrr;

            asm
            {
                lwz      cnt,info->cnt;
                subfic   cnt33,cnt,33;
                lwz      cb,info->currByte;

                subf. tmp, cnt33, ssss;
                subi     cnt1,cnt,1;

                bgt      _notEnoughBits;
                add      v,cnt,ssss;

                slw      cnt,cb,cnt1;
                stw      v,info->cnt;
                subfic   v,ssss,32;
                srw      rrrr,cnt,v;
            }

            ASSERT (info->cnt <=33);

            asm
            {
                b _Done;
            _notEnoughBits:

                lwz     tmp1, info->c;
                slw     v, cb, cnt1;

                lwzu    cb, 4(tmp1);
                addi    tmp, tmp, 1;
                stw     cb, info->currByte;
                srw     cb, cb, cnt33;
                stw     tmp1, info->c;

                add     v, cb, v;

                stw     tmp, info->cnt;

                subfic  tmp, ssss,32;
                srw     rrrr, v, tmp;
            _Done:
            }

            ASSERT (info->cnt <=33);

            if (__cntlzw((u32)rrrr) > 32 - ssss)
            {
                rrrr += ((0xFFFFFFFF << ssss) + 1);

            }

            block[__THPJpegNaturalOrder[k]] = (s16)rrrr;
        }

        else
        {
            if (rrrr != 15) break;
            k += 15;
        }
    }
}

BOOL THPInit(void)
{
    u8      *base;

    OSRegisterVersion(__THPVersion);

    base = LCGetBase();

    __THPLCWork512[THP_MCU_Y_OUT] = base;
    base += THP_MCU512_Y_SIZE;
    __THPLCWork512[THP_MCU_U_OUT] = base;
    base += THP_MCU512_UV_SIZE;
    __THPLCWork512[THP_MCU_V_OUT] = base;
    base += THP_MCU512_UV_SIZE;

    base = LCGetBase();

    __THPLCWork672[THP_MCU_Y_OUT] = base;
    base += THP_MCU672_Y_SIZE;
    __THPLCWork672[THP_MCU_U_OUT] = base;
    base += THP_MCU672_UV_SIZE;
    __THPLCWork672[THP_MCU_V_OUT] = base;
    base += THP_MCU672_UV_SIZE;

    OSInitFastCast();

    __THPInitFlag = TRUE;

    return TRUE;
}
