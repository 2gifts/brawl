#define MT_VEC2F_MEMBER_COPY // MATCH-ONLY: see mt_vector.h.
// BYTES: aiMgr registers with the fighter manager's event module through the s16 manage id that getManageId returns,
// which the original re-extends at every use (the same registration as mlFtEvent's constructor).
#define SO_EVENT_OBSERVER_MANAGE_S16
#include <ai/ai_cl_block.h>
#include <ai/ai_enemy.h>
#include <ai/ai_input.h>
#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <ai/ai_weapon.h>
#include <ai/ai_param.h>
#include <ft/ft_manager.h>
#include <gf/gf_archive.h>
#include <gf/gf_area.h>
#include <gf/gf_area_manager.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <gm/gm_lib.h>
#include <mt/mt_prng.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_test_types.h>
#include <st/stage.h>
#include <st_madein/st_madein.h>
#include <cm/cm_camera_controller.h>
#include <gm/gm_global.h>
#include <revolution/OS.h>
#include <types.h>

// The CPU player AI manager (aiMgr) and its helpers. HYPOTHESIS: the member names follow how the functions use them.

extern "C" {
// aiMgr readiness flag: -1 while the AI is disabled, 0.. once the floor information has been prepared
extern s8 lbl_27_data_47BF8;
}

// HYPOTHESIS: debug timings of aiMgr::processBegin (weapon pass, floor blocks, weapon/enemy update, fighter stats).
extern s64 g_aiMgrProcTime[4];
// HYPOTHESIS: the ladder that aiMgr::search_ladder picked last.
extern gfArea* g_aiLadderArea;

// The ladders (Stage Builder parts) that a CPU can climb from one floor block to another (see aiMgr::search_ladder).
// HYPOTHESIS: the member names; the layout follows the original's stack object.
class chkAreaProc : public gfAreaForeachProc {
public:
    AiClBlock* m_block;   // 0x04: the block the CPU stands on (ignored by the collision checks)
    AiClBlock* m_target;  // 0x08: the block to reach
    float m_width;        // 0x0C: margins added around both blocks
    float m_height;       // 0x10
    gfArea* m_areas[16];  // 0x14: the ladders found
    u32 m_numAreas;       // 0x54

    virtual bool exec(gfArea* area);
};

// Whether the segment from -> to crosses a collision other than ignore.
static inline bool aiChkCross(const Vec2f& from, const Vec2f& to, AiClBlock* ignore) {
    return g_aiMgr->chk_cl_cross(const_cast<Vec2f*>(&from), const_cast<Vec2f*>(&to), 0xFF, NULL, ignore);
}

// Sets box from its corners.
static inline void aiSetBox(clAABBox2D* box, const Vec2f& min, const Vec2f& max) {
    box->m_unk1 = min.m_x;
    box->m_unk2 = min.m_y;
    box->m_unk3 = max.m_x;
    box->m_unk4 = max.m_y;
}

// HYPOTHESIS: create precedes setEnable in the unit, so the original spells setEnable's body out here instead of calling it.
void aiMgr::create(int enable) {
    if (g_aiMgr == NULL) {
        new (Heaps::System) aiMgr();
    }
    if (g_aiMgr != NULL) {
        g_aiMgr->m_enable = enable;
        if (enable == 0) {
            lbl_27_data_47BF8 = -1;
            if (g_aiMgr->m_weaponMgr != NULL) {
                g_aiMgr->m_weaponMgr->reset();
            }
            if (g_aiMgr->m_enemyMgr != NULL) {
                g_aiMgr->m_enemyMgr->reset();
            }
            g_aiMgr->CrearWork();
        }
    }
    lbl_27_data_47BF8 = -1;
}

// Takes the common AI pac: the refer and system parameter tables and the common script part.
// HYPOTHESIS: owned = the manager frees the data when it is replaced (the caller passes 0).
void aiMgr::pacCmnRegist(void* data, int size, bool owned) {
    if (g_aiMgr != NULL && data != NULL) {
        gfArchive archive;
        archive.setFileImage(data, size, Heaps::Tmp);
        if (g_aiMgr->m_pacData != NULL && g_aiMgr->m_pacOwned) {
            if (g_aiMgr->m_pacData != NULL) {
                gfHeapManager::free(g_aiMgr->m_pacData);
                g_aiMgr->m_pacData = NULL;
            }
            g_aiMgr->m_pacOwned = false;
        }
        g_aiMgr->m_pacData = data;
        g_aiMgr->m_pacOwned = owned;
        aiMgr* mgr = g_aiMgr;
        mgr->m_referPar = static_cast<aiReferPar*>(archive.getData(0, Data_Type_Misc, 0, 0xFFFE));
        mgr->m_systemPar = archive.getData(0, Data_Type_Misc, 1, 0xFFFE);
        mgr->m_unk134 = archive.getData(2, Data_Type_Misc, 0, 0xFFFE);
        if (mgr->m_aiInput != NULL) {
            for (int i = 0; i < NumChrs; i++) {
                if (mgr->m_aiInput[i] != NULL) {
                    mgr->m_aiInput[i]->setCpuType(-1);
                }
            }
        }
    }
}

aiMgr::aiMgr() : gfTask("aiMgr", Category_AI, 5, 6, true), ftOutsideEventObserver(-1, 0) {
    g_aiMgr = this;
    m_stat = new (Heaps::System) aiStat[NumChrs + 1];
    m_weaponMgr = new (Heaps::System) aiWeaponMgr;
    m_enemyMgr = new (Heaps::System) aiEnemyMgr;
    CrearWork();
    m_jointLists = NULL;
    m_blocks = NULL;
    m_maxJointLists = 0x176;
    m_maxBlocks = 0x176;
    if (m_sendID <= -1) {
        soEventManageModule* module = &g_ftManager->m_eventManageModule;
        addObserverSubS16(module->getManageId(), *this, -1);
    }
    for (int i = 0; i < NumChrs; i++) {
        m_aiInput[i] = NULL;
        m_chrState[i] = -1;
    }
    for (int i = 0; i < NumPlayers; i++) {
        m_cpuTypeLock[i] = -1;
    }
    int i = 0;
    while (i < NumPlayers) {
        m_cpuRankLock[i++] = -1;
    }
    m_enable = 1;
    m_adventureMd = 0;
    m_flags = 0;
    m_pacOwned = 0;
    m_pacData = NULL;
    m_unk134 = NULL;
    m_referPar = NULL;
    m_systemPar = NULL;
    if (m_maxJointLists != 0 && m_maxBlocks != 0) {
        m_jointLists = new (Heaps::System) tag_AiClJointList[m_maxJointLists];
        m_blocks = new (Heaps::System) AiClBlock[m_maxBlocks];
    }
}

AiClBlock::AiClBlock() { }

tag_AiClJointList::tag_AiClJointList() { }

aiMgr::~aiMgr() {
    if (m_stat != NULL) {
        delete[] m_stat;
        m_stat = NULL;
    }
    if (m_weaponMgr != NULL) {
        delete m_weaponMgr;
        m_weaponMgr = NULL;
    }
    if (m_enemyMgr != NULL) {
        delete m_enemyMgr;
        m_enemyMgr = NULL;
    }
    if (m_jointLists != NULL) {
        delete[] m_jointLists;
        m_jointLists = NULL;
    }
    if (m_blocks != NULL) {
        delete[] m_blocks;
        m_blocks = NULL;
    }
    if (m_pacOwned == 1 && m_pacData != NULL) {
        gfHeapManager::free(m_pacData);
        m_pacData = NULL;
    }
    g_aiMgr = NULL;
}

