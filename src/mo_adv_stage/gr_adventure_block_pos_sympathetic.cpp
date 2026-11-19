#include <mt/mt_vector.h>
#include <sora_adv_stage/gr_adventure_block_sympathetic_manager.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_gimmick_block_sympathetic.h>
#include <st/st_trigger.h>

grGimmickBlockSympathetic::~grGimmickBlockSympathetic() { }

grAdventureBlockSympatheticManager* grAdventureBlockSympatheticManager::create(int mdlIndex, const char* taskName) {
    grAdventureBlockSympatheticManager* gimmick = new (Heaps::StageInstance) grAdventureBlockSympatheticManager(taskName);
    gimmick->setMdlIndex(mdlIndex);
    return gimmick;
}

void grAdventureBlockSympatheticManager::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<Data*>(m_gimmickData);
    createBlock(data);

    grGimmickBlockSympathetic* block;
    u32 count;
    u32 i = 0;
    nw4r::g3d::ResMdl mdl = m_sceneModels[0]->m_resMdl;
    Node* head = m_head;
    count = m_count;
    m_cursor = head;
    for (; i < count; i++) {
        block = m_cursor->m_block;
        nw4r::g3d::ResNode node = mdl.GetResNode(static_cast<int>(block->unk150));
        Vec3f pos;
        pos.m_x = node->m_translation.m_x;
        pos.m_y = node->m_translation.m_y;
        pos.m_z = node->m_translation.m_z;
        float radius = m_data->unk44;
        u8 kind = m_data->unk40;
        block->setPos(&pos);
        block->updateCallback(0);
        block->updateG3dProcCalcWorld();
        Vec2f xy;
        xy.m_x = pos.m_x;
        xy.m_y = pos.m_y;
        block->m_hitCircle = addHitCircle(&xy, kind, 1, radius);
        m_cursor = m_cursor->m_next;
    }
    if (m_data->unk48.m_triggerId != 0) {
        g_stTriggerMng->createTrigger(Gimmick::Area_None, &m_data->unk48);
    }
    createInvalidCameraClippingSphere();
}

void grAdventureBlockSympatheticManager::setInitializeFlag() {
    if (m_data->unk48.m_triggerId != 0 && g_stTriggerMng->getTriggerFlag(&m_data->unk48, true) == true) {
        u32 count = m_count;
        m_cursor = m_head;
        for (u32 i = 0; i < count; i++) {
            m_cursor->m_block->setValid(0);
            m_cursor = m_cursor->m_next;
        }
    }
}

void grAdventureBlockSympatheticManager::notifyHitCircle(void* circle) {
    u32 count = m_count;
    m_cursor = m_head;
    for (u32 i = 0; i < count; i++) {
        grGimmickBlockSympathetic* block = m_cursor->m_block;
        if (circle == block->m_hitCircle) {
            block->requestBreak();
            enableHitCircle(0, 1, circle);
            break;
        }
        m_cursor = m_cursor->m_next;
    }
}

