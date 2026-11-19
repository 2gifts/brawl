#include <gr/gr_visible_production_sound_effect.h>
#include <types.h>

extern int lbl_27_data_57DB0[20];

grVisibleProductionSoundEffect* grVisibleProductionSoundEffect::create(grGimmick* ground, grGimmick::SimpleEffectData* data) {
    return new (Heaps::StageInstance) grVisibleProductionSoundEffect(ground, data);
}

void grVisibleProductionSoundEffect::initialize() { }

void grVisibleProductionSoundEffect::remove() { }

void grVisibleProductionSoundEffect::productFlowStandby() { }

void grVisibleProductionSoundEffect::productFlowStart() {
    m_frame = 0.0f;
    m_started = false;
    m_soundHandle = -1;
    setState(lbl_27_data_57DB0[0]);
}

void grVisibleProductionSoundEffect::productFlowProducing(float deltaFrame) { }

void grVisibleProductionSoundEffect::productFlowEnd() { }

void grVisibleProductionSoundEffect::setForcedEnding() { }

#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <revolution/OS/OSError.h>
// TODO: clean up, give variables real names
namespace jj_271170 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void __ct__14snd3DGeneratorFv(void*);
extern "C" unsigned char lbl_27_data_57CB0[];
extern "C" unsigned char lbl_27_data_57E00[];
extern "C" const float lbl_27_rodata_4460;

struct Vec3 {
    float x, y, z;

    Vec3(float a, float b, float c) : x(a), y(b), z(c) {}

    Vec3& operator=(const Vec3& other) {
        ((unsigned int*)this)[0] = ((const unsigned int*)&other)[0];
        ((unsigned int*)this)[1] = ((const unsigned int*)&other)[1];
        ((unsigned int*)this)[2] = ((const unsigned int*)&other)[2];
        return *this;
    }
};

struct Input {
    unsigned int word;
    unsigned short half;
};

struct Obj {
    virtual void slot0();
    virtual void slot1();
    virtual void init();

    unsigned int field4;
    float field8;
    unsigned char fieldC, fieldD;
    unsigned short padE;
    int field10;
    Input input;
    Vec3 position;
    float field28;
    unsigned char field2C;
    unsigned char pad2D[3];
    unsigned char generator[8];
    int field38;
};

extern "C" Obj* fn_27_271170(int arg0, const Input* arg1) {
    Obj* obj = (Obj*)__nw__FUlQ25Heaps8HeapType(0x3C, 15);
    if (obj) {
        *(void**)obj = lbl_27_data_57CB0;
        obj->field4 = 0;
        obj->field8 = lbl_27_rodata_4460;
        obj->fieldC = 1;
        obj->fieldD = 0;
        *(void**)obj = lbl_27_data_57E00;
        obj->field10 = arg0;
        obj->input = *arg1;
        obj->position = Vec3(
            lbl_27_rodata_4460,
            lbl_27_rodata_4460,
            lbl_27_rodata_4460
        );
        obj->field28 = lbl_27_rodata_4460;
        obj->field2C = 0;
        __ct__14snd3DGeneratorFv(obj->generator);
        obj->field38 = -1;
        obj->init();
    }
    return obj;
}
}

// TODO: clean up, give variables real names
namespace jj_27129C {
struct Ground_base { char pad[0x3c]; };
class Ground : public Ground_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void slot2();
    virtual void slot3();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void slot20();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
    virtual void loadGroundData();
    virtual void slot25();
    virtual void setupMelee();
    virtual void slot27();
    virtual void setVisibilityByClipping();
    virtual void slot29();
    virtual void receiveCollMsg();
    virtual void slot31();
    virtual void receiveCollMsg_Heading();
    virtual void receiveCollMsg_Wall();
    virtual void receiveCollMsg_Attack();
    virtual void unloadData();
    virtual void getModelCount();
    virtual void slot37();
    virtual void slot38();
    virtual void setStageData();
    virtual void getStageData();
    virtual void initStageData();
    virtual void setMdlIndex();
    virtual void getMdlIndex();
    virtual void isEnableCalcCollision();
    virtual void enableCalcCollision();
    virtual void disableCalcCollision();
    virtual void getNodeIndex();
    virtual void getNodePosition(Vec3f*, unsigned long, unsigned long);
    virtual bool getNodePosition_2(Vec3f*, unsigned long, const char*);
};
struct Data { char pad[12]; int failure; int done; char report[44]; char node[18]; };
extern "C" Data lbl_27_data_57DA8;
extern "C" int isPlay__14snd3DGeneratorFl(snd3DGenerator*, int);
struct Object {
    char pad0[4];
    int state;
    char pad8[5];
    unsigned char flag;
    char padE[2];
    Ground* ground;
    SndID sound;
    unsigned short delay;
    char pad1a[2];
    Vec3f position;
    float timer;
    unsigned char started;
    char pad2d[3];
    snd3DGenerator generator;
    int handle;
};
extern "C" void fn_27_27129C(Object* self, float delta) {
    Data* data = &lbl_27_data_57DA8;
    if (!self->started) {
        if (self->timer >= (float)self->delay) {
            self->handle = self->generator.prepareSE(self->sound, -1);
            if (self->handle < 0) {
                OSReport(data->report, self->sound);
                self->state = data->failure;
            }
            if (!self->ground->getNodePosition_2(&self->position, 0, data->node)) {
                self->ground->getNodePosition(&self->position, 0, 0);
            }
            self->generator.setPos(&self->position);
            if (self->handle >= 0) {
                self->generator.startSE(self->handle, 0);
            }
            self->flag = 1;
            self->started = 1;
        } else {
            self->timer += delta;
        }
    } else if (!isPlay__14snd3DGeneratorFl(&self->generator, self->handle)) {
        self->state = data->done;
    } else {
        if (!self->ground->getNodePosition_2(&self->position, 0, data->node)) {
            self->ground->getNodePosition(&self->position, 0, 0);
        }
        self->generator.setPos(&self->position);
    }
}
}

