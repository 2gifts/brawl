// scapi_prdinfo: reads the product info blob (area, code, serial, game region). the blob is xor-obfuscated
// with a rotating 32 bit key, and each entry looks like KEY=value.
#include <types.h>

#define XOR_KEY 0x73B5DBFA
#define PRDINFO_ADDR 0x80003800
#define PRDINFO_SIZE 0x100

extern int strcmp(const char *, const char *);
extern int sscanf(const char *, const char *, ...);

typedef struct {
    s8 area;
    char name[4];
} AreaEntry;

typedef struct {
    s8 region;
    char name[3];
} RegionEntry;

static AreaEntry ProductAreaTbl[] = {
    {0, "JPN"}, {1, "USA"}, {2, "EUR"}, {3, "AUS"}, {4, "BRA"}, {5, "TWN"}, {5, "ROC"}, {6, "KOR"}, {7, "HKG"},
    {8, "ASI"}, {9, "LTN"}, {10, "SAF"}, {-1, ""},
};

static RegionEntry GameRegionTbl[] = {
    {0, "JP"},
    {1, "US"},
    {2, "EU"},
    {-1, ""},
};

static char ProductCode[8];

int __SCF1(const char *type, char *buf, u32 sz) {
    u8 ptext;
    int found = 0;
    u32 i;
    const u8 *settings = (const u8 *)PRDINFO_ADDR;
    u32 key = XOR_KEY;
    u32 typeOfs = 0;
    u32 bufOfs = 0;

    for (i = 0; i < PRDINFO_SIZE; i++, key = key >> 31 | key << 1) {
        ptext = settings[i];

        if (ptext != 0x00) {
            ptext ^= key;
            if (type[typeOfs] == 0x00 && ptext == '=') {
                found = 1;
                break;
            }

            if (((ptext ^ type[typeOfs]) & 0xDF) == 0) {
                typeOfs++;
            } else {
                typeOfs = 0;
            }
        }
    }

    if (found) {
        for (i++; i < PRDINFO_SIZE && bufOfs < sz; i++) {
            key = key >> 31 | key << 1;
            ptext = settings[i];

            if (ptext != 0x00) {
                ptext ^= key;
                if (ptext == '\r' || ptext == '\n') {
                    ptext = 0x00;
                }
            }

            buf[bufOfs++] = ptext;

            if (ptext == 0x00) {
                return 1;
            }
        }
    }

    return 0;
}

s8 SCGetProductArea(void) {
    s8 area;
    char name[4];
    AreaEntry *iter = ProductAreaTbl;

    if (__SCF1("AREA", name, sizeof(name))) {
        for (; (area = iter->area) != -1; iter++) {
            if (!strcmp(iter->name, name)) {
                return area;
            }
        }
    }

    return -1;
}

char *SCGetProductCode(void) {
    if (__SCF1("CODE", ProductCode, 6)) {
        return ProductCode;
    }

    return 0;
}

int SCGetProductSN(u32 *sn) {
    char buf[11];

    if (__SCF1("SERNO", buf, sizeof(buf)) && sscanf(buf, "%u", sn) == 1) {
        return 1;
    }

    return 0;
}

s8 SCGetProductGameRegion(void) {
    s8 region;
    char name[3];
    RegionEntry *iter = GameRegionTbl;

    if (__SCF1("GAME", name, sizeof(name))) {
        for (; (region = iter->region) != -1; iter++) {
            if (!strcmp(iter->name, name)) {
                return region;
            }
        }
    }

    return -1;
}