bool aiMgr::isReady() {
    return lbl_27_data_47BF8 > 0;
}

bool aiMgr::isReadyUpdate() {
    return lbl_27_data_47BF8 >= 0;
}

char* aiMgr::getMadeinAiData(bool includePreparation) {
    if (g_aiMgr->m_stageKind == Stages::Madein && g_Stage != NULL) {
        stMadeinAiData* data = static_cast<stMadeinAiData*>(g_Stage->getMadeinAiData());
        // The stage sets +1 while the microgame is running; +0 exposes its
        // preparation phase only to callers that request it.
        if (data != NULL && (data->m_unk1 != 0 || (includePreparation && data->m_active != 0))) {
            return reinterpret_cast<char*>(data);
        }
    }
    return NULL;
}

bool aiMgr::isSmallMap() {
    switch (g_aiMgr->m_stageKind) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 9:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1C:
    case 0x1E:
    case 0x21:
    case 0x24:
    case 0x2B:
    case 0x2E:
    case 0x32:
        return true;
    default:
        return false;
    }
}

void* aiMgr::getAiSystemPar() {
    return g_aiMgr->m_systemPar;
}

aiReferPar* aiMgr::getAiReferPar(int index) {
    if (index < 0 || index > 0x35) {
        index = 0;
    }
    return g_aiMgr->m_referPar + index;
}

int aiMgr::getCpuType(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return m_aiInput[player]->m_cpuType;
    }
    return 0;
}

int aiMgr::getCpuRank(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return m_aiInput[player]->m_par.m_rank;
    }
    return 0;
}

int aiMgr::getAiCpuTarget(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return m_aiInput[player]->m_target.m_idx;
    }
    return -1;
}

bool aiMgr::chkNanaForceMove(int player) {
    if (m_aiInput != NULL && player >= 0 && player < 7 && m_aiInput[player + 7] != NULL) {
        aiInput* nana = m_aiInput[player + 7];
        if (nana->m_md == 0x79 && (nana->m_flags & 8) == 0) {
            return true;
        }
    }
    return false;
}

int aiChrIdx::GetPlayerNo(const aiChrIdx* idx) {
    if (idx->m_idx >= 0 && idx->m_idx < 14) {
        return idx->m_idx % 7;
    }
    return idx->m_idx;
}

int aiChrIdx::GetFighterNo(const aiChrIdx* idx) {
    s8 value = idx->m_idx;
    if (value >= 0 && value < 14) {
        return value / 7;
    }
    return 0;
}

bool aiChrIdx::IsValid(const aiChrIdx* idx) {
    return idx->m_idx >= 0;
}

bool aiChrIdx::IsPlayer(const aiChrIdx* idx) {
    bool result = false;
    if (idx->m_idx >= 0) {
        if (idx->m_idx < 14) {
            result = true;
        }
    }
    return result;
}

// Danger zones: 24 axis-aligned boxes the CPUs avoid (hazards, projectiles, ...). Slots 0-7 are the stage's zones and
// 8-23 the dynamic ones. m_dangerAttr (one byte per slot, bit 0 = in use, the rest an attribute mask), m_dangerZone (the
// box corners and the team) and m_numDangerZones (the in-use count).
void aiMgr::delDangerZone(int index) {
    if (index < 0 || (u32)index >= 0x18 || m_dangerAttr[index] == 0) {
        return;
    }
    m_dangerAttr[index] = 0;
    m_numDangerZones--;
}

void aiMgr::clearDangerZone(int index) {
    if (index == 0 && (m_flags & 0x100) != 0) {
        return;
    }
    int first = 0;
    int last = 0x18;
    switch (index) {
    case 0:
        last = 8;
        break;
    case 1:
        first = 8;
        break;
    }
    for (; first < last; first++) {
        delDangerZone(first);
    }
}

int aiMgr::setDangerZone(Vec2f* p1, Vec2f* p2, int index, bool unk, bool attr) {
    int mode = unk;
    if (mode == 0 && (m_flags & 0x100) != 0) {
        return -1;
    }
    int first;
    int last;
    if (mode == 1) {
        first = 8;
        last = 0x18;
    } else {
        if (m_stageKind == 0x1F) {
            return -1;
        }
        first = 0;
        last = 8;
    }
    if (index < first || last <= index) {
        if (index >= 0) {
            return -1;
        }
        index = first;
        for (int i = first; i < last; i++) {
            if (m_dangerAttr[index] == 0) {
                break;
            }
            index++;
        }
        if ((u32)index >= 0x18) {
            return -1;
        }
    }
    float x0 = p1->m_x;
    float x1 = p2->m_x;
    if (x0 <= x1) {
        m_dangerZone[index].m_minX = x0;
        m_dangerZone[index].m_maxX = x1;
    } else {
        m_dangerZone[index].m_maxX = x0;
        m_dangerZone[index].m_minX = x1;
    }
    float y0 = p1->m_y;
    float y1 = p2->m_y;
    if (y0 <= y1) {
        m_dangerZone[index].m_minY = y0;
        m_dangerZone[index].m_maxY = y1;
    } else {
        m_dangerZone[index].m_maxY = y0;
        m_dangerZone[index].m_minY = y1;
    }
    if (m_dangerAttr[index] == 0) {
        m_numDangerZones++;
        m_dangerAttr[index] = attr | 1;
        m_dangerZone[index].m_team = -1;
    }
    return index;
}

void aiMgr::setDangerAttr(int index, u8 attr) {
    u8 current;
    if (index < 0 || (u32)index >= 0x18 || (current = m_dangerAttr[index]) == 0) {
        return;
    }
    m_dangerAttr[index] = current | attr;
}

void aiMgr::setDangerTeam(int index, int team) {
    if (index < 0 || (u32)index >= 0x18 || m_dangerAttr[index] == 0) {
        return;
    }
    u8* zone = reinterpret_cast<u8*>(this) + index * 0x14;
    *reinterpret_cast<int*>(zone + 0x15C) = team;
}

