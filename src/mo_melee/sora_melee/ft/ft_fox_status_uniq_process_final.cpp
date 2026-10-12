#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the article manage module (void* in the shared enumeration). Slot 0x48 takes two ints.
// P pad slots before a named slot place it at vtable index P + 2 (the destructor pair leads), so 16 pads give 0x48.
class ftFoxFinalArticleManageView {
public:
    virtual ~ftFoxFinalArticleManageView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void unk48(int, int);
};

class ftFoxStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftFoxStatusUniqProcessFinal g_ftFoxStatusUniqProcessFinal;

void ftFoxStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) { }

// Status kinds 0x116 and up are remapped when the stage object reports subkind 0x13 (+3); the article-manage
// view then gets a call unless the status lands on one of the final-kind values.
void ftFoxStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int curStatus = moduleAccesser->getStatusModule().getStatusKind();
    if (curStatus > 0x116) {
        switch (moduleAccesser->getStageObject().soGetSubKind()) {
        case 0x6:
            break;
        case 0x13:
            curStatus += 3;
            break;
        case 0x2c:
            break;
        }
    }
    if (curStatus != 0x11f) {
        int nextKind = nextStatus;
        if (nextKind > 0x116) {
            switch (moduleAccesser->getStageObject().soGetSubKind()) {
            case 0x6:
                break;
            case 0x13:
                nextKind += 3;
                break;
            case 0x2c:
                break;
            }
        }
        if (nextKind != 0x11d && nextKind != 0x11e && nextKind != 0x11f) {
            reinterpret_cast<ftFoxFinalArticleManageView&>(*moduleAccesser->m_enumerationStart->m_generateArticleManageModule)
                .unk48(3, 0);
        }
    }
}

void ftFoxStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) { }
