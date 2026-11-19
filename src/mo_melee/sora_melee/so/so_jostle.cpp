#include <so/jostle/so_jostle.h>
#include <types.h>

soJostle::soJostle(u8 priority, s32 arg) {
    m_0x0 = 0;
    m_0x4 = arg;
    m_0x14 = 0xFFFF;
    m_priority = priority;
    m_0x19 = 2;
    m_0x1A = 0;
    m_0x8 = 0.0f;
    m_logNo = -1;
    m_0x10 = 0.0f;
    m_0xC = 0.0f;
}

soJostle::~soJostle() { }

void soJostle::reset() {
    m_logNo = -1;
    m_0x8 = 0.0f;
    m_0x10 = 0.0f;
    m_0xC = 0.0f;
}

void soJostle::initLog() {
    m_logNo = -1;
    m_0x8 = 0.0f;
}

bool soJostle::setLogNo(s32 logNo) {
    if (m_logNo == -1) {
        m_logNo = logNo;
        return true;
    }
    return false;
}

void soJostle::setPriority(u8 priority) {
    m_priority = priority;
}

#include <nw4r/math.h>
#include <stdlib.h>
#include <nw4r/ut/ut_algorithm.h>
// TODO: clean up, give variables real names
namespace jj_C1B10 {

extern const float lbl_27_rodata_EC0;
extern "C" int randi__Fl(long);

struct Entry {
    int a;
    float b;
    int c;
    float d;
};

struct Self {
    int u0;
    float* arr;
    float u8;
    char pad[0x0F];
    signed char mode;
    short idx;
};

extern "C" void fn_27_C1B10(Self* self, float* p4, float* p5, float* p6, Entry* table, float f1, float f2)
{
    float f31 = f1;
    float f30 = f2;

    if (lbl_27_rodata_EC0 == f2) {
        if (self->mode >= 2) {
            f30 = *(float*)((char*)self->arr + 4);
        } else {
            f30 = self->arr[self->mode];
        }
    }

    if (*p5 == *p6) {
        if (lbl_27_rodata_EC0 == *p4) {
            if (randi__Fl(2) == 0) {
                f30 = f30;
            } else {
                f30 = -f30;
            }
            *p4 = f30;
        } else {
            if (*p4 < lbl_27_rodata_EC0) {
                f30 = f30;
            } else {
                f30 = -f30;
            }
            *p4 = f30;
        }
    } else if (*p5 > *p6) {
        *p4 = f30;
    } else {
        *p4 = -f30;
    }

    Entry* e = (Entry*)((char*)table + self->idx * 16);
    float d = (float)(double)nw4r::math::FAbs(*p6 - *p5);

    if (*p4 > lbl_27_rodata_EC0) {
        if (e->a == 0 || d < e->b) {
            if (lbl_27_rodata_EC0 == f31) {
                e->a = 2;
            } else if (f31 > lbl_27_rodata_EC0) {
                e->a = 3;
            } else {
                e->a = 1;
            }
            e->b = d;
        }
    } else if (e->c == 0 || d < e->d) {
        if (lbl_27_rodata_EC0 == f31) {
            e->c = 2;
        } else if (f31 > lbl_27_rodata_EC0) {
            e->c = 3;
        } else {
            e->c = 1;
        }
        e->d = d;
    }

    unsigned int flag = (e->a != 0 && e->c != 0);
    if (flag == 1) {
        self->u8 = lbl_27_rodata_EC0;
        return;
    }
    self->u8 = *p4;
}
}

