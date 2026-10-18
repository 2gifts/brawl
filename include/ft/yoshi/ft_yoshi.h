#pragma once
#include <so/anim/so_anim_obj.h>
#include <ft/fighter.h>
// Status/kinetic view of Yoshi. The constructor allocates 0x222B0 bytes;
// getExtendParam establishes the instance-data pointer at 0x22284.
struct ftData;
class wnYoshiStar;
class soModuleAccesser;

class ftYoshi : public Fighter {
    u8 unkTail[0x22284 - sizeof(Fighter)];
    ftData* m_data; // Instance parameter data, proved by getExtendParam.
    soAnimObj m_guardColorAnim; // +0x22288: egg-shield material animation.
public:
    void** getExtendParam();
    virtual void onDeactivate();
    virtual void onStart(int kind);
    virtual void processUpdate();
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog*, soModuleAccesser*);
    virtual void notifyEventChangeSituation(SituationKind, SituationKind, soModuleAccesser*);
    static bool activeArticle(wnYoshiStar* article, soModuleAccesser* acc);
    void createGuardColorAnim();
    void deleteGuardColorAnim();
    void updateGuardColorAnim();
};
static_assert(sizeof(ftYoshi) == 0x222b0, "Yoshi allocation size is wrong!");
