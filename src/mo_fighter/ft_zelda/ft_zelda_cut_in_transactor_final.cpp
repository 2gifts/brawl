#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: the cut-in transactor interface (RTTI name soCutInTransactor; its only slot after the destructor is the
// per-frame update, which returns whether the cut-in is over).
class soCutInTransactor {
public:
    virtual ~soCutInTransactor() { }
    virtual bool processUpdate(soModuleAccesser* moduleAccesser) = 0;
};

// The camera work of Zelda's (and Sheik's) Final Smash cut-in.
// TODO: processUpdate (the zoom-in on the targets, work ints 0x20000019/0x2000001a).
class ftZeldaCutInTransactorFinal : public soCutInTransactor {
public:
    virtual ~ftZeldaCutInTransactorFinal() { }
    virtual bool processUpdate(soModuleAccesser* moduleAccesser);
};

ftZeldaCutInTransactorFinal g_ftZeldaCutInTransactorFinal;