// Whether a point lies inside an active danger zone (other than the ones registered for the given team, if team >= 0,
// and only those carrying one of the attribute bits, if attr != 0).
bool aiMgr::chkDangerZone(Vec2f* pos, int team, u8 attr) {
    aiMgr* manager = g_aiMgr;
    if (manager->m_numDangerZones != 0) {
        // MATCH-ONLY: the attribute bytes are read through a byte pointer that starts at the manager (m_dangerAttr is at
        // +0x32C), and the zones through a float pointer; typed array indexing did not reproduce the loop (the original
        // steps one pointer by 2 bytes and one by 0x28 per iteration). Two zones per iteration (the loop is unrolled by two).
        u8* base = reinterpret_cast<u8*>(manager);
        float* box = &manager->m_dangerZone[0].m_minX;
        for (int i = 0; i < 12; i++, box += 10, base += 2) {
            if (base[0x32C] != 0 && (team < 0 || *reinterpret_cast<int*>(&box[4]) != team)) {
                if (attr == 0 || (attr & base[0x32C]) != 0) {
                    if (!(pos->m_x > box[2]) && !(pos->m_x < box[0]) && !(pos->m_y > box[3]) && !(pos->m_y < box[1])) {
                        return true;
                    }
                }
            }
            if (base[0x32D] != 0 && (team < 0 || *reinterpret_cast<int*>(&box[9]) != team)) {
                if (attr == 0 || (attr & base[0x32D]) != 0) {
                    if (!(pos->m_x > box[7]) && !(pos->m_x < box[5]) && !(pos->m_y > box[8]) && !(pos->m_y < box[6])) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

static inline bool aiIndexValid(s8 index) {
    bool valid = false;
    if (index >= 0 && index < 14) {
        valid = true;
    }
    return valid;
}

aiStat* aiMgr::get_aiStat(aiChrIdx* idx) {
    aiStat* stat;
    s8 index = idx->m_idx;
    if (index >= 0) {
        if (aiIndexValid(index)) {
            stat = &g_aiMgr->m_stat[index];
        } else {
            aiEnemy* enemy = g_aiMgr->m_enemyMgr->getEnemy(index - 14);
            stat = &g_aiMgr->m_stat[14];
            if (g_aiMgr->m_unkA4.m_idx != idx->m_idx - 14 && enemy != NULL) {
                stat->update_enemy(enemy, idx);
                g_aiMgr->m_unkA4.m_idx = idx->m_idx - 14;
            }
        }
    } else {
        stat = &g_aiMgr->m_stat[14];
    }
    return stat;
}

// Whether a character slot has a registered CPU controller. HYPOTHESIS: an inline of the original, as the same test
// is expanded in several aiMgr functions.
static inline bool aiIsRegistered(aiMgr* mgr, int slot) {
    if (slot >= 0 && slot < aiMgr::NumChrs && mgr->m_chrState[slot] != 0 && mgr->m_aiInput[slot] != NULL)
    {
        return true;
    }
    return false;
}

// A player leaves the target lists: every CPU of that player, and (when onlySelf is 0) every CPU that was after it,
// starts over as if it had been hit. HYPOTHESIS: the parameter names follow the two callers (removePlayer passes 0,
// notifyEventDead 1).
void aiMgr::removeTgtRefer(int player, int onlySelf) {
    for (int i = 0; i < NumChrs; i++) {
        aiChrIdx idx;
        idx.m_idx = i;
        if (get_aiStat(&idx)->m_chrKind != -1 && aiIsRegistered(this, i)) {
            aiInput* input = m_aiInput[i];
            if (input != NULL) {
                if (player == aiChrIdx::GetPlayerNo(&idx) ||
                    (onlySelf == 0 && aiChrIdx::IsValid(&input->m_target) && player == aiChrIdx::GetPlayerNo(&input->m_target)))
                {
                    input->damageStart(NULL, 1, 1);
                }
            }
        }
    }
}

// A fighter took damage: its CPU reacts, and the CPU whose attack it was (same task id) records the hit.
void aiMgr::notifyEventOnDamage(int entryId, u32 hp, soDamage* damage) {
    int player = g_ftManager->getPlayerNo(entryId);
    if (aiIsRegistered(this, player)) {
        aiChrIdx idx;
        idx.m_idx = player;
        get_aiStat(&idx)->m_damageTeam = damage->m_collisionLog.m_teamNo;
        m_aiInput[idx.m_idx]->damageStart(damage, 0, 0);
    }
    for (int i = 0; i < NumPlayers; i++) {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* stat = get_aiStat(&idx);
        if (stat->m_chrKind != -1 && damage->m_collisionLog.m_taskId == stat->m_taskId && m_chrState[i] == 1 &&
            m_aiInput[i] != NULL)
        {
            aiChrIdx victim;
            victim.m_idx = player;
            m_aiInput[i]->swingHit(&victim, damage);
        }
    }
}

void aiMgr::setEnable(int enable) {
    g_aiMgr->m_enable = enable;
    if (enable == 0) {
        lbl_27_data_47BF8 = -1;
        if (g_aiMgr->m_weaponMgr != NULL) {
            g_aiMgr->m_weaponMgr->reset();
        }
        if (g_aiMgr->m_enemyMgr != NULL) {
            g_aiMgr->m_enemyMgr->reset();
        }
        g_aiMgr->CrearWork();
    }
}

void aiMgr::CrearWork() {
    m_frame = 0;
    m_unkA4.m_idx = -1;
    for (int i = 0; i < 24; i++) {
        m_dangerAttr[i] = 0;
    }
    m_numDangerZones = 0;
    m_unk104 = 0;
    m_unk110 = 0;
    m_unk11C = 0;
    m_unk128 = 0;
    m_unk344 = 1;
    m_unk35C = 0;
    m_unk35A = 0;
    m_unk358 = 0;
    m_numBlocks = 0;
    m_unk354 = 0;
    m_areaLeft = -99999.9f;
    m_areaBottom = -99999.9f;
    m_areaRight = 99999.9f;
    m_areaTop = 99999.9f;
    m_stageKind = 0;
    m_unk378 = 0;
    m_unk374 = 0;
    m_unk37C = 0.0f;
    m_ruleTime = 0;
    m_numTeams = 0;
    m_unk385 = 0;
    m_unk386 = 0;
    gfMemFill(m_work, 0, sizeof(m_work));
}

// HYPOTHESIS: refreshes the aiStat of every fighter in the match and of its sub-character, averages their m_unkC4
// into m_unk37C and counts the distinct m_unkCC values (teams) in m_numTeams.
static inline void aiUpdateStats(aiMgr* mgr, ftManager* ftMgr, int preparing) {
    mgr->m_numTeams = 0;
    int teams[aiMgr::NumPlayers];
    int count = 0;
    for (int i = 0; i < aiMgr::NumPlayers; i++) {
        if (ftMgr->getEntryId(i) >= 0) {
            mgr->m_stat[i].update(preparing);
            (mgr->m_stat + aiMgr::NumPlayers)[i].update(preparing);
            mgr->m_unk37C += mgr->m_stat[i].m_unkC4;
            count++;
            int j;
            for (j = 0; j < mgr->m_numTeams; j++) {
                if (teams[j] == mgr->m_stat[i].m_unkCC) {
                    break;
                }
            }
            if (j >= mgr->m_numTeams) {
                teams[j] = mgr->m_stat[i].m_unkCC;
                mgr->m_numTeams++;
            }
        }
    }
    if (count > 0) {
        mgr->m_unk37C /= count;
    }
}

// Per frame, while a match is running: reads the stage area the CPUs stay inside (lowered for the stages whose floor
// kills: Jungle Japes, Norfair's magma, Brinstar's acid, Summit's water zones), refreshes the weapons, the floor blocks
// and every fighter's aiStat. The pieces are timed into g_aiMgrProcTime.
void aiMgr::processBegin() {
    s64 start = OSGetTime();
    ftManager* ftMgr = g_ftManager;
    Stage* stage;
    if (m_jointLists != NULL && m_enable && (stage = g_Stage) != NULL && (stage->unk40 == 3 || stage->unk40 == 4) &&
        ftMgr->isExistEntry())
    {
        if (stage != NULL) {
            gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
            if (melee != NULL) {
                switch (melee->m_meleeInitData.m_gameRule) {
                    case 1:
                        m_ruleTime = 0;
                        break;
                    case 0:
                    case 2:
                    default:
                        m_ruleTime = stage->getFrameRuleTime();
                        break;
                }
            }
            m_stageKind = stage->m_stageKind;
            CameraController* camera = CameraController::getInstance();
            m_areaLeft = camera->unk158;
            m_areaRight = camera->unk15C;
            m_areaTop = camera->unk160;
            m_areaBottom = camera->unk164;
            switch (m_stageKind) {
                case Stages::JungleJapes:
                    m_areaBottom = -40.0f;
                    break;
                case Stages::Norfair: {
                    Stage* st = g_Stage;
                    if (m_areaBottom < st->getMagmaHeight()) {
                        m_areaBottom = st->getMagmaHeight();
                    }
                    break;
                }
                case Stages::Brinstar: {
                    Stage* st = g_Stage;
                    if (m_areaBottom < st->getAcidHeight()) {
                        m_areaBottom = st->getAcidHeight();
                    }
                    break;
                }
                case Stages::Summit:
                    if (m_numDangerZones) {
                        aiDangerZone* zone = m_dangerZone;
                        bool found = false;
                        for (int i = 0; i < 8; i++, zone++) {
                            if (m_dangerAttr[i]) {
                                if (!found || m_areaBottom > zone->m_maxY - 5.0f) {
                                    m_areaBottom = zone->m_maxY - 5.0f;
                                    found = true;
                                }
                            }
                        }
                    }
                    break;
            }
        } else {
            m_areaLeft = -99999.9f;
            m_areaBottom = -99999.9f;
            m_areaRight = 99999.9f;
            m_areaTop = 99999.9f;
        }
        m_unk374 = -999999;
        m_unk378 = 999999;
        m_unk37C = 0.0f;
        if (m_unk385) {
            m_unk385--;
        }
        m_unk386 = 0;
        m_weaponMgr->prepare();
        g_aiMgrProcTime[0] = OSGetTime() - start;
        start = OSGetTime();
        if (!update_cl_blocks()) {
            lbl_27_data_47BF8 = 0;
            aiUpdateStats(this, ftMgr, true);
        } else {
            g_aiMgrProcTime[1] = OSGetTime() - start;
            s64 t = OSGetTime();
            m_weaponMgr->update();
            m_enemyMgr->update();
            g_aiMgrProcTime[2] = OSGetTime() - t;
            start = OSGetTime();
            m_unkA4.m_idx = -1;
            aiUpdateStats(this, ftMgr, false);
            lbl_27_data_47BF8 = 1;
        }
    }
    m_frame++;
    g_aiMgrProcTime[3] = OSGetTime() - start;
}

// Turns the CPU of one player (and of its sub-character) on or off. On re-enable the controllers re-apply their
// rank; on disable with removeRefer set, the other CPUs drop the player as a target.
// HYPOTHESIS: aiStat flag 0x40000000 marks a character whose CPU is switched off; the third parameter's name follows
// its only effect.
void aiMgr::setEnablePlayer(int player, int enable, int removeRefer) {
    if (player < 0 || player >= NumPlayers) {
        return;
    }
    if (g_aiMgr->m_aiInput[player] != NULL) {
        g_aiMgr->m_aiInput[player]->setEnable(enable);
    }
    // BYTES: indexing the sub-character half from its start folds the slot into the offset (0x68 + player * 4, sharing
    // player * 4 with the line above); m_aiInput[player + NumPlayers] recomputes (player + 7) * 4.
    if ((g_aiMgr->m_aiInput + NumPlayers)[player] != NULL) {
        (g_aiMgr->m_aiInput + NumPlayers)[player]->setEnable(enable);
    }
    if (enable != 0) {
        aiChrIdx idx;
        idx.m_idx = player;
        get_aiStat(&idx)->m_flags &= ~0x40000000;
        aiChrIdx child;
        child.m_idx = player + NumPlayers;
        get_aiStat(&child)->m_flags &= ~0x40000000;
        aiInput* input = g_aiMgr->m_aiInput[player];
        if (input != NULL) {
            aiInput* childInput = input->m_child;
            // BYTES: the rank is loaded as an unsigned byte and sign-extended into the argument (lbz r0; extsb r4, r0),
            // as aiParam::setRank's own (s8) compare does; the plain s8 member loads straight into r4.
            input->m_par.setRank((s8)(u8)input->m_par.m_rank, 1, 1);
            if (childInput != NULL) {
                childInput->m_par.setRank((s8)(u8)childInput->m_par.m_rank, 1, 1);
            }
        }
    } else if (removeRefer != 0) {
        aiChrIdx idx;
        idx.m_idx = player;
        get_aiStat(&idx)->m_flags |= 0x40000000;
        aiChrIdx child;
        child.m_idx = player + NumPlayers;
        get_aiStat(&child)->m_flags |= 0x40000000;
        g_aiMgr->removeTgtRefer(player, 0);
    }
}

void aiMgr::set_ai_player(int player, s8 state, aiInput* input, int ftKind) {
    if (m_aiInput != NULL && player >= 0 && player < NumPlayers) {
        m_chrState[player] = state;
        if (input != NULL) {
            m_aiInput[player] = input;
            m_stat[player].init(player, ftKind, input, 0);
            if (m_cpuTypeLock[player] >= 0) {
                m_aiInput[player]->setCpuType(m_cpuTypeLock[player]);
            }
            if (m_cpuRankLock[player] >= 0) {
                aiInput* cpu = m_aiInput[player];
                cpu->m_par.setRank(m_cpuRankLock[player], 0, (cpu->m_flags >> 15) & 1);
            }
        }
        if (m_aiInput[player] != NULL) {
            if (state != 0 && state != m_chrState[player] && input == NULL) {
                m_aiInput[player]->reset();
            }
            aiInput* child = m_aiInput[player]->m_child;
            if (child != NULL) {
                if (state == 0) {
                    aiSystemPar* systemPar = static_cast<aiSystemPar*>(getAiSystemPar());
                    child->m_par.setRank((s8)systemPar->m_childRank, 0, (child->m_flags >> 15) & 1);
                } else {
                    child->m_par.setRank(m_aiInput[player]->m_par.m_rank, 0, (child->m_flags >> 15) & 1);
                }
                child->setCpuType(0);
            }
            m_aiInput[player]->setEnableRumble(state == 0);
        }
    }
}

void aiMgr::setCpuType(int player, int type) {
    if (m_aiInput != NULL && player >= 0 && player < 7) {
        aiInput* input = m_aiInput[player];
        if (input != NULL) {
            if (m_enable != 1 && m_cpuTypeLock[player] >= 0) {
                return;
            }
            input->setCpuType(type);
        }
    }
}

void aiMgr::setCpuRank(int player, s8 rank) {
    if (m_aiInput != NULL && player >= 0 && player < 7) {
        aiInput* input = m_aiInput[player];
        if (input != NULL) {
            if (m_enable == 1 || m_cpuRankLock[player] < 0) {
                aiInput* child = input->m_child;
                input->m_par.setRank(rank, 0, (input->m_flags >> 15) & 1);
                if (child != NULL) {
                    child->m_par.setRank(rank, 0, (child->m_flags >> 15) & 1);
                }
            }
        }
    }
}

void aiMgr::regist_ai_child(int player, aiInput* input, int chrKind) {
    int index = player + 7;
    m_chrState[index] = 1;
    m_aiInput[index] = input;
    m_stat[index].init(index, chrKind, input, 0);
    if (input != NULL) {
        input->setEnableRumble(false);
    }
}

void aiMgr::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    setEnablePlayer(playerNo, 0, 1);
    m_unk385 = 0xF0;
}

void aiMgr::notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    if (playerNo < 7) {
        if (m_aiInput[playerNo] != NULL) {
            m_aiInput[playerNo]->changeFtKind(kind);
            m_stat[playerNo].change(playerNo, kind, m_aiInput[playerNo], activeInstanceIndex);
        }
    }
}

void aiMgr::notifyEventDead(int entryId, int deadCount, Fighter::Dead::Reason deadReason, int respawnFrames) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    removeTgtRefer(playerNo, 1);
    if (playerNo < 0 || playerNo >= 7) {
        return;
    }
    if (g_aiMgr->m_aiInput[playerNo] != NULL) {
        g_aiMgr->m_aiInput[playerNo]->reset();
    }
    if ((&g_aiMgr->m_aiInput[7])[playerNo] != NULL) {
        (&g_aiMgr->m_aiInput[7])[playerNo]->reset();
    }
}

static inline tag_AI_ATK_DATA_HED* aiFindAtkDataHed(aiMgr* mgr, aiInput** inputs, int ftKind) {
    if (inputs != NULL) {
        for (int i = 0; i < 14; i++) {
            aiInput* input = inputs[i];
            if (input != NULL && (input->m_flags & 0x8000) != 0 && ftKind == input->m_ftKind) {
                return (tag_AI_ATK_DATA_HED*)inputs[i]->m_par.m_atkData;
            }
        }
    }
    return NULL;
}

void* aiMgr::searchAtkData(int ftKind, u32 id) {
    tag_AI_ATK_DATA_HED* hed = aiFindAtkDataHed(this, m_aiInput, ftKind);
    if (hed != NULL) {
        return hed->searchAtkData(id);
    }
    return NULL;
}

bool AiClBlock::chk_fl_danger(int team) {
    if ((m_flags & 0x800) != 0) {
        if (team < 0 || team >= 8 || (m_teamMask & (1 << team)) != 0) {
            return true;
        }
    }
    return false;
}

// Picks a usable block other than exclude at random. The hazard blocks (0x1000) only count for a team-less CPU and the
// blocks dangerous to the team get a small weight. HYPOTHESIS: the name; get_via_blk_team falls back to it when it has
// no goal block.
AiClBlock* aiMgr::get_random_block(AiClBlock* exclude, int team) {
    AiClBlock* block = m_blocks;
    float total = 0.0f;
    for (int i = 0; i < m_numBlocks; i++, block++) {
        if ((block->m_flags & 2) && block != exclude) {
            if (block->m_flags & 0x1000) {
                if (team < 0) {
                    total += 0.0001f;
                }
            } else if (block->chk_fl_danger(team)) {
                total += 0.01f;
            } else {
                total += 1.0f;
            }
        }
    }
    total *= randf();
    block = m_blocks;
    for (int i = 0; i < m_numBlocks - 1; i++, block++) {
        if ((block->m_flags & 2) && block != exclude) {
            if (block->m_flags & 0x1000) {
                if (team < 0) {
                    total -= 0.0001f;
                }
            } else if (block->chk_fl_danger(team)) {
                total -= 0.01f;
            } else {
                total -= 1.0f;
            }
            if (total <= 0.0f) {
                break;
            }
        }
    }
    return block;
}

// HYPOTHESIS: an inline of the original: the difference vector is a temporary of the call, not a local of the caller.
static inline float aiDistance(Vec2f* from, Vec2f* to) {
    Vec2f disp;
    from->sub(to, &disp);
    return disp.length();
}

// BYTES: the original has this function before get_random_block; it follows it here so that the literal pool
// keeps get_random_block's offsets while the unit is incomplete.
// For every block: the block itself (NULL when it is not usable), the end of the block nearest to the centre of goal and
// its distance (goal itself gets its centre and 0). HYPOTHESIS: the name; get_via_blk_team and the block route search
// call it right after picking their goal block.
void aiMgr::get_blk_goal_dist(AiClBlock** blocks, Vec2f* points, float* dists, AiClBlock* goal) {
    AiClBlock* block = m_blocks;
    Vec2f center = (goal->m_edgeL + goal->m_edgeR) * 0.5f;
    for (int i = 0; i < m_numBlocks; i++, block++) {
        if (!(block->m_flags & 2)) {
            blocks[i] = NULL;
            continue;
        }
        blocks[i] = block;
        if (block == goal) {
            points[i].m_x = 0.5f * (block->m_edgeL.m_x + block->m_edgeR.m_x);
            points[i].m_y = 0.5f * (block->m_edgeL.m_y + block->m_edgeR.m_y);
            dists[i] = 0.0f;
            continue;
        }
        float distL = aiDistance(&block->m_edgeL, &center);
        float distR = aiDistance(&block->m_edgeR, &center);
        if (distL < distR) {
            points[i] = block->m_edgeL;
            dists[i] = distL;
        } else {
            points[i] = block->m_edgeR;
            dists[i] = distR;
        }
    }
}

AiClBlock* aiMgr::get_top_block(int team) {
    int count = m_numBlocks;
    AiClBlock* block = m_blocks;
    float best = -99999.9f;
    AiClBlock* result = NULL;
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        float value = block->m_top;
        if ((flags & 2) != 0 && !(value > 100.0f)) {
            if ((flags & 0x10000) != 0) {
                value -= 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                value -= 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) != 0 && (team < 0 || team >= 8 || (block->m_teamMask & (1 << team)) != 0)) {
                    danger = true;
                } else {
                    danger = false;
                }
                if (danger) {
                    value -= 1000.0f;
                }
            }
            if (best < value) {
                best = value;
                result = block;
            }
        }
    }
    return result;
}