#include <gf/gf_archive.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_data.h>
#include <gr/ground.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <st/stage.h>
#include <cm/cm_camera_controller.h>
// TODO: clean up, give variables real names
namespace jj_221A0 {
struct GroundListNode { GroundListNode* prev; GroundListNode* next; void* ground; };
struct GroundList { GroundListNode* first; GroundListNode* last; GroundListNode* current; u32 count; };
struct GimmickPrefix { char pad[0x3c]; };
class GimmickDispatch : public GimmickPrefix {
public:
#define SLOT(n) virtual void slot##n()
 SLOT(0); SLOT(1); SLOT(2); SLOT(3); SLOT(4); SLOT(5); SLOT(6); SLOT(7);
 SLOT(8); SLOT(9); SLOT(10); SLOT(11); SLOT(12); SLOT(13); SLOT(14); SLOT(15);
 SLOT(16); SLOT(17); SLOT(18); SLOT(19); SLOT(20); SLOT(21); SLOT(22); SLOT(23);
 SLOT(24); SLOT(25); SLOT(26); SLOT(27); SLOT(28); SLOT(29); SLOT(30); SLOT(31);
 SLOT(32); SLOT(33); SLOT(34); SLOT(35); SLOT(36);
 virtual void startup(gfArchive*, u32, u32);
 SLOT(38); SLOT(39); SLOT(40); SLOT(41); SLOT(42); SLOT(43); SLOT(44); SLOT(45);
 SLOT(46); SLOT(47); SLOT(48); SLOT(49); SLOT(50); SLOT(51); SLOT(52); SLOT(53);
 SLOT(54); SLOT(55); SLOT(56); SLOT(57); SLOT(58); SLOT(59); SLOT(60); SLOT(61);
 SLOT(62); SLOT(63); SLOT(64); SLOT(65); SLOT(66); SLOT(67); SLOT(68); SLOT(69);
 SLOT(70); SLOT(71); SLOT(72); SLOT(73); SLOT(74); SLOT(75); SLOT(76); SLOT(77);
 SLOT(78); SLOT(79);
 virtual void setGimmickData(void*);
#undef SLOT
};
#define STAGE_FIELD(type, offset) (*(type*)((u8*)stage + (offset)))
extern "C" {
extern char lbl_40_data_5CD0[];
extern Stage* g_Stage;
void* create__25grGimmickBlockSympatheticFiPCc(s16, const char*);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void __dl__FPv(void*);
}
inline void run(Stage* stage, gfArchive* archive) {
 nw4r::g3d::ResMdl model(*(void**)((u8*)*(void**)STAGE_FIELD(void*,0x44)+0xe8));
 GimmickDispatch* ground;
 u32 index;
 u32 end = model.GetResNodeNumEntries()-1;
 index = 0;
 while (index < end) {
  ground=(GimmickDispatch*)create__25grGimmickBlockSympatheticFiPCc((s16)((u8*)STAGE_FIELD(void*,0x164))[0x29],lbl_40_data_5CD0);
  *(void**)((u8*)ground+0x1c)=stage;
  GimmickDispatch* current=(GimmickDispatch*)STAGE_FIELD(void*,0x20);
  if (!current) STAGE_FIELD(void*,0x20)=ground;
  else { while (*(GimmickDispatch**)((u8*)current+0x24)) current=*(GimmickDispatch**)((u8*)current+0x24); *(GimmickDispatch**)((u8*)current+0x24)=ground; }
  ground->setGimmickData(STAGE_FIELD(void*,0x164));
  *(u8*)((u8*)ground+0x150)=(u8)(index+1);
  ground->startup(archive,0,0);
  if (((u8*)STAGE_FIELD(void*,0x164))[0x2d]!=0xff) {
   void* collData=__nw__FUlQ25Heaps8HeapType(0x28,15);
   g_Stage->testCollDataInit(archive,(grCollData*)collData,(u32)((u8*)STAGE_FIELD(void*,0x164))[0x2d]);
   grCollision::create(g_Stage,collData,(Ground*)ground,1);
   __dl__FPv(collData);
  }
  if (STAGE_FIELD(GroundListNode*,0x154)==0) {
   GroundListNode* node=(GroundListNode*)__nw__FUlQ25Heaps8HeapType(12,15);
   if (node) { node->prev=0; node->next=0; }
   STAGE_FIELD(GroundListNode*,0x15c)=node;
   STAGE_FIELD(GroundListNode*,0x158)=node;
   STAGE_FIELD(GroundListNode*,0x154)=node;
   node->ground=ground;
  } else {
   GroundListNode* node=(GroundListNode*)__nw__FUlQ25Heaps8HeapType(12,15);
   if (node) { node->prev=0; node->next=0; }
   STAGE_FIELD(GroundListNode*,0x158)->next=node;
   STAGE_FIELD(GroundListNode*,0x158)->next->ground=ground;
   STAGE_FIELD(GroundListNode*,0x158)->next->prev=STAGE_FIELD(GroundListNode*,0x158);
   STAGE_FIELD(GroundListNode*,0x158)=STAGE_FIELD(GroundListNode*,0x158)->next;
  }
  ++STAGE_FIELD(u32,0x160);
  ++index;
 }
}
extern "C" void fn_40_221A0(Stage* stage, gfArchive* archive) { run(stage, archive); }
}

