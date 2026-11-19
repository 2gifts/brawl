#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <so/so_external_value_accesser.h>
#include <so/so_world.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>
#include <gf/gf_camera.h>

void stMelee::setPlayerPositionIndexSerial() {
    m_playerPositionIndex[0] = 0;
    m_playerPositionIndex[1] = 1;
    m_playerPositionIndex[2] = 2;
    m_playerPositionIndex[3] = 3;
}

void stMelee::setPlayerPositionIndexFixed(int index) {
    m_playerPositionIndex[0] = index;
    m_playerPositionIndex[1] = index;
    m_playerPositionIndex[2] = index;
    m_playerPositionIndex[3] = index;
}

stMelee::~stMelee() {
    g_soWorld->m_gravityUp = 1.0f;
    g_soWorld->m_gravityDown = 1.0f;
    g_efScreen->m_notify = NULL;
    if (m_wind2ndData != NULL) {
        delete m_wind2ndData;
    }
}

void stMelee::setGravityNormal() {
    g_soWorld->m_gravityUp = 1.0f;
    g_soWorld->m_gravityDown = 1.0f;
}

void stMelee::setGravityHalf() {
    g_soWorld->m_gravityUp = 0.5f;
    g_soWorld->m_gravityDown = 0.5f;
}

grCollision* stMelee::createCollisionSelf(stCollisionWork* collisionWork, Ground* ground, const char* name1, const char* name2, u16 unk) {
    grCollision* collision;
    if (collisionWork == NULL) {
        return NULL;
    }
    grCollData* collData = new (Heaps::StageResource) grCollData;
    if (collData == NULL) {
        return NULL;
    }
    initCollData_fromCollWork(collData, collisionWork, name1, name2, unk);
    collision = grCollision::create(this, collData, ground, 1);
    setCollision(collision);
    delete collData;
    return collision;
}

void stMelee::updateWind2ndOnly() {
    if (m_wind2ndTrigger && m_wind2ndData) {
        m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);
    }
}

void stMelee::zoomOutCamera(float verticalRotationFactor, float horizontalRotationFactor) {
    cmStageParam* param = m_cameraParam1;
    if (param != NULL) {
        m_savedVerticalRotationFactor = param->m_verticalRotationFactor;
        m_savedHorizontalRotationFactor = param->m_horizontalRotationFactor;
        param->m_verticalRotationFactor = verticalRotationFactor;
        param->m_horizontalRotationFactor = horizontalRotationFactor;
    }
}

void stMelee::zoomInCamera() {
    cmStageParam* param = m_cameraParam1;
    if (param != NULL) {
        param->m_verticalRotationFactor = m_savedVerticalRotationFactor;
        param->m_horizontalRotationFactor = m_savedHorizontalRotationFactor;
    }
}

void stMelee::processBegin() {
    stCommonGimmick::processBegin();
}

void stMelee::processUpdate() {
    if (m_isPaused != true) {
        float deltaFrame = g_GameGlobal->getGameFrameDelta();
        deltaFrame *= g_GameGlobal->m_stageData->m_motionRatio;
        deltaFrame *= g_GameGlobal->m_stageData->m_motionSubRatio;
        g_ecMgr->pushsetCurrentGroup(0x2000000);
        update(deltaFrame);
        g_ecMgr->popCurrentGroup();
    }
}

void stMelee::update(float deltaFrame) { }

bool stMelee::getPlayerPositionCore(int playerNo, Vec3f* pos, int kind) {
    ftManager* manager = g_ftManager;
    int entryId = manager->getEntryId(playerNo);
    if (entryId == -1) {
        return false;
    }
    if (!manager->isFighterActivate(entryId, -1)) {
        return false;
    }
    if (!manager->isFighterPositionAccessable(entryId, -1)) {
        return false;
    }
    Fighter* fighter = manager->getFighter(entryId, -1);
    if (fighter != NULL) {
        Vec3f result;
        switch (kind) {
            case 0:
                result = soExternalValueAccesser::getPos(fighter);
                break;
            case 1:
                result = ftExternalValueAccesser::getHipPos(fighter);
                break;
            case 2:
                result = ftExternalValueAccesser::getCursorPos(fighter);
                break;
        }
        *pos = result;
        return true;
    }
    return false;
}