// TODO: clean up, give variables real names
namespace jj_C1E74 {
struct Stage_base { char pad[0x3c]; };
class Stage : public Stage_base {
public:
virtual void processDefault(); virtual void processBegin(); virtual void processAnim(); virtual void processUpdate();
virtual void processPreMapCorrection(); virtual void processMapCorrection(); virtual void processFixPosition(); virtual void processPreCollision();
virtual void processCollision(); virtual void processCatch(); virtual void processHit(); virtual void processCamera();
virtual void processFixCamera(); virtual void processEffect(); virtual void processGameProc(); virtual void processEnd();
virtual void renderPre(); virtual void renderOpa(); virtual void renderXlu(); virtual void processDebug();
virtual void renderDebug(); virtual void init(); virtual ~Stage(); virtual void createObj();
virtual void createObjPokeTrainer(); virtual void getPokeTrainerPointNum(); virtual void getPokeTrainerPointData(); virtual void getPokeTrainerPositionZ();
virtual void getPokeTrainerDrawLayer(); virtual void isAdventureStage(); virtual void getItemPac(); virtual void getItemGenPac();
virtual void getItemPacEnemyFigure(); virtual void getEnemyPac(); virtual void getAdvRadarData(); virtual void initializeStage();
virtual void closeStage(); virtual void renderDebugPositions(); virtual void loading(); virtual void process();
virtual void updateStagePositions(); virtual void debugCollision(); virtual void getAIRange(); virtual void getDefaultLightSetIndex();
virtual void getZoneLightSetIndex(); virtual void getScrollDir(); virtual void clearCameraParam(); virtual void initCameraParam();
virtual void startLoadLocalData(); virtual void isLoadLocalData(); virtual void entryLocalData(); virtual void removeLocalData();
virtual void appearanceFighterLocal(); virtual void getFighterStartPos(); virtual void getFighterReStartPos(); virtual void isReStartSamePoint();
virtual void getPokeTrainerStartPos(); virtual void getItemPosCount(); virtual void getItemPos(); virtual void getRandItemPos();
virtual void getKirifudaPos(); virtual void getKirifudaAngle(); virtual void getKirifudaScale(); virtual void getKirifudaModelType();
virtual void getPokeTrainerPosCount(); virtual void getPokeTrainerPos(); virtual void getFighterDeadEffectSizeRate(); virtual void getEnemyDeadEffectSizeRate();
virtual float getEnableZ();
};
extern Stage* g_Stage;
extern const float lbl_27_rodata_EC0;
struct Params { char pad[8]; float acceleration; float maximum; };
struct Self {
int unk0; Params* params; float position; float unkC; float speed;
char pad14[5]; signed char previous; signed char overrideMode; char pad1B; short index;
};
struct Entry { int first; int unk4; int second; int unkC; };

extern "C" void fn_27_C1E74(Self* self, Entry* entries) {
    int first;
    int mode = 2;
    if (entries != 0) {
        Entry* entry = &entries[self->index];
        first = entry->first;
        unsigned int both = 0;
        if (first != 0 && entry->second != 0) {
            both = 1;
        }
        if (static_cast<unsigned long>(both) == 1UL) {
            int second = entry->second;
            if (first == second) {
                if (first == 2) mode = self->previous;
                else if (first == 3) mode = 1;
                else mode = 3;
            } else if (__abs(first - second) >= 2) {
                mode = self->previous;
            } else if (first == 1 || second == 1) {
                mode = 3;
            } else {
                mode = 1;
            }
        } else {
            int neither = 0;
            if (first == 0 && entry->second == 0) neither = 1;
            if (neither == 0) {
                if (first == 0) first = entry->second;
                if (first == 1) mode = 3;
                else if (first == 3) mode = 1;
                else if (self->position < lbl_27_rodata_EC0) mode = 1;
                else mode = 3;
            }
        }
    }
    int forced = self->overrideMode;
    float acceleration = self->params->acceleration;
    float maximum = self->params->maximum;
    if (forced != 0 && mode != 2) mode = forced;
    switch (mode) {
    case 2:
        if ((float)(double)nw4r::ut::Abs(self->speed) < acceleration) {
            self->speed = lbl_27_rodata_EC0;
        } else if (self->speed > lbl_27_rodata_EC0) {
            self->speed -= acceleration;
        } else {
            self->speed += acceleration;
        }
        break;
    case 3:
        self->speed += acceleration;
        if (self->speed > maximum) self->speed = maximum;
        break;
    case 1:
        self->speed -= acceleration;
        if (self->speed < -maximum) self->speed = -maximum;
        float limit = g_Stage->getEnableZ();
        if (limit > lbl_27_rodata_EC0) {
            if (self->speed < -limit) self->speed = -limit;
        }
        break;
    }
}
}