// TODO: clean up, give variables real names
namespace jj_22584 {
extern "C" void update__9grGimmickFf(void);

extern "C" void fn_40_22584(void) {
    update__9grGimmickFf();
}
}

// TODO: clean up, give variables real names
namespace jj_22614 {

extern "C" s32 isClipIn__8gfCameraFPCQ28gfCamera10ClipSphere(void*, void*);

class Object_vptr_3c;
struct Node {
    u32 unknown;
    Node* next;
    Object_vptr_3c* obj;
};

struct grYakumono_base { char pad[0x3c]; };
class grYakumono : public grYakumono_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim();
    virtual void processUpdate();
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
    virtual void renderDebug();
    virtual void init();
    virtual void fn_40_229AC();
    virtual void fn_40_22584();
    virtual void loadGroundData();
    virtual void bindData();
    virtual void setupMelee();
    virtual void setVisibility();
    virtual void setVisibilityByClipping();
    virtual void setVisibilityAttachedEffect();
    virtual void receiveCollMsg();
    virtual void receiveCollMsg_Landing();
    virtual void receiveCollMsg_Heading();
    virtual void receiveCollMsg_Wall();
    virtual void receiveCollMsg_Attack();
    virtual void unloadData();
    virtual void getModelCount();
    virtual void startup__34grAdventureBlockSympatheticManagerFP9gfArchiveUlQ211gfSceneRoot9LayerType();
    virtual void setNode();
    virtual void setStageData();
    virtual void getStageData();
    virtual void initStageData();
    virtual void setMdlIndex();
    virtual void getMdlIndex();
    virtual void isEnableCalcCollision();
    virtual void enableCalcCollision();
    virtual void disableCalcCollision();
    virtual void getNodeIndex();
    virtual void getNodePosition();
    virtual void getNodePosition_2();
    virtual void getNodeMatrix();
    virtual void getNodeMatrix_2();
    virtual void setNodeVisibility();
    virtual void setNodeVisibility_2();
    virtual void setNodeVisibilityAll();
    virtual void isNodeVisible();
    virtual void setNodeCollision();
    virtual void setNodeCollision_2();
    virtual void getNodeScale();
    virtual void getNodeScale_2();
    virtual void setValid();
    virtual void setValidAttachedEffect();
    virtual void setInitializeFlag__34grAdventureBlockSympatheticManagerFv();
    virtual void setInitializeInfo();
    virtual void getInitializeInfo();
    virtual void setMotionRatio();
    virtual void setMotionFrame();
    virtual void getMotionFrame();
    virtual void setMotionLoop();
    virtual void setMatAlphaMul();
    virtual void setMatAlpha();
    virtual void updateG3dProcCalcWorld();
    virtual void preExit();
    virtual void adventureEventGetItem();
    virtual void fn_40_22614();
    virtual void setTransparencyFlag(char);
    virtual void updateCallback();
    virtual void fixedPosition();
    virtual void setTgtNode();
    virtual void getTgtNode();
    virtual void setGimmickData();
    virtual void getGimmickData();
    virtual void changeNodeAnim();
    virtual void createFadeVisibleProduction();
    virtual void createSoundEffectVisibleProductionForExcel();
    virtual void createEffectVisibleProductionForExcel();
    virtual void setSimpleEffectVisibleProduction();
    virtual void dbDispInvalidatedByCameraClippingSphere();
    virtual void setTransparency(u32, u32);
    virtual int getTransparencyFlag();
    char pad40[0xa4];
    u32 model;
    char padE8[4];
    Rect2D rect;
    char padFC[0x58];
    Node* head;
    char pad158[4];
    Node* cur;
    u32 count;
};

