#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <if/if_utility.h>
#include <memory.h>
#include <string.h>
#include <mu/mu_object.h>
#include <types.h>

// The scene that shows the pictures of the snapshot mode (module sora_scene; only what this menu needs is declared).
// HYPOTHESIS: the names follow how the menu uses the fields.
struct muSnapShotSceneHolder {
    u8 m_unk00[0x48];
    void* m_scnRoot;                // 0x48 the scene root that the models of the menu are added to
    u8 m_unk4C[0x1A4 - 0x4C];
    u8 m_hasScnRoot;                // 0x1A4
};

struct scSnapShot {
    u8 m_unk000[0x35C];
    muSnapShotSceneHolder** m_holder;   // 0x35C
};

// A text for the message window: the data and how much of it is used.
struct muSnapShotMsgBuf {
    u8 m_data[0x80];
    int m_size;                     // 0x80

    void append(const char* text, u32 size) {
        int used = m_size;
        memcpy(m_data + used, text, size);
        m_size = used + size;
    }
};

// The name of a file of the collection, as the collection hands it out.
// HYPOTHESIS: the size follows from how the name is copied around.
struct muCollectionFileName {
    char m_name[0x41];
};

// The decoded picture of a snapshot: the pixels and the size.
// HYPOTHESIS: the first word owns the pixels (the destructor gives them back).
struct muSnapShotJpeg;
// muRecorder::Data::reset(pixels), the function of main.dol that gives the pixels back and takes new ones
extern "C" void fn_800C2BB4(muSnapShotJpeg* jpeg, void* pixels);

struct muSnapShotJpeg {
    void* m_pixels;                 // 0x00
    u32 m_width;                    // 0x04
    u32 m_height;                   // 0x08

    muSnapShotJpeg() : m_pixels(0) { }
    ~muSnapShotJpeg() {
        if (this != 0) {
            reset(0);
        }
    }
    void reset(void* pixels) { fn_800C2BB4(this, pixels); }
};

// The base of the menus that browse through the pictures of the collection (module sora_menu_collect_viewer). HYPOTHESIS:
// only what the snapshot menu needs is declared; the names of the virtual functions come from the map of that module and
// the unnamed ones keep the offset of their function there.
class muCollectionViewer : public utility::StandardMenu {
public:
    // The files that the menu loads: the first four words of one of them are not known.
    struct MsgFile {
        u8 m_unk00[0x10];
        gfFileIOHandle m_handle;    // 0x10
        u8 m_unk14[4];
    };

    // One medium (the console or the card) that the pictures are on.
    struct Media {
        int m_numPic;               // 0x00
        const char* m_names;        // 0x04 the names of the files, one after the other
        int m_unk08;
        int m_unk0C;
    };

    // The names of the animations of a panel (11 for the left panel and 11 for the right one) and for each state of the
    // panel the one to play. HYPOTHESIS: the sizes follow from how the snapshot menu uses the table.
    struct PanelMotionTable {
        MuAnimNameData m_anim[22];
        u8 m_index[12];             // 0x1B8

        PanelMotionTable(const char* nameLeft, const char* nameRight);
    };