bool stMelee::getPlayerPositionEntryId(int entryId, Vec3f* pos) {
    if (entryId == -1) {
        return false;
    }
    ftManager* manager = g_ftManager;
    if (!manager->isFighterActivate(entryId, -1)) {
        return false;
    }
    if (!manager->isFighterPositionAccessable(entryId, -1)) {
        return false;
    }
    Fighter* fighter = manager->getFighter(entryId, -1);
    if (fighter != NULL) {
        Vec3f result;
        result = soExternalValueAccesser::getPos(fighter);
        *pos = result;
        return true;
    }
    return false;
}

bool stMelee::getPlayerPosition(int playerNo, Vec3f* pos) {
    return getPlayerPositionCore(playerNo, pos, 0);
}

bool stMelee::getPlayerHipPosition(int playerNo, Vec3f* pos) {
    return getPlayerPositionCore(playerNo, pos, 1);
}

bool stMelee::getPlayerCursorPosition(int playerNo, Vec3f* pos) {
    return getPlayerPositionCore(playerNo, pos, 2);
}

void stMelee::getFighterStartPos(Vec3f* startPos, int fighterIndex) {
    // HYPOTHESIS: index 30 asks for a random restart point (compared unsigned in the original).
    if (fighterIndex == 30u) {
        u32 last = m_lastReStartPosIndex;
        u32 index;
        do {
            index = randi(4);
            m_lastReStartPosIndex = index;
        } while (index == last);
        Stage::getFighterReStartPos(startPos, index);
        startPos->m_y += 20.0f;
    } else {
        Stage::getFighterStartPos(startPos, m_playerPositionIndex[fighterIndex]);
    }
}

void stMelee::getFighterReStartPos(Vec3f* startPos, int fighterIndex) {
    // HYPOTHESIS: index 30 asks for a random restart point (compared unsigned in the original).
    if (fighterIndex == 30u) {
        u32 last = m_lastReStartPosIndex;
        u32 index;
        do {
            index = randi(4);
            m_lastReStartPosIndex = index;
        } while (index == last);
        Stage::getFighterReStartPos(startPos, index);
        startPos->m_y += 20.0f;
    } else {
        Stage::getFighterReStartPos(startPos, fighterIndex);
    }
}

int stMelee::playSeBasic(SndID id, float pan) {
    s32 handle = g_sndSystem->prepareSE(id, -1, -1);
    if (handle == -1) {
        return -1;
    }
    g_sndSystem->setPan(handle, pan);
    g_sndSystem->startSE(handle, 0, 0);
    return handle;
}

void stMelee::stopSeBasic(int handle, float fadeFrames) {
    if (handle != -1) {
        g_sndSystem->stopSE(static_cast<s32>(handle), static_cast<s32>(60.0f * fadeFrames));
    }
}

void stMelee::setSePan(int handle, float pan) {
    if (handle != -1) {
        g_sndSystem->setPan(handle, pan);
    }
}

bool stMelee::checkChangeScene() {
    return false;
}

void stMelee::resetChangeScene() { }

void stMelee::setChangeSceneNumber(s32 n) { }

void stCollisionWork::initialize() {
    m_vtxDatas = NULL;
    m_lineDatas = NULL;
    m_jointDatas = NULL;
    m_vtxLen = 0;
    m_isClosed = false;
}

void stCollisionWork::destroy() {
    // BYTES: the three tables are freed with a plain __dla (no array cookie).
    if (m_vtxDatas != NULL) {
        delete[] reinterpret_cast<u8*>(m_vtxDatas);
    }
    if (m_lineDatas != NULL) {
        delete[] reinterpret_cast<u8*>(m_lineDatas);
    }
    if (m_jointDatas != NULL) {
        delete[] reinterpret_cast<u8*>(m_jointDatas);
    }
    m_vtxDatas = NULL;
    m_lineDatas = NULL;
    m_jointDatas = NULL;
}

