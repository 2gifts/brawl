#include <gf/gf_scene.h>
#include <types.h>

// The sequences of the game; only their create functions are needed here.
// TODO: clean up - include the sequence headers once they all exist (most sequence units are not decompiled).
class sqVsMelee {
public:
    static gfSequence* create();
};
class sqToMelee {
public:
    static gfSequence* create();
};
class sqSpMelee {
public:
    static gfSequence* create();
};
class sqQuMelee {
public:
    static gfSequence* create();
};
class sqSingleSimple {
public:
    static gfSequence* create();
};
class sqSingleAllstar {
public:
    static gfSequence* create();
};
class sqSingleBoss {
public:
    static gfSequence* create();
};
class sqEvent {
public:
    static gfSequence* create();
};
class sqTargetBreak {
public:
    static gfSequence* create();
};
class sqHomerun {
public:
    static gfSequence* create();
};
class sqKumite {
public:
    static gfSequence* create();
};
class sqCoinshooter {
public:
    static gfSequence* create();
};
class sqBoot {
public:
    static gfSequence* create();
};
class sqPrizeCheck {
public:
    static gfSequence* create();
};
class sqChallenger {
public:
    static gfSequence* create();
};
class sqTyFigDisp {
public:
    static gfSequence* create();
};
class sqNetAnyOkiraku {
public:
    static gfSequence* create();
};
class sqNetAnyWatch {
public:
    static gfSequence* create();
};
class sqNetAnyTeamMelee {
public:
    static gfSequence* create();
};
class sqNetFriendList {
public:
    static gfSequence* create();
};
class sqNetFriendMelee {
public:
    static gfSequence* create();
};
class sqNetFriendKumite {
public:
    static gfSequence* create();
};
class sqNetFriendHomerun {
public:
    static gfSequence* create();
};
class sqButton {
public:
    static gfSequence* create();
};
class sqTitle {
public:
    static gfSequence* create();
};
class sqMenuMain {
public:
    static gfSequence* create();
};
class sqTraining {
public:
    static gfSequence* create();
};
class sqEdit {
public:
    static gfSequence* create();
};
class sqReplay {
public:
    static gfSequence* create();
};
class sqAdventure {
public:
    static gfSequence* create();
};

// Registers every sequence of the game with the scene manager (sora_scene sq_sequence_init.o).
void sqRegistSequence() {
    gfSceneManager* manager = gfSceneManager::getInstance();
    manager->registSequence(sqVsMelee::create());
    manager->registSequence(sqToMelee::create());
    manager->registSequence(sqSpMelee::create());
    manager->registSequence(sqQuMelee::create());
    manager->registSequence(sqSingleSimple::create());
    manager->registSequence(sqSingleAllstar::create());
    manager->registSequence(sqSingleBoss::create());
    manager->registSequence(sqEvent::create());
    manager->registSequence(sqTargetBreak::create());
    manager->registSequence(sqHomerun::create());
    manager->registSequence(sqKumite::create());
    manager->registSequence(sqCoinshooter::create());
    manager->registSequence(sqBoot::create());
    manager->registSequence(sqPrizeCheck::create());
    manager->registSequence(sqChallenger::create());
    manager->registSequence(sqTyFigDisp::create());
    manager->registSequence(sqNetAnyOkiraku::create());
    manager->registSequence(sqNetAnyWatch::create());
    manager->registSequence(sqNetAnyTeamMelee::create());
    manager->registSequence(sqNetFriendList::create());
    manager->registSequence(sqNetFriendMelee::create());
    manager->registSequence(sqNetFriendKumite::create());
    manager->registSequence(sqNetFriendHomerun::create());
    manager->registSequence(sqButton::create());
    manager->registSequence(sqTitle::create());
    manager->registSequence(sqMenuMain::create());
    manager->registSequence(sqTraining::create());
    manager->registSequence(sqEdit::create());
    manager->registSequence(sqReplay::create());
    manager->registSequence(sqAdventure::create());
}

// Starts the boot sequence: right away, or as the next sequence of the scene manager.
void sqSystemSetStartSequence(bool immediately) {
    gfSceneManager* manager = gfSceneManager::getInstance();
    if (immediately) {
        manager->startSequence("sqBoot", 0);
    } else {
        manager->setNextSequence("sqBoot", 0);
    }
}
