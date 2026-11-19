#include <st/st_trigger_observe.h>
#include <gf/gf_area.h>
#include <math.h>
#include <st/st_area_manager.h>
#include <st/stage.h>
#include <types.h>
#include <cm/cm_camera_controller.h>
#include <gm/gm_global.h>

stObsTriggerCameraReactionCB::stObsTriggerCameraReactionCB(bool* flag, Rect2D* area, bool keep)
    : stObsTriggerCB(flag, keep) {
    __memcpy(&unk10, area, sizeof(unk10));
}

void stObsTriggerCameraReactionCB::userProc() {
    if (CameraController::getInstance() != nullptr && CameraController::getInstance()->isIntersect(&unk10)) {
        *unk8 = true;
    }
}

stObsTriggerPlayerIncludeAreaCB::stObsTriggerPlayerIncludeAreaCB(bool* flag, Rect2D* area, bool keep)
    : stObsTriggerCB(flag, keep) {
    __memcpy(&unk10, area, sizeof(unk10));
    unk20 = 0;
    unk24 = false;
    if (g_GameGlobal->m_advData->m_flags.m_unk0 == true) {
        unk24 = true;
    }
}

stObsTriggerCB::stObsTriggerCB(bool* flag, bool unkC) : unk8(flag), unkC(unkC) { }

void stObsTriggerCB::resetTrigger() {
    if (!unkC) {
        *unk8 = false;
    }
}

void stObsNumTriggerCB2::userProc() {
    bool allOn = true;
    bool anyOn = false;
    bool allOff = true;
    bool anyOff = false;
    for (u32 i = 0; i < m_referenceNum; i++) {
        if (*m_referenceFlags[i] == true) {
            anyOn = true;
            allOff = false;
        } else {
            allOn = false;
            anyOff = true;
        }
    }
    bool result;
    if (unk28) {
        result = !unk29 ? allOn : allOff;
    } else {
        result = !unk29 ? anyOn : anyOff;
    }
    if (result) {
        *unk8 = true;
    }
}

stObsTriggerArea::stObsTriggerArea(bool* flag, bool unkC) : stObsTriggerCB(flag, unkC) { }

stObsTriggerArea::~stObsTriggerArea() {
    if (m_area != nullptr) {
        stAreaManager::getInstance()->erase(m_area->m_id);
        delete m_area;
    }
    m_area = nullptr;
}

void stObsTriggerArea::userProc() { }

bool stObsTriggerArea::getAreaID(int* areaID) {
    if (m_area != nullptr) {
        *areaID = m_area->m_id;
        return true;
    }
    return false;
}

void stObsTriggerArea::setAreaSleep(bool isSleep) {
    if (isSleep) {
        m_area->m_isDrawFill = false;
    } else {
        m_area->m_isDrawFill = true;
    }
}

void stObsTriggerArea::setDefaultGimmickAreaData(stSimpleAreaData* simpleAreaData, stGimmickAreaData* gimmickAreaData) {
    bool useTwoBoundingPoints = simpleAreaData->m_useTwoBoundingPoints;
    // HYPOTHESIS: the default shape value is the gimmick group, overwritten by both shape branches.
    gimmickAreaData->m_shapeType = (gfArea::ShapeType)gfArea::Stage_Group_Gimmick_Normal;
    gimmickAreaData->m_group = gfArea::Stage_Group_Space_Normal;
    gimmickAreaData->m_checkTargetGroup = 0;
    gimmickAreaData->m_checkTargetLocalGroup = 0;
    gimmickAreaData->m_16 = 0;
    gimmickAreaData->m_18 = 0;
    gimmickAreaData->unk14 = 0;
    if (useTwoBoundingPoints == true) {
        float x1 = simpleAreaData->m_pos1.m_x;
        float y1 = simpleAreaData->m_pos1.m_y;
        float x2 = simpleAreaData->m_pos2.m_x;
        float y2 = simpleAreaData->m_pos2.m_y;
        gimmickAreaData->m_shapeType = gfArea::Shape_Rectangle;
        gimmickAreaData->m_offsetPos.m_x = (x1 + x2) / 2.0f;
        gimmickAreaData->m_offsetPos.m_y = (y1 + y2) / 2.0f;
        gimmickAreaData->m_range.m_y = fabs(y2 - y1);
        gimmickAreaData->m_range.m_x = fabs(x2 - x1);
    } else if (useTwoBoundingPoints == false) {
        float x = simpleAreaData->m_pos1.m_x;
        float y = simpleAreaData->m_pos1.m_y;
        float radius = simpleAreaData->m_pos2.m_y;
        gimmickAreaData->m_shapeType = gfArea::Shape_Circle;
        gimmickAreaData->m_offsetPos.m_x = x;
        gimmickAreaData->m_offsetPos.m_y = y;
        gimmickAreaData->m_range.m_x = radius;
        gimmickAreaData->m_range.m_y = radius;
    }
}

