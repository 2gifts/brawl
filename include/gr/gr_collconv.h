#pragma once

// Local BrawlHeaders shadow: BrawlHeaders has no grCollConv; declares the stage collision-file converter
// that parses a text collision description into vertex, line and joint data.

#include <StaticAssert.h>
#include <gr/ground.h>
#include <types.h>

// HYPOTHESIS: parse records filled by grCollConvGetColFile; layouts from the parser's stores.
struct grCollConvVtxWork {
    s32 m_type; // HYPOTHESIS: the sscanf field count of the POINT line
    float m_x;
    float m_y;
    float m_z;
    float m_unk10;
    float m_unk14;
    float m_unk18;
};
static_assert(sizeof(grCollConvVtxWork) == 0x1C, "Class is wrong size!");

struct grCollConvLineWork {
    u16 m_unk0;
    s16 m_status;
    s16 m_unk4;
    u16 m_vtx[4];
    float m_unk10;
    float m_unk14;
    float m_unk18;
};
static_assert(sizeof(grCollConvLineWork) == 0x1C, "Class is wrong size!");

struct grCollConvJointWork {
    u16 m_unk0;
    u16 m_vtxStart;
    u16 m_vtxLen;
    u16 m_lineStart;
    u16 m_lineLen;
    float m_unkC;
    float m_unk10;
    float m_unk14;
    float m_unk18;
    s32 m_nodeIndex;
};
static_assert(sizeof(grCollConvJointWork) == 0x20, "Class is wrong size!");

// HYPOTHESIS: converted output records, same sizes as grCollData's vertex, line and joint data.
struct grCollConvVtxData {
    float m_x;
    float m_y;
};
static_assert(sizeof(grCollConvVtxData) == 0x8, "Class is wrong size!");

struct grCollConvLineData {
    u16 m_vtx0;
    u16 m_vtx1;
    s16 m_unk4;
    s16 m_unk6;
    s16 m_unk8;
    s16 m_unkA;
    u16 m_unkC;
    u16 m_unkE;
};
static_assert(sizeof(grCollConvLineData) == 0x10, "Class is wrong size!");

struct grCollConvJointData {
    u16 m_lineStart;
    u16 m_lineLen;
    u16 m_unk4;
    u16 m_unk6;
    u16 m_unk8;
    u16 m_unkA;
    u16 m_unkC;
    u16 m_unkE;
    u16 m_unk10;
    u16 m_unk12;
    float m_unk14;
    float m_unk18;
    float m_unk1C;
    float m_unk20;
    u16 m_vtxStart;
    u16 m_vtxLen;
    u16 m_unk28;
    u16 m_nodeIndex;
    char _2C[0x40];
};
static_assert(sizeof(grCollConvJointData) == 0x6C, "Class is wrong size!");

// HYPOTHESIS: the leading fields mirror grCollData (counts, then vertex/line/joint pointers).
class grCollConv {
public:
    u16 m_vtxLen;
    u16 m_lineLen;
    u16 m_jointLen;
    u16 m_unk6;
    grCollConvVtxData* m_vtxDatas;
    grCollConvLineData* m_lineDatas;
    grCollConvJointData* m_jointDatas;
    float m_maxX;
    float m_maxY;
    float m_minX;
    float m_minY;
    Ground* m_ground;

    grCollConv() : m_unk6(1) { }
    static grCollConv* create();
    virtual ~grCollConv();
    // HYPOTHESIS: parameter types of the virtual methods.
    virtual void process(char* data);
    virtual void makeDummyCollision();
    virtual void grCollConvMain(char* data);
    virtual void grCollConvGetColFile(char* data);
    virtual void grCollConvCollLineLink();
    virtual void grCollConvCollMakeLine();
    virtual u16 LineCheck(u16 vtx0, u16 vtx1);
    virtual void get_Line2_point(u16 line, s16* vtx0, s16* vtx1);

    u16 getVtxLen();
    u16 getLineLen();
    u16 getJointLen();
    grCollConvVtxData* getVtxData();
    grCollConvLineData* getLineData();
    grCollConvJointData* getJointData();
};
static_assert(sizeof(grCollConv) == 0x2C, "Class is wrong size!");
