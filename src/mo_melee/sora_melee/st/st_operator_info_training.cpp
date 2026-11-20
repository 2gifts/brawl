#include <st/operator/st_operator_info.h>
#include <types.h>

// The training-mode info operator (sora_melee 0x2575E4..0x2585D8; processBegin is not decompiled yet).
class stOperatorInfoTraining : public stOperatorInfo {
public:
    bool m_isEnableOperation; // 0x11B
    bool m_isStopRequest;     // 0x11C
    bool m_isEnd;             // 0x11D HYPOTHESIS: getEndType answers 3 when set
    char _11e[0xA];
    bool m_128;               // 0x128
    char _129[0xF];
    s32 m_138;                // 0x138
    u8 m_13c;                 // 0x13C
    u32 m_140;                // 0x140
    u32 m_144;                // 0x144

    stOperatorInfoTraining();
    virtual void processBegin();
    virtual ~stOperatorInfoTraining();
    virtual void startOperator();
    virtual void stopOperatorRequest();
    virtual bool isEnableOperation();
    virtual u32 getEndType();
    virtual u32 getErrorLevel();
};

// TODO: clean up - the task name string and a data word of the unit, referenced by their labels until the unit's data is
// split.
extern "C" {
extern u32 lbl_27_data_532B8[1];
extern const char lbl_27_data_532BC[];
}

// TODO: clean up - MATCH-ONLY: the original inlines the (empty) stOperatorInfo destructor into the derived one, which
// then calls ~stOperator directly; the header declares it out of line, so this unit redeclares it inline.
inline stOperatorInfo::~stOperatorInfo() { }

stOperatorInfoTraining::~stOperatorInfoTraining() { }

stOperatorInfoTraining::stOperatorInfoTraining() : stOperatorInfo(lbl_27_data_532BC) {
    m_isEnableOperation = false;
    m_isStopRequest = false;
    m_isEnd = false;
    m_128 = false;
    m_138 = 1;
    m_13c = 0xff;
    m_140 = lbl_27_data_532B8[0];
    m_144 = 0;
}

void stOperatorInfoTraining::startOperator() {
    m_isEnableOperation = true;
    m_140 = lbl_27_data_532B8[0];
    m_144 = 0;
}

bool stOperatorInfoTraining::isEnableOperation() {
    return m_isEnableOperation;
}

u32 stOperatorInfoTraining::getEndType() {
    if (m_isEnd) {
        return 3;
    }
    return 1;
}

u32 stOperatorInfoTraining::getErrorLevel() {
    return 0;
}

void stOperatorInfoTraining::stopOperatorRequest() {
    m_isStopRequest = true;
}

// TODO: clean up - placeholder pair type and names (the same pair of statics as so_sinit_pair_*.cpp); replace them with
// the real static object type and names once the header that declares them is identified.
struct SinitPair {
    s32 m_a;
    s32 m_b;
    SinitPair(s32 a, s32 b) : m_a(a), m_b(b) {}
};

SinitPair lbl_27_bss_59E8(0xFF, 0);
SinitPair lbl_27_bss_59F0(0xFF, 1);
