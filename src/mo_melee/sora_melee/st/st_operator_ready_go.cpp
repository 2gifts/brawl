#include <snd/snd_id.h>
#include <sr/sr_common.h>
#include <st/operator/st_operator_ready_go.h>
#include <types.h>

static StSeUtil::UnkStruct s_seqCountDown[4] = {
    {(SndID)0x1FE5, 0.0f, 0.0f, 0.0f},
    {(SndID)0x1FE6, 0.0f, 60.0f, 0.0f},
    {(SndID)0x1FE7, 0.0f, 120.0f, 0.0f},
    {(SndID)0x1FDB, 0.0f, 180.0f, 0.0f},
};
static StSeUtil::UnkStruct s_seqReadyGo[2] = {
    {(SndID)0x1FDA, 0.0f, 0.0f, 0.0f},
    {(SndID)0x1FDB, 0.0f, 60.0f, 0.0f},
};
static SndID s_seIds[5] = {(SndID)0x1FE5, (SndID)0x1FE6, (SndID)0x1FE7, (SndID)0x1FDB, (SndID)0x1FDA};

stOperatorReadyGo::stOperatorReadyGo(bool flag) :
    stOperator("OperatorGo"), unk48(0.0f), unk54_b7(0), unk54_b6(flag), unk54_b5(0), unk54_b4(0), unk74(0) {
    unk58 = 0;
    unk5C = 0;
    unk60 = 0;
    unk64 = 0;
    unk68 = 0;
    unk6C = 0;
    unk70 = 0;
    m_state = 0;
    unk4C = flag ? 182.0f : 62.0f;
    unk50 = unk4C;
    m_seSeq.registId(s_seIds, 5);
    if (flag) {
        m_seSeq.registSeq(0, s_seqCountDown, 4, Heaps::System);
    } else {
        m_seSeq.registSeq(0, s_seqReadyGo, 2, Heaps::System);
    }
}

stOperatorReadyGo::~stOperatorReadyGo() { }

bool stOperatorReadyGo::isEnd() {
    return m_state == 5;
}

void stOperatorReadyGo::startOperator() {
    m_state = 1;
}

#include <snd/snd_system.h>
// TODO: clean up, give variables real names
namespace jj_2466C0 {

struct Object_vptr_3c_base {
    char pad[0x3c];
};

#define V(n) virtual void v##n();
class Object_vptr_3c : public Object_vptr_3c_base {
public:
    V(0) V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9) V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17)
    V(18) V(19) V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29) V(30) V(31) V(32) V(33) V(34)
    virtual void initializeStage();
    V(36) V(37) V(38) V(39) V(40) V(41) V(42) V(43) V(44) V(45) V(46) V(47) V(48) V(49) V(50) V(51) V(52)
    V(53) V(54) V(55) V(56) V(57) V(58) V(59) V(60) V(61) V(62) V(63) V(64) V(65) V(66) V(67) V(68) V(69)
    V(70) V(71) V(72) V(73) V(74) V(75) V(76) V(77) V(78) V(79) V(80) V(81) V(82) V(83) V(84) V(85) V(86)
    V(87) V(88) V(89) V(90) V(91) V(92) V(93) V(94) V(95) V(96) V(97) V(98) V(99) V(100) V(101) V(102) V(103)
    V(104) V(105) V(106) V(107) V(108) V(109) V(110) V(111) V(112) V(113) V(114) V(115) V(116) V(117) V(118) V(119) V(120)
    V(121) V(122) V(123) V(124) V(125)
    virtual void setVision(unsigned char);
};
#undef V

struct Obj {
    char p[0x40];
    unsigned char state;
    char p41;
    unsigned char f42;
    unsigned char f43;
    void* sub44;
    float timer;
    float f4c;
    float f50;
    unsigned char flags;
    char s55[3];
    Object_vptr_3c* a[7];
    void* b;
};

extern "C" {
extern void* g_GameGlobal;
extern void* g_IfMngr;
extern void* g_ftManager;
extern Object_vptr_3c* g_Stage;
extern const float lbl_27_rodata_3F80[];
float getGameFrameDelta__10GameGlobalCFv(void*);
int isReady__13stOperatorBgmFv(void*);
void requestPlay__13stOperatorBgmFv(void*);
void setCenterState__8IfCenterFi(void*, int);
void readyGo__9ftManagerFv(void*);
void playFrame__Q28StSeUtil5SeSeqFlf(void*, long, float);
}

