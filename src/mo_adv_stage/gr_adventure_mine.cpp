#include <mt/mt_matrix.h>
#include <sora_adv_stage/gr_adventure_mine.h>

extern "C" const char lbl_40_data_6380[];

grAdventureMine* grAdventureMine::create(int mdlIndex, const char* taskName) {
    grAdventureMine* mine = new (Heaps::StageInstance) grAdventureMine(taskName);
    if (mine != NULL) {
        mine->setMdlIndex(mdlIndex);
    }
    return mine;
}

void grAdventureMine::processUpdate() {
    if (m_isUpdate) {
        grGimmick::processUpdate();
    }
}

grAdventureMine::~grAdventureMine() { }

void grAdventureMine::update(float deltaFrame) {
    grGimmick::updateCallback(0);
    m_isUpdate = false;
}

void grAdventureMine::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmickBlock::startup(data, unk1 | 0x108, layerType);
    setCalcuCallbackRoot(8);
    setSoundEffect(0, 0x2279, Vec2f(0.0f, 0.0f));
    setEffect(0, 0x1030022, lbl_40_data_6380, Vec2f(0.0f, 0.0f), 1.0f);
    grGimmick::changeNodeAnim(0, 0);
}

void grAdventureMine::callBreak() {
    Matrix matrix(true);
    getMatrix(&matrix);
    Vec3f pos = matrix.getTranslation();
    startGimmickSE(0);
    startGimmickEffect(0);
}

// TODO: clean up, give variables real names
namespace jj_2EC24 {
struct Color {
    unsigned char r, g, b, a;
};

extern "C" {
extern unsigned char lbl_40_data_BE30[];
extern Color lbl_40_rodata_560;
extern Color lbl_40_rodata_564;

void setMaterialTevColor__9grGimmickFPUlPCcPUlPUl(void*, void*, void*, Color*, void*);
void setMaterialColor__9grGimmickFPUlPCcPUlPUl(void*, void*, void*, Color*, void*);

void fn_40_2EC24(void* obj) {
    unsigned char* self = (unsigned char*)obj;
    unsigned char* data = lbl_40_data_BE30;

    if (self[0x16c] == 1) {
        Color a = lbl_40_rodata_560;
        Color b = lbl_40_rodata_564;

        setMaterialTevColor__9grGimmickFPUlPCcPUlPUl(obj, data + 0x28, data + 0x10, &a, data + 0x2c);
        setMaterialColor__9grGimmickFPUlPCcPUlPUl(obj, data + 0x30, data + 0x1c, &b, data + 0x34);

        self[0x16c] = 0;
    }
}
}
}