AiClBlock* aiMgr::get_right_block(int team) {
    int count = m_numBlocks;
    AiClBlock* block = m_blocks;
    float best = -99999.9f;
    AiClBlock* result = NULL;
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        float value = block->m_edgeL.m_x;
        if ((flags & 2) != 0 && !(value > 100.0f)) {
            if ((flags & 0x10000) != 0) {
                value -= 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                value -= 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) != 0 && (team < 0 || team >= 8 || (block->m_teamMask & (1 << team)) != 0)) {
                    danger = true;
                } else {
                    danger = false;
                }
                if (danger) {
                    value -= 1000.0f;
                }
            }
            if (best < value) {
                best = value;
                result = block;
            }
        }
    }
    return result;
}

AiClBlock* aiMgr::get_center_block(int team) {
    AiClBlock* block = m_blocks;
    AiClBlock* result = NULL;
    float best = 9999979520.0f;
    int count = m_numBlocks;
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        if ((flags & 2) != 0) {
            Vec2f center = (block->m_edgeL + block->m_edgeR) * 0.5f;
            float centerY = center.m_y - 20.0f;
            float score = center.m_x * center.m_x + centerY * centerY;
            if (block->m_edgeL.m_x > 0.0f || block->m_edgeR.m_x < 0.0f) {
                score += 100.0f;
            }
            if (block->m_top < m_areaBottom + 40.0f
                || m_areaTop - 40.0f < block->m_bottom) {
                score += 10000.0f;
            }
            if ((flags & 0x10000) != 0) {
                score += 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                score += 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) != 0 && (team < 0 || team >= 8 || (block->m_teamMask & (1 << team)) != 0)) {
                    danger = true;
                } else {
                    danger = false;
                }
                if (danger) {
                    score += 1000.0f;
                }
            }
            if (best > score) {
                best = score;
                result = block;
            }
        }
    }
    return result;
}

