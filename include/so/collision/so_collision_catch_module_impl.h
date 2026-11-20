#pragma once

// soCollisionCatchModuleImpl (sora_melee so_collision_catch_module_impl.o): the grab boxes of an object. Layout from the
// constructor and the virtual table. Not for units that include ft/builder/ft_dol_types.h, which declares an opaque copy.

#include <StaticAssert.h>
#include <types.h>
#include <mt/mt_vector.h>
#include <so/so_array.h>
#include <so/collision/so_collision.h>
#include <so/collision/so_collision_group.h>
#include <so/collision/so_collision_catch_event_presenter.h>
#include <so/collision/templates/so_collision_catch_part.h>
#include <so/event/so_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/status/so_status_event_presenter.h>

class soModuleAccesser;
class soStatusData;

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soCollisionCatchStatusDataView {
    char _0[0xc];
    u32 : 22;
    u32 m_keepCatch : 1;
};
class acAnimCmd;

class soCollisionCatchModule {
public:
    virtual ~soCollisionCatchModule() { }
    virtual void activate(float posX, float scale, float lr, int taskId, bool isCheck) = 0;
    virtual void deactivate() = 0;
    virtual void begin() = 0;
    virtual void clearAll() = 0;
    virtual void clear(u32 partIndex) = 0;
    virtual void set(u32 partIndex, soCollisionCatchData* data) = 0;
    virtual bool isCatch(u32 partIndex) = 0;
    virtual float getSize(u32 partIndex) = 0;
    virtual void setSize(u32 partIndex, float size) = 0;
    virtual float getPosX() = 0;
    virtual Vec3f getCenterPos(u32 partIndex) = 0;
    virtual soCollisionCatchData* getData(u32 partIndex) const = 0;
    virtual void update(float posX, float scale, float lr) = 0;
    virtual void updatePos(u32 partIndex) = 0; // HYPOTHESIS: name
    virtual void check() = 0;
    virtual void renderDebug() = 0;
};
static_assert(sizeof(soCollisionCatchModule) == 4, "Class is wrong size!");

class soCollisionCatchModuleImpl : public soCollisionCatchModule,
                                   public soStatusEventObserver,
                                   public soAnimCmdEventObserver,
                                   public soEventPresenter<soCollisionCatchEventObserver> {
public:
    soModuleAccesser* m_moduleAccesser;               // 0x28
    soArray<soCollisionCatchPart>* m_collisionCatchPartArray; // 0x2C
    float m_posX;                                     // 0x30
    float m_scale;                                    // 0x34
    float m_lr;                                       // 0x38
    soArrayVector<soCollisionGroup, 1> m_collisionGroupArrayVector; // 0x3C
    soCollision m_collision;                          // 0xC0
    char _D8[8];                                      // 0xD8
    bool m_isCheck;                                   // 0xE0: HYPOTHESIS: activate's last argument
    bool m_checkFront;                                // 0xE1: HYPOTHESIS: only catch in front (posX difference times lr >= 0)
    bool m_isUpdated;                                 // 0xE2: set once update ran this frame
    bool m_keepLog;                                   // 0xE3: deactivate keeps the logs instead of cleaning

    soCollisionCatchModuleImpl(soModuleAccesser* acc, int taskId, u8 category, soArray<soCollisionCatchPart>* parts,
                               soEventObserverRegistrationDesc* regDesc, bool checkFront, bool keepLog);
    virtual ~soCollisionCatchModuleImpl();
    virtual void activate(float posX, float scale, float lr, int taskId, bool isCheck);
    virtual void deactivate();
    virtual void begin();
    virtual void clearAll();
    virtual void clear(u32 partIndex);
    virtual void set(u32 partIndex, soCollisionCatchData* data);
    virtual bool isCatch(u32 partIndex);
    virtual float getSize(u32 partIndex);
    virtual void setSize(u32 partIndex, float size);
    virtual float getPosX() { return m_posX; }
    virtual Vec3f getCenterPos(u32 partIndex);
    virtual soCollisionCatchData* getData(u32 partIndex) const;
    virtual void update(float posX, float scale, float lr);
    virtual void updatePos(u32 partIndex);
    virtual void check();
    virtual void renderDebug();

    virtual bool isObserv(char unitId) { return unitId == 6; }
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
        if (!reinterpret_cast<soCollisionCatchStatusDataView*>(statusData)->m_keepCatch) {
            clearAll();
        }
    }
};
static_assert(sizeof(soCollisionCatchModuleImpl) == 0xE4, "Class is wrong size!");
