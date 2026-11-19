#include <st/operator/st_operator_sudden_death.h>
#include <types.h>

stOperatorSuddenDeath::stOperatorSuddenDeath() : stOperatorReadyGo(true) {
    m_state = 0;
    unk4C = 62.0f;
}

stOperatorSuddenDeath::~stOperatorSuddenDeath() { }

bool stOperatorSuddenDeath::isEnd() {
    return m_state == 5;
}

void stOperatorSuddenDeath::startOperator() {
    m_state = 1;
}

// TODO: clean up, give variables real names
namespace jj_247134 {
struct sndSystem;struct B{char p[0x3c];};
#define V(n) virtual void v##n();
class Stage:public B{public:V(0)V(1)V(2)V(3)V(4)V(5)V(6)V(7)V(8)V(9)V(10)V(11)V(12)V(13)V(14)V(15)V(16)V(17)V(18)V(19)V(20)V(21)V(22)V(23)V(24)V(25)V(26)V(27)V(28)V(29)V(30)V(31)V(32)V(33)V(34)virtual void v35();V(36)V(37)V(38)V(39)V(40)V(41)V(42)V(43)V(44)V(45)V(46)V(47)V(48)V(49)V(50)V(51)V(52)V(53)V(54)V(55)V(56)V(57)V(58)V(59)V(60)V(61)V(62)V(63)V(64)V(65)V(66)V(67)V(68)V(69)V(70)V(71)V(72)V(73)V(74)V(75)V(76)V(77)V(78)V(79)V(80)V(81)V(82)V(83)V(84)V(85)V(86)V(87)V(88)V(89)V(90)V(91)V(92)V(93)V(94)V(95)V(96)V(97)V(98)V(99)V(100)V(101)V(102)V(103)V(104)V(105)V(106)V(107)V(108)V(109)V(110)V(111)V(112)V(113)V(114)V(115)V(116)V(117)V(118)V(119)V(120)V(121)V(122)V(123)V(124)V(125)virtual void v126(int);};
#undef V
struct Obj{char p[0x40];unsigned char state,p41,f42,f43;char q[4];float timer,f4c;char r[4];unsigned char flags;char s[3];Stage*a[7];void*b;};
extern "C"{extern void*g_GameGlobal;extern void*g_sndSystem;extern void*g_IfMngr;extern Stage*g_Stage;extern const float lbl_27_rodata_3FB8,lbl_27_rodata_3FBC;extern const char lbl_27_data_4EB48[];void OSReport(const char*,...);float getGameFrameDelta__10GameGlobalCFv(void*);int isReady__13stOperatorBgmFv(const void*);void requestPlay__13stOperatorBgmFv(void*);int prepareSE__9sndSystemF5SndIDll(void*,int,int,int);void setPan__9sndSystemFlf(void*,int,float);void startSE__9sndSystemFlll(void*,int,int,int);void setCenterState__8IfCenterFi(void*,int);}
extern "C" void fn_27_247134(Obj*x){if(x->f43!=1&&x->f42!=1){float delta=getGameFrameDelta__10GameGlobalCFv(g_GameGlobal);float timer=x->timer+delta;x->timer=timer;switch(x->state){case 1:if(x->b&&!isReady__13stOperatorBgmFv(x->b)){OSReport(lbl_27_data_4EB48);return;}if(x->b)requestPlay__13stOperatorBgmFv(x->b);{int h=prepareSE__9sndSystemF5SndIDll(g_sndSystem,22,-1,-1);if(h!=-1){setPan__9sndSystemFlf(g_sndSystem,h,lbl_27_rodata_3FBC);startSE__9sndSystemFlll(g_sndSystem,h,0,0);}}{int h=prepareSE__9sndSystemF5SndIDll(g_sndSystem,0x1fdd,-1,-1);if(h!=-1){setPan__9sndSystemFlf(g_sndSystem,h,lbl_27_rodata_3FBC);startSE__9sndSystemFlll(g_sndSystem,h,0,0);}}{void*c=*(void**)((char*)g_IfMngr+0x44);*(float*)((char*)c+0x170)=delta;setCenterState__8IfCenterFi(c,6);}g_Stage->v126(8);x->state=3;x->timer=lbl_27_rodata_3FBC;return;
case 3:x->f4c=x->f4c-delta;if(timer>=lbl_27_rodata_3FB8){int h=prepareSE__9sndSystemF5SndIDll(g_sndSystem,0x1fdb,-1,-1);g_Stage->v126(7);if(h!=-1){setPan__9sndSystemFlf(g_sndSystem,h,lbl_27_rodata_3FBC);startSE__9sndSystemFlll(g_sndSystem,h,0,0);}int i=0;do{if(x->a[i])x->a[i]->v35();++i;}while(i<7);unsigned char f=x->flags;x->state=4;x->timer=lbl_27_rodata_3FBC;x->f4c=lbl_27_rodata_3FBC;x->flags=f|0x80;}break;
case 4:{void*c=*(void**)((char*)g_IfMngr+0x44);if(*(int*)((char*)c+4)==0)x->state=5;break;}
case 5:break;
default:break;}}}
}

