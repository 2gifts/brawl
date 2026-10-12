#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the effect module. The header's soEffectModule is missing slots, so setWhole is placed by its
// vtable offset (0xC4): two leading words, then 47 unnamed slots.
class ftZeldaEffectModuleView {
public:
    virtual void unused00();
    virtual void unused01();
    virtual void unused02();
    virtual void unused03();
    virtual void unused04();
    virtual void unused05();
    virtual void unused06();
    virtual void unused07();
    virtual void unused08();
    virtual void unused09();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual void unused41();
    virtual void unused42();
    virtual void unused43();
    virtual void unused44();
    virtual void unused45();
    virtual void unused46();
    virtual void setWhole(bool);
};

class ftZeldaStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftZeldaStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftZeldaEffectModuleView*>(&moduleAccesser->getEffectModule())->setWhole(false);
}

// HYPOTHESIS: not reconstructed yet (final smash timing and effect/article control); left as a stub.
void ftZeldaStatusUniqProcessFinal::execStatus(soModuleAccesser*) {
}

// HYPOTHESIS: not reconstructed yet; left as a stub.
void ftZeldaStatusUniqProcessFinal::execFixPosCounter(soModuleAccesser*) {
}

void ftZeldaStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int) {
    reinterpret_cast<ftZeldaEffectModuleView*>(&moduleAccesser->getEffectModule())->setWhole(true);
}

ftZeldaStatusUniqProcessFinal g_ftZeldaStatusUniqProcessFinal;
