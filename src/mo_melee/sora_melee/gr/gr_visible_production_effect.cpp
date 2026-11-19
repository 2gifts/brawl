#include <gr/gr_visible_production_effect.h>
#include <ec/ec_mgr.h>
#include <mt/mt_matrix.h>
#include <types.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale);

grVisibleProductionEffect* grVisibleProductionEffect::create(grGimmick* ground, grGimmick::SimpleEffectData* data) {
    return new (Heaps::StageInstance) grVisibleProductionEffect(ground, data);
}

void grVisibleProductionEffect::initialize() { }

void grVisibleProductionEffect::remove() { }

void grVisibleProductionEffect::productFlowStandby() { }

void grVisibleProductionEffect::productFlowStart() {
    m_started = false;
    m_frame = 0.0f;
    setState(VisibleProduction_Executing);
}

void grVisibleProductionEffect::productFlowProducing(float deltaFrame) {
    if (!m_started) {
        if (m_frame >= m_startFrame) {
            Matrix matrix(true);
            if (!m_ground->getNodeMatrix(&matrix, 0, "effect_locator")) {
                m_ground->getNodeMatrix(&matrix, 0, u32(0));
            }
            m_pos.set(matrix(0, 3), matrix(1, 3), matrix(2, 3));
            matrix.getRotate(&m_rot);
            getScalingFactor(&matrix, &m_scale);
            g_ecMgr->setEffect((EfID)m_effectId, &m_pos, &m_rot, &m_scale);
            m_visiblePermission = true;
            m_started = true;
        } else {
            m_frame += deltaFrame;
        }
    } else {
        setState(VisibleProduction_Finished);
    }
}

void grVisibleProductionEffect::productFlowEnd() { }

void grVisibleProductionEffect::setForcedEnding() { }

// TODO: clean up, give variables real names
namespace jj_270EF4 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" unsigned char lbl_27_data_57CB0[];
extern "C" unsigned char lbl_27_data_57D30[];
extern "C" const float lbl_27_rodata_4450;
struct Vec3 {
 float x,y,z;
 Vec3(float a,float b,float c):x(a),y(b),z(c){}
 Vec3& operator=(const Vec3& o) {
  ((unsigned int*)this)[0]=((const unsigned int*)&o)[0];
  ((unsigned int*)this)[1]=((const unsigned int*)&o)[1];
  ((unsigned int*)this)[2]=((const unsigned int*)&o)[2];
  return *this;
 }
};
struct Input { unsigned int word; unsigned short half; };
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 unsigned int field4;
 float field8;
 unsigned char fieldC, fieldD;
 unsigned short padE;
 int field10;
 Input input;
 Vec3 position;
 unsigned char pad28[0x18];
 float field40;
 unsigned char field44;
};
extern "C" Object_vptr_0* fn_27_270EF4(int arg0, const Input* arg1) {
 Object_vptr_0* obj=(Object_vptr_0*)__nw__FUlQ25Heaps8HeapType(0x48,15);
 if(obj) {
  *(void**)obj=lbl_27_data_57CB0;
  obj->field4=0;
  obj->field8=lbl_27_rodata_4450;
  obj->fieldC=1;
  obj->fieldD=0;
  *(void**)obj=lbl_27_data_57D30;
  obj->field10=arg0;
  obj->input=*arg1;
  obj->position=Vec3(lbl_27_rodata_4450,lbl_27_rodata_4450,lbl_27_rodata_4450);
  obj->field40=lbl_27_rodata_4450;
  obj->field44=0;
  obj->slot2();
 }
 return obj;
}
}