bool stCollisionWork::isValid() {
    return m_vtxLen != 0;
}

void stMelee::getItemGenPac(gfArchive** archive) {
    *archive = &m_itemSheetArchive;
}

void stMelee::calcCamera(Vec3f* eye, Vec3f* target, float roll, float fov) {
    gfCameraManager* manager = gfCameraManager::getManager();
    if (manager != NULL && (eye->m_x != target->m_x || eye->m_y != target->m_y || eye->m_z != target->m_z)) {
        gfCamera* camera = manager->getCamera(0);
        camera->m_transformFlag.m_mask = 0x43;
        camera->m_centerPos = *eye;
        camera->m_targetPos = *target;
        u16 flags = camera->unkFA.m_mask;
        camera->m_rot.m_z = roll;
        camera->unkFA.m_mask = flags | 0x42;
        camera->m_fovScale = fov;
        camera->calc();
    }
}

#include <gf/gf_3d_scene.h>
#include <revolution/OS/OSFastCast.h>
// TODO: clean up, give variables real names
namespace jj_238BDC {
extern "C" {
void __ct__15stCommonGimmickFPCcQ26Stages11srStageKind(void*, const char*, int);
extern char __vt__7stMelee[];
extern char lbl_27_bss_58F0[];
extern const float lbl_27_rodata_3930;
extern const float lbl_27_rodata_3934;
extern char* g_GameGlobal;
}
struct SceneFlags { unsigned char high:1; unsigned char copy:1; unsigned char low:6; };
struct stMelee {
char pad0[0x3C]; char* unk3C; char pad40[0x160];
int unk1A0, unk1A4, unk1A8;
unsigned char unk1AC, unk1AD; char pad1AE[6];
int unk1B4, unk1B8, unk1BC, unk1C0, unk1C4, unk1C8, unk1CC;
float unk1D0, unk1D4;
};
extern "C" stMelee* __ct__7stMeleeFPCcQ26Stages11srStageKind(stMelee* self, const char* name, int kind) {
__ct__15stCommonGimmickFPCcQ26Stages11srStageKind(self,name,kind);
self->unk3C=__vt__7stMelee;
self->unk1D0=lbl_27_rodata_3930;
self->unk1D4=lbl_27_rodata_3930;
self->unk1AC=0; self->unk1A0=0; self->unk1A4=0; self->unk1A8=0;
self->unk1AD=0; self->unk1C4=0; self->unk1C8=0;
self->unk1B4=0; self->unk1B8=1; self->unk1BC=2; self->unk1C0=3;
unsigned char* st=*(unsigned char**)(g_GameGlobal+8);
unsigned int t=(st[8]>>2)&0x3F;
switch(t) {
case 0: case 1: case 14: case 23: case 24: case 28: case 33: {
int var=0;
if(st[0x99]!=3) var=1;
if(st[0xF5]!=3) var+=1;
if(st[0x151]!=3) var+=1;
if(st[0x1AD]!=3) var+=1;
unsigned int ia,m;
int n=var-1;
int k=3;
if(n<=3) k=n;
if(k>0) {
m=(unsigned int)(k+1);
for(int cnt=0;cnt<0x20;cnt++) {
ia=(unsigned int)randi(m);
if(ia>=(unsigned int)k) ia=(unsigned int)k;
unsigned int ib=(unsigned int)randi(m);
if(ib>=(unsigned int)k) ib=(unsigned int)k;
char* pa=(char*)self+ia*4;
char* pb=(char*)self+ib*4;
int tmp=*(int*)(pa+0x1B4);
*(int*)(pa+0x1B4)=*(int*)(pb+0x1B4);
*(int*)(pb+0x1B4)=tmp;
}
}
break;
}
}
((SceneFlags*)((char*)g_gfSceneRoot+0x464))->copy=true;
g_soWorld->m_gravityUp=lbl_27_rodata_3934;
g_soWorld->m_gravityDown=lbl_27_rodata_3934;
*(void**)(lbl_27_bss_58F0+8)=self;
*(char**)((char*)g_efScreen+0x1DC)=lbl_27_bss_58F0;
self->unk1CC=0xFFFF;
return self;
}
}

