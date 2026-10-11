// dvdFatal: the "an error has occurred, eject the disc" screen the dvd code can put up on its own.
// the message table is per sc language; strings are written as raw octal escapes because the
// japanese one is shift-jis and the rest are latin-1.
#include "revolution/dvd.h"
#include "revolution/os.h"
#include "revolution/sc.h"

extern u16 OSSetFontEncode(u16);

static void (*FatalFunc)(void) = NULL;

const char* const __DVDErrorMessage[] = {
    // japanese (shift-jis)
    "\n\n\n\203G\203\211\201[\202\252\224\255\220\266\202\265\202\334\202\265\202\275\201B\n\n"
    "\203C\203W\203F\203N\203g\203{\203^\203\223\202\360\211\237\202\265\202\304\203f\203B\203X\203N\202\360\216\346\202\350\217o\202\265\202\304\202\251\n"
    "\202\347\201A\226{\221\314\202\314\223d\214\271\202\360OFF\202\311\202\265\202\304\201A\226{\221\314\202\314\216\346\210\265\220\340\226\276\217\221\202\314\n"
    "\216w\216\246\202\311\217]\202\301\202\304\202\255\202\276\202\263\202\242\201B",

    // english
    "\n\n\nAn error has occurred.\n"
    "Press the Eject Button, remove the\n"
    "Game Disc, and turn the power off.\n"
    "Please read the Wii operations manual\n"
    "for more information.",

    // german
    "\n\n\nEin Fehler ist aufgetreten.\n"
    "Dr\374cke den Ausgabeknopf, entnimm die\n"
    "Game Disc und schalte die Wii-Konsole\n"
    "aus. Bitte lies die Bedienungsanleitung der\n"
    "Wii-Konsole, um weitere Informationen zu\n"
    "erhalten.",

    // french
    "\n\n\nUne erreur est survenue.\n"
    "Appuyez sur le bouton EJECT, retirez\n"
    "le disque de jeu et \351teignez la console.\n"
    "Veuillez vous r\351f\351rer au mode d'emploi\n"
    "de la console pour de plus amples\n"
    "informations.",

    // spanish
    "\n\n\nSe ha producido un error.\n"
    "Pulsa el Bot\363n EJECT, saca el disco y\n"
    "apaga la consola. Consulta el manual de\n"
    "instrucciones de la consola Wii para\n"
    "obtener m\341s informaci\363n.",

    // italian
    "\n\n\nSi \350 verificato un errore.\n"
    "Premi il pulsante EJECT, estrai il disco\n"
    "di gioco e spegni la console. Per maggiori\n"
    "informazioni consulta il manuale di\n"
    "istruzioni della console Wii.",

    // dutch
    "\n\n\nEr is een fout opgetreden.\n"
    "Druk op de ejectknop, verwijder de\n"
    "speldisk en zet het systeem uit. Lees\n"
    "de Wii-handleiding voor meer informatie."};

void __DVDShowFatalMessage(void) {
    const char* msg;
    GXColor bgColor = {0, 0, 0, 0};
    GXColor textColor = {255, 255, 255, 0};

    if (SCGetLanguage() == 0) {
        OSSetFontEncode(1);
    } else {
        OSSetFontEncode(0);
    }

    msg = (SCGetLanguage() > 6) ? __DVDErrorMessage[1] : __DVDErrorMessage[SCGetLanguage()];

    OSFatal(textColor, bgColor, msg);
}

BOOL DVDSetAutoFatalMessaging(BOOL enable) {
    BOOL enabled;
    BOOL old;

    enabled = OSDisableInterrupts();

    old = (FatalFunc != NULL);

    if (enable) {
        FatalFunc = __DVDShowFatalMessage;
    } else {
        FatalFunc = NULL;
    }

    OSRestoreInterrupts(enabled);
    return old;
}

BOOL __DVDGetAutoFatalMessaging(void) {
    return FatalFunc != NULL;
}

void __DVDPrintFatalMessage(void) {
    if (FatalFunc != NULL) {
        FatalFunc();
    }
}