gfArea* stObsTriggerArea::getAreaPointer() {
    return m_area;
}

void stObsTriggerArea::setArea(gfArea* area) {
    m_area = area;
}

stGimmickAreaData* stObsTriggerArea::getAreaDataPointer() {
    return &m_gimmickAreaData;
}

stObsTriggerFrameCB::stObsTriggerFrameCB(bool* flag, float frame) : stObsTriggerCB(flag, false), unk10(frame), unk14(0.0f) { }

void stObsTriggerFrameCB::userProc() {
    if (unk14 >= unk10) {
        *unk8 = true;
    }
    float delta = g_stGameFrame.m_frameDelta;
    unk14 += delta;
}

stObsTriggerSquareWindCB::stObsTriggerSquareWindCB(bool* flag, grGimmickWindData2nd* windData)
    : stObsTriggerArea(flag, false) {
    windData->m_areaData.m_group = gfArea::Stage_Group_Space_Wind;
    createArea(&windData->m_areaData, Vec2f(windData->m_pos.m_x, windData->m_pos.m_y));
    stMovementAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_Wind_2nd;
    info.m_data.m_speed = windData->m_speed;
    info.m_data.m_vector = windData->m_vector;
    info.m_data._0xC = windData->m_60;
    info.m_data._0x10 = windData->m_64;
    info.m_data._0x14 = windData->m_68;
    info.m_data._0x1c = windData->m_72;
    getAreaID(&info.m_areaId);
    m_windIndex = stAreaManager::getInstance()->setWind(&info);
}

stObsTriggerSquareWindCB::~stObsTriggerSquareWindCB() { }

stObsTriggerSquareBeltConveyorCB::stObsTriggerSquareBeltConveyorCB(bool* flag, grGimmickPressureAreaData* beltData)
    : stObsTriggerArea(flag, false) {
    beltData->m_areaData.m_group = gfArea::Stage_Group_Space_Movement_Area;
    createArea(&beltData->m_areaData, Vec2f(beltData->m_pos.m_x, beltData->m_pos.m_y));
    stMovementAreaInfoData info;
    if (beltData->m_isPow == true) {
        info.m_data.m_gimmickKind = Gimmick::Area_Pow;
    } else {
        info.m_data.m_gimmickKind = Gimmick::Area_Yuka;
    }
    info.m_data.m_speed = beltData->m_speed;
    info.m_data._0xC = 0.0f;
    info.m_data._0x10 = 0.0f;
    info.m_data.m_vector = beltData->m_vector;
    info.m_data.m_taskId = beltData->m_taskId;
    info.m_data._0x1c = 0;
    getAreaID(&info.m_areaId);
    m_beltConveyorIndex = stAreaManager::getInstance()->setBelt(&info);
}

stObsTriggerSquareBeltConveyorCB::~stObsTriggerSquareBeltConveyorCB() { }

void stObsTriggerSquareBeltConveyorCB::setParam(grGimmickBeltConveyorData* beltData) {
    if (getAreaPointer() != nullptr && dynamic_cast<gfAreaRectangle*>(getAreaPointer()) != nullptr) {
        dynamic_cast<gfAreaRectangle*>(getAreaPointer())->setShape(
            Vec2f(beltData->m_areaData.m_offsetPos.m_x + beltData->m_pos.m_x,
                  beltData->m_areaData.m_offsetPos.m_y + beltData->m_pos.m_y),
            Vec2f(beltData->m_areaData.m_range.m_x, beltData->m_areaData.m_range.m_y));
    }
    stMovementAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_BeltConveyor;
    info.m_data.m_speed = beltData->m_speed;
    info.m_data._0xC = 0.0f;
    info.m_data._0x10 = 0.0f;
    if (beltData->m_isRight) {
        info.m_data.m_vector = 0.0f;
    }
    if (!beltData->m_isRight) {
        info.m_data.m_vector = 180.0f;
    }
    info.m_data._0x1c = 0;
    getAreaID(&info.m_areaId);
    stAreaManager::getInstance()->setBeltParam(&info, m_beltConveyorIndex);
}

void stObsTriggerSquareBeltConveyorCB::setParam(grGimmickPressureAreaData* beltData) {
    if (getAreaPointer() != nullptr && dynamic_cast<gfAreaRectangle*>(getAreaPointer()) != nullptr) {
        dynamic_cast<gfAreaRectangle*>(getAreaPointer())->setShape(
            Vec2f(beltData->m_areaData.m_offsetPos.m_x + beltData->m_pos.m_x,
                  beltData->m_areaData.m_offsetPos.m_y + beltData->m_pos.m_y),
            Vec2f(beltData->m_areaData.m_range.m_x, beltData->m_areaData.m_range.m_y));
    }
    stMovementAreaInfoData info;
    if (beltData->m_isPow == true) {
        info.m_data.m_gimmickKind = Gimmick::Area_Pow;
    } else {
        info.m_data.m_gimmickKind = Gimmick::Area_Yuka;
    }
    info.m_data.m_speed = beltData->m_speed;
    info.m_data._0xC = 0.0f;
    info.m_data._0x10 = 0.0f;
    info.m_data.m_vector = beltData->m_vector;
    info.m_data.m_taskId = beltData->m_taskId;
    info.m_data._0x1c = 0;
    getAreaID(&info.m_areaId);
    stAreaManager::getInstance()->setBeltParam(&info, m_beltConveyorIndex);
}

