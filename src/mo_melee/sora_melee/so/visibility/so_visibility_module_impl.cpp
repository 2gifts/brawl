#include <so/visibility/so_visibility_module_impl.h>
#include <types.h>
#include <ac/ac_anim_cmd_impl.h>

// HYPOTHESIS: status metadata words followed by transition preservation flags; soStatusData itself stays empty because
// the enemy's emStatusData derives from it.
struct soVisibilityStatusData : public soStatusData {
    unsigned int unk0;
    unsigned int unk4;
    unsigned int unk8;
    unsigned int unkC;
};

soVisibilityModuleImpl::~soVisibilityModuleImpl() {
    delete[] m_setData;
}

void soVisibilityModuleImpl::setStatusDefault(u32 index, int value, bool apply) {
    m_setData[index * 3] = value;
    unk2C = true;
    if (apply == true) {
        set(index, m_setData[index * 3]);
    }
}

void soVisibilityModuleImpl::set(u32 index, int value) {
    s8* setData = &m_setData[index * 3];
    if (value != setData[1]) {
        setData[1] = value;
        unk2C = true;
    }
}

int soVisibilityModuleImpl::get(u32 index) {
    return m_setData[index * 3 + 1];
}

void soVisibilityModuleImpl::setData(soVisibilityData* data) {
    m_data = data;
}

void soVisibilityModuleImpl::activate() {
    int count = m_data->unkC;
    for (int i = 0; i < count; ++i) {
        setStatusDefault(m_data->unk8[i].unk0, m_data->unk8[i].unk4, false);
    }
    setDefaultAll();
    soVisibilityModuleSimple::activate();
}

void soVisibilityModuleImpl::setDefaultAll() {
    if (unk2C == true) {
        for (int i = 0; i < m_data->unk4; ++i) {
            set(i, m_setData[i * 3]);
        }
        unk2C = false;
    }
}

void soVisibilityModuleImpl::hideAll() {
    for (int i = 0; i < m_data->unk4; ++i) {
        set(i, -1);
    }
}

void soVisibilityModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (!((static_cast<soVisibilityStatusData*>(statusData)->unkC >> 6) & 1)) {
        setDefaultAll();
    }
}

bool soVisibilityModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 4) {
        return false;
    }
    switch (acmd->getType()) {
    case 0: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        const acCmdArg& index = acGetCmdArg(args, 0);
        acShiftCmdArg(args);
        const acCmdArg& value = acGetCmdArg(args, 0);
        set(index.getIntData(), value.getIntData());
        return true;
    }
    case 1: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        const acCmdArg& index = acGetCmdArg(args, 0);
        acShiftCmdArg(args);
        const acCmdArg& value = acGetCmdArg(args, 0);
        setStatusDefault(index.getIntData(), value.getIntData(), true);
        return true;
    }
    case 3: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        const acCmdArg& index = acGetCmdArg(args, 0);
        u32 setIndex = index.getIntData();
        set(setIndex, m_setData[setIndex * 3]);
        return true;
    }
    }
    return soVisibilityModuleSimple::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

#include <sr/sr_common.h>
// TODO: clean up, give variables real names
namespace jj_5B248 {
extern "C" void __ct__24soVisibilityModuleSimpleFP16soModuleAccesserP16soVisibilityDatai(void*, void*, int, int);
extern "C" void* __vt__21soStatusEventObserver[];
extern "C" char lbl_27_data_6248[];
class Getter {
public:
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual s16 get();
};
class Class_C80 {
public:
 virtual void __dt__30soModelModuleCalcWorldCallbackFv();
 virtual void ExecCallbackA__30soModelModuleCalcWorldCallbackFPQ34nw4r3g3d12ChrAnmResultQ34nw4r3g3d6ResMdlPQ34nw4r3g3d16FuncObjCalcWorld();
 virtual Heaps::HeapType ExecCallbackB__30soModelModuleCalcWorldCallbackFPQ34nw4r3g3d13WorldMtxManipQ34nw4r3g3d6ResMdlPQ34nw4r3g3d16FuncObjCalcWorld();
};
class Obs {
public:
 virtual void addObserver(s32, s32);
 s16 id, kind, link;
};
struct Data { int unk; int count; };
struct Entry { signed char a, b, c; };
struct Self {
 void* vt; void* vt4; char pad[12]; Obs obs;
 Data* data; Entry* entries; int value; bool active;
};
extern "C" Self* __ct__22soVisibilityModuleImplFP16soModuleAccesserP16soVisibilityDatai(Self* self, void* owner, Data* data, int value) {
 __ct__24soVisibilityModuleSimpleFP16soModuleAccesserP16soVisibilityDatai(self, owner, 0, -1);
 char* module = *(char**)((char*)owner + 0xd8);
 char* object = *(char**)(module + 0x80);
 s16 id = ((Getter*)(object + 8))->get();
 self->obs.id = -1;
 self->obs.kind = 4;
 self->obs.link = -1;
 *(void***)&self->obs = __vt__21soStatusEventObserver;
 self->obs.addObserver((s32)id, -1);
 self->vt = lbl_27_data_6248;
 self->vt4 = lbl_27_data_6248 + 0x34;
 *(void**)&self->obs = lbl_27_data_6248 + 0x50;
 self->data = data;
 self->entries = 0;
 self->value = value;
 self->active = true;
 module = *(char**)((char*)owner + 0xd8);
 Heaps::HeapType heap = (*(Class_C80**)(module + 0xc0))->ExecCallbackB__30soModelModuleCalcWorldCallbackFPQ34nw4r3g3d13WorldMtxManipQ34nw4r3g3d6ResMdlPQ34nw4r3g3d16FuncObjCalcWorld();
 self->entries = (Entry*)operator new[](self->data->count * 3, heap);
 for (int i = 0; i < self->data->count; i++) {
  self->entries[i].b = -2;
  self->entries[i].a = -1;
  self->entries[i].c = 0;
 }
 return self;
}
}