bool aiMgr::search_ladder(AiClBlock* block, AiClBlock* target, float width, float height) {
    if (m_stageKind != Stages::Edit) {
        return false;
    }
    chkAreaProc proc;
    proc.m_block = block;
    proc.m_target = target;
    proc.m_numAreas = 0;
    proc.m_width = 1.1f * width;
    proc.m_height = 0.9f * height;
    lbl_8059FF90->forEachArea(&proc, gfArea::Stage_Group_Gimmick_Ladder);
    int num = proc.m_numAreas;
    if (num > 0) {
        int idx;
        if (num == 1) {
            idx = 0;
        } else {
            idx = num * randf();
            int last = num - 1;
            if (idx > last) {
                idx = last;
            }
        }
        g_aiLadderArea = proc.m_areas[idx];
        return true;
    }
    return false;
}

bool aiMgr::chkUnderFloor(float slope, Vec2f* pos) {
    int count = m_numBlocks;
    AiClBlock* block = m_blocks;
    for (int i = 0; i < count; i++, block++) {
        if ((block->m_flags & 2) != 0 && !(block->m_top > pos->m_y)) {
            if (block->m_edgeL.m_x > pos->m_x) {
                if (slope > 0.01f) {
                    if (pos->m_y - block->m_edgeL.m_y > (block->m_edgeL.m_x - pos->m_x) / slope) {
                        return true;
                    }
                } else if (pos->m_y > block->m_edgeL.m_y) {
                    return true;
                }
            } else {
                if (block->m_edgeR.m_x >= pos->m_x) {
                    return true;
                }
                if (slope > 0.01f) {
                    if (pos->m_y - block->m_edgeR.m_y > (pos->m_x - block->m_edgeR.m_x) / slope) {
                        return true;
                    }
                } else if (pos->m_y > block->m_edgeR.m_y) {
                    return true;
                }
            }
        }
    }
    return false;
}