stObsTriggerTriBeltConveyorCB::~stObsTriggerTriBeltConveyorCB() { }

stObsTriggerSquareWaterCB::stObsTriggerSquareWaterCB(bool* flag, grGimmickWaterData* waterData)
    : stObsTriggerArea(flag, false) {
    waterData->m_areaData.m_group = gfArea::Stage_Group_Space_Water;
    createArea(&waterData->m_areaData, Vec2f(0.0f, 0.0f));
    stWaterAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_Water;
    info.m_data.m_swimHeight = waterData->m_swimHeight;
    info.m_data.m_0xc = waterData->m_canDrown != false;
    info.m_data.m_currentSpeed = waterData->m_speed;
    getAreaID(&info.m_areaId);
    m_waterIndex = stAreaManager::getInstance()->setWater(&info);
}

stObsTriggerSquareWaterCB::~stObsTriggerSquareWaterCB() { }

void stObsTriggerSquareWaterCB::setParam(grGimmickWaterData* waterData) {
    if (getAreaPointer() != nullptr && dynamic_cast<gfAreaRectangle*>(getAreaPointer()) != nullptr) {
        dynamic_cast<gfAreaRectangle*>(getAreaPointer())->setShape(
            Vec2f(waterData->m_areaData.m_offsetPos.m_x, waterData->m_areaData.m_offsetPos.m_y),
            Vec2f(waterData->m_areaData.m_range.m_x, waterData->m_areaData.m_range.m_y));
    }
    stWaterAreaInfoData info;
    info.m_data.m_gimmickKind = Gimmick::Area_Water;
    info.m_data.m_swimHeight = waterData->m_swimHeight;
    info.m_data.m_0xc = waterData->m_canDrown != false;
    info.m_data.m_currentSpeed = waterData->m_speed;
    getAreaID(&info.m_areaId);
    stAreaManager::getInstance()->setWaterParam(&info, m_waterIndex);
}

stObsTriggerSquareHitPointEffectCB::~stObsTriggerSquareHitPointEffectCB() { }

stObsTriggerCameraReactionCB::~stObsTriggerCameraReactionCB() { }

stObsTriggerPlayerIncludeAreaCB::~stObsTriggerPlayerIncludeAreaCB() { }

#include <st/st_trigger.h>
#include <sr/sr_common.h>
#include <mt/mt_vector.h>
#include <nw4r/math/math_types.h>
#include <ft/ft_manager.h>
// TODO: clean up, give variables real names
namespace jj_234B2C {

struct FnNode {
    FnNode* next;
    u32 pad4;
    u32 pad8;
    int id;
    u8 kind;
};

extern "C" {
FnNode* fn_27_232304(const void* self, int id, u32 kind);
}

struct stTriggerDataHead {
    u16 id;
    u8 pad2;
    u8 kind;
};

struct TriggerList {
    char pad[0x10];
    void* arr[5];
    u32 count;
};

extern "C" bool fn_27_234B2C(void* self, stTriggerDataHead* data) {
    if (g_stTriggerMng != 0) {
        FnNode* node = fn_27_232304(g_stTriggerMng, data->id, data->kind);
        if (node == 0) {
            node = (FnNode*)g_stTriggerMng->createTrigger((Gimmick::AreaKind)0, (stTriggerData*)data);
        }
        if (node != 0) {
            TriggerList* w = (TriggerList*)self;
            w->arr[w->count] = (char*)node + 0x2c;
            w->count = w->count + 1;
            return true;
        }
    }
    return false;
}
}

