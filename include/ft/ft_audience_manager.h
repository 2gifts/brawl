// Local shadow of BrawlHeaders' ft/ft_audience_manager.h: adds the manager's methods named in the sora map.
#pragma once

#include <StaticAssert.h>
#include <ft/ft_audience_impl.h>
#include <sr/sr_common.h>
#include <types.h>

class ftAudienceNull : public ftAudience {
public:
    virtual ~ftAudienceNull();
    virtual void activate();
    virtual void deactivate();
    virtual void reqHandclapDead(float, bool);
    virtual float correctCheerReaction(float, float);
    virtual void reqCallFgmStopCheer();
    virtual void checkCheer(float, int entryId1, int entryId2);
    virtual bool checkTumultRange(float);
    virtual bool checkTumultLanding(float, float, int entryId);
    virtual bool checkTumultFallSpecial(float, int entryId);
    virtual void checkCheerDefeat(int entryId);
};
static_assert(sizeof(ftAudienceNull) == 4, "Class is wrong size!");

class ftAudienceManager {
public:
    ftAudienceImpl* m_audienceImpl;
    ftAudienceNull m_audienceNull; // the fallback while no audience task runs
    ftAudience* m_audience;

    // Starts on the null audience, with the audience task created alongside (sr/sr_common.h for the heap new).
    ftAudienceManager() : m_audience(&m_audienceNull) {
        m_audienceImpl = new (Heaps::System) ftAudienceImpl;
    }

    static void initialize();
    void activate();
    void deactivate();
};
static_assert(sizeof(ftAudienceManager) == 12, "Class is wrong size!");

extern ftAudienceManager* g_ftAudienceManager;