// TODO: clean up, give variables real names
namespace jj_5B6A8 {
extern "C" void update__24soVisibilityModuleSimpleFv(void* self);

class Class_6170 {
public:
    virtual void activate__24soVisibilityModuleSimpleFv();
    virtual void setData__24soVisibilityModuleSimpleFP16soVisibilityData();
    virtual void set__24soVisibilityModuleSimpleFUli();
    virtual void setStatusDefault__24soVisibilityModuleSimpleFUlib();
    virtual void setDefaultAll__24soVisibilityModuleSimpleFv();
    virtual s32 update__24soVisibilityModuleSimpleFv();
    virtual void hideAll__24soVisibilityModuleSimpleFv();
    virtual void setWhole__24soVisibilityModuleSimpleFUc();
    virtual void getWhole__24soVisibilityModuleSimpleFv();
    virtual void fn_27_5AF1C();
};

class Class_Result {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual s32 slot9();
};

class DispatchPrefix {
public:
    u32 word0;
    u32 word1;
};

class Class_1250Call : public DispatchPrefix {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
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
    virtual void method(s32, s32);
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void recordMethod(s32, s32, s32, s32);
};

extern "C" void fn_27_5B6A8(void* arg0) {
    s32 result;
    char* a = (char*)arg0;
    s32 outerOffset;
    s32 innerOffset;
    s32 outer;
    void* elem;
    s32 inner;

    update__24soVisibilityModuleSimpleFv(arg0);

    result = 0;
    void* p = *(void**)(a + 0x10);
    p = *(void**)((char*)p + 0xD8);
    Class_6170* p68 = (Class_6170*)*(void**)((char*)p + 0x68);
    if (p68->update__24soVisibilityModuleSimpleFv() != 0) {
        result = ((Class_Result*)arg0)->slot9();
    }

    s32 i = 1;
    while (i < *(s32*)(a + 0x28)) {
        Class_1250Call* p4 = (Class_1250Call*)*(void**)((char*)*(void**)((char*)*(void**)(a + 0x10) + 0xD8) + 4);
        p4->method(result, i);
        i++;
    }

    outer = 0;
    outerOffset = 0;
    while (outer < *(s32*)(a + 0x28)) {
        elem = *(void**)((char*)(*(void**)(*(void**)(a + 0x20))) + outerOffset);
        if (elem != 0) {
            inner = 0;
            innerOffset = 0;
            while (inner < *(s32*)((char*)*(void**)(a + 0x20) + 4)) {
                char* data = (char*)*(void**)(a + 0x24) + innerOffset;
                s32 selected = *(s8*)(data + 1) + *(u8*)(data + 2);
                if (*(s32*)((char*)elem + 4) > 0) {
                    s32 j = 0;
                    result = 0;
                    while (j < *(s32*)((char*)elem + 4)) {
                        if (j != selected) {
                            Class_1250Call* p4 = (Class_1250Call*)*(void**)((char*)*(void**)((char*)*(void**)(a + 0x10) + 0xD8) + 4);
                            char* record = (char*)*(void**)elem + result;
                            p4->recordMethod(
                                *(s32*)(record + 4), *(s32*)record, 0, outer);
                        }
                        result += 8;
                        j++;
                    }
                    if (selected != -1) {
                        Class_1250Call* p4 = (Class_1250Call*)*(void**)((char*)*(void**)((char*)*(void**)(a + 0x10) + 0xD8) + 4);
                        char* record = (char*)*(void**)elem + selected * 8;
                        p4->recordMethod(
                            *(s32*)(record + 4), *(s32*)record, 1, outer);
                    }
                }
                innerOffset += 3;
                elem = (void*)((char*)elem + 8);
                inner++;
            }
        }
        outerOffset += 4;
        outer++;
    }
}
}

