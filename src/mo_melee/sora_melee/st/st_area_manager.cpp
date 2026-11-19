#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <st/st_area_manager.h>
#include <st/st_trigger.h>
#include <types.h>

template <class T> bool stAreaManager::search(T* areas, u32 areaId, u32* index) {
    u32 i;
    for (i = 0; i < m_size; i++) {
        if (areas[i].m_areaId != 0 && areaId == areas[i].m_areaId) {
            *index = i;
            return true;
        }
    }
    for (i = 0; i < m_size; i++) {
        if (areas[i].m_areaId == 0) {
            *index = i;
            return false;
        }
    }
    *index = m_size;
    return false;
}

stAreaManager::~stAreaManager() { }

void stAreaManager::eraseAll() {
    for (u32 i = 0; i < m_size; i++) {
        m_windAreas[i].m_areaId = 0;
    }
    for (u32 i = 0; i < m_size; i++) {
        m_beltAreas[i].m_areaId = 0;
    }
    for (u32 i = 0; i < m_size; i++) {
        m_waterAreas[i].m_areaId = 0;
    }
    for (u32 i = 0; i < m_size; i++) {
        m_hitPointEffectAreas[i].m_areaId = 0;
    }
}

void stAreaManager::clearAll() {
    m_size = 0;
    if (m_windAreas) {
        delete[] m_windAreas;
    }
    if (m_beltAreas) {
        delete[] m_beltAreas;
    }
    if (m_waterAreas) {
        delete[] m_waterAreas;
    }
    if (m_hitPointEffectAreas) {
        delete[] m_hitPointEffectAreas;
    }
    m_windAreas = nullptr;
    m_beltAreas = nullptr;
    m_waterAreas = nullptr;
    m_hitPointEffectAreas = nullptr;
}

u32 stAreaManager::setWind(stMovementAreaInfoData* data) {
    u32 index;
    search(m_windAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_windAreas[index] = *data;
    }
    return index;
}

u32 stAreaManager::setBelt(stMovementAreaInfoData* data) {
    u32 index;
    search(m_beltAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_beltAreas[index] = *data;
    }
    return index;
}

u32 stAreaManager::setWater(stWaterAreaInfoData* data) {
    u32 index;
    search(m_waterAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_waterAreas[index] = *data;
    }
    return index;
}

u32 stAreaManager::setHitPointEffect(stHitPointEffectAreaInfoData* data) {
    u32 index;
    search(m_hitPointEffectAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_hitPointEffectAreas[index] = *data;
    }
    return index;
}

void stAreaManager::setWindParam(stMovementAreaInfoData* data, u32 index) {
    if (index < m_size) {
        m_windAreas[index] = *data;
    }
}

void stAreaManager::setBeltParam(stMovementAreaInfoData* data, u32 index) {
    if (index < m_size) {
        m_beltAreas[index] = *data;
    }
}

void stAreaManager::setWaterParam(stWaterAreaInfoData* data, u32 index) {
    if (index < m_size) {
        m_waterAreas[index] = *data;
    }
}

void stAreaManager::erase(u32 areaId) {
    u32 index;
    if (m_size != 0) {
        if (search(m_windAreas, areaId, &index)) {
            m_windAreas[index].m_areaId = 0;
        }
        if (search(m_beltAreas, areaId, &index)) {
            m_beltAreas[index].m_areaId = 0;
        }
        if (search(m_waterAreas, areaId, &index)) {
            m_waterAreas[index].m_areaId = 0;
        }
        if (search(m_hitPointEffectAreas, areaId, &index)) {
            m_hitPointEffectAreas[index].m_areaId = 0;
        }
    }
}

bool stAreaManager::getMovementArea(u32 areaId, stMovementData* data) {
    u32 index;
    if (search(m_windAreas, areaId, &index)) {
        *data = m_windAreas[index].m_data;
        return true;
    }
    if (search(m_beltAreas, areaId, &index)) {
        *data = m_beltAreas[index].m_data;
        return true;
    }
    return false;
}

