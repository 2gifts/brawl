#pragma once

#include <if/if_utility.h>

class IfRewardListTask;
class IfCoinShooterLabelImpl;
class Vec3f;
class MuObject;

class IfCoinShooter : public utility::StandardMenu {
public:
    // HYPOTHESIS: unrecovered members between the standard menu and reward list.
    u8 unk1AC[0x180];
    IfRewardListTask* unk32C;
    u32 unk330;

    IfCoinShooter();
    static void create();
    static void destroy();
    bool isQuit();
    bool isLoadFinish();
    void start();
    virtual void loadResource();
    virtual void createModel();
    virtual void update();
};

class IfCoinShooterLabel {
public:
    IfCoinShooterLabelImpl* unk0;
    IfCoinShooterLabel();
    ~IfCoinShooterLabel();
    void reset();
    void setPos(const Vec3f* position);
};

class IfCoinShooterLabelImpl : public utility::AutoList<IfCoinShooterLabelImpl>::Node {
public:
    // HYPOTHESIS: owner pointer, four-bit state, and two behavior flags.
    IfCoinShooterLabel* unkC;
    u32 unk10 : 4;
    u8 unk10_3 : 1;
    u8 unk10_2 : 1;
    u8 unk10_0 : 2;
    u8 unk11[3];

    virtual ~IfCoinShooterLabelImpl();
    virtual MuObject* model() const = 0;
    virtual void update();
    virtual void detach() = 0;
};
