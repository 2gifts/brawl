#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: only the explicitly inline SDK helpers are inlined in this unit.
#pragma auto_inline off

static NWC24Err fn_8039D30C(const u8 *data, int dataSize, int *dataRead, char *text, int textSize, int *textWritten);
NWC24Err NWC24Base64Encode(const u8 *data, s32 dataSize, char *text, s32 textSize, u32 *textWritten);
NWC24Err fn_8039D45C(const u8 *data, int dataSize, u8 *out, u32 outSize, u32 *outWritten);
void NWC24InitBase64Table(char *table);
static NWC24Err fn_8039D7C4(char *text, u32 textSize, u32 *textWritten, const u8 *data, u32 dataSize,
                            u32 *dataRead, BOOL noSoftBreaks);
NWC24Err NWC24EncodeQuotedPrintable(char *text, u32 textSize, u32 *textWritten, const u8 *data, u32 dataSize,
                                    u32 *dataRead);

/* ---------------------------------------------------------------------------------------------
 * base64 / quoted printable
 * ------------------------------------------------------------------------------------------- */

// the alphabet, with the pad char on the end
static char *Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static NWC24Err fn_8039D30C(const u8 *data, int dataSize, int *dataRead, char *text, int textSize, int *textWritten) {
    int out;
    NWC24Err result;
    int in;

    in = 0;
    result = NWC24_OK;
    out = 0;

    for (; in < dataSize; in += 3) {
        BOOL has2;
        BOOL has1;
        u32 value;

        if (out + 3 >= textSize) {
            result = NWC24_ERR_OVERFLOW;
            break;
        }

        value = (u32)data[in] << 8;
        has2 = FALSE;
        has1 = FALSE;
        if (in + 1 < dataSize) {
            value |= data[in + 1];
            has1 = TRUE;
        }

        value <<= 8;
        if (in + 2 < dataSize) {
            value |= data[in + 2];
            has2 = TRUE;
        }

        text[out] = Base64Chars[(value >> 18) & 0x3F];
        text[out + 1] = Base64Chars[(value >> 12) & 0x3F];
        text[out + 2] = Base64Chars[has1 ? (value >> 6) & 0x3F : 64];
        text[out + 3] = Base64Chars[has2 ? value & 0x3F : 64];
        out += 4;
    }

    *textWritten = out;
    if (dataRead != NULL) {
        *dataRead = dataSize < in ? dataSize : in;
    }

    return result;
}

NWC24Err NWC24Base64Encode(const u8 *data, s32 dataSize, char *text, s32 textSize, u32 *textWritten) {
    return fn_8039D30C(data, dataSize, NULL, text, textSize, (int *)textWritten);
}

// goes the other way, skips everything that is not in the alphabet
NWC24Err fn_8039D45C(const u8 *data, int dataSize, u8 *out, u32 outSize, u32 *outWritten) {
    int bits = 0;
    int acc = 0;
    u32 n = 0;
    char *table = NWC24WorkP->base64Work;
    int i;

    for (i = 0; i < dataSize; i++) {
        int value = table[*data];

        if (value >= 0 && value < 64) {
            bits += 6;
            acc <<= 6;
            acc |= value;
            if (bits >= 8) {
                if (n >= outSize) {
                    *outWritten = outSize;
                    return NWC24_ERR_OVERFLOW;
                }

                bits -= 8;
                n++;
                *out++ = acc >> bits;
            }
        }

        data++;
    }

    *outWritten = n;
    return NWC24_OK;
}

// makes a lookup table that maps ascii to the 6 bit value (-1 for everything that is not part of the alphabet)
void NWC24InitBase64Table(char *table) {
    int i;

    for (i = 0; i < 256; i++) {
        table[i] = -1;
    }

    for (i = 'A'; i <= 'Z'; i++) {
        table[i] = i - 'A';
    }

    for (i = 'a'; i <= 'z'; i++) {
        table[i] = 26 + i - 'a';
    }

    for (i = '0'; i <= '9'; i++) {
        table[i] = 52 + i - '0';
    }

    table['+'] = 62;
    table['/'] = 63;
}

static NWC24Err fn_8039D7C4(char *text, u32 textSize, u32 *textWritten, const u8 *data, u32 dataSize,
                            u32 *dataRead, BOOL noSoftBreaks) {
    char *cursor;
    u32 written;
    u32 consumedTotal;
    NWC24Err result;
    int lineLen;
    u8 buf[4];
    BOOL softBreak;
    int out;
    int consumed;
    s16 ch;
    int i;

    result = NWC24_OK;
    if (text == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }
    if (data == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }
    if (dataSize == 0) {
        return NWC24_ERR_OVERFLOW;
    }
    if (dataRead != NULL) {
        *dataRead = 0;
    }
    if (textWritten != NULL) {
        *textWritten = 0;
    }
    cursor = text;
    lineLen = 0;
    written = 0;
    consumedTotal = 0;
    while (consumedTotal < dataSize) {
        ch = *data;
        softBreak = FALSE;
        if ((char)ch >= 0x21 && (char)ch <= 0x7E && (char)ch != '=') {
            consumed = 1;
            out = 1;
            buf[0] = *data;
        } else if (consumedTotal + 1 < dataSize && (char)ch == '\r' && (char)data[1] == '\n') {
            // BUG: Original CRLF handling consumes both bytes but emits only CR.
            consumed = 2;
            out = 1;
            buf[0] = *data;
            buf[1] = data[1];
        } else {
            buf[0] = '=';
            if ((*data >> 4) >= 10) {
                buf[1] = (*data >> 4) % 10 + 'A';
            }
            else {
                buf[1] = (*data >> 4) % 10 + '0';
            }
            if ((*data & 0xF) >= 10) {
                buf[2] = (*data & 0xF) % 10 + 'A';
            }
            else {
                buf[2] = (*data & 0xF) % 10 + '0';
            }
            consumed = 1;
            out = 3;
        }
        if (!noSoftBreaks && lineLen + out >= 75) {
            softBreak = TRUE;
            out += 3;
        }
        if (written + out >= textSize) {
            result = NWC24_ERR_OVERFLOW;
            break;
        }
        written += out;
        if (softBreak) {
            cursor[0] = '=';
            cursor[1] = '\r';
            cursor[2] = '\n';
            cursor += 3;
            out -= 3;
            lineLen = 0;
        }
        lineLen += out;
        for (i = 0; i < out; i++) {
            *cursor++ = buf[i];
        }
        consumedTotal += consumed;
        data += consumed;
    }
    if (dataRead != NULL) {
        *dataRead = consumedTotal;
    }
    if (textWritten != NULL) {
        *textWritten = written;
    }
    return result;
}

NWC24Err NWC24EncodeQuotedPrintable(char *text, u32 textSize, u32 *textWritten, const u8 *data, u32 dataSize,
                                    u32 *dataRead) {
    return fn_8039D7C4(text, textSize, textWritten, data, dataSize, dataRead, FALSE);
}
