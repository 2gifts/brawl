#include "revolution/wpad.h"
#include <cstring>
#include <cmath>

// HID input report parsing for the wiimote (WPAD). reports 0x20..0x3f come in here from the
// bluetooth stack, each one fills the rx buffer that the game is NOT currently reading and
// then flips rxBufIndex.
// HYPOTHESIS: most of the structure/names are from a hardware-diag build of the same library,
// every function below was checked against the dol asm for shape but not all are byte exact.

extern void DEBUGPrint(const char *msg, ...);
extern BOOL __OSInIPL;

void __a1_20_status_report(u8 chan, u8 *data);
void __a1_21_user_data(u8 chan, u8 *data);
void __a1_22_ack(u8 chan, u8 *data);
void __a1_30_data_type(u8 chan, u8 *data);
void __a1_31_data_type(u8 chan, u8 *data);
void __a1_32_data_type(u8 chan, u8 *data);
void __a1_33_data_type(u8 chan, u8 *data);
void __a1_34_data_type(u8 chan, u8 *data);
void __a1_35_data_type(u8 chan, u8 *data);
void __a1_36_data_type(u8 chan, u8 *data);
void __a1_37_data_type(u8 chan, u8 *data);
void __a1_3d_data_type(u8 chan, u8 *data);
void __a1_3e_data_type(u8 chan, u8 *data);
void __a1_3f_data_type(u8 chan, u8 *data);
void __a1_unused_report(u8 chan, u8 *data);

static void initExtension(s32 chan);
static void getDevConfig(s32 chan, s32 result);
static void getGameInfo(s32 chan, s32 result, u8 index);
static void getExtType(s32 chan, s32 result);
static void getExtConfig(s32 chan, s32 result);
static BOOL isExtEncMain(u8 type);

// constants that are private in the original headers (values pinned by the asm)
#define WPAD_FS_STK_MAX 127
#define WPAD_FS_STK_MIN -128
#define WPAD_CL_STK_MAX 511
#define WPAD_CL_STK_MIN -512
#define WPAD_CL_STICK_RESO 1024
#define WPAD_DPD_EXP 3
#define WPAD_DPD_STD 1
#define WPAD_DPD_DUMMY_SIZE 12
#define WPAD_RX_SIZE WPAD_RX_DATASIZE

#define WPAD_FIRST_INPUT_REPORT (u8)(0x20)
#define WPAD_LAST_INPUT_REPORT (u8)(0x3f)

#define WPAD_DEV_MODE_CLASSIC1 1
#define WPAD_DEV_MODE_CLASSIC2 2
#define WPAD_DEV_MODE_CLASSIC3 3

#define WPAD_EXTENC_UNKNOWN 0
#define WPAD_EXTENC_DISABLE 1
#define WPAD_EXTENC_MAIN 2
#define WPAD_EXTENC_SUB 3

#define WPAD_CONF_CORE_ADDR (u32)0x00000000
#define WPAD_CONF_CORE_ADDR_OLD (u32)0x0000176c
#define WPAD_CONF_CORE_DPD_SIZE 10
#define WPAD_CONF_CORE_ACC_SIZE 9
#define WPAD_CONF_EXT_ADDR (u32)0x04a40020
#define WPAD_CONF_EXT_ORG 14
#define WPAD_CONF_EXT_TYPE (u32)0x04a400fa
#define WPAD_MEM_GAMEDATA_ADDR 0x0000002A

#define HID_WPAD_BUTTON_MASK (u16)(0x9F1F)
#define HID_NEAREMPTY_MASK (u8)(0x80)
#define HID_DPD_MASK (u8)(0x08)
#define HID_SPEAKER_MASK (u8)(0x04)
#define HID_ATTACH_MASK (u8)(0x02)
#define HID_LOWBAT_MASK (u8)(0x01)
#define HID_LED_MASK (u8)(0xf0)
#define HID_FW_MASK (u8)(0xf0)
#define HID_PRT_MASK (u8)(0x0f)
#define HID_DPDX98_MASK (u8)(0x30)
#define HID_DPDY98_MASK (u8)(0xC0)
#define HID_DPD_SIZE_MASK (u8)(0x0F)

// index into the report buffer, named after the byte number counting from 1 (index 0 is the report id)

#define B7 1
#define B8 2
#define B9 3
#define B10 4
#define B11 5
#define B12 6
#define B13 7
#define B14 8
#define B15 9
#define B16 10
#define B17 11
#define B18 12
#define B19 13
#define B20 14
#define B21 15
#define B22 16
#define B23 17
#define B24 18
#define B25 19
#define B26 20
#define B27 21

void (*__a1_input_reports_array[])(u8 chan, u8 *data) = {
    __a1_20_status_report, __a1_21_user_data, __a1_22_ack,

    __a1_unused_report, // 0x23
    __a1_unused_report, // 0x24
    __a1_unused_report, // 0x25
    __a1_unused_report, // 0x26
    __a1_unused_report, // 0x27
    __a1_unused_report, // 0x28
    __a1_unused_report, // 0x29
    __a1_unused_report, // 0x2a
    __a1_unused_report, // 0x2b
    __a1_unused_report, // 0x2c
    __a1_unused_report, // 0x2d
    __a1_unused_report, // 0x2e
    __a1_unused_report, // 0x2f
    __a1_30_data_type,
    __a1_31_data_type,
    __a1_32_data_type,
    __a1_33_data_type,
    __a1_34_data_type,
    __a1_35_data_type,
    __a1_36_data_type,
    __a1_37_data_type,
    __a1_unused_report, // 0x38
    __a1_unused_report, // 0x39
    __a1_unused_report, // 0x3a
    __a1_unused_report, // 0x3b
    __a1_unused_report, // 0x3c
    __a1_3d_data_type,
    __a1_3e_data_type,
    __a1_3f_data_type
};

static const f32 PI = 3.141592f;

static u8 _devType[WPAD_MAX_CONTROLLERS];
static u8 _devMode[WPAD_MAX_CONTROLLERS];
static u8 _devCmpt[WPAD_MAX_CONTROLLERS];
static u8 _retryCnt[WPAD_MAX_CONTROLLERS];

static u8 checkInvalidData[21] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
static u8 checkBuffer[21];

static f32 rolag[WPAD_MAX_CONTROLLERS];
static f32 calibX[WPAD_MAX_CONTROLLERS];
static f32 calibY[WPAD_MAX_CONTROLLERS];
static f32 centerX[WPAD_MAX_CONTROLLERS];
static f32 centerY[WPAD_MAX_CONTROLLERS];

// 10 bit x/y from the dpd bytes: low 8 bits in one byte, top 2 bits in the packed byte
#define DPD_X(lo, hi) (s16)((s16)((s16)(((u16)(lo)) & 0x00ff) | (u16)(((u16)((hi) & HID_DPDX98_MASK)) << 4)))
#define DPD_Y(lo, hi) (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(lo)) & 0x00ff) | (u16)(((u16)((hi) & HID_DPDY98_MASK)) << 2)))

#define BUTTONS(d) (u16)((u16)(((u16)((u16)((d)[B7]) & 0x00ff) | (u16)((u16)((d)[B8] << 8) & 0xff00))) & (u16)HID_WPAD_BUTTON_MASK)

// 10 bit accel with the 2 low bits stuffed in the button bytes
#define ACC_X(d, cb) (s16)((s16)((s16)((s16)((s16)((s16)((d)[B9]) << 2) & (s16)0xFFFC) | (s16)((s16)((u16)((d)[B7] >> 5)) & (s16)0x0003))) - (s16)((cb)->conf.acc_0g.x))
#define ACC_Y(d, cb) (s16)((s16)((s16)((s16)((s16)((s16)((d)[B10]) << 2) & (s16)0xFFFC) | (s16)((s16)((u16)((d)[B8] >> 4)) & (s16)0x0002))) - (s16)((cb)->conf.acc_0g.y))
#define ACC_Z(d, cb) (s16)((s16)((s16)((s16)((s16)((s16)((d)[B11]) << 2) & (s16)0xFFFC) | (s16)((s16)((u16)((d)[B8] >> 5)) & (s16)0x0002))) - (s16)((cb)->conf.acc_0g.z))

// nunchuk accel (3 bytes + 2 bits each packed in the last byte)
#define FS_ACC(d, ia, ib, sh, ref) (s16)((s16)((s16)((s16)((s16)((d)[ia]) << 2) & (s16)0xFFFC) | (s16)((s16)((d)[ib] >> (sh)) & (s16)(0x0003))) - (s16)(ref))

#define NEXT_RXBUF(cb) (cb)->rxBufIndex = (u8)(((cb)->rxBufIndex) ? 0 : 1)

static void abortInitExtension(s32 chan, s32 result) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    u32 type;

    if (result != WPAD_ERR_NONE) {
        WPADiClearQueue(&p_wpd->extq);
        p_wpd->extEnc = WPAD_EXTENC_UNKNOWN;

        if (result == WPAD_ERR_NO_CONTROLLER) {
            type = WPAD_DEV_NOT_FOUND;
        } else if (p_wpd->infos.attach) {
            if (_retryCnt[chan]++ < 32) {
                initExtension(chan);
                return;
            }
            type = WPAD_DEV_NOT_SUPPORTED;
        } else {
            DEBUGPrint("detaching extension during initialization.\n");
            WPADiSendSetReportType(&p_wpd->extq, p_wpd->dataFormat, p_wpd->pwrSave, NULL);
            return;
        }

        p_wpd->devType = (u8)type;
        if (p_wpd->extensionCallback) {
            p_wpd->extensionCallback(chan, (s32)type);
        }
    }
}

