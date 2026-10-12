#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/catch/so_catch_module.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftYoshiStatusUniqProcessSpecialNSwallow : public soStatusUniqProcess {
public:
    virtual ~ftYoshiStatusUniqProcessSpecialNSwallow() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

// Catch-module view: the header covers the vtable prefix up to catchCut (0x2c); the slots after it are
// not named yet, so they are kept as unkN. unk3C is the checkDamage hook of the catch module.
class ftYoshiCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool unk3C(soModuleAccesser* moduleAccesser, void* data);
};

ftYoshiStatusUniqProcessSpecialNSwallow g_ftYoshiStatusUniqProcessSpecialNSwallow;

void ftYoshiStatusUniqProcessSpecialNSwallow::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x118) {
        return;
    }
    if (nextStatus == 0x15b) {
        return;
    }
    static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
}
void ftYoshiStatusUniqProcessSpecialNSwallow::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
bool ftYoshiStatusUniqProcessSpecialNSwallow::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    return static_cast<ftYoshiCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->unk3C(moduleAccesser, data);
}