// The next line of a joint walk: left from m_line to m_leftLine, then right from m_line to m_rightLine, then NULL.
// HYPOTHESIS: an inline of the original (every branch ends in the same assignment).
static inline grCollisionLine* aiNextJointLine(tag_AiClJointList* list, grCollision* coll, grCollisionLine* l, bool& right) {
    if (!right) {
        if (l == list->m_leftLine) {
            if (list->m_line == list->m_rightLine) {
                return NULL;
            }
            grCollisionLine* next = coll->getRightLine(list->m_line);
            right = true;
            return next;
        }
        return coll->getLeftLine(l);
    }
    if (l == list->m_rightLine) {
        return NULL;
    }
    return coll->getRightLine(l);
}

AiClBlock* aiMgr::search_blk(grCollision* collision, grCollisionJoint* joint, grCollisionLine* line) {
    AiClBlock* block = m_blocks;
    for (int i = 0; i < m_numBlocks; i++, block++) {
        for (tag_AiClJointList* list = block->m_joint; list != NULL; list = list->m_next) {
            if (list->m_collision.isValid()) {
                grCollision* coll = list->m_collision.getCollision();
                if (coll == collision && list->m_collJoint == joint) {
                    bool right = false;
                    for (grCollisionLine* l = list->m_line; l != NULL;) {
                        if (l == line) {
                            return block;
                        }
                        l = aiNextJointLine(list, coll, l, right);
                    }
                }
            }
        }
    }
    return NULL;
}

AiClBlock* aiMgr::search_blk1(u32* key) {
    if (key[0] != 0) {
        AiClBlock* block = m_blocks;
        for (int count = m_numBlocks; count > 0; count--) {
            bool match;
            if (block->m_joint == NULL) {
                match = false;
            } else {
                match = false;
                if (key[1] == (u32)block->m_unk48 && key[0] == block->m_joint->m_id) {
                    match = true;
                }
            }
            if (match == true) {
                return block;
            }
            block++;
        }
    }
    return NULL;
}

aiStat* aiMgr::get_aiStatFinal() {
    int i = 0;
    do {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* stat = get_aiStat(&idx);
        if ((stat->m_flags & 0x8000) == 0) {
            if ((stat->m_flags & 0xC00) != 0 || stat->chk_chr_stat(2, 0x12)) {
                return stat;
            }
        }
        i++;
    } while (i < 7);
    return NULL;
}

void aiMgr::removePlayer(aiChrIdx* idx) {
    if (aiChrIdx::IsPlayer(idx)) {
        removeTgtRefer(aiChrIdx::GetPlayerNo(idx), 0);
        int slot = idx->m_idx;
        m_chrState[slot] = -1;
        aiInput* input = m_aiInput[slot];
        if (input != NULL) {
            input->setEnableRumble(false);
        }
        m_aiInput[idx->m_idx] = NULL;
        get_aiStat(idx)->init(idx->m_idx, -1, NULL, 0);
    }
}

AiClBlock* aiMgr::get_via_blk(AiClBlock* from, int goal, float x, Vec2f* points, int numPoints, int bestOut, float f1, float f2, float distWeight) {
    return get_via_blk_team(from, goal, x, -1, points, numPoints, bestOut, f1, f2, distWeight);
}

// The first block to walk to from `from` towards goal (a random usable block when goal is 0): every block that
// chk_goto_blk accepts is scored by its distance to goal plus distWeight times its distances to the given points and
// the penalties for the hazard, dangerous and pass-through blocks. A reachable block next to goal is taken at once.
// Without bestOut only reachable blocks are kept; with it the lowest-scored block of all is also written to bestOut.
// Returns from when nothing better is found or the tables cannot be allocated.
// HYPOTHESIS: goal and bestOut are ints in the original's signature (the map's mangled name); the callers pass a
// block and the address of an AiClBlock* in them.
AiClBlock* aiMgr::get_via_blk_team(AiClBlock* from, int goal, float x, int team, Vec2f* points, int numPoints, int bestOut, float f1, float f2, float distWeight) {
    AiClBlock** block;
    float* dist;
    Vec2f* goalPoint;
    int i;
    bool reachable;
    AiClBlock** blocks = (AiClBlock**)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(AiClBlock*));
    float* dists = (float*)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(float));
    Vec2f* goalPoints = (Vec2f*)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(Vec2f));
    if (blocks == NULL || dists == NULL || goalPoints == NULL) {
        if (blocks != NULL) {
            gfHeapManager::free(blocks);
        }
        if (dists != NULL) {
            gfHeapManager::free(dists);
        }
        if (goalPoints != NULL) {
            gfHeapManager::free(goalPoints);
        }
        return from;
    }
    if (goal == 0) {
        goal = (int)get_random_block(from, team);
    }
    get_blk_goal_dist(blocks, goalPoints, dists, (AiClBlock*)goal);
    if (bestOut == 0) {
        block = blocks;
        dist = dists;
        for (i = 0; i < m_numBlocks; i++, block++, dist++) {
            reachable = false;
            if (*block == NULL) {
                continue;
            }
            if (*block != from) {
                reachable = chk_goto_blk(from, *block, team, x, f1, f2);
            }
            if (!reachable) {
                *block = NULL;
                continue;
            }
            if (*dist < 0.1f && distWeight >= 0.0f) {
                AiClBlock* via = *block;
                if (blocks != NULL) {
                    gfHeapManager::free(blocks);
                }
                if (dists != NULL) {
                    gfHeapManager::free(dists);
                }
                if (goalPoints != NULL) {
                    gfHeapManager::free(goalPoints);
                }
                return via;
            }
        }
        float best = 10000000.0f;
        block = blocks;
        dist = dists;
        goalPoint = goalPoints;
        for (i = 0; i < m_numBlocks; i++, block++, dist++, goalPoint++) {
            if (*block == NULL) {
                continue;
            }
            float score = *dist;
            if (points != NULL) {
                Vec2f* point = points;
                for (int j = 0; j < numPoints; point++, j++) {
                    score += distWeight * aiDistance(point, goalPoint);
                }
            }
            if ((*block)->m_flags & 0x10000) {
                score += 10000000.0f;
            } else if ((*block)->m_flags & 0x1000) {
                score += 100000.0f;
            } else if ((*block)->chk_fl_danger(team)) {
                score += 1000.0f;
            }
            if ((*block)->m_flags & 0x20) {
                score += 50.0f;
            }
            if (best > score) {
                best = score;
                from = *block;
            }
        }
        if (blocks != NULL) {
            gfHeapManager::free(blocks);
        }
        if (dists != NULL) {
            gfHeapManager::free(dists);
        }
        if (goalPoints != NULL) {
            gfHeapManager::free(goalPoints);
        }
        return from;
    }
    AiClBlock* via = from;
    AiClBlock* bestBlock = from;
    float best = 10000000.0f;
    float bestScore = 10000000.0f;
    block = blocks;
    dist = dists;
    goalPoint = goalPoints;
    for (i = 0; i < m_numBlocks; i++, block++, dist++, goalPoint++) {
        reachable = false;
        if (*block == NULL) {
            continue;
        }
        float score = *dist;
        if (*block != from) {
            reachable = chk_goto_blk(from, *block, team, x, f1, f2);
        }
        if (reachable && score < 0.1f && distWeight >= 0.0f) {
            via = *block;
            break;
        }
        if (points != NULL) {
            Vec2f* point = points;
            for (int j = 0; j < numPoints; point++, j++) {
                score += distWeight * aiDistance(point, goalPoint);
            }
        }
        if ((*block)->m_flags & 0x20) {
            score += 50.0f;
        }
        if ((*block)->m_flags & 0x10000) {
            score += 10000000.0f;
        } else if ((*block)->m_flags & 0x1000) {
            score += 100000.0f;
        } else if ((*block)->chk_fl_danger(team)) {
            score += 1000.0f;
        }
        if (reachable && best > score) {
            best = score;
            via = *block;
        }
        if (bestScore > score) {
            bestScore = score;
            bestBlock = *block;
        }
    }
    *(AiClBlock**)bestOut = bestBlock;
    if (blocks != NULL) {
        gfHeapManager::free(blocks);
    }
    if (dists != NULL) {
        gfHeapManager::free(dists);
    }
    if (goalPoints != NULL) {
        gfHeapManager::free(goalPoints);
    }
    return via;
}