    class Work {
    public:
        int m_unk000;
        u8 m_unk004[0x10];
        MsgFile m_brres;            // 0x014 the models (a loader of the menu: it is reset, then asked to read a file)
        u8 m_brresFlag;             // 0x02C
        u8 m_unk02D[0x9C - 0x2D];
        int m_unk09C;
        u8 m_unk0A0[0x10];
        MsgFile m_wait;             // 0x0B0 the wait window
        u8 m_waitFlag;              // 0x0C8
        u8 m_unk0C9[0x104 - 0xC9];
        MuObject* m_objs[25];       // 0x104 the models of the menu (the last one holds the picture)
        u8 m_unk168[0x38];
        int m_curMedia;             // 0x1A0
        int m_unk1A4;
        Media m_media[2];           // 0x1A8
        u8 m_unk1C8[0x1B40 - 0x1C8];
        int m_numFiles;             // 0x1B40
        u8 m_unk1B44[0x1BF0 - 0x1B44];
        int m_panelState;           // 0x1BF0
        int m_exitRequest;          // 0x1BF4
        int m_unk1BF8;
        int m_state;                // 0x1BFC
        u8 m_unk1C00[0x1C80 - 0x1C00];
        MsgFile m_msgFile[3];       // 0x1C80
        u8 m_unk1CC8[0x1CE4 - 0x1CC8];
        void* m_collection;         // 0x1CE4
        u8 m_unk1CE8[0x1CEE - 0x1CE8];
        u8 m_extraPanel;            // 0x1CEE
        u8 m_unk1CEF[0x1CF8 - 0x1CEF];
        int m_unk1CF8;              // 0x1CF8
        u8 m_unk1CFC[0x1D08 - 0x1CFC];

        Work(int numObjs, int unk, const char* name);
        ~Work();
    };

    u8 m_unk40[0x168];
    u32 m_input;                    // 0x1A8
    Work* m_work;                   // 0x1AC

    muCollectionViewer(const char* name);
    virtual ~muCollectionViewer();
    virtual bool checkLoadFinish() const;
    virtual void update();

    virtual void createObjResFile(const char* name);                                    // 0x1260
    virtual void expandPic(int unk);                                                    // 0x1A38
    virtual void changePicAnim(const char* name, int unk, Work* work);                  // 0x1BCC
    virtual void unk_1D58();
    virtual void unk_1ECC();
    virtual void unk_3134();
    virtual void unk_2EC0();
    virtual void getMineLabel(muSnapShotMsgBuf* buf);
    virtual void getNewLabel();
    virtual void unk_2FA0();
    virtual u32 getCurFileID();
    virtual muCollectionFileName getFileHandle(int index);
    virtual void unk_2FC8();
    virtual void addObjCore(void* obj);
    virtual void removeObjCore(void* obj);
    virtual int getNumPic(int media);
    virtual int getNumPanel(int media);
    virtual void unk_3784();
    virtual void unk_38F8();
    virtual void dispPreview();                                                         // 0x21BC
    virtual void unk_2528();
    virtual int dispThumbnail();
    virtual void dispPreviewCore();
    virtual void erasePreviewCore();
    virtual void unk_3194();
    virtual void unk_31D8();
    virtual void* getScnRoot();
    virtual void setPanelStateCore(int state, int unk);
    virtual void unk_3B1C();
    virtual void unk_3B70();
    virtual void cancelAndExit();
    virtual void controlProc();                                                         // 0x5418
    virtual void controlWaitForDialog();                                                // 0x5A1C
    virtual void onDecided();
    virtual void controlExit();
    virtual void controlPreCheck();                                                     // 0x5018
    virtual void onPreCheckFinished();
    virtual void controlScanStart();                                                    // 0x527C
    virtual void controlScanAllStart();                                                 // 0x52E8
    virtual void controlScan();                                                         // 0x533C
    virtual void controlPicSelect();                                                    // 0x5A90
    virtual int slidePic(int unk, int unk2);                                            // 0x5C8C
    virtual void slidePicCore(int unk);                                                 // 0x5D4C
    virtual void controlLoadStart();                                                    // 0x5F24
    virtual void controlLoading();                                                      // 0x5F9C
    virtual void controlLoadFinish();
    virtual void onLoadFailed();
    virtual void controlErase();                                                        // 0x6068
    virtual void openEraseConfirmWnd();                                                 // 0x61FC
    virtual void closeEraseConfirmWnd();                                                // 0x62AC
    virtual void dispEraseConfirmWndYesNo();                                            // 0x6344
    virtual void openEraseReportWnd();                                                  // 0x6450
    virtual void unk_64C8();
    virtual void closeEraseReportWnd();                                                 // 0x6520
    virtual void controlEraseConfirm();                                                 // 0x65A0
    virtual void openWaitWnd();                                                         // 0x67B8
    virtual void closeWaitWnd();                                                        // 0x683C
    virtual void controlEraseReady();                                                   // 0x684C
    virtual void controlErasing();                                                      // 0x6900
    virtual void controlEraseReport();                                                  // 0x69F0
    virtual void endErase();                                                            // 0x6A84
    virtual void unk_6AE8();
    virtual void openMediaMenu();                                                       // 0x6E28
    virtual void beginCopy();                                                           // 0x71E0
    virtual void controlCopyReady();                                                    // 0x72B8
    virtual void controlCopying();                                                      // 0x73B4
    virtual void controlCopyReport();                                                   // 0x74D0
    virtual void endCopy();                                                             // 0x7274
    virtual void dispMediaMenuSelect();                                                 // 0x6E80
    virtual void controlMediaMenu();                                                    // 0x6EF8
    virtual void controlMediaCheck();                                                   // 0x7564
    virtual void controlBrowsing();
    virtual void controlBrowsingLoading();
    virtual void controlPopUpStart();
    virtual void controlPopUp();
    virtual void controlPopUpCheckStart();
    virtual void controlPopUpChecking();
    virtual void controlNewDataStart();
    virtual void controlNewDataChecking();
    virtual void controlNewData();
    virtual void controlNewDataExit();
    virtual void controlSend();                                                         // 0x7730
    virtual void controlContribute();                                                   // 0x7C90
    virtual void getSummary();                                                          // 0x4148
};
static_assert(sizeof(muCollectionViewer) == 0x1B0, "Class is wrong size!");
static_assert(sizeof(muCollectionViewer::Work) == 0x1D08, "Class is wrong size!");

