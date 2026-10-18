#pragma once
#include <ft/fighter.h>
#include <so/anim/so_anim_obj.h>
#include <ft/wario/ft_wario_param.h>
struct ftWarioData;

// Fighter-side views for status and shared behavior units. The main fighter
// exposes its parameter data and Waft animation after opaque builder storage.
class ftWario : public Fighter {
    u8 unkTail[0xD2E8 - sizeof(Fighter)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable; // +0xD2E8
    ftWarioData* m_data; // +0xD2F8, proved by getExtendParam and Waft/bike parameter reads.
    soAnimObj m_waftAnim; // +0xD2FC: charge gauge material animation.
public:
    ftWarioExtendParam* getExtendParam();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onStart(int startKind);
    virtual void toDead(Dead::Reason reason);
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    virtual void notifyEventChangeStatus(int, int, soStatusData*, soModuleAccesser*);
    virtual bool notifyEventAnimCmd(acAnimCmd*, soModuleAccesser*, s32);
    virtual void notifyEventCollisionSearch(soCollisionLog*, soModuleAccesser*);
    virtual void change(void*, int);
    virtual void setupChangeSucceedWork(void*);
    virtual void changeSucceedCore(void*, int);
    virtual void processUpdate();
    static bool activeArticle(class wnWarioGarlic*, soModuleAccesser*);
};
class ftWarioMan : public Fighter {
    u8 unk194[0xB24C - sizeof(Fighter)];
public:
    ftWarioManExtendParamAccesser* m_extendParam; // 0xB24C
    soAnimObj m_animObj;                          // 0xB250 the Waft gauge animation

    ftWarioExtendParam* getExtendParam();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onStart(int startKind);
    virtual void toDead(Dead::Reason deadReason);
    virtual void onDeadEnd();
    virtual void notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void change(void* unk1, int unk2);
    virtual void setupChangeSucceedWork(void* changeSucceedWork);
    virtual void changeSucceedCore(void* changeSucceedWork, int unk2);
    virtual void processUpdate();
    virtual void processFixPosition();
};