bool stAreaManager::getHitPointEffect(u32 areaId, stHitPointEffectData* data) {
    u32 index;
    if (search(m_hitPointEffectAreas, areaId, &index)) {
        *data = m_hitPointEffectAreas[index].m_data;
        return true;
    }
    return false;
}

void stAreaManager::setValidTriggerfromAreaID(u32 unused, u32 areaId) {
    if (g_stTriggerMng) {
        g_stTriggerMng->setValidfromAreaID(areaId);
    }
}

// TODO: clean up, give variables real names
namespace jj_22A914 {

extern "C" {
extern const char lbl_27_data_4A818[];
extern const char lbl_27_data_4A828[];

gfTask* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(
    gfTask*, const char*, gfTask::Category, int, int, bool);
}

struct SoraTaskOwner {
    char pad[0x3C];
    const char* name;
    u32 unk40;
    u32 unk44;
    u32 unk48;
    char pad2[0x4];
    u32 unk50;
};

extern "C" SoraTaskOwner* __ct__13stAreaManagerFv(SoraTaskOwner* self) {
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(
        reinterpret_cast<gfTask*>(self), lbl_27_data_4A818,
        static_cast<gfTask::Category>(0), 1, 0x0D, true);
    self->unk40 = 0;
    self->name = lbl_27_data_4A828;
    self->unk44 = 0;
    self->unk48 = 0;
    self->unk50 = 0;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_22A9D4 {

struct Data36 { u32 value; char pad[0x20]; };
struct Data20 { u32 value; char pad[0x10]; };
struct Data16 { u32 value; char pad[0x0C]; };
struct Obj {
    char pad[0x40];
    Data36* a;
    Data36* b;
    Data20* c;
    Data16* d;
    u32 count;
};

extern "C" void initMng__13stAreaManagerFUl(Obj* self, int count) {
    if (self->count != 0) {
        self->count = 0;
        if (self->a) delete[] self->a;
        if (self->b) delete[] self->b;
        if (self->c) delete[] self->c;
        if (self->d) delete[] self->d;
        self->a = 0;
        self->b = 0;
        self->c = 0;
        self->d = 0;
    }
    self->count = count;
    self->a = new (Heaps::StageInstance) Data36[count];
    self->b = new (Heaps::StageInstance) Data36[count];
    self->c = new (Heaps::StageInstance) Data20[count];
    self->d = new (Heaps::StageInstance) Data16[count];
    for (u32 i = 0; i < self->count; ++i) self->a[i].value = 0;
    for (u32 i = 0; i < self->count; ++i) self->b[i].value = 0;
    for (u32 i = 0; i < self->count; ++i) self->c[i].value = 0;
    for (u32 i = 0; i < self->count; ++i) self->d[i].value = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_22B4C8 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct TriggerParam {
    u8 unk0;
    u8 unk1;
    s16 unk2;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s16 unk10;
};

extern void* g_stTriggerMng;
extern "C" void fn_27_2323EC(void* p, void* q);

extern "C" void fn_27_22B4C8(void* a, void* b, TriggerParam* p) {
    p->unk0 = 0;
    p->unk2 = 0;
    p->unk4 = 0;
    p->unk8 = 0;
    p->unk1 = 0;
    p->unkC = -1;
    p->unk10 = 0;
    if (g_stTriggerMng != 0) {
        fn_27_2323EC(g_stTriggerMng, b);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_22B520 {
struct Source {
    unsigned char pad0[0x40];
    unsigned int value40;
    unsigned char pad44[0xC];
    unsigned int value50;
};

struct Destination {
    unsigned char pad0[0x7C];
    unsigned int value7C;
    unsigned int value80;

    void set(unsigned int a, unsigned int b) {
        value7C = a;
        value80 = b;
    }
};

extern "C" Destination* lbl_8049DEC8;

extern "C" void fn_27_22B520(Source* source) {
    if (lbl_8049DEC8) {
        lbl_8049DEC8->set(source->value40, source->value50);
    }
}
}