// TODO: clean up, give variables real names
namespace jj_234DDC {

extern "C" {
void __ct__6gfAreaFRC5Vec2fb(void*, void*, unsigned int);
void setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(void*, void*, void*);
void setShape__12gfAreaCircleFRC5Vec2ff(void*, void*, float);
extern float lbl_27_rodata_38F0;
extern const float lbl_27_rodata_38F4;
extern char __vt__15gfAreaRectangle[];
extern char __vt__12gfAreaCircle[];
}

struct AreaFlags {
    unsigned int reserved : 24;
    unsigned int kind : 8;
};

struct AreaData {
    s32 kind, a, b, c;
    u16 d, e;
    s32 f;
    f32 x, y, w, h;
};

extern "C" void createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(void* self, AreaData src, Vec2f vec) {
    char* s = (char*)self;
    char* p = (char*)&src;
    char* v = (char*)&vec;

    f32 f1c;
    f32 f18;
    s32 f14;
    u16 f12;
    u16 f10;
    s32 fc;
    s32 f8;
    s32 f4;
    s32 f0;

    f0 = *(s32*)(p + 0x00);
    f18 = *(f32*)(p + 0x18);
    f1c = *(f32*)(p + 0x1C);
    *(f32*)(s + 0x2C) = f18;
    f4 = *(s32*)(p + 0x04);
    *(f32*)(s + 0x30) = f1c;
    f8 = *(s32*)(p + 0x08);
    fc = *(s32*)(p + 0x0C);
    f10 = *(u16*)(p + 0x10);
    f12 = *(u16*)(p + 0x12);
    f14 = *(s32*)(p + 0x14);
    f32 f24;
    f32 f20 = *(f32*)(p + 0x20);
    f24 = *(f32*)(p + 0x24);
    *(s32*)(s + 0x14) = f0;
    *(s32*)(s + 0x18) = f4;
    *(s32*)(s + 0x1C) = f8;
    *(s32*)(s + 0x20) = fc;
    *(u16*)(s + 0x24) = f10;
    *(u16*)(s + 0x26) = f12;
    *(s32*)(s + 0x28) = f14;
    *(f32*)(s + 0x34) = f20;
    *(f32*)(s + 0x38) = f24;
    *(nw4r::math::VEC2*)(s + 0x2C) += *(nw4r::math::VEC2*)v;

    f32 pa[2];
    f32 pd[2];
    f32 pc[2];
    f32 pe[2];
    f32 pf[2];
    f32 pb[2];

    char* o;
    if (f0 == 0) {
        o = (char*)operator new(0x68, Heaps::StageInstance);
        if (o) {
            pa[0] = lbl_27_rodata_38F0;
            pa[1] = lbl_27_rodata_38F0;
            __ct__6gfAreaFRC5Vec2fb(o, pa, 1);
            *(void**)(o + 0x44) = __vt__15gfAreaRectangle;
            f32 c = lbl_27_rodata_38F4;
            *(s32*)(o + 0x18) = 0;
            pb[0] = c;
            pb[1] = c;
            setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(o, pa, pb);
        }
        f32 cx, cy;
        cy = *(f32*)(s + 0x38);
        cx = *(f32*)(s + 0x34);
        pc[0] = cx;
        pc[1] = cy;
        f32 dx, dy;
        dy = *(f32*)(s + 0x30);
        dx = *(f32*)(s + 0x2C);
        pd[0] = dx;
        pd[1] = dy;
        setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(o, pd, pc);
        *(void**)(s + 0x10) = o;
    } else {
        o = (char*)operator new(0x54, Heaps::StageInstance);
        if (o) {
            pe[0] = lbl_27_rodata_38F0;
            pe[1] = lbl_27_rodata_38F0;
            __ct__6gfAreaFRC5Vec2fb(o, pe, 1);
            void* table = __vt__12gfAreaCircle;
            f32 c = lbl_27_rodata_38F4;
            *(void**)(o + 0x44) = table;
            *(s32*)(o + 0x18) = 1;
            setShape__12gfAreaCircleFRC5Vec2ff(o, pe, c);
        }
        f32 px, py;
        py = *(f32*)(p + 0x1C);
        px = *(f32*)(p + 0x18);
        pf[1] = py;
        pf[0] = px;
        setShape__12gfAreaCircleFRC5Vec2ff(o, pf, *(f32*)(s + 0x34));
        *(void**)(s + 0x10) = o;
    }

    char*& t = *(char**)(s + 0x10);
    ((AreaFlags*)(t + 0x1C))->kind = 2;
    *(u8*)(t + 0x1D) = (u8)*(s32*)(s + 0x18);
    *(s32*)(t + 0x08) = *(s32*)(s + 0x1C);
    *(s32*)(t + 0x0C) = *(s32*)(s + 0x20);
}
}