// The first block on the route from `from` towards goal (a random usable block when goal is NULL): the blocks are
// tried in order of their distance to goal, plus the distance from pos weighted by distWeight and penalties for the
// hazard, dangerous and pass-through blocks. Returns from when the tables cannot be allocated.
// HYPOTHESIS: the parameter names; f1 and f2 are passed on to get_via_blk2_core after pos's x.
AiClBlock* aiMgr::get_via_blk2_team(AiClBlock* from, Vec2f* pos, AiClBlock* goal, int team, float f1, float f2, float distWeight) {
    AiClBlock** blocks = (AiClBlock**)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(AiClBlock*));
    float* dists = (float*)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(float));
    Vec2f* points = (Vec2f*)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(Vec2f));
    if (blocks == NULL || dists == NULL || points == NULL) {
        if (blocks != NULL) {
            gfHeapManager::free(blocks);
        }
        if (dists != NULL) {
            gfHeapManager::free(dists);
        }
        if (points != NULL) {
            gfHeapManager::free(points);
        }
        return from;
    }
    if (goal == NULL) {
        goal = get_random_block(from, team);
    }
    get_blk_goal_dist(blocks, points, dists, goal);
    AiClBlock** block = blocks;
    float* dist = dists;
    Vec2f* point = points;
    int i;
    for (i = 0; i < m_numBlocks; i++, block++, dist++, point++) {
        if (*block == NULL) {
            continue;
        }
        if (*block == goal) {
            *block = NULL;
            continue;
        }
        Vec2f p = *pos;
        *dist += distWeight * aiDistance(&p, point);
        if ((*block)->m_flags & 0x10000) {
            *dist += 10000000.0f;
        } else if ((*block)->m_flags & 0x1000) {
            *dist += 100000.0f;
        } else if ((*block)->chk_fl_danger(team)) {
            *dist += 1000.0f;
        }
        if ((*block)->m_flags & 0x20) {
            *dist += 50.0f;
        }
    }
    block = blocks;
    dist = dists;
    int j;
    for (i = 0; i < m_numBlocks - 1; i++, block++, dist++) {
        AiClBlock** other = &blocks[i + 1];
        float* otherDist = &dists[i + 1];
        for (j = i + 1; j < m_numBlocks; j++, other++, otherDist++) {
            if (*other != NULL && (*block == NULL || *dist > *otherDist)) {
                AiClBlock* swap = *block;
                float swapDist = *dist;
                *block = *other;
                *other = swap;
                *dist = *otherDist;
                *otherDist = swapDist;
            }
        }
    }
    AiClBlock* via = get_via_blk2_core(from, goal, blocks, team, pos->m_x, f1, f2);
    if (blocks != NULL) {
        gfHeapManager::free(blocks);
    }
    if (dists != NULL) {
        gfHeapManager::free(dists);
    }
    if (points != NULL) {
        gfHeapManager::free(points);
    }
    return via;
}

// One step of the route search between floor blocks: every block still in the table that can be reached from
// `from` is taken out of it and searched from in turn. Returns the block to go to first, or goal when there is no
// route (the caller compares the result with goal).
AiClBlock* aiMgr::get_via_blk2_core(AiClBlock* goal, AiClBlock* from, AiClBlock** blocks, int team, float f1, float f2, float f3) {
    AiClBlock** next;
    int numReached = 0;
    int i;
    AiClBlock** reached = (AiClBlock**)gfHeapManager::alloc(Heaps::Tmp, m_numBlocks * sizeof(AiClBlock*));
    if (reached == NULL) {
        return goal;
    }
    next = reached;
    for (i = 0; i < m_numBlocks; i++) {
        if (blocks[i] != NULL && chk_goto_blk(blocks[i], from, team, f1, f2, f3)) {
            if (blocks[i] == goal) {
                if (reached != NULL) {
                    gfHeapManager::free(reached);
                }
                return from;
            }
            *next++ = blocks[i];
            numReached++;
            blocks[i] = NULL;
        }
    }
    if (numReached == 0) {
        if (reached != NULL) {
            gfHeapManager::free(reached);
        }
        return goal;
    }
    for (i = 0; i < numReached; i++) {
        AiClBlock* via = get_via_blk2_core(goal, reached[i], blocks, team, f1, f2, f3);
        if (via != goal) {
            if (reached != NULL) {
                gfHeapManager::free(reached);
            }
            return via;
        }
    }
    if (reached != NULL) {
        gfHeapManager::free(reached);
    }
    return goal;
}

bool aiMgr::chk_cl_cross(Vec2f* from, Vec2f* to, u32 mask, Vec2f* hitPos, AiClBlock* ignore) {
    return chk_cl_cross2(from, to, mask, hitPos, ignore) != NULL;
}

// Whether the segments a-b and c-d cross; the crossing point goes to cross when given.
// HYPOTHESIS: the name; a free function of the unit (the callers pass the points in r3-r6, no aiMgr).
bool aiChkSegmentCross(const Vec2f* a, const Vec2f* b, const Vec2f* c, const Vec2f* d, Vec2f* cross) {
    float det = (c->m_x - d->m_x) * (a->m_y - b->m_y) - (a->m_x - b->m_x) * (c->m_y - d->m_y);
    float num = (a->m_x - b->m_x) * (d->m_y - b->m_y) - (d->m_x - b->m_x) * (a->m_y - b->m_y);
    if (__fabsf(det) < 0.00001f) {
        return false;
    }
    float t = num / det;
    if (t < 0.0f || t > 1.0f) {
        return false;
    }
    float s;
    if (a->m_x != b->m_x) {
        s = (t * (c->m_x - d->m_x) + (d->m_x - b->m_x)) / (a->m_x - b->m_x);
    } else {
        s = (t * (c->m_y - d->m_y) + (d->m_y - b->m_y)) / (a->m_y - b->m_y);
    }
    if (s < 0.0f || s > 1.0f) {
        return false;
    }
    if (cross != NULL) {
        cross->m_x = a->m_x * s + b->m_x * (1.0f - s);
        cross->m_y = a->m_y * s + b->m_y * (1.0f - s);
    }
    return true;
}


