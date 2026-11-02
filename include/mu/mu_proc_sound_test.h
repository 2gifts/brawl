#pragma once

#include <gf/gf_archive.h>
#include <ms/ms_message.h>
#include <mu/mu_msg.h>
#include <mu/mu_name_letter_table.h>
#include <mu/mu_proc_menu.h>
#include <snd/snd_bgm.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// One category of the sound effects of the sound test: the list "sndSeListData" of the menu archive, ended by a
// category whose m_msgIndex is -1.
// HYPOTHESIS: the names of the fields follow how muProcSoundTest uses them.
struct sndSeListData {
    u32 m_msgIndex;         // 0x000 the name of the category (a line of the category messages)
    u32 m_iconKind;         // 0x004 the icon of the category (see muProcSoundTest::getIconFrame)
    u32 m_groupCount;       // 0x008
    s32 m_groups[60];       // 0x00C the sound groups to load before the effects play (-1 and 0: none)
    u32 m_rangeCount;       // 0x0FC twice the number of ranges
    u32 m_ranges[128];      // 0x100 the ranges of sound ids, as pairs (first, last)
};

// The sound test, a screen of the Vault. It has two pages: the music (a series of the games, then a track of it; A plays
// the track, Start stops it) and the sound effects (a category, then a number in it; A plays the effect). On each page
// the cursor goes down three lines: the page, the series or the category, the track or the effect.
// HYPOTHESIS: the names of the fields, of the steps and of the objects follow what the functions do; the names of the
// functions come from the map of the module where it has them.
class muProcSoundTest : public muProcMenu {
public:
    enum Step {
        Step_Music = 0,     // the music page
        Step_LoadSe = 2,    // the sound groups of the category load
        Step_Se = 3,        // the sound effect page
        Step_Leave = 4,     // B: the panels go out, then back to the Vault
    };

    enum Entry {
        Obj_Music = 0x69,       // the panel of the music
        Obj_Se = 0x6A,          // the panel of the sound effects
        Obj_State = 0x6B,       // the icon of the buttons and of the track that plays
        Obj_Back = 0x6F,        // the background
        Msg_Title = 0,          // the title of the track
        Msg_TrackCount = 1,     // how many tracks are unlocked
        Msg_Credits = 2,        // the composer and the arranger of the track (or, while a button is held, the other text)
        Msg_Series = 3,         // the name of the series
        Msg_Info = 4,           // the game the track comes from
        Msg_SeCategory = 5,     // the name of the category of the sound effects
        Msg_SeNumber = 6,       // the number of the sound effect
        MsgCount = 7,
        Msg_Buf = 0x400,
        SeriesCount = 21,
        SeCategoryCount = 101,
        SeGroupHeap = 0x1B,     // the heap level the sound groups of the effects load at
    };

    MuMsg* m_msg;                   // 0x66C
    bool m_isPlayingBGM;            // 0x670
    u32 m_bgmPlayId;                // 0x674 the play id of the track that plays (0: none)
    s32 m_seHandle;                 // 0x678 the sound effect that plays (-1: none)
    u32 m_seId;                     // 0x67C the sound id under the cursor
    s32 m_unk680;                   // 0x680
    s32 m_series;                   // 0x684
    u32 m_track;                    // 0x688 the track in the series
    s32 m_seCategory;               // 0x68C
    u32 m_seIndex;                  // 0x690 the number of the effect in the category, from 0
    void* m_seriesMsgData;          // 0x694 the names of the series
    void* m_titleMsgData;           // 0x698 the titles of the tracks
    void* m_infoMsgData;            // 0x69C the games the tracks come from
    void* m_altMsgData;             // 0x6A0 the other text of a track
    void* m_creditMsgData;          // 0x6A4 the composers and arrangers
    void* m_seCategoryMsgData;      // 0x6A8 the names of the categories of the effects
    sndSeListData* m_seList;        // 0x6AC
    u32 m_seCategoryCount;          // 0x6B0
    u32 m_seCount[SeCategoryCount]; // 0x6B4 how many effects each category has

    muProcSoundTest(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* seriesMsgData, void* titleMsgData,
                    gfArchive* archive);
    virtual ~muProcSoundTest() { delete m_msg; }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int pad);
    virtual void playSE_CursorMove();

    bool selBGMProc(int pad);  // the input of the music page
    void printBgmData();         // shows the track under the cursor
    void setPlaySEId(bool next);
    bool isEnableSEKategorie(int category);

    // The frame of the icon of a series or of a category.
    static float getIconFrame(u32 kind) {
        switch (static_cast<int>(kind)) {
        case 0x14:
            return 21.0f;
        case 0x15:
            return 50.0f;
        case 0x16:
            return 22.0f;
        case 0x17:
            return 23.0f;
        default:
            return 1.0f + kind;
        }
    }
    // Moves the series by step, skipping the series without unlocked tracks.
    void moveSeries(int step) {
        do {
            m_series += step;
            if (m_series < 0) {
                m_series = SeriesCount - 1;
            }
            if (m_series >= SeriesCount) {
                m_series = 0;
            }
        } while (lbl_805A01D8->getSeriesTitleCount(m_series) == 0);
    }
    // Prints a number of up to three digits with the digits of the menu.
    void printNumber(u32 msgIndex, u32 number) {
        char text[16] = { 0 };
        if (number >= 100) {
            Message::appendSubstr(text, g_muNameDigits, number / 100, 1);
        }
        if (number >= 10) {
            Message::appendSubstr(text, g_muNameDigits, number % 100 / 10, 1);
        }
        Message::appendSubstr(text, g_muNameDigits, number % 10, 1);
        m_msg->printf(msgIndex, text);
    }
    // Shows the category of the effects under the cursor and the number of the effect.
    void showSeCategory() {
        m_msg->printIndex(Msg_SeCategory, m_seList[m_seCategory].m_msgIndex, m_seCategoryMsgData);
        setAnimFrame(Obj_Se, getIconFrame(m_seList[m_seCategory].m_iconKind), 4);
        printNumber(Msg_SeNumber, m_seIndex + 1);
    }
};