// TODO: clean up, give variables real names
namespace jj_2350AC {
struct Pair { float x, y; };
struct Request { void* owner; unsigned short id; float x, y, z, a, b; unsigned int c, d; };

struct Class_4C3F4_base { unsigned int first; };
class Class_4C3F4 : public Class_4C3F4_base {
public:
    virtual void userProc__16stObsTriggerAreaFv();
    virtual void __dt__16stObsTriggerAreaFv();
    virtual void* getAreaPointer__16stObsTriggerAreaFv();
    virtual void getAreaID__16stObsTriggerAreaFPi(Request*);
    void* arg;
    unsigned char flag;
    char pad[0x2F];
    unsigned int result;
};

struct Param {
    char pad0[4];
    int type;
    char pad8[0x20];
    float pos_x;
    float pos_y;
    char pad30[4];
    float x;
    float y;
    unsigned short trig;
    unsigned char b3e;
};

struct Obj1C { char pad[0x1c]; unsigned char b7 : 1; };
struct Blk { unsigned int u0; void* u4; unsigned int u8; unsigned char uC; void* u10; };
struct Blk2 { unsigned int u0; void* u4; unsigned int u8; bool uC; void* u10; };

extern "C" {
extern char __vt__24stObsTriggerSquareWindCB[];
extern char lbl_27_data_4C498[];
extern const float lbl_27_rodata_38F0[];
extern void* lbl_27_bss_5780;
extern void* g_stTriggerMng;
void createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(Class_4C3F4*, Param*, Pair*);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__13stAreaManagerFv(void*);
unsigned int setWind__13stAreaManagerFP22stMovementAreaInfoData(void*, Request*);
void* createTrigger__12stTriggerMngFQ27Gimmick8AreaKindP13stTriggerData(void*, int, void*);
void setValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
void setUnValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
}

extern "C" Class_4C3F4* fn_27_2350AC(Class_4C3F4* self, void* arg, Param* p) {
    const float* k = lbl_27_rodata_38F0;
    self->first = 0;
    self->arg = arg;
    self->flag = 0;
    *(void**)((char*)self + 4) = __vt__24stObsTriggerSquareWindCB;

    Pair copy;
    Pair tmp;
    tmp.y = p->pos_y;
    tmp.x = p->pos_x;
    p->type = 0x1A;
    ((unsigned int*)&copy)[0] = ((unsigned int*)&tmp)[0];
    ((unsigned int*)&copy)[1] = ((unsigned int*)&tmp)[1];
    createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(self, p, &copy);

    if (p->trig != 0) {
        int var_r27;
        void* trig;
        void* cb;
        trig = createTrigger__12stTriggerMngFQ27Gimmick8AreaKindP13stTriggerData(g_stTriggerMng, 0, &p->trig);
        if (p->b3e & 1) {
            var_r27 = 1;
            Obj1C* t = (Obj1C*)self->getAreaPointer__16stObsTriggerAreaFv();
            t->b7 = 0;
        } else {
            var_r27 = 0;
        }
        cb = __nw__FUlQ25Heaps8HeapType(0x14, 0xF);
        if (cb != 0) {
            Blk* b = (Blk*)cb;
            void* v = self->getAreaPointer__16stObsTriggerAreaFv();
            b->u0 = 0;
            b->u8 = 0;
            b->u4 = lbl_27_data_4C498;
            b->uC = var_r27;
            b->u10 = v;
        }
        setValidEdgeCallback__9stTriggerFP10gfCallBack(trig, cb);
        cb = __nw__FUlQ25Heaps8HeapType(0x14, 0xF);
        if (cb != 0) {
            Blk2* b = (Blk2*)cb;
            void* v = self->getAreaPointer__16stObsTriggerAreaFv();
            b->u0 = 0;
            b->u8 = 0;
            b->u4 = lbl_27_data_4C498;
            b->uC = var_r27 == 0;
            b->u10 = v;
        }
        setUnValidEdgeCallback__9stTriggerFP10gfCallBack(trig, cb);
    }

    Request req;
    req.id = 0xC;
    req.x = p->x;
    req.y = p->y;
    if (k[3] == req.y) {
        req.z = k[4];
    } else {
        req.z = k[5];
    }
    req.a = k[6];
    req.b = k[0];
    req.d = 0x3C;
    self->getAreaID__16stObsTriggerAreaFPi(&req);

    if (lbl_27_bss_5780 == 0) {
        void* mgr = __nw__FUlQ25Heaps8HeapType(0x54, 2);
        if (mgr != 0) mgr = __ct__13stAreaManagerFv(mgr);
        lbl_27_bss_5780 = mgr;
    }
    self->result = setWind__13stAreaManagerFP22stMovementAreaInfoData(lbl_27_bss_5780, &req);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_235514 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vec2 { float x, y; };

struct SelfBase { u32 unk0; };
class Self : public SelfBase {
public:
    virtual void v0();
    virtual void v1();
    virtual void* v2();
    virtual void v3(void*);
    char pad[0x34];
    u32 m3c;
};

struct Param {
    char pad[0x18];
    float a;  // 0x18
    float b;  // 0x1c
    float c;  // 0x20
    float d;  // 0x24
    float e;  // 0x28
    float f;  // 0x2c
    char pad2[4];  // 0x30
    float g;  // 0x34
    float h;  // 0x38
    float i;  // 0x3c
    float j;  // 0x40
    float k;  // 0x44
    u32 l;  // 0x48
};

struct Request {
    u32 pad;
    u16 id;
    u16 pad2;
    float f0;
    float f1;
    float f2;
    float f3;
    float f4;
    u32 f5;
    u32 f6;
};

extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" char lbl_27_data_157C8[];
extern "C" char __RTTI__6gfArea[];
extern "C" void* lbl_27_bss_5780;
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__13stAreaManagerFv(void*);
extern "C" void setWindParam__13stAreaManagerFP22stMovementAreaInfoDataUl(void*, void*, u32);
extern "C" void setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(void*, Vec2*, Vec2*);

extern "C" void fn_27_235514(Self* self, Param* p, u32 flag) {
    Request req;
    Vec2 v1;
    Vec2 v0;

    if (self->v2()) {
        if (__dynamic_cast(self->v2(), 0x44, lbl_27_data_157C8, __RTTI__6gfArea, 0)) {
            float x0, y0, x1, y1;
            y0 = p->d; x0 = p->c;
            v0.x = x0; v0.y = y0;
            y1 = p->b + p->f; x1 = p->a + p->e;
            v1.y = y1; v1.x = x1;
            setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(__dynamic_cast(self->v2(), 0x44, lbl_27_data_157C8, __RTTI__6gfArea, 0), &v1, &v0);
        }
    }

    req.id = 0xd;
    req.f0 = p->g;
    req.f1 = p->h;
    req.f2 = p->i;
    req.f3 = p->j;
    req.f4 = p->k;
    req.f6 = p->l;
    self->v3(&req);
    if (flag == 1) {
        req.id = 0xc;
    }

    if (!lbl_27_bss_5780) {
        void* mgr = __nw__FUlQ25Heaps8HeapType(0x54, 2);
        if (mgr) mgr = __ct__13stAreaManagerFv(mgr);
        lbl_27_bss_5780 = mgr;
    }
    setWindParam__13stAreaManagerFP22stMovementAreaInfoDataUl(lbl_27_bss_5780, &req, self->m3c);
}
}

// TODO: clean up, give variables real names
namespace jj_2356A4 {
struct Pair { float x, y; };
struct Request { void* owner; unsigned short id; float x, y, z, a, b; unsigned int c, d; };

class Class_4C3A4_base { public: unsigned int first; };
class Class_4C3A4 : public Class_4C3A4_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void* slot2();
    virtual void slot3(Request*);
    void* arg;
    unsigned char flag;
    char pad[0x2F];
    unsigned int result;
};

struct Param {
    char pad0[4];
    int type;
    char pad8[0x20];
    float x;
    float y;
    char pad30[4];
    float z;
    unsigned char b38;
    char pad39[3];
    unsigned short trig;
    unsigned char b3e;
};

struct Obj1C { char pad[0x1c]; unsigned char b7 : 1; };
struct Blk { unsigned int u0; void* u4; unsigned int u8; unsigned char uC; void* u10; };
struct Blk2 { unsigned int u0; void* u4; unsigned int u8; bool uC; void* u10; };

extern "C" {
extern char __vt__32stObsTriggerSquareBeltConveyorCB[];
extern char lbl_27_data_4C498[];
extern const float lbl_27_rodata_38F0;
extern const float lbl_27_rodata_390C;
extern void* lbl_27_bss_5780;
extern void* g_stTriggerMng;
void createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(Class_4C3A4*, Param*, Pair*);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__13stAreaManagerFv(void*);
unsigned int setBelt__13stAreaManagerFP22stMovementAreaInfoData(void*, Request*);
void* createTrigger__12stTriggerMngFQ27Gimmick8AreaKindP13stTriggerData(void*, int, void*);
void setValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
void setUnValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
}

extern "C" Class_4C3A4* fn_27_2356A4(Class_4C3A4* self, void* arg, Param* p) {
    self->first = 0;
    self->arg = arg;
    self->flag = 0;
    *(void**)((char*)self + 4) = __vt__32stObsTriggerSquareBeltConveyorCB;

    Pair copy;
    Pair tmp;
    tmp.y = p->y;
    tmp.x = p->x;
    p->type = 0x1B;
    ((unsigned int*)&copy)[0] = ((unsigned int*)&tmp)[0];
    ((unsigned int*)&copy)[1] = ((unsigned int*)&tmp)[1];
    createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(self, p, &copy);

    if (p->trig != 0) {
        int var_r27;
        void* trig;
        void* cb;
        trig = createTrigger__12stTriggerMngFQ27Gimmick8AreaKindP13stTriggerData(g_stTriggerMng, 0, &p->trig);
        if (p->b3e & 1) {
            var_r27 = 1;
            Obj1C* t = (Obj1C*)self->slot2();
            t->b7 = 0;
        } else {
            var_r27 = 0;
        }
        cb = __nw__FUlQ25Heaps8HeapType(0x14, 0xF);
        if (cb != 0) {
            Blk* b = (Blk*)cb;
            void* v = self->slot2();
            b->u0 = 0;
            b->u8 = 0;
            b->u4 = lbl_27_data_4C498;
            b->uC = var_r27;
            b->u10 = v;
        }
        setValidEdgeCallback__9stTriggerFP10gfCallBack(trig, cb);
        cb = __nw__FUlQ25Heaps8HeapType(0x14, 0xF);
        if (cb != 0) {
            Blk2* b = (Blk2*)cb;
            void* v = self->slot2();
            b->u0 = 0;
            b->u8 = 0;
            b->u4 = lbl_27_data_4C498;
            b->uC = var_r27 == 0;
            b->u10 = v;
        }
        setUnValidEdgeCallback__9stTriggerFP10gfCallBack(trig, cb);
    }

    Request req;
    req.id = 0xE;
    req.x = p->z;
    req.z = lbl_27_rodata_38F0;
    req.a = lbl_27_rodata_38F0;
    if (p->b38 != 0) {
        req.y = lbl_27_rodata_38F0;
    }
    if (p->b38 == 0) {
        req.y = lbl_27_rodata_390C;
    }
    req.d = 0;
    self->slot3(&req);

    if (lbl_27_bss_5780 == 0) {
        void* mgr = __nw__FUlQ25Heaps8HeapType(0x54, 2);
        if (mgr != 0) mgr = __ct__13stAreaManagerFv(mgr);
        lbl_27_bss_5780 = mgr;
    }
    self->result = setBelt__13stAreaManagerFP22stMovementAreaInfoData(lbl_27_bss_5780, &req);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_236400 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vec2 {
    float x;
    float y;
};

struct Tri {
    Vec2 v[3];
};

struct Param {
    char pad0[0x18];
    Vec2 center;
    char pad1[0x8];
    Vec2 off[3];
    float size;
    u8 flag;
};

struct Request {
    void* owner;
    u16 id;
    float x;
    float y;
    float z;
    float a;
    float b;
    u32 c;
    u32 d;
};

struct SelfBase {
    u32 unk0;
};

class Self : public SelfBase {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void* vf4();
    virtual void vf5(Request* req);

    char pad[0x34];
    u32 m3c;
};

struct Dst {
    char pad[0x48];
    Tri pts;
};

extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" char __RTTI__14gfAreaTriangle[];
extern "C" char __RTTI__6gfArea[];
extern "C" const float lbl_27_rodata_38F0;
extern "C" const float lbl_27_rodata_390C;
extern "C" void* lbl_27_bss_5780;
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__13stAreaManagerFv(void*);
extern "C" void setBeltParam__13stAreaManagerFP22stMovementAreaInfoDataUl(void*, Request*, u32);

extern "C" void setParam__29stObsTriggerTriBeltConveyorCBFP15stEarthBeltData(Self* self, Param* p) {
    Request req;
    Tri tris[2];

    if (self->vf4()) {
        if (__dynamic_cast(self->vf4(), 0x44, __RTTI__14gfAreaTriangle, __RTTI__6gfArea, 0)) {
            float px0, py0, px1, py1, px2, py2;
            py2 = p->center.y + p->off[2].y;
            py1 = p->center.y + p->off[1].y;
            py0 = p->center.y + p->off[0].y;
            px2 = p->center.x + p->off[2].x;
            px1 = p->center.x + p->off[1].x;
            px0 = p->center.x + p->off[0].x;

            tris[0].v[0].y = py2;
            tris[0].v[0].x = px2;
            tris[0].v[1].x = px1;
            tris[0].v[1].y = py1;
            tris[0].v[2].x = px0;
            tris[0].v[2].y = py0;
            tris[1].v[0].x = px0;
            tris[1].v[0].y = py0;
            tris[1].v[1].x = px1;
            tris[1].v[1].y = py1;
            tris[1].v[2].x = px2;
            tris[1].v[2].y = py2;

            ((Dst*)__dynamic_cast(self->vf4(), 0x44, __RTTI__14gfAreaTriangle, __RTTI__6gfArea, 0))->pts = tris[1];
        }
    }

    req.id = 0xe;
    req.x = p->size;
    req.z = lbl_27_rodata_38F0;
    req.a = lbl_27_rodata_38F0;
    if (p->flag != 0) {
        req.y = lbl_27_rodata_38F0;
    }
    if (p->flag == 0) {
        req.y = lbl_27_rodata_390C;
    }
    req.d = 0;
    self->vf5(&req);

    if (!lbl_27_bss_5780) {
        void* mgr = __nw__FUlQ25Heaps8HeapType(0x54, 2);
        if (mgr) {
            mgr = __ct__13stAreaManagerFv(mgr);
        }
        lbl_27_bss_5780 = mgr;
    }
    setBeltParam__13stAreaManagerFP22stMovementAreaInfoDataUl(lbl_27_bss_5780, &req, self->m3c);
}
}

// TODO: clean up, give variables real names
namespace jj_23697C {
struct Pair { float x, y; };
struct Class_4C338_base { unsigned int first; };
class Class_4C338 : public Class_4C338_base {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void* slot2();
 virtual void slot3(void*);
 void* arg;
 unsigned char flag;
 char pad[0x2f];
 unsigned int result;
};
struct Obj1C { char pad[0x1c]; unsigned char b7 : 1; };
struct Trig { unsigned int u0; void* u4; unsigned int u8; unsigned char uC; void* u10; };
struct Trig2 { unsigned int u0; void* u4; unsigned int u8; bool uC; void* u10; };
struct Param {
 char pad0[4];
 int type;
 char pad8[0x20];
 unsigned char b28;
 unsigned char b29;
 unsigned short h2a;
 unsigned short h2c;
 unsigned char b2e;
};
struct Req { int a; short b; float c; unsigned short d; };

extern "C" {
extern char lbl_27_data_4C338[];
extern char lbl_27_data_4C498[];
extern const float lbl_27_rodata_38F0;
extern void* lbl_27_bss_5780;
extern void* g_stTriggerMng;
void createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(void*, void*, void*);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__13stAreaManagerFv(void*);
unsigned int setHitPointEffect__13stAreaManagerFP28stHitPointEffectAreaInfoData(void*, void*);
void* createTrigger__12stTriggerMngFQ27Gimmick8AreaKindP13stTriggerData(void*, int, void*);
void setValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
void setUnValidEdgeCallback__9stTriggerFP10gfCallBack(void*, void*);
}

extern "C" Class_4C338* fn_27_23697C(Class_4C338* self, void* arg1, Param* p) {
 Pair copy;
 Pair tmp;
 self->first = 0;
 self->arg = arg1;
 tmp.x = lbl_27_rodata_38F0;
 tmp.y = lbl_27_rodata_38F0;
 self->flag = 0;
 *(void**)((char*)self + 4) = lbl_27_data_4C338;
 p->type = 0x19;
 ((unsigned int*)&copy)[0] = ((unsigned int*)&tmp)[0];
 ((unsigned int*)&copy)[1] = ((unsigned int*)&tmp)[1];
 createArea__16stObsTriggerAreaFP17stGimmickAreaData5Vec2f(self, p, &copy);
 if (p->h2c != 0) {
  Trig* n1;
  int var_r28;
  void* r27 = createTrigger__12stTriggerMngFQ27Gimmick8AreaKindP13stTriggerData(g_stTriggerMng, 0, &p->h2c);
  if (p->b2e & 1) {
   var_r28 = 1;
   Obj1C* t = (Obj1C*)self->slot2();
   t->b7 = 0;
  } else {
   var_r28 = 0;
  }
  n1 = (Trig*)__nw__FUlQ25Heaps8HeapType(0x14, 0xF);
  if (n1 != 0) {
   void* v = self->slot2();
   n1->u0 = 0;
   n1->u8 = 0;
   n1->u4 = lbl_27_data_4C498;
   n1->uC = var_r28;
   n1->u10 = v;
  }
  setValidEdgeCallback__9stTriggerFP10gfCallBack(r27, n1);
  Trig2* n2 = (Trig2*)__nw__FUlQ25Heaps8HeapType(0x14, 0xF);
  if (n2 != 0) {
   void* v = self->slot2();
   n2->u0 = 0;
   n2->u8 = 0;
   n2->u4 = lbl_27_data_4C498;
   n2->uC = var_r28 == 0;
   n2->u10 = v;
  }
  setUnValidEdgeCallback__9stTriggerFP10gfCallBack(r27, n2);
 }
 Req sp18;
 sp18.b = 0x12;
 if (p->b29 == 0) {
  sp18.c = (float)p->b28;
 } else {
  sp18.c = (float)(-(int)p->b28);
 }
 sp18.d = p->h2a;
 self->slot3(&sp18);
 if (lbl_27_bss_5780 == 0) {
  void* mgr = __nw__FUlQ25Heaps8HeapType(0x54, 2);
  if (mgr != 0) mgr = __ct__13stAreaManagerFv(mgr);
  lbl_27_bss_5780 = mgr;
 }
 self->result = setHitPointEffect__13stAreaManagerFP28stHitPointEffectAreaInfoData(lbl_27_bss_5780, &sp18);
 return self;
}
}

// TODO: clean up, give variables real names
namespace jj_236F58 {

extern "C" s32 enumIncludeEntryId__9ftManagerCFi(ftManager* manager, void* base, s32* ids, void* buf, s32 flag);

struct FnArg {
    u8 pad0[0x8];
    u8* flag;
    u8 pad1[0x4];
    u8 base[0x10];
    s32 playerNo;
    u8 flag2;
};

extern "C" void fn_27_236F58(FnArg* arg) {
    s32 ids[16];
    u8 buf[0xC0];
    if (g_ftManager != NULL) {
        s32 count = enumIncludeEntryId__9ftManagerCFi(g_ftManager, arg->base, ids, buf, 0);
        for (s32 i = 0; i < count; i++) {
            if (arg->playerNo == g_ftManager->getPlayerNo(ids[i])) {
                *arg->flag = 1;
            }
        }
        if (arg->flag2 != 0) {
            for (s32 i = 0; i < count; i++) {
                if (g_ftManager->getPlayerNo(ids[i]) == 1) {
                    *arg->flag = 1;
                }
            }
        }
    }
}
}