extern "C" void fn_27_2466C0(Obj* o) {
    const float* ro = lbl_27_rodata_3F80;

    if (o->f43 != 1 && o->f42 != 1) {
        float delta = getGameFrameDelta__10GameGlobalCFv(g_GameGlobal);
        o->timer = o->timer + delta;

        switch (o->state) {
        case 1:
            if (o->b != NULL) {
                if (isReady__13stOperatorBgmFv(o->b) == 0) {
                    break;
                }
                if (g_IfMngr != NULL) {
                    int v;
                    void* gg8 = *(void**)((char*)g_GameGlobal + 8);
                    if (*(u16*)((char*)gg8 + 0x1a) != 0x25) {
                        v = *(int*)((char*)o->b + 0x50);
                    } else {
                        v = -1;
                    }
                    if ((o->flags >> 6) & 1) {
                        void* ifo = *(void**)((char*)g_IfMngr + 0x44);
                        *(int*)((char*)ifo + 0x168) = v;
                        *(u8*)((char*)ifo + 0x16c) = 1;
                        *(float*)((char*)ifo + 0x170) = delta;
                        setCenterState__8IfCenterFi(ifo, 2);
                        g_Stage->setVision(4);
                    } else {
                        void* ifo = *(void**)((char*)g_IfMngr + 0x44);
                        *(int*)((char*)ifo + 0x168) = v;
                        *(u8*)((char*)ifo + 0x16c) = 1;
                        *(float*)((char*)ifo + 0x170) = delta;
                        setCenterState__8IfCenterFi(ifo, 1);
                        g_Stage->setVision(1);
                    }
                }
                requestPlay__13stOperatorBgmFv(o->b);
            }
            o->state = 3;
            o->timer = ro[0];
            break;

        case 3:
            playFrame__Q28StSeUtil5SeSeqFlf((char*)o + 0x78, 0, o->timer);
            o->f4c = o->f4c - delta;
            if ((o->flags >> 6) & 1) {
                if (o->timer >= ro[3]) {
                    if (!((o->flags >> 5) & 1)) {
                        g_Stage->setVision(5);
                        o->flags = o->flags | 0x20;
                    }
                }
                if (o->timer >= ro[4]) {
                    if (!((o->flags >> 4) & 1)) {
                        g_Stage->setVision(6);
                        o->flags = o->flags | 0x10;
                    }
                }
            }
            if (o->timer >= o->f50) {
                int i = 0;
                do {
                    if (o->a[i] != NULL) {
                        o->a[i]->initializeStage();
                    }
                    i++;
                } while (i < 7);
                readyGo__9ftManagerFv(g_ftManager);
                g_Stage->setVision(7);
                int v = (*(u8*)((char*)o->sub44 + 8) >> 2) & 0x3F;
                switch (v) {
                case 0x10:
                case 0xA:
                    g_sndSystem->playSE((SndID)0x16, -1, 0, 0, -1);
                    break;
                default:
                    g_sndSystem->playSE((SndID)0x1B, -1, 0, 0, -1);
                    break;
                }
                o->state = 4;
                o->timer = ro[0];
                o->f4c = ro[0];
                o->flags = o->flags | 0x80;
            }
            break;

        case 4: {
            void* gg = g_GameGlobal;
            void* gg8 = *(void**)((char*)gg + 8);
            u16 id = *(u16*)((char*)gg8 + 0x1a);
            if (id == 5 || id == 8 || id == 0xE) {
                void* p = *(void**)((char*)gg + 0x44);
                *(float*)((char*)p + 4) = *(float*)((char*)p + 4) + ro[5];
                if (*(float*)((char*)*(void**)((char*)g_GameGlobal + 0x44) + 4) >= ro[6]) {
                    *(float*)((char*)*(void**)((char*)g_GameGlobal + 0x44) + 4) = ro[6];
                }
            }
            void* ifm = g_IfMngr;
            if (ifm != NULL && *(int*)((char*)*(void**)((char*)ifm + 0x44) + 4) == 0) {
                if (ro[6] == *(float*)((char*)*(void**)((char*)g_GameGlobal + 0x44) + 4)) {
                    o->state = 5;
                }
            }
            break;
        }

        case 5:
            break;

        default:
            break;
        }
    }
}
}

