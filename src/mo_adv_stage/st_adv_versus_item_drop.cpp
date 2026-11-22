#include <ad/ad_keep_manager.h>
#include <gf/gf_task.h>
#include <it/it_gen_archive.h>
#include <it/it_manager.h>
#include <mt/mt_prng.h>
#include <st/st_positions.h>
#include <types.h>

// Two header-level records {0xFF, 0} and {0xFF, 1} that every unit of this kind carries (see sc_prize_info).
// HYPOTHESIS: their type and names; only the values are known.
struct stAdvItemDropEntry {
    int m_id;
    int m_flag;
    stAdvItemDropEntry(int id, int flag) : m_id(id), m_flag(flag) { }
};
static stAdvItemDropEntry s_itemDropEntry0(0xFF, 0);
static stAdvItemDropEntry s_itemDropEntry1(0xFF, 1);

// The item drop of the adventure versus stages: once started it creates an item of its item table at a random item
// position of the stage, every [min, max) frames (picked at random each time).
// HYPOTHESIS: the member names follow how the functions use them.
class stAdventureVersusItemDrop : public gfTask {
public:
    stPositions* m_positions;      // 0x40
    itGenSheetKind m_sheetKind;    // 0x44
    u8 m_isActive;                 // 0x4C
    u8 m_isReady;                  // 0x4D
    u8 m_isStarted;                // 0x4E
    u32 m_minInterval;             // 0x50
    u32 m_maxInterval;             // 0x54
    u32 m_interval;                // 0x58
    u32 m_frame;                   // 0x5C
    u32 m_waitFrame;               // 0x60

    void reset();
    void ready(stPositions* positions, u32 minInterval, u32 maxInterval);
    void start();
    void end();
    u8 isReady();
    u8 isStarted();
    virtual void processUpdate();
    virtual ~stAdventureVersusItemDrop();
};

void stAdventureVersusItemDrop::reset() {
    m_positions = NULL;
    m_isActive = false;
    m_isReady = false;
    m_isStarted = false;
    m_minInterval = 0;
    m_maxInterval = 0;
    m_interval = 0;
    m_frame = 0;
}

void stAdventureVersusItemDrop::ready(stPositions* positions, u32 minInterval, u32 maxInterval) {
    m_positions = positions;
    m_minInterval = minInterval;
    m_maxInterval = maxInterval;
    m_isReady = true;
}

void stAdventureVersusItemDrop::start() {
    m_isActive = true;
    m_isStarted = true;
    m_frame = 0;
    m_interval = m_minInterval + (u32)((m_maxInterval - m_minInterval) * randf());
}

void stAdventureVersusItemDrop::end() {
    m_positions = NULL;
    m_isActive = false;
    m_isReady = false;
    m_isStarted = false;
    m_minInterval = 0;
    m_maxInterval = 0;
    m_interval = 0;
    m_frame = 0;
}

u8 stAdventureVersusItemDrop::isReady() {
    return m_isReady;
}

u8 stAdventureVersusItemDrop::isStarted() {
    return m_isStarted;
}

void stAdventureVersusItemDrop::processUpdate() {
    if (m_waitFrame < 120) {
        m_waitFrame++;
    } else if (m_isActive) {
        if (m_frame == m_interval) {
            if (m_positions != NULL) {
                Vec3f pos;
                pos.m_z = 0.0f;
                pos.m_y = 0.0f;
                pos.m_x = 0.0f;
                m_positions->getRandItemPos(&pos);
                // TODO: clean up - global flag 1001 picks the second item table (the getter's result is compared
                // with zero as a word).
                itGenId genId = static_cast<itGenId>(10001 + (adKeepManager::getInstance()->getGlobalFlag(1001) != 0));
                itManager::getInstance()->lotCreateItem(m_sheetKind, genId, &pos, -1, -1, static_cast<itKind>(-1));
            }
            m_frame++;
        } else if (m_frame >= m_maxInterval) {
            start();
        } else {
            m_frame++;
        }
    }
}

stAdventureVersusItemDrop::~stAdventureVersusItemDrop() { }
