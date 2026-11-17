#pragma once

// The Charge Shot status processes of Samus (and of Kirby's copy): sora_melee unit ft_samus_status_uniq_process_special_charge_shot.
#include <types.h>

class soModuleAccesser;

#include <so/status/so_status_uniq_process.h>

// Class shapes from the module map; the empty method bodies are not reconstructed yet.
class ftSamusStatusUniqProcessSpecialNStart : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftSamusStatusUniqProcessSpecialAirNStart : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftSamusStatusUniqProcessSpecialNHold : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNHold() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialNFire : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialAirNFire : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirNFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