// The snapshot browser of the collection: shows the pictures that the player took in the snapshot mode, one at a time.
class muSnapShotTask : public muCollectionViewer {
public:
    // The data of the menu. HYPOTHESIS: the picture is the one that is decoded from the file for the screen.
    class Work : public muCollectionViewer::Work {
    public:
        Work();
        virtual ~Work();

        scSnapShot* m_scene;                // 0x1D0C the scene the menu belongs to
        int m_slideResult;                  // 0x1D10 what the last slide of the picture gave
        muSnapShotJpeg m_jpeg;              // 0x1D14
    };

    muSnapShotTask(const char* name) : muCollectionViewer(name) { }
    static muSnapShotTask* create(scSnapShot* scene);

    virtual ~muSnapShotTask();
    virtual bool preLoad();
    virtual void loadResource();
    virtual void createModel();
    virtual void getMineLabel(muSnapShotMsgBuf* buf);
    virtual void getNewLabel();
    virtual u32 getCurFileID();
    virtual muCollectionFileName getFileHandle(int index);
    virtual void addObjCore(void* obj);
    virtual void removeObjCore(void* obj);
    virtual int getNumPic(int media);
    virtual int getNumPanel(int media);
    virtual int dispThumbnail();
    virtual void dispPreviewCore();
    virtual void erasePreviewCore();
    virtual void* getScnRoot();
    virtual void setPanelStateCore(int state, int unk);
    virtual void cancelAndExit();
    virtual void onDecided();
    virtual void controlExit();
    virtual void onPreCheckFinished();
    virtual void controlLoadFinish();
    virtual void onLoadFailed();
    virtual void controlBrowsing();
    virtual void controlBrowsingLoading();
    virtual void controlPopUpStart();
    virtual void controlPopUp();
    virtual void controlPopUpCheckStart();
    virtual void controlPopUpChecking();
    virtual void controlNewDataStart();
    virtual void controlNewDataChecking();
    virtual void controlNewData();
    virtual void controlNewDataExit();

    Work* getWork() const { return static_cast<Work*>(m_work); }
};
static_assert(sizeof(muSnapShotTask) == 0x1B0, "Class is wrong size!");
