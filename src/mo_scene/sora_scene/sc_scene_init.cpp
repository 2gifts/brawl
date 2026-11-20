#include <gf/gf_scene.h>
#include <sc/sc_scene_init.h>
#include <types.h>

// Most scene classes have no header yet; only their static create() is needed here.
#define SC_DECLARE_SCENE(name)                                                 \
    class name {                                                               \
    public:                                                                    \
        static gfScene* create();                                              \
    }

SC_DECLARE_SCENE(scMemoryChange);
SC_DECLARE_SCENE(scEnding);
SC_DECLARE_SCENE(scStaffRoll);
SC_DECLARE_SCENE(muMenuMain);
SC_DECLARE_SCENE(scTyFigList);
SC_DECLARE_SCENE(scTyFigDisp);
SC_DECLARE_SCENE(scTyFigDisp2);
SC_DECLARE_SCENE(scSealList);
SC_DECLARE_SCENE(scSealDisp);
SC_DECLARE_SCENE(scClearGetter);
SC_DECLARE_SCENE(scButton);
SC_DECLARE_SCENE(scSoftList);
SC_DECLARE_SCENE(scDemo);
SC_DECLARE_SCENE(scTutorial);
SC_DECLARE_SCENE(scVCTrial);
SC_DECLARE_SCENE(scPrizeInfo);
SC_DECLARE_SCENE(scTourRule);
SC_DECLARE_SCENE(scTourEntry);
SC_DECLARE_SCENE(scQMSetting);
SC_DECLARE_SCENE(scQMEntry);
SC_DECLARE_SCENE(scEditSelect);
SC_DECLARE_SCENE(scEditMenu);
SC_DECLARE_SCENE(scEdit);
SC_DECLARE_SCENE(scReplay);
SC_DECLARE_SCENE(scSnapShot);
SC_DECLARE_SCENE(scSelEvent);
SC_DECLARE_SCENE(scSelStage);
SC_DECLARE_SCENE(scGameover);
SC_DECLARE_SCENE(scIntro);
SC_DECLARE_SCENE(scNetFriendList);
SC_DECLARE_SCENE(scSelChar);
SC_DECLARE_SCENE(scNetWatchBet);
SC_DECLARE_SCENE(scNetTimeResult);
SC_DECLARE_SCENE(scCoinShooter);
SC_DECLARE_SCENE(scBoot);
SC_DECLARE_SCENE(scChallenger);
SC_DECLARE_SCENE(scStrap);
SC_DECLARE_SCENE(scHowToPlay);
SC_DECLARE_SCENE(scTitle);
SC_DECLARE_SCENE(scFigureGetDemo);
SC_DECLARE_SCENE(scMelee);
SC_DECLARE_SCENE(scVsResult);
SC_DECLARE_SCENE(scSimpleResult);
SC_DECLARE_SCENE(scCharacterRoll);
SC_DECLARE_SCENE(scAdvSeal);
SC_DECLARE_SCENE(scAdvSelChar);
SC_DECLARE_SCENE(scAdvSave);
SC_DECLARE_SCENE(scAdvLoad);
SC_DECLARE_SCENE(scAdvDiff);
SC_DECLARE_SCENE(scAdvName);
SC_DECLARE_SCENE(scAdvMap);
SC_DECLARE_SCENE(scAdvVisual);
SC_DECLARE_SCENE(scAdvResult);
SC_DECLARE_SCENE(scAdvGameover);
SC_DECLARE_SCENE(scAdvEnding);

// Registers every scene of sora_scene with the scene manager (map: scsceneinit).
void scSystemRegisterScene() {
    gfSceneManager* mgr = gfSceneManager::getInstance();
    mgr->registScene(scMemoryChange::create());
    mgr->registScene(scEnding::create());
    mgr->registScene(scStaffRoll::create());
    mgr->registScene(muMenuMain::create());
    mgr->registScene(scTyFigList::create());
    mgr->registScene(scTyFigDisp::create());
    mgr->registScene(scTyFigDisp2::create());
    mgr->registScene(scSealList::create());
    mgr->registScene(scSealDisp::create());
    mgr->registScene(scClearGetter::create());
    mgr->registScene(scButton::create());
    mgr->registScene(scSoftList::create());
    mgr->registScene(scDemo::create());
    mgr->registScene(scTutorial::create());
    mgr->registScene(scVCTrial::create());
    mgr->registScene(scPrizeInfo::create());
    mgr->registScene(scTourRule::create());
    mgr->registScene(scTourEntry::create());
    mgr->registScene(scQMSetting::create());
    mgr->registScene(scQMEntry::create());
    mgr->registScene(scEditSelect::create());
    mgr->registScene(scEditMenu::create());
    mgr->registScene(scEdit::create());
    mgr->registScene(scReplay::create());
    mgr->registScene(scSnapShot::create());
    mgr->registScene(scSelEvent::create());
    mgr->registScene(scSelStage::create());
    mgr->registScene(scGameover::create());
    mgr->registScene(scIntro::create());
    mgr->registScene(scNetFriendList::create());
    mgr->registScene(scSelChar::create());
    mgr->registScene(scNetWatchBet::create());
    mgr->registScene(scNetTimeResult::create());
    mgr->registScene(scCoinShooter::create());
    mgr->registScene(scBoot::create());
    mgr->registScene(scChallenger::create());
    mgr->registScene(scStrap::create());
    mgr->registScene(scHowToPlay::create());
    mgr->registScene(scTitle::create());
    mgr->registScene(scFigureGetDemo::create());
    mgr->registScene(scMelee::create());
    mgr->registScene(scVsResult::create());
    mgr->registScene(scSimpleResult::create());
    mgr->registScene(scCharacterRoll::create());
    mgr->registScene(scAdvSeal::create());
    mgr->registScene(scAdvSelChar::create());
    mgr->registScene(scAdvSave::create());
    mgr->registScene(scAdvLoad::create());
    mgr->registScene(scAdvDiff::create());
    mgr->registScene(scAdvName::create());
    mgr->registScene(scAdvMap::create());
    mgr->registScene(scAdvVisual::create());
    mgr->registScene(scAdvResult::create());
    mgr->registScene(scAdvGameover::create());
    mgr->registScene(scAdvEnding::create());
}
