#pragma once

#include <StaticAssert.h>
#include <st/loader/st_loader_info.h>
#include <types.h>

// The Wi-Fi match HUD of the stage loader (the IfWifiprMngr in main.dol): stepped like stLoaderInfoStageEdit.
class stLoaderInfoWifi : public stLoaderInfo {
public:
    stLoaderInfoWifi();

    virtual void processBegin();
    virtual ~stLoaderInfoWifi();
    virtual void entryEntity();
    virtual void removeEntity();
};
