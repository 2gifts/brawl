#pragma once

// Local BrawlHeaders shadow: parameter lists of the voice, step and landing overrides recovered from their callers
// (getId) and their argument use; the destructors are implicit (emitted after the methods).

#include <StaticAssert.h>
#include <snd/snd_id.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>

class ftSoundIdExchangerImpl : public soSoundIdExchanger {
public:
    virtual SndID getId(SndID sndID, soModuleAccesser* moduleAccesser);
    virtual int getStepId(int stepId, soModuleAccesser* moduleAccesser);
    virtual int getLandingId(int landingId, soModuleAccesser* moduleAccesser);
    virtual bool isVoiceId(int sndID, soModuleAccesser* moduleAccesser);
    virtual bool isVoiceMute(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftSoundIdExchangerImpl) == 8, "Class is wrong size!");

class ftKirbySoundIdExchangerImpl : public ftSoundIdExchangerImpl {
public:
    virtual bool isVoiceId(int sndID, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftKirbySoundIdExchangerImpl) == 8, "Class is wrong size!");

extern ftSoundIdExchangerImpl g_ftSoundIdExchangerImpl;
extern ftKirbySoundIdExchangerImpl g_ftKirbySoundIdExchangerImpl;