// The block whose lines the segment from -> to crosses (the crossing nearest to the origin goes to hitPos when given;
// without hitPos the first block found is returned). mask selects the directions that are tested: 2 down, 4 up, 8 left,
// 0x10 right; a vertical movement also tests the side lines and a horizontal one the ceilings.
AiClBlock* aiMgr::chk_cl_cross2(Vec2f* from, Vec2f* to, u32 mask, Vec2f* hitPos, AiClBlock* ignore) {
    Vec2f dir = *to - *from;
    AiClBlock* block = m_blocks;
    float bestDist = 9999980000.0f;
    u8 dirs = 0;
    AiClBlock* best = NULL;
    if (dir.m_y < 0.0f) {
        dirs |= 2;
    }
    if (dir.m_y > 0.0f) {
        dirs |= 4;
    }
    if (dir.m_x < 0.0f) {
        dirs |= 8;
    }
    if (dir.m_x > 0.0f) {
        dirs |= 0x10;
    }
    dirs &= mask;
    if ((dirs & 2) || (dirs & 4)) {
        dirs |= 0x18;
    } else if (dirs & 0x18) {
        dirs |= 4;
    }
    Vec2f min;
    Vec2f max;
    if (from->m_x < to->m_x) {
        min.m_x = from->m_x;
        max.m_x = to->m_x;
    } else {
        min.m_x = to->m_x;
        max.m_x = from->m_x;
    }
    if (from->m_y < to->m_y) {
        min.m_y = from->m_y;
        max.m_y = to->m_y;
    } else {
        min.m_y = to->m_y;
        max.m_y = from->m_y;
    }
    u32 down = dirs & 2;
    u32 up = dirs & 4;
    u32 left = dirs & 8;
    u32 right = dirs & 0x10;
    for (int i = 0; i < m_numBlocks; i++, block++) {
        if (block == ignore || (block->m_flags & dirs) == 0) {
            continue;
        }
        tag_AiClJointList* list = block->m_joint;
        if (max.m_x < block->m_box.m_unk1 || min.m_x > block->m_box.m_unk3 || max.m_y < block->m_box.m_unk2 ||
            min.m_y > block->m_box.m_unk4)
        {
            continue;
        }
        for (; list != NULL; list = list->m_next) {
            grCollisionLine* l = list->m_line;
            bool walkRight = false;
            if (list->m_collision.isValid()) {
                grCollision* coll = list->m_collision.getCollision();
                for (; l != NULL; l = aiNextJointLine(list, coll, l, walkRight)) {
                    if ((down && l->m_isUnder) || (up && l->m_isUpper && !l->m_isThroughable) || (left && l->m_isLeft) ||
                        (right && l->m_isRight))
                    {
                        grCollSegmentEx seg;
                        Vec2f cross;
                        coll->getSegment(&seg, l, 0);
                        if (aiChkSegmentCross(from, to, &seg.m_segment.m_p0Pos, &seg.m_segment.m_p1Pos, &cross) == true) {
                            if (hitPos == NULL) {
                                return block;
                            }
                            float dist = cross.m_x * cross.m_x + cross.m_y * cross.m_y;
                            if (bestDist > dist) {
                                hitPos->m_x = cross.m_x;
                                bestDist = dist;
                                best = block;
                                hitPos->m_y = cross.m_y;
                            }
                        }
                    }
                }
            }
        }
    }
    return best;
}
// Whether the input of the character in this slot still comes from its controller: true until the AI has its floor
// information, and for every slot that the AI has not registered.
bool aiMgr::chkIpHumanUpdate(aiChrIdx* idx) {
    if (aiChrIdx::IsPlayer(idx) && aiChrIdx::GetFighterNo(idx) == 0) {
        int player = aiChrIdx::GetPlayerNo(idx);
        if (m_jointLists == NULL || m_chrState[player] <= 0) {
            return true;
        }
    }
    return false;
}

bool aiMgr::chkChildOptionMd(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        aiInput* child = m_aiInput[player]->m_child;
        if (child != NULL && child->m_md == 0x79) {
            if ((get_aiStat(&child->m_chrIdx)->m_flags & 0x8000) == 0) {
                return true;
            }
        }
    }
    return false;
}

void aiMgr::renderDebug() {
}

void aiMgr::setAdventureMd(u32 md) {
    u32 current = g_aiMgr->m_adventureMd;
    if (current == md) {
        return;
    }
    g_aiMgr->m_adventureMd = md;
    for (int i = 0; i < 14; i++) {
        aiInput* input = g_aiMgr->m_aiInput[i];
        if (input != NULL) {
            input->m_unk112 = g_aiMgr->m_adventureMd;
        }
    }
}

// Keeps a ladder that touches both blocks and from which the near edge of the target can be reached without
// crossing other collisions.
bool chkAreaProc::exec(gfArea* area) {
    if (m_numAreas < 16) {
        clAABBox2D* box = &area->m_aabBox;
        clAABBox2D targetBox;
        aiSetBox(&targetBox, Vec2f(m_target->m_box.m_unk1 - m_width, m_target->m_box.m_unk2 - m_height),
                 Vec2f(m_target->m_box.m_unk3 + m_width, m_target->m_box.m_unk4 + m_height));
        if (box->checkIntersect1(targetBox)) {
            clAABBox2D blockBox;
            aiSetBox(&blockBox, Vec2f(m_block->m_box.m_unk1 - m_width, m_block->m_box.m_unk2 - m_height),
                     Vec2f(m_block->m_box.m_unk3 + m_width, m_block->m_box.m_unk4 + m_height));
            if (box->checkIntersect1(blockBox)) {
                Vec2f to;
                Vec2f from;
                from.m_x = 0.5f * (box->m_unk1 + box->m_unk3);
                if (from.m_x > 0.5f * (m_target->m_edgeL.m_x + m_target->m_edgeR.m_x)) {
                    to = m_target->m_edgeR;
                    to.m_x += 1.0f;
                } else {
                    to = m_target->m_edgeL;
                    to.m_x -= 1.0f;
                }
                to.m_y += 3.0f;
                if (to.m_y < box->m_unk2) {
                    from.m_y = box->m_unk2;
                } else if (to.m_y > box->m_unk4) {
                    from.m_y = box->m_unk4;
                } else {
                    from.m_y = to.m_y;
                }
                // A ladder whose straight path is blocked still counts when the target edge is close enough to be
                // reached with one step across and one step up.
                if (!aiChkCross(from, to, m_block) ||
                    (__fabsf(to.m_x - from.m_x) < 0.5f * m_width && to.m_y - from.m_y < 0.8f * m_height &&
                     !aiChkCross(from, Vec2f(to.m_x, from.m_y), m_block) &&
                     !aiChkCross(Vec2f(to.m_x, from.m_y), to, m_block)))
                {
                    m_areas[m_numAreas] = area;
                    m_numAreas++;
                }
            }
        }
    }
    return true;
}