// TODO: clean up, give variables real names
namespace jj_2394CC {
struct Rect {
    float left, right, up, down;
};

struct cmAIController {
    unsigned char pad[0x94];
    unsigned char flags;
    unsigned char pad95[0xF3];
    Rect range;
};

extern cmAIController* g_cmAIController;

extern "C" void setCameraLimitRange__7stMeleeFffff(
    void*, float a, float b, float c, float d)
{
    cmAIController* camera = g_cmAIController;
    if (camera) {
        Rect range;
        range.left = camera->range.left;
        range.right = camera->range.right;
        camera->flags |= 2;
        range.left = a;
        range.right = b;
        range.up = c;
        range.down = d;
        camera->range = range;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_239E58 {

extern "C" {
extern const float lbl_27_rodata_3930[];
float SinFIdx__Q24nw4r4mathFf(float);
float CosFIdx__Q24nw4r4mathFf(float);

void fn_27_239E58(void*, float* out, int mode,
                  float start, float end, float t) {
    const float* data = lbl_27_rodata_3930;
    if (out != 0) {
        if (t < data[0]) t = data[0];
        if (t > data[1]) t = data[1];

        float delta = end - start;
        switch (mode) {
        case 1: {
            t = data[13] * t;
            unsigned short angle = __OSf32tou16(t);
            t = SinFIdx__Q24nw4r4mathFf(data[14] * __OSu16tof32(&angle));
            break;
        }
        case 2: {
            unsigned short angle = __OSf32tou16(data[13] * t);
            t = CosFIdx__Q24nw4r4mathFf(data[14] * __OSu16tof32(&angle));
            break;
        }
        }
        *out = start + delta * t;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_239FEC {
typedef unsigned long long u64;

extern "C" {
extern void* g_ftManager;
int getEntryId__9ftManagerFi(void*, int);
u64 getFighterRhombusCenterPos__9ftManagerCFii(void*, int, int);
extern float lbl_27_rodata_3930;
}

union Position2 {
    u64 bits;
    struct {
        float x, y;
    } pos;
};

extern "C" void fn_27_239FEC(void*, int slot, float* out) {
    void* manager = g_ftManager;
    Position2 p;
    u64 bits = getFighterRhombusCenterPos__9ftManagerCFii(
        manager, getEntryId__9ftManagerFi(manager, slot), -1);
    p = *reinterpret_cast<Position2*>(&bits);

    float z = lbl_27_rodata_3930;
    float y = p.pos.y;
    float x = p.pos.x;

    out[0] = x;
    out[1] = y;
    out[2] = z;
}
}

// TODO: clean up, give variables real names
namespace jj_23A074 {
struct Color {
    unsigned char r, g, b, a;
};

struct S {
    int a;
    int b;
    void* m;
};

extern "C" void setBlendColorEnable__5StageFv(void*);
extern "C" void setBlendColor__5StageF8_GXColor(void*, const Color*, unsigned char, unsigned char);
extern "C" void setBlendColorDisable__5StageFv(void*);

extern "C" void fn_27_23A074(S* s, int mode, Color* c) {
    unsigned char a, b, g, r;

    switch (mode) {
    case 0:
        setBlendColorEnable__5StageFv(s->m);
        c->a = 0;
    case 1: {
        r = c->r;
        g = c->g;
        b = c->b;
        a = c->a;
        Color t;
        t.r = r;
        t.g = g;
        t.b = b;
        t.a = a;
        setBlendColor__5StageF8_GXColor(s->m, &t, g, r);
        break;
    }
    case 2:
        setBlendColorDisable__5StageFv(s->m);
        break;
    }
}
}