struct Object_vptr_3c_base { char pad[0x3c]; };
class Object_vptr_3c : public Object_vptr_3c_base {
public:
    virtual void processDefault();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void processPreMapCorrection();
    virtual void slot5();
    virtual void slot6();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void slot12();
    virtual void processEffect();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void slot20();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void slot60();
    virtual void slot61();
    virtual void slot62();
    virtual void slot63();
    virtual void slot64();
    virtual void slot65();
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void slot69();
    virtual void slot70();
    virtual void slot71();
    virtual void slot72();
    virtual void slot73();
    virtual void slot74();
    virtual void slot75();
    virtual void slot76();
    virtual void slot77();
    virtual void slot78();
    virtual void slot79();
    virtual void slot80();
    virtual void slot81();
    virtual void slot82();
    virtual void slot83();
    virtual void slot84();
    virtual void slot85();
    virtual void slot86();
    virtual void slot87();
    virtual void slot88(u32, u32);
    virtual void slot89();
};

extern "C" void fn_40_22614(grYakumono* self) {
    gfCameraController* camera = CameraController::getInstance()->m_currentController;
    if (camera != 0 && *(void**)camera != 0) {
        float left, right, up, down;
        down = self->rect.m_down;
        up = self->rect.m_up;
        right = self->rect.m_right;
        left = self->rect.m_left;
        Rect2D rect;
        rect.m_left = left;
        rect.m_right = right;
        rect.m_up = up;
        rect.m_down = down;
        if (isClipIn__8gfCameraFPCQ28gfCamera10ClipSphere(*(void**)camera, &rect) == 0) {
            if (self->getTransparencyFlag() != 0) {
                u32 count = self->count;
                self->cur = self->head;
                for (u32 i = 0; i < count; i++) {
                    self->cur->obj->slot88(0, self->model);
                    self->cur = self->cur->next;
                }
                self->setTransparencyFlag(0);
            }
        } else if (self->getTransparencyFlag() == 0) {
            u32 count = self->count;
            self->cur = self->head;
            for (u32 i = 0; i < count; i++) {
                self->cur->obj->slot88(1, self->model);
                self->cur = self->cur->next;
            }
            self->setTransparencyFlag(1);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_229AC {
struct grYakumono_base { char pad[0x3c]; };
class grYakumono : public grYakumono_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim();
    virtual void processUpdate();
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
    virtual void renderDebug();
    virtual void init();
    virtual void fn_40_229AC(int);
};
struct Node {
    void* prev;
    Node* next;
    grYakumono* value;
};
struct Object {
    char pad[0x3c];
    char* vtable;
    char pad40[0x10c];
    char* vtable2;
    char pad150[4];
    Node* head;
    Node* tail;
    Node* current;
    unsigned int count;
};
extern "C" {
extern char lbl_40_data_5CEC[];
void __dl__FPv(void*);
void __dt__18grHitCircleSubjectFv(void*, int);
void __dt__9grGimmickFv(void*, int);
void* fn_40_229AC(Object* self, int flag) {
    if (self) {
        unsigned int i;
        unsigned int n = self->count;
        self->vtable = lbl_40_data_5CEC;
        self->vtable2 = lbl_40_data_5CEC + 0x170;
        self->current = self->head;
        for (i = 0; i < n; i++) {
            if (self->current->value)
                self->current->value->fn_40_229AC(1);
            self->current->value = 0;
            self->current = self->current->next;
        }
        n = self->count;
        self->current = self->head;
        for (i = 0; i < n; i++) {
            Node* p = self->current;
            self->current = p->next;
            if (p)
                __dl__FPv(p);
        }
        self->head = 0;
        self->tail = 0;
        self->current = 0;
        self->count = 0;
        __dt__18grHitCircleSubjectFv(&self->vtable2, 0);
        __dt__9grGimmickFv(self, 0);
        if (flag > 0)
            __dl__FPv(self);
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_22E14 {
extern "C" void notifyHitCircle__34grAdventureBlockSympatheticManagerFPv(void*);

extern "C" void fn_40_22E14(void* self) {
    notifyHitCircle__34grAdventureBlockSympatheticManagerFPv(reinterpret_cast<u8*>(self) - 0x14c);
}
}

// TODO: clean up, give variables real names
namespace jj_22E1C {
extern "C" void fn_40_229AC(void*);

extern "C" void fn_40_22E1C(void* self) {
    fn_40_229AC(reinterpret_cast<u8*>(self) - 0x14c);
}
}