static void getDevConfig(s32 chan, s32 result) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    u8 *p_buf = (u8 *)p_wpd->readBufPtr;
    u8 checkSum;
    u8 i;
    int j;
    int index;
    DPDObject obj[WPAD_DPD_MAX_OBJECTS];
    const s16 dummyObjX[] = {127, 896, 896, 127};
    const s16 dummyObjY[] = {93, 93, 674, 674};
    f32 difaveX;
    f32 difaveY;
    f32 deltaX;
    f32 deltaY;
    f32 x[WPAD_DPD_MAX_OBJECTS];
    f32 y[WPAD_DPD_MAX_OBJECTS];
    f32 ag[WPAD_DPD_MAX_OBJECTS];
    f32 ag_org[WPAD_DPD_MAX_OBJECTS];

    // start from fallback values in case the saved calibration is bad
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        p_wpd->conf.obj[i].x = dummyObjX[i];
        p_wpd->conf.obj[i].y = dummyObjY[i];
        p_wpd->conf.obj[i].size = (u16)p_wpd->dpdDummyObjSize;
        p_wpd->conf.obj[i].traceId = i;
    }

    p_wpd->conf.acc_0g.x = 530;
    p_wpd->conf.acc_0g.y = 530;
    p_wpd->conf.acc_0g.z = 530;
    p_wpd->conf.acc_1g.x = 106 + 530;
    p_wpd->conf.acc_1g.y = 106 + 530;
    p_wpd->conf.acc_1g.z = 106 + 530;
    p_wpd->conf.volume = 0;
    p_wpd->conf.motor = 0;

    if (result != WPAD_ERR_NONE) {
        return;
    }

    if (!p_wpd->oldFw) {
        for (i = 0; i < 2; i++) {
            checkSum = 0;
            index = i * (WPAD_CONF_CORE_DPD_SIZE + 1);
            for (j = index; j < index + WPAD_CONF_CORE_DPD_SIZE; j++) {
                checkSum += p_buf[j];
            }
            checkSum = (u8)(checkSum + 0x55);

            if (checkSum == p_buf[index + WPAD_CONF_CORE_DPD_SIZE]) {
                DEBUGPrint("Dpd Setting is ok.\n");

                obj[0].x = DPD_X(p_buf[index], p_buf[index + 2]);
                obj[0].y = DPD_Y(p_buf[index + 1], p_buf[index + 2]);
                obj[0].size = (u16)p_wpd->dpdDummyObjSize;
                obj[0].traceId = 0;

                obj[1].x = (s16)((s16)(((u16)(p_buf[index + 3])) & 0x00ff) | (u16)(((u16)(p_buf[index + 2] & (u8)(HID_DPDX98_MASK >> 4))) << 8));
                obj[1].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(p_buf[index + 4])) & 0x00ff) | (u16)(((u16)(p_buf[index + 2] & (u8)(HID_DPDY98_MASK >> 4))) << 6)));
                obj[1].size = (u16)p_wpd->dpdDummyObjSize;
                obj[1].traceId = 1;

                obj[2].x = DPD_X(p_buf[index + 5], p_buf[index + 7]);
                obj[2].y = DPD_Y(p_buf[index + 6], p_buf[index + 7]);
                obj[2].size = (u16)p_wpd->dpdDummyObjSize;
                obj[2].traceId = 2;

                obj[3].x = (s16)((s16)(((u16)(p_buf[index + 8])) & 0x00ff) | (u16)(((u16)(p_buf[index + 7] & (u8)(HID_DPDX98_MASK >> 4))) << 8));
                obj[3].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(p_buf[index + 9])) & 0x00ff) | (u16)(((u16)(p_buf[index + 7] & (u8)(HID_DPDY98_MASK >> 4))) << 6)));
                obj[3].size = (u16)p_wpd->dpdDummyObjSize;
                obj[3].traceId = 3;

                // sort the four saved points into screen quadrants
                for (j = 0; j < WPAD_DPD_MAX_OBJECTS; j++) {
                    if ((obj[j].x < (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y < (s16)(WPAD_DPD_IMG_RESO_WY / 2))) {
                        p_wpd->conf.obj[0].x = obj[j].x;
                        p_wpd->conf.obj[0].y = obj[j].y;
                        p_wpd->conf.obj[0].size = obj[j].size;
                        p_wpd->conf.obj[0].traceId = obj[j].traceId;
                    }
                    if ((obj[j].x > (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y < (s16)(WPAD_DPD_IMG_RESO_WY / 2))) {
                        p_wpd->conf.obj[1].x = obj[j].x;
                        p_wpd->conf.obj[1].y = obj[j].y;
                        p_wpd->conf.obj[1].size = obj[j].size;
                        p_wpd->conf.obj[1].traceId = obj[j].traceId;
                    }
                    if ((obj[j].x > (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y > (s16)(WPAD_DPD_IMG_RESO_WY / 2))) {
                        p_wpd->conf.obj[2].x = obj[j].x;
                        p_wpd->conf.obj[2].y = obj[j].y;
                        p_wpd->conf.obj[2].size = obj[j].size;
                        p_wpd->conf.obj[2].traceId = obj[j].traceId;
                    }
                    if ((obj[j].x < (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y > (s16)(WPAD_DPD_IMG_RESO_WY / 2))) {
                        p_wpd->conf.obj[3].x = obj[j].x;
                        p_wpd->conf.obj[3].y = obj[j].y;
                        p_wpd->conf.obj[3].size = obj[j].size;
                        p_wpd->conf.obj[3].traceId = obj[j].traceId;
                    }
                }
                break;
            } else {
                DEBUGPrint("Dpd Setting is broken.\n");
            }
        }
    } else {
        DEBUGPrint("Old Firmware uses default values because it has no Dpd Setting.\n");
    }

    difaveX = difaveY = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        x[i] = (f32)(p_wpd->conf.obj[i].x);
        y[i] = (f32)(p_wpd->conf.obj[i].y);

        difaveX += p_wpd->conf.obj[i].x - dummyObjX[i];
        difaveY += p_wpd->conf.obj[i].y - dummyObjY[i];

        DEBUGPrint("x = %lf, y = %lf\n", x[i], y[i]);
    }

    difaveX = (f32)((f32)difaveX / (f32)4.0f);
    difaveY = (f32)((f32)difaveY / (f32)4.0f);

    // how far the whole point set is rotated/shifted vs. the nominal layout
    deltaX = (f32)atan((f32)((f32)((f32)126.5f + difaveX) / (f32)(1332.5f))) - (f32)atan((f32)((f32)126.5f / (f32)1332.5f));
    deltaY = (f32)atan((f32)((f32)((f32)93.0f + difaveY) / (f32)(1337.4f))) - (f32)atan((f32)((f32)93.0f / (f32)1337.4f));

    calibX[chan] = (f32)((f32)-1.0f * (f32)tan((f32)deltaX)) * (f32)(1332.5f);
    calibY[chan] = (f32)((f32)-1.0f * (f32)tan((f32)deltaY)) * (f32)(1337.4f);

    DEBUGPrint("difaveX = %lf, difaveY = %lf\n", difaveX, difaveY);
    DEBUGPrint("deltaX  = %lf, deltaY  = %lf\n", deltaX, deltaY);
    DEBUGPrint("calibX = %lf, calibY = %lf\n", calibX[chan], calibY[chan]);

    centerX[chan] = centerY[chan] = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        x[i] += calibX[chan];
        y[i] += calibY[chan];
        centerX[chan] += x[i];
        centerY[chan] += y[i];
    }

    centerX[chan] = (f32)((f32)centerX[chan] / (f32)4.0f);
    centerY[chan] = (f32)((f32)centerY[chan] / (f32)4.0f);

    DEBUGPrint("centX  = %lf, centY  = %lf\n", centerX[chan], centerY[chan]);

    rolag[chan] = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        ag[i] = (f32)atan((f32)(y[i] - centerY[chan]) / (f32)(x[i] - centerX[chan]));
        ag_org[i] = (f32)atan((f32)((f32)(dummyObjY[i]) - 383.5f) / (f32)((f32)(dummyObjX[i]) - 511.5f));
        rolag[chan] += ag[i] - ag_org[i];
    }

    rolag[chan] = (f32)((f32)rolag[chan] / (f32)4.0f);

    DEBUGPrint("rolag  = %lf\n", rolag[chan]);

    // accelerometer calibration, two copies saved, first one with a good checksum wins
    for (i = 0; i < 2; i++) {
        checkSum = 0;
        index = i * (WPAD_CONF_CORE_ACC_SIZE + 1);
        if (!p_wpd->oldFw) {
            index += 2 * (WPAD_CONF_CORE_DPD_SIZE + 1);
        }

        for (j = index; j < index + WPAD_CONF_CORE_ACC_SIZE; j++) {
            checkSum += p_buf[j];
        }
        checkSum = (u8)(checkSum + 0x55);

        if (checkSum == p_buf[index + WPAD_CONF_CORE_ACC_SIZE]) {
            DEBUGPrint("Acc is ok.\n");

            p_wpd->conf.acc_0g.x = (s16)((u16)(((u16)(p_buf[index + 0]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 4) & 0x0003));
            p_wpd->conf.acc_0g.y = (s16)((u16)(((u16)(p_buf[index + 1]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 2) & 0x0003));
            p_wpd->conf.acc_0g.z = (s16)((u16)(((u16)(p_buf[index + 2]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3])) & 0x0003));

            p_wpd->conf.acc_1g.x = (s16)((u16)(((u16)(p_buf[index + 4]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 4) & 0x0003));
            p_wpd->conf.acc_1g.y = (s16)((u16)(((u16)(p_buf[index + 5]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 2) & 0x0003));
            p_wpd->conf.acc_1g.z = (s16)((u16)(((u16)(p_buf[index + 6]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7])) & 0x0003));

            p_wpd->conf.volume = (u8)(p_buf[index + 8] & (u8)0x7F);
            p_wpd->conf.motor = (u8)(p_buf[index + 8] & (u8)0x80);
            break;
        } else {
            DEBUGPrint("Acc is broken.\n");
        }
    }

    DEBUGPrint("0G:  accX = %d,  accY = %d,  accZ = %d\n", p_wpd->conf.acc_0g.x, p_wpd->conf.acc_0g.y, p_wpd->conf.acc_0g.z);
    DEBUGPrint("1G:  accX = %d,  accY = %d,  accZ = %d\n", p_wpd->conf.acc_1g.x, p_wpd->conf.acc_1g.y, p_wpd->conf.acc_1g.z);
    DEBUGPrint("Volume: %d,   Motor: %d\n", p_wpd->conf.volume, p_wpd->conf.motor);
}

static void getExtConfig(s32 chan, s32 result) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    u8 *p_buf = (u8 *)p_wpd->readBufPtr;
    int i, j, k;
    int index = -1;
    int checkSum;

    switch (result) {
    case WPAD_ERR_NO_CONTROLLER:
        p_wpd->devType = WPAD_DEV_NOT_FOUND;
        break;
    case WPAD_ERR_NONE:
        p_wpd->devType = _devType[chan];
        break;
    default:
        p_wpd->devType = WPAD_DEV_NOT_SUPPORTED;
        break;
    }

    p_wpd->devMode = _devMode[chan];

    if (p_wpd->devType == WPAD_DEV_FREESTYLE || p_wpd->devType == WPAD_DEV_CLASSIC) {
        // two copies of the calibration block, 0x55/0xaa style checksums
        for (i = 0; i < 2; i++) {
            checkSum = 0;
            k = i * (WPAD_CONF_EXT_ORG + 2);
            for (j = k; j < k + WPAD_CONF_EXT_ORG; j++) {
                checkSum += p_buf[j];
            }
            if (((u8)(checkSum + 0x55) == p_buf[k + 14]) && ((u8)(checkSum + 0xaa) == p_buf[k + 15])) {
                index = k;
            }
        }

        switch (p_wpd->devType) {
        case WPAD_DEV_FREESTYLE:
            if (index < 0) {
                DEBUGPrint("check sum error.\n");
                p_wpd->extConf.fs.acc_0g.x = 512;
                p_wpd->extConf.fs.acc_0g.y = 512;
                p_wpd->extConf.fs.acc_0g.z = 512;
                p_wpd->extConf.fs.acc_1g.x = 716;
                p_wpd->extConf.fs.acc_1g.y = 716;
                p_wpd->extConf.fs.acc_1g.z = 716;
            } else {
                p_wpd->extConf.fs.acc_0g.x = (s16)((u16)(((u16)(p_buf[index + 0]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 4) & 0x0003));
                p_wpd->extConf.fs.acc_0g.y = (s16)((u16)(((u16)(p_buf[index + 1]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 2) & 0x0003));
                p_wpd->extConf.fs.acc_0g.z = (s16)((u16)(((u16)(p_buf[index + 2]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3])) & 0x0003));

                p_wpd->extConf.fs.acc_1g.x = (s16)((u16)(((u16)(p_buf[index + 4]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 4) & 0x0003));
                p_wpd->extConf.fs.acc_1g.y = (s16)((u16)(((u16)(p_buf[index + 5]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 2) & 0x0003));
                p_wpd->extConf.fs.acc_1g.z = (s16)((u16)(((u16)(p_buf[index + 6]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7])) & 0x0003));

                p_wpd->extConf.fs.stick.x_max = (s8)p_buf[index + 8];
                p_wpd->extConf.fs.stick.x_min = (s8)p_buf[index + 9];
                p_wpd->extConf.fs.stick.x = (s8)p_buf[index + 10];
                p_wpd->extConf.fs.stick.y_max = (s8)p_buf[index + 11];
                p_wpd->extConf.fs.stick.y_min = (s8)p_buf[index + 12];
                p_wpd->extConf.fs.stick.y = (s8)p_buf[index + 13];

                DEBUGPrint("0G:  fsaccX = %d,  fsaccY = %d,  fsaccZ = %d\n", p_wpd->extConf.fs.acc_0g.x, p_wpd->extConf.fs.acc_0g.y, p_wpd->extConf.fs.acc_0g.z);
                DEBUGPrint("1G:  fsaccX = %d,  fsaccY = %d,  fsaccZ = %d\n", p_wpd->extConf.fs.acc_1g.x, p_wpd->extConf.fs.acc_1g.y, p_wpd->extConf.fs.acc_1g.z);
                DEBUGPrint("FS:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.fs.stick.x, p_wpd->extConf.fs.stick.x_max, p_wpd->extConf.fs.stick.x_min);
                DEBUGPrint("FS:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.fs.stick.y, p_wpd->extConf.fs.stick.y_max, p_wpd->extConf.fs.stick.y_min);
            }
            break;

        case WPAD_DEV_CLASSIC:
            p_wpd->extConf.cl.lstk.x_max = (s8)p_buf[index + 0];
            p_wpd->extConf.cl.lstk.x_min = (s8)p_buf[index + 1];
            p_wpd->extConf.cl.lstk.x = (s8)p_buf[index + 2];
            p_wpd->extConf.cl.lstk.y_max = (s8)p_buf[index + 3];
            p_wpd->extConf.cl.lstk.y_min = (s8)p_buf[index + 4];
            p_wpd->extConf.cl.lstk.y = (s8)p_buf[index + 5];

            p_wpd->extConf.cl.rstk.x_max = (s8)p_buf[index + 6];
            p_wpd->extConf.cl.rstk.x_min = (s8)p_buf[index + 7];
            p_wpd->extConf.cl.rstk.x = (s8)p_buf[index + 8];
            p_wpd->extConf.cl.rstk.y_max = (s8)p_buf[index + 9];
            p_wpd->extConf.cl.rstk.y_min = (s8)p_buf[index + 10];
            p_wpd->extConf.cl.rstk.y = (s8)p_buf[index + 11];

            p_wpd->extConf.cl.triggerL = p_buf[index + 12];
            p_wpd->extConf.cl.triggerR = p_buf[index + 13];

            DEBUGPrint("CL:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.cl.lstk.x, p_wpd->extConf.cl.lstk.x_max, p_wpd->extConf.cl.lstk.x_min);
            DEBUGPrint("CL:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.cl.lstk.y, p_wpd->extConf.cl.lstk.y_max, p_wpd->extConf.cl.lstk.y_min);
            DEBUGPrint("CR:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.cl.rstk.x, p_wpd->extConf.cl.rstk.x_max, p_wpd->extConf.cl.rstk.x_min);
            DEBUGPrint("CR:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.cl.rstk.y, p_wpd->extConf.cl.rstk.y_max, p_wpd->extConf.cl.rstk.y_min);
            DEBUGPrint("LR:  L = %d,  R = %d\n", p_wpd->extConf.cl.triggerL, p_wpd->extConf.cl.triggerR);
            break;
        }

        p_wpd->isSetStickOrigin = 0;
    }

    if (p_wpd->resultCallback) {
        p_wpd->resultCallback(chan, p_wpd->devType);
        p_wpd->resultCallback = NULL;
    }
}

static BOOL isExtEncMain(u8 type) {
    if (type == WPAD_DEV_FREESTYLE || type == WPAD_DEV_CLASSIC) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static void getExtType(s32 chan, s32 result) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    WPADCmdQueue *cmdq = &p_wpd->extq;
    u8 *p_buf = (u8 *)p_wpd->readBufPtr;

    if (result == WPAD_ERR_NONE) {
        DEBUGPrint("devId: %d, subId: %d\n", p_buf[5], p_buf[0]);

        _devCmpt[chan] = 0;
        _devMode[chan] = p_buf[4];

        switch (p_buf[5]) {
        case 0x00:
            _devType[chan] = WPAD_DEV_FREESTYLE;
            break;
        case 0x01:
            _devType[chan] = WPAD_DEV_CLASSIC;
            break;
        case 0x03:
            switch (p_buf[0]) {
            default:
                // HYPOTHESIS: in the ipl an unknown id 3 device gets treated like a classic controller
                if (__OSInIPL) {
                    _devType[chan] = WPAD_DEV_CLASSIC;
                    _devMode[chan] = WPAD_DEV_MODE_CLASSIC1;
                    _devCmpt[chan] = 1;
                } else {
                    _devType[chan] = WPAD_DEV_FUTURE;
                }
                break;
            }
            break;
        default:
            _devType[chan] = WPAD_DEV_FUTURE;
            break;
        }

        if (_devType[chan] == WPAD_DEV_CLASSIC) {
            if (_devMode[chan] < WPAD_DEV_MODE_CLASSIC1 || _devMode[chan] > WPAD_DEV_MODE_CLASSIC3) {
                _devType[chan] = WPAD_DEV_NOT_SUPPORTED;
            }
        }

        DEBUGPrint("type : %d\n", _devType[chan]);
        DEBUGPrint("mode : %d\n", _devMode[chan]);

        if (_devType[chan] == WPAD_DEV_FUTURE || _devType[chan] == WPAD_DEV_NOT_SUPPORTED) {
            p_wpd->devType = _devType[chan];
            p_wpd->devMode = _devMode[chan];

            if (p_wpd->extensionCallback) {
                p_wpd->extensionCallback(chan, p_wpd->devType);
            }
        } else {
            // switch the extension into encrypted mode, then read its calibration block
            if (isExtEncMain(_devType[chan])) {
                p_wpd->extEnc = WPAD_EXTENC_MAIN;
                WPADiSendWriteDataCmd(cmdq, 0xaa, 0x04a400f0, abortInitExtension);
                WPADiCreateKey(chan);
                WPADiSendWriteData(cmdq, p_wpd->key, 6, 0x04a40040, abortInitExtension);
                WPADiSendWriteData(cmdq, p_wpd->key + 6, 6, 0x04a40046, abortInitExtension);
                WPADiSendWriteData(cmdq, p_wpd->key + 12, 4, 0x04a4004C, abortInitExtension);
                WPADiSendReadData(cmdq, p_wpd->readBuf, 32, 0x04a40020, p_wpd->extensionCallback);
            } else {
                p_wpd->extEnc = WPAD_EXTENC_SUB;
                WPADiSendWriteDataCmd(cmdq, 0xaa, 0x04a400f0, abortInitExtension);
                WPADiCreateKeyFor3rd(chan);
                WPADiSendWriteData(cmdq, p_wpd->key, 6, 0x04a40040, abortInitExtension);
                WPADiSendWriteData(cmdq, p_wpd->key + 6, 6, 0x04a40046, abortInitExtension);
                WPADiSendWriteData(cmdq, p_wpd->key + 12, 4, 0x04a4004C, abortInitExtension);
                WPADiSendReadData(cmdq, p_wpd->readBuf, 32, 0x04a40020, p_wpd->extensionCallback);
            }
        }
    } else {
        p_wpd->devType = WPAD_DEV_NOT_SUPPORTED;
        p_wpd->devMode = 0;
    }
}

static void getGameInfo(s32 chan, s32 result, u8 index) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    WPADMEMGameInfo *p_info = (WPADMEMGameInfo *)p_wpd->readBufPtr;
    u8 *p_buf = (u8 *)p_wpd->readBufPtr;
    int i;
    u8 sum = 0;

    if (result == WPAD_ERR_NONE) {
        for (i = 0; i < 47; i++) {
            sum += p_buf[i];
        }
        sum += 0x55;

        if (p_info->checkSum == sum) {
            memcpy(&p_wpd->gameInfo, p_buf, sizeof(WPADMEMGameInfo));
            p_wpd->gameInfoErr[index] = WPAD_ERR_NONE;
        } else {
            p_wpd->gameInfoErr[index] = WPAD_ERR_INVALID;
        }
    }
}

static void initExtension(s32 chan) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    u8 extcmd = 0;

    WPADiClearQueue(&p_wpd->extq);
    WPADiSendSetReportType(&p_wpd->extq, p_wpd->dataFormat, p_wpd->pwrSave, abortInitExtension);

    p_wpd->extEnc = WPAD_EXTENC_DISABLE;
    WPADiSendWriteDataCmd(&p_wpd->extq, 0x55, 0x04a400f0, abortInitExtension);
    WPADiSendWriteDataCmd(&p_wpd->extq, extcmd, 0x04a400fb, abortInitExtension);
    WPADiSendReadData(&p_wpd->extq, p_wpd->readBuf, 6, 0x04a400fa, abortInitExtension);
}

s32 WPADiHIDParser(u8 chan, u8 *data) {
    int enable;
    s32 ret;

    ret = 0;
    if ((data[0] >= WPAD_FIRST_INPUT_REPORT) && (data[0] <= WPAD_LAST_INPUT_REPORT)) {
        enable = OSDisableInterrupts();
        (*(__a1_input_reports_array[data[0] - WPAD_FIRST_INPUT_REPORT]))(chan, data);
        OSRestoreInterrupts(enable);

        WPADiExcludeButton(chan);
        WPADiCopyOut(chan);
    } else {
        ret = -1;
    }
    return ret;
}

// classic controller decoding for the three data modes; e0.. are report indices of the extension bytes
#define CL_MODE1(p, d, e0, e1, e2, e3, e4, e5)                                                                                         \
    (p)->clLStickX = (s16)((s16)((s16)((s16)((d)[e0]) & (s16)0x003F) << 4) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clLStickY = (s16)((s16)((s16)((s16)((d)[e1]) & (s16)0x003F) << 4) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clRStickX = (s16)((s16)((s16)((s16)((s16)((s16)((d)[e0]) >> 3) & (s16)0x0018) |                                                \
                                     (s16)((s16)((s16)((d)[e1]) >> 5) & (s16)0x0006) |                                                  \
                                     (s16)((s16)((s16)((d)[e2]) >> 7) & (s16)0x0001)) << 5) - (s16)(WPAD_CL_STICK_RESO / 2));          \
    (p)->clRStickY = (s16)((s16)((s16)((s16)((d)[e2]) & (s16)0x001F) << 5) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clTriggerL = (u8)((u8)((u8)((u8)((d)[e2] >> 2) & (u8)0x18) | (u8)((u8)((d)[e3] >> 5) & (u8)0x07)) << 3);                       \
    (p)->clTriggerR = (u8)((u8)((u8)((d)[e3]) & (u8)0x1F) << 3);                                                                       \
    (p)->clButton = (u16)((u16)((u16)((u16)((d)[e4]) << 8) & (u16)0xFF00 | (u16)((d)[e5]) & (u16)0x00FF) ^ 0xFFFF)

#define CL_MODE2(p, d, e0, e1, e2, e3, e4, e5, e6, e7, e8)                                                                             \
    (p)->clLStickX = (s16)((s16)((s16)((s16)((s16)((d)[e0]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)((d)[e4])) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2)); \
    (p)->clRStickX = (s16)((s16)((s16)((s16)((s16)((d)[e1]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)((d)[e4]) >> 2) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2)); \
    (p)->clLStickY = (s16)((s16)((s16)((s16)((s16)((d)[e2]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)((d)[e4]) >> 4) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2)); \
    (p)->clRStickY = (s16)((s16)((s16)((s16)((s16)((d)[e3]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)((d)[e4]) >> 6) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2)); \
    (p)->clTriggerL = (u8)((d)[e5]);                                                                                                   \
    (p)->clTriggerR = (u8)((d)[e6]);                                                                                                   \
    (p)->clButton = (u16)((u16)((u16)((u16)((d)[e7]) << 8) & (u16)0xFF00 | (u16)((d)[e8]) & (u16)0x00FF) ^ 0xFFFF)

#define CL_MODE3(p, d, e0, e1, e2, e3, e4, e5, e6, e7)                                                                                 \
    (p)->clLStickX = (s16)((s16)((s16)((s16)((d)[e0]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clRStickX = (s16)((s16)((s16)((s16)((d)[e1]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clLStickY = (s16)((s16)((s16)((s16)((d)[e2]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clRStickY = (s16)((s16)((s16)((s16)((d)[e3]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));                            \
    (p)->clTriggerL = (u8)((d)[e4]);                                                                                                   \
    (p)->clTriggerR = (u8)((d)[e5]);                                                                                                   \
    (p)->clButton = (u16)((u16)((u16)((u16)((d)[e6]) << 8) & (u16)0xFF00 | (u16)((d)[e7]) & (u16)0x00FF) ^ 0xFFFF)

// remember the first sample as the stick origin, then subtract + clamp everything against it
#define CL_FINISH(cb, p, chan)                                                                                                         \
    if (!(cb)->isSetStickOrigin) {                                                                                                     \
        (cb)->isSetStickOrigin = 1;                                                                                                    \
        (cb)->extConf.cl.lstk.x = (p)->clLStickX;                                                                                      \
        (cb)->extConf.cl.lstk.y = (p)->clLStickY;                                                                                      \
        (cb)->extConf.cl.rstk.x = (p)->clRStickX;                                                                                      \
        (cb)->extConf.cl.rstk.y = (p)->clRStickY;                                                                                      \
        (cb)->extConf.cl.triggerL = (p)->clTriggerL;                                                                                   \
        (cb)->extConf.cl.triggerR = (p)->clTriggerR;                                                                                   \
    }                                                                                                                                  \
    x = (s16)((p)->clLStickX - (cb)->extConf.cl.lstk.x);                                                                               \
    y = (s16)((p)->clLStickY - (cb)->extConf.cl.lstk.y);                                                                               \
    if (x < WPAD_CL_STK_MIN) { (p)->clLStickX = WPAD_CL_STK_MIN; }                                                                     \
    else if (x > WPAD_CL_STK_MAX) { (p)->clLStickX = WPAD_CL_STK_MAX; }                                                                \
    else { (p)->clLStickX = x; }                                                                                                       \
    if (y < WPAD_CL_STK_MIN) { (p)->clLStickY = WPAD_CL_STK_MIN; }                                                                     \
    else if (y > WPAD_CL_STK_MAX) { (p)->clLStickY = WPAD_CL_STK_MAX; }                                                                \
    else { (p)->clLStickY = y; }                                                                                                       \
    x = (s16)((p)->clRStickX - (cb)->extConf.cl.rstk.x);                                                                               \
    y = (s16)((p)->clRStickY - (cb)->extConf.cl.rstk.y);                                                                               \
    if (x < WPAD_CL_STK_MIN) { (p)->clRStickX = WPAD_CL_STK_MIN; }                                                                     \
    else if (x > WPAD_CL_STK_MAX) { (p)->clRStickX = WPAD_CL_STK_MAX; }                                                                \
    else { (p)->clRStickX = x; }                                                                                                       \
    if (y < WPAD_CL_STK_MIN) { (p)->clRStickY = WPAD_CL_STK_MIN; }                                                                     \
    else if (y > WPAD_CL_STK_MAX) { (p)->clRStickY = WPAD_CL_STK_MAX; }                                                                \
    else { (p)->clRStickY = y; }                                                                                                       \
    x = (s16)((u8)(p)->clTriggerL - (u8)(cb)->extConf.cl.triggerL);                                                                    \
    y = (s16)((u8)(p)->clTriggerR - (u8)(cb)->extConf.cl.triggerR);                                                                    \
    if (x < 0) { (p)->clTriggerL = 0; }                                                                                                \
    else { (p)->clTriggerL = (p)->clTriggerL = (u8)x; }                                                                                \
    if (y < 0) { (p)->clTriggerR = 0; }                                                                                                \
    else { (p)->clTriggerR = (p)->clTriggerR = (u8)y; }                                                                                \
    if (_devCmpt[chan]) {                                                                                                              \
        (p)->clRStickX = 0;                                                                                                            \
        (p)->clRStickY = 0;                                                                                                            \
        (p)->clTriggerL = 0;                                                                                                           \
        (p)->clTriggerR = 0;                                                                                                           \
    }

// one ir point out of the "full" 0x3e/0x3f reports (position, size/radius, bounding box)
#define FULL_OBJ(p, d, n, xlo, ylo, hi, pixhi, pixlo, rx1, ry1, rx2, ry2)                                                              \
    (p)->obj[n].x = DPD_X((d)[xlo], (d)[hi]);                                                                                          \
    (p)->obj[n].y = DPD_Y((d)[ylo], (d)[hi]);                                                                                          \
    (p)->exp[n].pixel = (u16)(((s16)((s16)(((u16)((d)[pixhi]) << 8) & 0xff00) | (u16)(((u16)((d)[pixlo] & 0x00ff))))) * 64);           \
    (p)->exp[n].radius = (s8)(((d)[hi] & 0x0f));                                                                                       \
    (p)->exp[n].range_x1 = (s16)(((s8)((d)[rx1]) == -1) ? 0 : (d)[rx1]);                                                               \
    (p)->exp[n].range_y1 = (s16)(((s8)((d)[ry1]) == -1) ? 0 : (d)[ry1]);                                                               \
    (p)->exp[n].range_x2 = (s16)(((s8)((d)[rx2]) == -1) ? 0 : (d)[rx2]);                                                               \
    (p)->exp[n].range_y2 = (s16)(((s8)((d)[ry2]) == -1) ? 0 : (d)[ry2]);                                                               \
    (p)->exp[n].range_x1 = (s16)((p)->exp[n].range_x1 * 8);                                                                            \
    (p)->exp[n].range_y1 = (s16)((s16)(WPAD_DPD_IMG_RESO_WY - 1) - (s16)((p)->exp[n].range_y1 * 8));                                   \
    (p)->exp[n].range_x2 = (s16)((p)->exp[n].range_x2 * 8);                                                                            \
    (p)->exp[n].range_y2 = (s16)((s16)(WPAD_DPD_IMG_RESO_WY - 1) - (s16)((p)->exp[n].range_y2 * 8));                                   \
    (p)->obj[n].size = (u16)((f32)(p)->exp[n].radius * (f32)(p)->exp[n].radius * PI);                                                  \
    if ((p)->obj[n].size == 0 || (p)->obj[n].x == (s16)(WPAD_DPD_IMG_RESO_WX - 1) ||                                                   \
        (p)->obj[n].y == (s16)(WPAD_DPD_IMG_RESO_WY - 1) || (p)->exp[n].radius == 0xf) {                                               \
        (p)->obj[n].x = (s16)0;                                                                                                        \
        (p)->obj[n].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);                                                                               \
        (p)->obj[n].size = 0;                                                                                                          \
        (p)->exp[n].pixel = 0;                                                                                                         \
        (p)->exp[n].radius = 0;                                                                                                        \
    }                                                                                                                                  \
    (p)->obj[n].traceId = n;


// report 0x20: status (buttons, leds, battery, extension plugged or not)
void __a1_20_status_report(u8 chan, u8 *data) {
    WPADControlBlock *p_wpd;
    BOOL enable;
    BOOL attach;
    WPADStatus *p_status;
    WPADExtensionCallback extcb;
    u8 *p_clear;
    int i;

    DEBUGPrint("Received report 20\n");

    enable = OSDisableInterrupts();
    p_wpd = _wpdcb[chan];

    if (!p_wpd->setup) {
        OSRestoreInterrupts(enable);
        return;
    }

    p_status = (WPADStatus *)(p_wpd->rxBuf + p_wpd->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    attach = p_wpd->infos.attach;

    p_status->button = BUTTONS(data);

    p_wpd->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);
    p_wpd->infos.dpd = (BOOL)((data[B9] & HID_DPD_MASK) >> 3);
    p_wpd->infos.speaker = (BOOL)((data[B9] & HID_SPEAKER_MASK) >> 2);
    p_wpd->infos.attach = (BOOL)((data[B9] & HID_ATTACH_MASK) >> 1);
    p_wpd->infos.lowBat = (BOOL)(data[B9] & HID_LOWBAT_MASK);
    p_wpd->infos.led = (u8)((data[B9] & HID_LED_MASK) >> 4);
    p_wpd->infos.protocol = (u8)((data[B11] & HID_PRT_MASK) >> 4);
    p_wpd->infos.firmware = (u8)(data[B11] & HID_FW_MASK);

    if ((u8)(data[B12]) >= 0x55) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_MAX;
    } else if ((u8)(data[B12]) >= 0x44) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_HIGH;
    } else if ((u8)(data[B12]) >= 0x33) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_MEDIUM;
    } else if ((u8)(data[B12]) >= 0x3) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_LOW;
    } else {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_CRITICAL;
    }

    if (p_wpd->infos.attach) {
        if (!attach) {
            DEBUGPrint("initialize attachment\n");
            _retryCnt[chan] = 0;
            extcb = p_wpd->extensionCallback;
            initExtension(chan);
            p_wpd->isSetStickOrigin = 0;
            p_wpd->devType = WPAD_DEV_UNKNOWN;
            p_wpd->devMode = 0;
            if (extcb) {
                extcb(chan, WPAD_DEV_UNKNOWN);
            }
        } else {
            DEBUGPrint("already initialized\n");
        }
    } else {
        p_wpd->devType = WPAD_DEV_CORE;
        p_wpd->devMode = 0;
        WPADiClearQueue(&p_wpd->extq);
        WPADiSendSetReportType(&p_wpd->extq, p_wpd->dataFormat, p_wpd->pwrSave, NULL);

        if (attach) {
            if (p_wpd->extensionCallback) {
                p_wpd->extensionCallback(chan, WPAD_DEV_CORE);
            }
        } else {
            DEBUGPrint("already disconnected\n");
        }
    }

    if (p_wpd->infoBuf) {
        memcpy(p_wpd->infoBuf, &p_wpd->infos, sizeof(WPADInfo));
        p_wpd->infoBuf = NULL;
    }

    if (p_wpd->resultCallback && p_wpd->getStatFlag) {
        p_wpd->resultCallback(chan, WPAD_ERR_NONE);
        p_wpd->resultCallback = NULL;
    }

    p_status->err = WPAD_ERR_BUSY;
    p_status->dev = p_wpd->devType;
    p_wpd->getStatFlag = 0;

    NEXT_RXBUF(p_wpd);
    OSRestoreInterrupts(enable);
}

// report 0x21: reply to a memory/register read
void __a1_21_user_data(u8 chan, u8 *data) {
    WPADControlBlock *p_wpd;
    BOOL enable;
    WPADStatus *p_status;
    u8 *p_clear;
    u16 addr;
    u16 base;
    u16 i2c;
    u8 err;
    u8 len;
    s16 offset;
    int i;
    s32 status;

    enable = OSDisableInterrupts();
    p_wpd = _wpdcb[chan];

    p_status = (WPADStatus *)(p_wpd->rxBuf + p_wpd->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    err = (u8)((u8)(data[B9]) & 0x0f);
    len = (u8)(((u8)((u8)(data[B9]) >> 4) & 0x0f) + 1);
    addr = (u16)((u16)((u16)((u16)(data[B10]) << 8) & (u16)0xff00) | (u16)((u16)(data[B11]) & (u16)0x00ff));

    if (err != 0) {
        DEBUGPrint("read error happens!\n");
        p_wpd->readError = -1;

        if (p_wpd->resultCallback) {
            if (!p_wpd->extensionCallback || p_wpd->extensionCallback != p_wpd->resultCallback) {
                p_wpd->resultCallback(chan, WPAD_ERR_TRANSFER);
            }
            p_wpd->resultCallback = NULL;
        }
        p_wpd->status = WPAD_ERR_NONE;
    }

    base = (u16)((p_wpd->readBaseAddr & 0x0000FFFF));
    i2c = (u16)((p_wpd->readBaseAddr & 0xFFFF0000) >> 16);
    offset = (s16)(addr - base);

    if (addr >= base && addr <= base + p_wpd->readLength) {
        memcpy((u8 *)p_wpd->readBufPtr + offset, data + B12, len);

        if (base + p_wpd->readLength == addr + len) {
            DEBUGPrint("base addr: %08x\n", p_wpd->readBaseAddr);
            DEBUGPrint("length   : %d\n", p_wpd->readLength);

            status = (p_wpd->readError < 0) ? WPAD_ERR_TRANSFER : WPAD_ERR_NONE;

            DEBUGPrint("i2c = %04x\n", i2c);
            DEBUGPrint("enc = %d\n", p_wpd->extEnc);

            if (i2c == 0x04a4) {
                DEBUGPrint("Access to extension register.\n");

                if (p_wpd->extEnc == WPAD_EXTENC_MAIN || p_wpd->extEnc == WPAD_EXTENC_SUB) {
                    DEBUGPrint("Decode!!!!\n");
                    DEBUGPrint("    len = %d, addr = %04x\n", len, base);
                    WPADiDecode(chan, p_wpd->readBufPtr, len, base);
                }
            }

            if ((p_wpd->readBaseAddr == WPAD_CONF_CORE_ADDR && p_wpd->oldFw == FALSE) ||
                (p_wpd->readBaseAddr == WPAD_CONF_CORE_ADDR_OLD && p_wpd->oldFw == TRUE)) {
                getDevConfig(chan, status);
            }
            if (p_wpd->readBaseAddr == WPAD_CONF_EXT_ADDR) {
                getExtConfig(chan, status);
            }
            if (p_wpd->readBaseAddr == WPAD_CONF_EXT_TYPE) {
                getExtType(chan, status);
            }
            if (p_wpd->readBaseAddr == WPAD_MEM_GAMEDATA_ADDR) {
                getGameInfo(chan, status, 0);
            }
            if (p_wpd->readBaseAddr == WPAD_MEM_GAMEDATA_ADDR + sizeof(WPADMEMGameInfo)) {
                getGameInfo(chan, status, 1);
            }

            if (p_wpd->resultCallback) {
                p_wpd->resultCallback(chan, status);
                p_wpd->resultCallback = NULL;
            }
            p_wpd->status = WPAD_ERR_NONE;
        }
    } else {
        DEBUGPrint("received data is out of range!\n");
    }

    p_status->err = WPAD_ERR_BUSY;
    p_status->dev = p_wpd->devType;

    NEXT_RXBUF(p_wpd);
    OSRestoreInterrupts(enable);
}

// report 0x22: ack for an output report we sent
void __a1_22_ack(u8 chan, u8 *data) {
    WPADControlBlock *p_wpd = _wpdcb[chan];
    u8 type;
    u8 err;
    BOOL enable;
    WPADStatus *p_status;
    u8 *p_clear;
    s32 status;
    int i;

    enable = OSDisableInterrupts();

    DEBUGPrint("Received ack!\n");

    p_wpd = _wpdcb[chan];

    p_status = (WPADStatus *)(p_wpd->rxBuf + p_wpd->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    type = (u8)data[B9];
    err = (u8)data[B10];

    DEBUGPrint("ack --> report ID = %02x, error code = %d\n", type, err);

    status = (err == 0) ? WPAD_ERR_NONE : WPAD_ERR_TRANSFER;

    p_status->err = WPAD_ERR_BUSY;
    p_status->dev = p_wpd->devType;

    if (err) {
        DEBUGPrint("ack error --> report ID = %d, error code = %d\n", type, err);
    }

    if (p_wpd->cmdId == type) {
        if (p_wpd->resultCallback) {
            p_wpd->resultCallback(chan, status);
            p_wpd->resultCallback = NULL;
        }
        p_wpd->status = WPAD_ERR_NONE;
    } else {
        DEBUGPrint("invalid ack!\n");
    }

    NEXT_RXBUF(p_wpd);
    OSRestoreInterrupts(enable);
}

// fills obj[] of the status from the raw ir bytes (basic = 2 points per 5 bytes, extended = 3 bytes per point),
// then applies the rotation/shift calibration that getDevConfig worked out
static void __parse_dpd_data(s32 chan, WPADStatus **p_status, u8 fmt, u8 *p_data, u8 len) {
    u8 i;
    u8 x;
    u8 y;
    u8 xy;
    f32 a;
    f32 b;

    if (fmt == WPAD_DPD_EXP) {
        for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
            if (i * 3 + 2 < len) {
                x = *(p_data + i * 3);
                y = *(p_data + i * 3 + 1);
                xy = *(p_data + i * 3 + 2);

                (*p_status)->obj[i].x = DPD_X(x, xy);
                (*p_status)->obj[i].y = DPD_Y(y, xy);
                (*p_status)->obj[i].size = (u16)(((xy & HID_DPD_SIZE_MASK) & 0x00ff));
                (*p_status)->obj[i].size = (u8)((f32)((*p_status)->obj[i].size) * (f32)((*p_status)->obj[i].size) * PI);

                if ((*p_status)->obj[i].size == 0 ||
                    (*p_status)->obj[i].x == (s16)(WPAD_DPD_IMG_RESO_WX - 1) ||
                    (*p_status)->obj[i].y == (s16)(WPAD_DPD_IMG_RESO_WY - 1)) {
                    (*p_status)->obj[i].x = (s16)0;
                    (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);
                    (*p_status)->obj[i].size = 0;
                }
                (*p_status)->obj[i].traceId = i;
            } else {
                (*p_status)->obj[i].x = (s16)0;
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);
                (*p_status)->obj[i].size = 0;
                (*p_status)->obj[i].traceId = i;
            }
        }
    } else if (fmt == WPAD_DPD_STD) {
        for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
            x = *(p_data + i * 3 - (i / 2));
            y = *(p_data + i * 3 - (i / 2) + 1);
            xy = *(p_data + (i / 2) * 5 + 2);

            if ((i % 2) == 0) {
                (*p_status)->obj[i].x = DPD_X(x, xy);
                (*p_status)->obj[i].y = DPD_Y(y, xy);
            } else {
                (*p_status)->obj[i].x = (s16)((s16)(((u16)x) & 0x00ff) | (u16)(((u16)(xy & (u8)(HID_DPDX98_MASK >> 4))) << 8));
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)y) & 0x00ff) | (u16)(((u16)(xy & (u8)(HID_DPDY98_MASK >> 4))) << 6)));
            }

            if ((*p_status)->obj[i].x == (s16)(WPAD_DPD_IMG_RESO_WX - 1) || (*p_status)->obj[i].y == (s16)(WPAD_DPD_IMG_RESO_WY - 1)) {
                (*p_status)->obj[i].x = (s16)0;
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);
                (*p_status)->obj[i].size = 0;
            } else {
                (*p_status)->obj[i].size = WPAD_DPD_DUMMY_SIZE;
            }
            (*p_status)->obj[i].traceId = i;
        }
    }

    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        if ((*p_status)->obj[i].x != 0 || (*p_status)->obj[i].y != (WPAD_DPD_IMG_RESO_WY - 1)) {
            a = (((f32)(*p_status)->obj[i].x) + calibX[chan] - centerX[chan]);
            b = (((f32)(*p_status)->obj[i].y) + calibY[chan] - centerY[chan]);

            (*p_status)->obj[i].x = (s16)((f32)((f32)a * (f32)cos(-1.0f * rolag[chan])) - (f32)((f32)b * (f32)sin(-1.0f * rolag[chan])) + centerX[chan]);
            (*p_status)->obj[i].y = (s16)((f32)((f32)a * (f32)sin(-1.0f * rolag[chan])) + (f32)((f32)b * (f32)cos(-1.0f * rolag[chan])) + centerY[chan]);
        }
    }
}

// report 0x30: buttons only
void __a1_30_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    u8 *p_clear;
    int old;
    int i;

    old = OSDisableInterrupts();

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// report 0x31: buttons + accel
void __a1_31_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    u8 *p_clear;
    int old;
    int i;

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_CORE_ACC) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    p_status->accX = ACC_X(data, p_cb);
    p_status->accY = ACC_Y(data, p_cb);
    p_status->accZ = ACC_Z(data, p_cb);

    old = OSDisableInterrupts();
    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// shared bits for the extension decoders: clamp helpers are written out like the original (no helper fn)

// report 0x32: buttons + 8 bytes of extension
void __a1_32_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    WPADFSStatus *p_fsStat;
    WPADCLStatus *p_clStat;
    u8 *p_clear;
    int old;
    int i;
    s16 x;
    s16 y;

    old = OSDisableInterrupts();

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_FREESTYLE || p_cb->dataFormat == WPAD_FMT_CLASSIC) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    memcpy(checkBuffer, data + B9, 8);
    WPADiDecode(chan, data + B9, 8, 0);

    if (p_cb->infos.attach) {
        if (p_cb->devType == WPAD_DEV_FREESTYLE) {
            p_fsStat = (WPADFSStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

            p_fsStat->fsStickX = (s8)(data[B9]);
            p_fsStat->fsStickY = (s8)(data[B10]);
            p_fsStat->fsAccX = FS_ACC(data, B11, B14, 2, p_cb->extConf.fs.acc_0g.x);
            p_fsStat->fsAccY = FS_ACC(data, B12, B14, 4, p_cb->extConf.fs.acc_0g.y);
            p_fsStat->fsAccZ = FS_ACC(data, B13, B14, 6, p_cb->extConf.fs.acc_0g.z);
            p_fsStat->button |= (u16)(((u16)(~data[B14]) << 13) & (u16)0x6000);

            if (!p_cb->isSetStickOrigin) {
                p_cb->isSetStickOrigin = 1;
                p_cb->extConf.fs.stick.x = p_fsStat->fsStickX;
                p_cb->extConf.fs.stick.y = p_fsStat->fsStickY;
            }

            x = (s16)((u8)p_fsStat->fsStickX - (u8)p_cb->extConf.fs.stick.x);
            y = (s16)((u8)p_fsStat->fsStickY - (u8)p_cb->extConf.fs.stick.y);

            if (x < WPAD_FS_STK_MIN) { p_fsStat->fsStickX = WPAD_FS_STK_MIN; }
            else if (x > WPAD_FS_STK_MAX) { p_fsStat->fsStickX = WPAD_FS_STK_MAX; }
            else { p_fsStat->fsStickX = (s8)x; }

            if (y < WPAD_FS_STK_MIN) { p_fsStat->fsStickY = WPAD_FS_STK_MIN; }
            else if (y > WPAD_FS_STK_MAX) { p_fsStat->fsStickY = WPAD_FS_STK_MAX; }
            else { p_fsStat->fsStickY = (s8)y; }
        } else if (p_cb->devType == WPAD_DEV_CLASSIC) {
            switch (p_cb->devMode) {
            case WPAD_DEV_MODE_CLASSIC1:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE1(p_clStat, data, B9, B10, B11, B12, B13, B14);
                break;
            case WPAD_DEV_MODE_CLASSIC2:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE2(p_clStat, data, B9, B10, B11, B12, B13, B14, B15, B16, B17);
                break;
            case WPAD_DEV_MODE_CLASSIC3:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE3(p_clStat, data, B9, B10, B11, B12, B13, B14, B15, B16);
                break;
            }

            CL_FINISH(p_cb, p_clStat, chan);
        }

        if (!memcmp(checkBuffer, checkInvalidData, 8)) {
            if (p_status->err == WPAD_ERR_NONE) {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }

    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// report 0x33: buttons + accel + 12 bytes ir
void __a1_33_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    u8 *p_clear;
    int old;
    int i;

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_CORE_ACC || p_cb->dataFormat == WPAD_FMT_CORE_ACC_DPD) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    p_status->accX = ACC_X(data, p_cb);
    p_status->accY = ACC_Y(data, p_cb);
    p_status->accZ = ACC_Z(data, p_cb);

    __parse_dpd_data(chan, &p_status, p_cb->dpdFormat, data + B12, 12);

    old = OSDisableInterrupts();
    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// report 0x34: 19 bytes of extension (balance board style), nothing here is decoded for our device types
void __a1_34_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    u8 *p_clear;
    int old;
    int i;

    old = OSDisableInterrupts();

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (0) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    memcpy(checkBuffer, data + B9, 19);
    WPADiDecode(chan, data + B9, 19, 0);

    if (p_cb->infos.attach) {
        if (!memcmp(checkBuffer, checkInvalidData, 19)) {
            if (p_status->err == WPAD_ERR_NONE) {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }

    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// report 0x35: buttons + accel + 16 bytes extension
void __a1_35_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    WPADFSStatus *p_fsStat;
    WPADCLStatus *p_clStat;
    u8 *p_clear;
    int old;
    int i;
    s16 x;
    s16 y;

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_CORE_ACC ||
        p_cb->dataFormat == WPAD_FMT_FREESTYLE || p_cb->dataFormat == WPAD_FMT_FREESTYLE_ACC ||
        p_cb->dataFormat == WPAD_FMT_CLASSIC || p_cb->dataFormat == WPAD_FMT_CLASSIC_ACC) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    p_status->accX = ACC_X(data, p_cb);
    p_status->accY = ACC_Y(data, p_cb);
    p_status->accZ = ACC_Z(data, p_cb);

    memcpy(checkBuffer, data + B12, 16);
    WPADiDecode(chan, data + B12, 16, 0);

    if (p_cb->infos.attach) {
        if (p_cb->devType == WPAD_DEV_FREESTYLE) {
            p_fsStat = (WPADFSStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

            p_fsStat->fsStickX = (s8)(data[B12]);
            p_fsStat->fsStickY = (s8)(data[B13]);
            p_fsStat->fsAccX = FS_ACC(data, B14, B17, 2, p_cb->extConf.fs.acc_0g.x);
            p_fsStat->fsAccY = FS_ACC(data, B15, B17, 4, p_cb->extConf.fs.acc_0g.y);
            p_fsStat->fsAccZ = FS_ACC(data, B16, B17, 6, p_cb->extConf.fs.acc_0g.z);
            p_fsStat->button |= (u16)(((u16)(~data[B17]) << 13) & (u16)0x6000);

            if (!p_cb->isSetStickOrigin) {
                p_cb->isSetStickOrigin = 1;
                p_cb->extConf.fs.stick.x = p_fsStat->fsStickX;
                p_cb->extConf.fs.stick.y = p_fsStat->fsStickY;
            }

            x = (s16)((u8)p_fsStat->fsStickX - (u8)p_cb->extConf.fs.stick.x);
            y = (s16)((u8)p_fsStat->fsStickY - (u8)p_cb->extConf.fs.stick.y);

            if (x < WPAD_FS_STK_MIN) { p_fsStat->fsStickX = WPAD_FS_STK_MIN; }
            else if (x > WPAD_FS_STK_MAX) { p_fsStat->fsStickX = WPAD_FS_STK_MAX; }
            else { p_fsStat->fsStickX = (s8)x; }

            if (y < WPAD_FS_STK_MIN) { p_fsStat->fsStickY = WPAD_FS_STK_MIN; }
            else if (y > WPAD_FS_STK_MAX) { p_fsStat->fsStickY = WPAD_FS_STK_MAX; }
            else { p_fsStat->fsStickY = (s8)y; }
        } else if (p_cb->devType == WPAD_DEV_CLASSIC) {
            switch (p_cb->devMode) {
            case WPAD_DEV_MODE_CLASSIC1:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE1(p_clStat, data, B12, B13, B14, B15, B16, B17);
                break;
            case WPAD_DEV_MODE_CLASSIC2:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE2(p_clStat, data, B12, B13, B14, B15, B16, B17, B18, B19, B20);
                break;
            case WPAD_DEV_MODE_CLASSIC3:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE3(p_clStat, data, B12, B13, B14, B15, B16, B17, B18, B19);
                break;
            }

            CL_FINISH(p_cb, p_clStat, chan);
        }

        if (!memcmp(checkBuffer, checkInvalidData, 16)) {
            if (p_status->err == WPAD_ERR_NONE) {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }

    old = OSDisableInterrupts();
    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// report 0x36: buttons + 10 bytes ir + 9 bytes extension
void __a1_36_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    u8 *p_clear;
    int old;
    int i;

    old = OSDisableInterrupts();

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (0) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    __parse_dpd_data(chan, &p_status, p_cb->dpdFormat, data + B9, 10);

    memcpy(checkBuffer, data + B19, 9);
    WPADiDecode(chan, data + B19, 9, 0);

    if (p_cb->infos.attach) {
        if (!memcmp(checkBuffer, checkInvalidData, 9)) {
            if (p_status->err == WPAD_ERR_NONE) {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }

    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

// report 0x37: buttons + accel + 10 bytes ir + 6 bytes extension
void __a1_37_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatus *p_status;
    WPADFSStatus *p_fsStat;
    WPADCLStatus *p_clStat;
    u8 *p_clear;
    int old;
    int i;
    s16 x;
    s16 y;

    p_cb = _wpdcb[chan];
    p_status = (WPADStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    p_clear = (u8 *)(p_status);
    for (i = 0; i < WPAD_RX_DATASIZE; i++) {
        p_clear[i] = 0;
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_CORE_ACC ||
        p_cb->dataFormat == WPAD_FMT_CORE_ACC_DPD || p_cb->dataFormat == WPAD_FMT_FREESTYLE ||
        p_cb->dataFormat == WPAD_FMT_FREESTYLE_ACC || p_cb->dataFormat == WPAD_FMT_FREESTYLE_ACC_DPD ||
        p_cb->dataFormat == WPAD_FMT_CLASSIC || p_cb->dataFormat == WPAD_FMT_CLASSIC_ACC ||
        p_cb->dataFormat == WPAD_FMT_CLASSIC_ACC_DPD) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = (u8)((data[B7] & HID_NEAREMPTY_MASK) >> 7);

    p_status->accX = ACC_X(data, p_cb);
    p_status->accY = ACC_Y(data, p_cb);
    p_status->accZ = ACC_Z(data, p_cb);

    __parse_dpd_data(chan, &p_status, p_cb->dpdFormat, data + B12, 10);

    memcpy(checkBuffer, data + B22, 6);
    WPADiDecode(chan, data + B22, 6, 0);

    if (p_cb->infos.attach) {
        if (p_cb->devType == WPAD_DEV_FREESTYLE) {
            p_fsStat = (WPADFSStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

            p_fsStat->fsStickX = (s8)(data[B22]);
            p_fsStat->fsStickY = (s8)(data[B23]);
            p_fsStat->fsAccX = FS_ACC(data, B24, B27, 2, p_cb->extConf.fs.acc_0g.x);
            p_fsStat->fsAccY = FS_ACC(data, B25, B27, 4, p_cb->extConf.fs.acc_0g.y);
            p_fsStat->fsAccZ = FS_ACC(data, B26, B27, 6, p_cb->extConf.fs.acc_0g.z);
            p_fsStat->button |= (u16)(((u16)(~data[B27]) << 13) & (u16)0x6000);

            if (!p_cb->isSetStickOrigin) {
                p_cb->isSetStickOrigin = 1;
                p_cb->extConf.fs.stick.x = p_fsStat->fsStickX;
                p_cb->extConf.fs.stick.y = p_fsStat->fsStickY;
            }

            x = (s16)((u8)p_fsStat->fsStickX - (u8)p_cb->extConf.fs.stick.x);
            y = (s16)((u8)p_fsStat->fsStickY - (u8)p_cb->extConf.fs.stick.y);

            if (x < WPAD_FS_STK_MIN) { p_fsStat->fsStickX = WPAD_FS_STK_MIN; }
            else if (x > WPAD_FS_STK_MAX) { p_fsStat->fsStickX = WPAD_FS_STK_MAX; }
            else { p_fsStat->fsStickX = (s8)x; }

            if (y < WPAD_FS_STK_MIN) { p_fsStat->fsStickY = WPAD_FS_STK_MIN; }
            else if (y > WPAD_FS_STK_MAX) { p_fsStat->fsStickY = WPAD_FS_STK_MAX; }
            else { p_fsStat->fsStickY = (s8)y; }
        } else if (p_cb->devType == WPAD_DEV_CLASSIC) {
            switch (p_cb->devMode) {
            case WPAD_DEV_MODE_CLASSIC1:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                CL_MODE1(p_clStat, data, B22, B23, B24, B25, B26, B27);
                break;
            case WPAD_DEV_MODE_CLASSIC2:
                // only 6 ext bytes in this report so the trigger R / buttons just can't be filled in
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                p_clStat->clLStickX = (s16)((s16)((s16)((s16)((s16)(data[B22]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)(data[B26])) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clRStickX = (s16)((s16)((s16)((s16)((s16)(data[B23]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)(data[B26]) >> 2) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clLStickY = (s16)((s16)((s16)((s16)((s16)(data[B24]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)(data[B26]) >> 4) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clRStickY = (s16)((s16)((s16)((s16)((s16)(data[B25]) << 2) & (s16)0xFFFC) | (s16)((s16)((s16)(data[B26]) >> 6) & (s16)0x0003)) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clTriggerL = (u8)(data[B27]);
                p_clStat->clTriggerR = (u8)0;
                p_clStat->clButton = (u16)0;
                break;
            case WPAD_DEV_MODE_CLASSIC3:
                p_clStat = (WPADCLStatus *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);
                p_clStat->clLStickX = (s16)((s16)((s16)((s16)(data[B22]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clRStickX = (s16)((s16)((s16)((s16)(data[B23]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clLStickY = (s16)((s16)((s16)((s16)(data[B24]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clRStickY = (s16)((s16)((s16)((s16)(data[B25]) & (s16)0x00FF) << 2) - (s16)(WPAD_CL_STICK_RESO / 2));
                p_clStat->clTriggerL = (u8)(data[B26]);
                p_clStat->clTriggerR = (u8)(data[B27]);
                p_clStat->clButton = (u16)0;
                break;
            }

            CL_FINISH(p_cb, p_clStat, chan);
        }

        if (!memcmp(checkBuffer, checkInvalidData, 6)) {
            if (p_status->err == WPAD_ERR_NONE) {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }

    old = OSDisableInterrupts();
    NEXT_RXBUF(p_cb);
    OSRestoreInterrupts(old);
}

void __a1_3d_data_type(u8 chan, u8 *data) {
#pragma unused(chan, data)
}

// 0x3e/0x3f are the two halves of the "full" 4 point ir report, the buffer only flips once both arrived
u8 _recv_3e[WPAD_MAX_CONTROLLERS] = {0, 0, 0, 0};
u8 _recv_3f[WPAD_MAX_CONTROLLERS] = {0, 0, 0, 0};

void __a1_3e_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatusEx *p_status;
    u8 *p_clear;
    int old;
    int i;

    p_cb = _wpdcb[chan];
    p_status = (WPADStatusEx *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    if (_recv_3e[chan] == 0 && _recv_3f[chan] == 0) {
        p_clear = (u8 *)(p_status);
        for (i = 0; i < WPAD_RX_DATASIZE; i++) {
            p_clear[i] = 0;
        }
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_CORE_ACC || p_cb->dataFormat == WPAD_FMT_CORE_ACC_DPD_FULL) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = 0;

    p_status->accX = (s16)((s16)((s16)((s16)((s16)((s16)(data[B9]) << 2) & (s16)0xFFFC) | (s16)((s16)((u16)(data[B7] >> 6)) & (s16)0x0002))) - (s16)(p_cb->conf.acc_0g.x));
    p_status->accZ |= (s16)((s16)((s16)((s16)((s16)((s16)(data[B8]) << 3) & (s16)0xFF00) | (s16)((s16)((u16)(data[B7] << 1)) & (s16)0x00C0))));

    FULL_OBJ(p_status, data, 0, B10, B11, B12, B17, B18, B13, B14, B15, B16);
    FULL_OBJ(p_status, data, 1, B19, B20, B21, B26, B27, B22, B23, B24, B25);

    old = OSDisableInterrupts();
    _recv_3e[chan] = 1;
    if (_recv_3e[chan] && _recv_3f[chan]) {
        p_status->accZ -= (s16)(p_cb->conf.acc_0g.z);
        NEXT_RXBUF(p_cb);
        _recv_3e[chan] = _recv_3f[chan] = 0;
    }
    OSRestoreInterrupts(old);
}

void __a1_3f_data_type(u8 chan, u8 *data) {
    WPADControlBlock *p_cb;
    WPADStatusEx *p_status;
    u8 *p_clear;
    int old;
    int i;

    p_cb = _wpdcb[chan];
    p_status = (WPADStatusEx *)(p_cb->rxBuf + p_cb->rxBufIndex * WPAD_RX_DATASIZE);

    if (_recv_3e[chan] == 0 && _recv_3f[chan] == 0) {
        p_clear = (u8 *)(p_status);
        for (i = 0; i < WPAD_RX_DATASIZE; i++) {
            p_clear[i] = 0;
        }
    }

    p_status->button = BUTTONS(data);

    if (p_cb->dataFormat == WPAD_FMT_CORE || p_cb->dataFormat == WPAD_FMT_CORE_ACC || p_cb->dataFormat == WPAD_FMT_CORE_ACC_DPD_FULL) {
        p_status->err = WPAD_ERR_NONE;
    } else {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;

    p_cb->infos.nearempty = 0;

    p_status->accY = (s16)((s16)((s16)((s16)((s16)((s16)(data[B9]) << 2) & (s16)0xFFFC) | (s16)((s16)((u16)(data[B7] >> 6)) & (s16)0x0002))) - (s16)(p_cb->conf.acc_0g.y));
    p_status->accZ |= (s16)((s16)((s16)((s16)((s16)((s16)(data[B8]) >> 1) & (s16)0x0030) | (s16)((s16)((u16)(data[B7] >> 3)) & (s16)0x000C))));

    FULL_OBJ(p_status, data, 2, B10, B11, B12, B17, B18, B13, B14, B15, B16);
    FULL_OBJ(p_status, data, 3, B19, B20, B21, B26, B27, B22, B23, B24, B25);

    old = OSDisableInterrupts();
    _recv_3f[chan] = 1;
    if (_recv_3e[chan] && _recv_3f[chan]) {
        p_status->accZ -= (s16)(p_cb->conf.acc_0g.z);
        NEXT_RXBUF(p_cb);
        _recv_3e[chan] = _recv_3f[chan] = 0;
    }
    OSRestoreInterrupts(old);
}

void __a1_unused_report(u8 chan, u8 *data) {
#pragma unused(chan, data)
}
