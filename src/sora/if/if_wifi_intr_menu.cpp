#include <snd/snd_system.h>
#include <gf/gf_pad_system.h>
#include <if/if_wifi_intr_menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// muMenuController::init (the map name); mu_menu_lifecycle.cpp calls it by this stand-in name.
extern "C" void fn_800B96E8(muMenuController* controller, int pad);

// HYPOTHESIS: the one pause model this menu builds. start and end select the object slots.
struct IfWifiIntrModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

IfWifiIntrModel lbl_805A26E8 = {"InfPause0020_TopN", 0, 0, 0xC0};

// HYPOTHESIS: visibility frame for each controller type (GameCube, classic, remote, nunchuk).
const u8 lbl_805A2700[8] = {0, 3, 2, 1, 0, 0, 0, 0};

IfWifiIntrMenu::IfWifiIntrMenu() {
    m_group = NULL;
    m_object = NULL;
    m_msg = NULL;
    m_mode = 0;
    m_cursor = 0;
    m_padNo = 0;
    m_active = 0;
    m_timer = 0;
}

IfWifiIntrMenu::~IfWifiIntrMenu() {
    m_msg->detachMuObject(m_object);
    for (u32 i = 0; i < 1; i++) {
        if (m_object) {
            delete m_object;
            m_object = NULL;
        }
    }
    if (m_msg) {
        delete m_msg;
        m_msg = NULL;
    }
}

void IfWifiIntrMenu::startMenu(int padNo, int kind) {
    MuObject* object = m_object;
    float frame = object->m_modelAnim->m_anmObjVisRes->GetFrame();
    object->changeAnimN(kind != 0, 2, false, false);
    object->setFrameVisible(frame);
    m_mode = (kind != 0) + 1;
    m_msg->printIndex(0, 0x1D, NULL);
    if (kind != 0) {
        m_msg->printIndex(1, 0x1E, NULL);
    } else {
        m_msg->printf(1, "");
    }
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(padNo, &status);
    m_object->setFrameVisible(lbl_805A2700[status.m_controllerType]);
    m_cursor = 0;
    m_object->changeAnimN(0, 0x10, true, false);
    m_group->Insert(m_group->sceneItemsCount, m_object->m_sceneModel);
    m_padNo = padNo;
    m_active = 1;
    m_timer = 0xF;
    fn_800B96E8(&m_controller, padNo & 0xFF);
}

void IfWifiIntrMenu::endMenu() {
    m_group->Remove(m_object->m_sceneModel);
    m_active = 0;
}

// TODO: clean up, give variables real names
namespace jj_800FBFDC {
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
 virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
 virtual void slot12(); virtual void slot13(void*);
};
struct State {
 Object_vptr_0* scene; MuObject* object; void* msg; int count; int unk10;
 int selection; int port; bool enabled; char pad[3]; int timer; char sub[12]; int directions;
};
extern "C" void fn_800B9984(void*);
extern "C" int fn_800BA3D8(void*);
extern "C" void fn_800B5854(void*, int, int, int, int);
extern "C" const unsigned char lbl_805A2700[8];
extern "C" unsigned int lbl_804090E0[];

static inline bool checkMask(gfPadStatus& s) {
 u32 mask = lbl_804090E0[s.m_controllerType];
 if (mask == (mask & s.m_buttonsCurrentFrame2.bits) && (s.m_buttonsPressedThisFrame.bits & mask)) return true;
 return false;
}
static inline bool checkOne(gfPadStatus& s, u32 b) {
 u32 pressed = s.m_buttonsPressedThisFrame.bits;
 int mask = *(int*)((char*)lbl_804090E0 + (s.m_controllerType << 2));
 if (pressed & b) {
  if (!(mask & b)) return true;
  if (!(s.m_buttonsCurrentFrame2.bits & mask & ~b)) return true;
 }
 return false;
}
static inline bool checkTwo(gfPadStatus& s, u32 b, u32 c) {
 u32 pressed = s.m_buttonsPressedThisFrame.bits;
 u32 mask = *(u32*)((char*)lbl_804090E0 + ((u32)s.m_controllerType << 2));
 if (pressed & b) {
  if (!(mask & b)) return true;
  if (!(s.m_buttonsCurrentFrame2.bits & mask & ~b)) return true;
 }
 if (pressed & c) {
  if (!(mask & c)) return true;
  if (!(s.m_buttonsCurrentFrame2.bits & mask & ~c)) return true;
 }
 return false;
}
extern "C" int fn_800FBFDC(State* self) {
 int result = 1;
 gfPadStatus status2;
 gfPadStatus status;
 fn_800B9984(self->sub);
 if (!self->enabled) return 0;
 g_gfPadSystem->getSysPadStatus(fn_800BA3D8(self->sub), &status);
 self->object->setFrameVisible((float)lbl_805A2700[status.m_controllerType]);
 g_gfPadSystem->getSysPadStatus(self->port, &status2);
 int flags = self->directions;
 if (self->count > 1) {
  int old = self->selection;
  int next = old;
  if (old <= 0) {
   if (flags & 4) next = old + 1;
  } else if (flags & 8) next = old - 1;
  if (next != old) {
   self->selection = next;
   fn_800B5854(self->object, next, 16, 1, 0);
   g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
  }
 }
 if (--self->timer < 0) {
  if (checkMask(status2)) {
   result = 4;
  } else {
   if (checkOne(status2, 0x100)) {
    result = 3;
    if (self->selection == 0) result = 2;
   }
   if (checkTwo(status2, 0x200, 0x1000)) result = 2;
  }
  if (result != 1) {
   self->scene->slot13(self->object->m_sceneModel);
   self->enabled = false;
  }
 }
 return result;
}
}

