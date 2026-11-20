#include <gf/gf_scene.h>
#include <sc/sc_melee.h>
#include <sr/sr_common.h>
#include <types.h>

// scMelee helpers of the scene module that have no names yet.
// TODO: clean up - name them as scMelee members.
extern "C" void fn_1_139E0(scMelee* scene);
extern "C" void fn_1_194A4(scMelee* scene, int unk);
extern "C" void fn_1_19968(scMelee* scene, int unk);

// The result of the fight as stOperatorRuleVsResult keeps it (sora_melee).
// HYPOTHESIS: the names; only the two bytes this scene copies are known.
struct scVsResultData {
    u8 m_unk0[0x668];
    u8 m_unk668;                        // 0x668 written from the uploader operator
    u8 m_unk669;                        // 0x669 the scene's end number depends on it
};

// What the scene reads of the result rule operator and the uploader operator.
// TODO: clean up - use the real stOperatorRuleVsResult / stOperatorUploader classes.
struct scVsResultRule {
    u8 m_unk0[0xCC];
    scVsResultData* m_result;           // 0xCC
};
struct scVsResultUploader {
    u8 m_unk0[0x50];
    u8 m_unk50;                         // 0x50
};

// A static pair of the module that its initializer function fills in (the same shape as the one of scButton).
// HYPOTHESIS: the names; the pairs are not read in this unit.
struct scVsResultEntry {
    int m_id;
    int m_shown;
    scVsResultEntry(int id, int shown) : m_id(id), m_shown(shown) { }
};
static scVsResultEntry s_entryHidden(0xFF, 0);
static scVsResultEntry s_entryShown(0xFF, 1);

// The scene of the result screen of a versus fight (module sora_scene): a melee scene whose stage shows the fighters
// with the result rule operator.
class scVsResult : public scMelee {
public:
    scVsResultData* m_result;           // 0x100
    u8 m_unk104;                        // 0x104 HYPOTHESIS: the next scene is the character select (end number 1, else 3)

    scVsResult() : scMelee("scVsResult"), m_result(NULL), m_unk104(1) { fn_1_139E0(this); }
    static scVsResult* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual ~scVsResult();
    virtual int getEndNum();
};
static_assert(sizeof(scVsResult) == 0x108, "Class is wrong size!");

scVsResult* scVsResult::create() {
    return new (Heaps::GlobalMode) scVsResult();
}

s32 scVsResult::process() {
    s32 result = scMelee::process();
    scVsResultRule* rule = reinterpret_cast<scVsResultRule*>(m_operatorRuleGameMode);
    if (rule != NULL) {
        m_result = rule->m_result;
        if (m_result != NULL) {
            scVsResultUploader* uploader = reinterpret_cast<scVsResultUploader*>(m_operatorUploader);
            if (uploader != NULL) {
                m_result->m_unk668 = uploader->m_unk50;
            }
            m_unk104 = m_result->m_unk669;
        }
    }
    return result;
}

s32 scVsResult::exit() {
    m_result = NULL;
    fn_1_194A4(this, 1);
    fn_1_19968(this, 1);
    return 0;
}

int scVsResult::getEndNum() {
    return m_unk104 ? 1 : 3;
}

scVsResult::~scVsResult() { }
