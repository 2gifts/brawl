// HYPOTHESIS: native partial-animation callers evaluate the index before the float.
#define SO_MOTION_PARTIAL_INDEX_FIRST
// BYTES: the feedback effects call the shared Vec3f constructor out of line.
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/donkey/ft_donkey_final.h>
#include <mt/mt_prng.h>
#include <so/controller/so_controller_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>

namespace {
// HYPOTHESIS: descriptive names for native work slots, verified by their uses
// in the rhythm update and the feedback/attack update below.
enum {
    Work_Remaining = 0x20000000,
    Work_StrikeInterval = 0x20000001,
    Work_StrikesSinceNote = 0x20000002,
    Work_ButtonInterval = 0x20000003,
    Work_NoteInterval = 0x20000004,
    Work_BeatIndex = 0x20000005,
    Work_HitInterval = 0x20000006,
    Work_Combo = 0x20000007,
    Work_BeatProgress = 0x20000008,
    Work_AttackLife = 0x20000009,
    Work_Attack = 0x2000000A,
    Work_EffectAngle = 0x21000005,
    Work_PowerBonus = 0x21000006,
    Work_SpeedBonus = 0x21000007,
    Work_ScaleBonus = 0x21000008,
    Work_ArmRate = 0x21000009,
    Flag_GoodStrike = 0x22000011,
    Flag_HitBeat = 0x22000012,
    Flag_WideWindow = 0x22000013,
    Flag_AllBeats = 0x22000014,
    Flag_StrongStrikeMotion = 0x22000015,
    Flag_ResyncArms = 0x22000016,
    Flag_InBeatWindow = 0x22000017,
    Flag_StrikeEffect = 0x22000018,
    Flag_BeatCue = 0x22000019,
    Flag_CountedBeat = 0x2200001B,
    Work_NextBeat = 0x10000043
};
}

class ftDonkeyStatusUniqProcessFinalLoop : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinalLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDonkeyStatusUniqProcessFinalLoop::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int handle = moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0x1B4B), true);
    workManage.setInt(handle, 0x10000042);
    workManage.setInt(-1, 0x2000000A);
    workManage.setFloat(1.0f, 0x21000006);
    workManage.setFloat(1.0f, 0x21000007);
    workManage.setFloat(1.0f, 0x21000008);
    workManage.setInt(0, 0x10000043);
}
// Score attack-button strikes against the beat table. The two arm layers can
// also cross a strike frame automatically when their animation rate is raised.
void ftDonkeyStatusUniqProcessFinalLoop::execStatus(soModuleAccesser* moduleAccesser) {
    soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soMotionModule& motion = moduleAccesser->getMotionModule();
    const ftDonkeyFinalData* data = static_cast<const ftDonkeyFinalData*>(g_ftCommonDataAccesser.getData(Fighter_Donkey));
    const ftDonkeyFinalParam* param = static_cast<const ftDonkeyFinalParam*>(data->extendParam[4]);
    const ftDonkeyFinalBeat* beats = data->beats;
    int beatCount = data->beatCount;
    int elapsed = param->duration - work.getInt(Work_Remaining);
    int hitInterval = work.getInt(Work_HitInterval);
    int beatIndex = work.getInt(Work_BeatIndex);
    int beatFrame = beats[beatIndex].frame;
    int window = param->beatWindow;
    if (work.isFlag(Flag_WideWindow)) {
        window = param->wideBeatWindow;
    }
    int strikeInterval = work.getInt(Work_StrikeInterval);
    int buttonInterval = work.getInt(Work_ButtonInterval);
    int strikesSinceNote = work.getInt(Work_StrikesSinceNote);
    bool strike = false;
    bool automaticStrike = false;
    float armRate = 1.0f;
    int nextFrame = 0;
    if (beatIndex < beatCount - 1) {
        nextFrame = beats[beatIndex + 1].frame;
    }
    if (nextFrame - elapsed > 0) {
        armRate = 30.0f / (float)(nextFrame - elapsed);
    }
    work.setFloat(armRate, Work_ArmRate);

    if (controller.getTrigger() & soController::getButtonMask(soController::Pad_Button_Attack)) {
        if (strikeInterval >= param->inputInterval) {
            strikeInterval = 0;
            strike = true;
        }
        float fastInterval = 60.0f / param->fastInputBpm;
        float slowInterval = 60.0f / param->slowInputBpm;
        float interval = (float)buttonInterval;
        if (interval < slowInterval) {
            if (interval < fastInterval) {
                interval = fastInterval;
            }
            float rate = 1.0f - (interval - fastInterval) / (slowInterval - fastInterval);
            rate *= param->maxMotionRate - 1.0f;
            rate += 1.0f;
            motion.setRate(rate);
        }
        buttonInterval = 0;
    } else {
        float rate = motion.getRate();
        if (rate > 1.0f) {
            rate -= param->motionRateDecay;
            if (rate < 1.0f) {
                rate = 1.0f;
            }
            motion.setRate(rate);
        }
    }
    if (work.isFlag(Flag_ResyncArms)) {
        float rate = work.getFloat(Work_ArmRate);
        motion.setPartialRate(2, rate);
        motion.setPartialRate(3, rate);
        work.offFlag(Flag_ResyncArms);
    }
    if (!strike && !work.isFlag(Flag_StrongStrikeMotion)) {
        float rate = motion.getPartialRate(2);
        float frame = motion.getPartialFrame(2);
        float previous = frame - rate;
        if (previous < 0.0f && frame >= 0.0f) {
            automaticStrike = true;
        } else if (previous < 30.0f && frame >= 30.0f) {
            automaticStrike = true;
        }
    }
    if (elapsed >= (beatFrame - window) && elapsed <= beatFrame + window) {
        if (!work.isFlag(Flag_InBeatWindow)) {
            work.onFlag(Flag_InBeatWindow);
        }
    } else {
        work.offFlag(Flag_InBeatWindow);
    }
    if (elapsed == beatFrame - 8) {
        work.onFlag(Flag_BeatCue);
    } else {
        work.offFlag(Flag_BeatCue);
    }

    if (strike || automaticStrike) {
        bool goodStrike = false;
        bool strongStrike = false;
        if (!automaticStrike) {
            if (hitInterval >= param->repeatHitInterval && elapsed >= (beatFrame - window) &&
                elapsed <= beatFrame + window && !work.isFlag(Flag_CountedBeat)) {
                goodStrike = true;
            }
            hitInterval = 0;
            strikesSinceNote++;
            if (goodStrike) {
                work.onFlag(Flag_GoodStrike);
                work.incInt(Work_Combo);
                work.incInt(Work_BeatProgress);
                float bonus = work.getFloat(Work_PowerBonus);
                bonus += param->bonuses[0].gain - 1.0f;
                if (bonus > param->bonuses[0].maximum) {
                    bonus = param->bonuses[0].maximum;
                }
                work.setFloat(bonus, Work_PowerBonus);
                bonus = work.getFloat(Work_SpeedBonus);
                bonus += param->bonuses[1].gain - 1.0f;
                if (bonus > param->bonuses[1].maximum) {
                    bonus = param->bonuses[1].maximum;
                }
                work.setFloat(bonus, Work_SpeedBonus);
                bonus = work.getFloat(Work_ScaleBonus);
                bonus += param->bonuses[2].gain - 1.0f;
                if (bonus > param->bonuses[2].maximum) {
                    bonus = param->bonuses[2].maximum;
                }
                work.setFloat(bonus, Work_ScaleBonus);
                if (work.getInt(Work_Combo) >= param->wideWindowCombo) {
                    work.onFlag(Flag_WideWindow);
                }
                if (work.getInt(Work_Combo) >= param->strongStrikeCombo && beats[beatIndex].strongStrike != 0) {
                    work.onFlag(Flag_StrongStrikeMotion);
                    strongStrike = true;
                }
                if (work.getInt(Work_BeatProgress) >= beatCount) {
                    work.onFlag(Flag_AllBeats);
                }
                work.onFlag(Flag_HitBeat);
            } else {
                work.offFlag(Flag_GoodStrike);
                work.setInt(0, Work_Combo);
                work.setFloat(1.0f, Work_PowerBonus);
                work.setFloat(1.0f, Work_SpeedBonus);
                work.setFloat(1.0f, Work_ScaleBonus);
            }
            work.onFlag(Flag_CountedBeat);
        }
        int sound = 0;
        if (strongStrike) {
            soMotionChangeParam change(0x1FB, 0.0f, 1.0f, 0, 0, 0, 0);
            motion.changeMotionRequest(&change);
            motion.removePartialAnimChr(2);
            motion.removePartialAnimChr(3);
            articles.setFrame(0, 0.0f);
            articles.setRate(0, 0.0f);
            work.setInt(2, Work_Attack);
            work.setInt(param->strongStrikeAttackLife, Work_AttackLife);
            sound = 0xABB;
        } else if (work.isFlag(Flag_StrongStrikeMotion)) {
            soMotionChangeParam change(0x1F9, 0.0f, 1.0f, 0, 1, 0, 0);
            motion.changeMotionRequest(&change);
            motion.addPartialAnimChr(2, 0x1FA, 0x12, false, 0.0f, 1.0f, false);
            motion.addPartialAnimChr(3, 0x1FA, 0x2A, false, 0.0f, 1.0f, false);
            articles.setFrame(0, 0.0f);
            articles.setRate(0, 1.0f);
            work.offFlag(Flag_StrongStrikeMotion);
            sound = 0xAB9;
        } else if (!automaticStrike) {
            if (motion.getPartialFrame(2) > 29.0f) {
                motion.setPartialFrame(2, 0.0f);
                motion.setPartialFrame(3, 0.0f);
                articles.setFrame(0, 0.0f);
                sound = 0xAB9;
            } else {
                motion.setPartialFrame(2, 30.0f);
                motion.setPartialFrame(3, 30.0f);
                articles.setFrame(0, 30.0f);
                sound = 0xABA;
            }
            if (work.isFlag(Flag_HitBeat)) {
                moduleAccesser->getSoundModule().playSENo3d((SndID)0xABC, false);
                work.setInt(1, Work_Attack);
                work.setInt(param->hitAttackLife, Work_AttackLife);
                moduleAccesser->getControllerModule().setRumble(0xD, 0, false, -1);
            } else {
                moduleAccesser->getSoundModule().playSENo3d((SndID)0xABD, false);
                work.setInt(0, Work_Attack);
                work.setInt(param->missAttackLife, Work_AttackLife);
                moduleAccesser->getControllerModule().setRumble(0x11, 0, false, -1);
            }
        } else if (motion.getPartialFrame(2) > 29.0f) {
            sound = 0xABA;
            articles.setFrame(0, 30.0f);
        } else {
            sound = 0xAB9;
            articles.setFrame(0, 0.0f);
        }
        if (sound != 0) {
            moduleAccesser->getSoundModule().playSENo3d((SndID)sound, false);
        }
        if (!strongStrike) {
            if (!automaticStrike) {
                work.onFlag(Flag_StrikeEffect);
            }
            float rate = work.getFloat(Work_ArmRate);
            motion.setPartialRate(2, rate);
            motion.setPartialRate(3, rate);
        }
    }

    int framesUntilBeat = -1;
    if ((elapsed + 1) > beatFrame) {
        if (beatIndex < beatCount - 1) {
            framesUntilBeat = beats[beatIndex + 1].frame - (elapsed + 1);
        }
    } else {
        framesUntilBeat = beatFrame - (elapsed + 1);
    }
    work.setInt(framesUntilBeat, Work_NextBeat);
    if ((elapsed + 1) > beatFrame + window) {
        beatIndex++;
        if (beatIndex <= param->beatProgressLimit) {
            work.setInt(beatIndex, Work_BeatProgress);
        }
        if (!work.isFlag(Flag_HitBeat)) {
            work.offFlag(Flag_GoodStrike);
            work.setInt(0, Work_Combo);
            work.setFloat(1.0f, Work_PowerBonus);
            work.setFloat(1.0f, Work_SpeedBonus);
            work.setFloat(1.0f, Work_ScaleBonus);
        }
        work.offFlag(Flag_HitBeat);
        work.offFlag(Flag_CountedBeat);
    }
    work.setInt(strikeInterval + 1, Work_StrikeInterval);
    work.setInt(buttonInterval + 1, Work_ButtonInterval);
    work.setInt(strikesSinceNote, Work_StrikesSinceNote);
    work.setInt(beatIndex, Work_BeatIndex);
    work.setInt(hitInterval + 1, Work_HitInterval);
    work.decInt(Work_Remaining);
}

// The native loop calls this constructor separately for the base and strong-strike motions.
#pragma dont_inline on
soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) { }
#pragma dont_inline reset

// Emit note articles in opposite directions, show strike/beat feedback, then
// install or expire the hitbox selected by the rhythm update.
void ftDonkeyStatusUniqProcessFinalLoop::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soEffectModule& effects = moduleAccesser->getEffectModule();
    const ftDonkeyFinalParam* param = static_cast<const ftDonkeyFinalParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Donkey)->extendParam[4]);
    int interval = work.getInt(Work_NoteInterval);
    int strikes = work.getInt(Work_StrikesSinceNote);
    if (interval >= param->noteEffectInterval) {
        float angle = randf() * 3.1415927f;
        work.setFloat(angle, Work_EffectAngle);
        articles.generate(1, NULL, false);
        angle += 3.1415927f;
        work.setFloat(angle, Work_EffectAngle);
        articles.generate(1, NULL, false);
        interval = 0;
        strikes = 0;
    }
    work.setInt(interval + 1, Work_NoteInterval);
    work.setInt(strikes, Work_StrikesSinceNote);
    if (work.isFlag(Flag_StrikeEffect)) {
        work.offFlag(Flag_StrikeEffect);
        if (work.isFlag(Flag_HitBeat)) {
            effects.req((EfID)0xE10001, 0, &Vec3f(0.0f, 6.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f),
                        1.0f, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), true, 0);
        } else {
            effects.reqFollow((EfID)0xE1000A, 0, &Vec3f(0.0f, 6.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f),
                              1.0f, true, 0, 0, -1);
        }
    }
    if (work.isFlag(Flag_BeatCue)) {
        work.offFlag(Flag_BeatCue);
        effects.reqFollow((EfID)0xE1000B, 3, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f),
                          1.0f, true, 0, 0, -1);
    }
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    int pending = work.getInt(Work_Attack);
    if (pending != -1) {
        for (int i = 0; i < 4; i++) {
            if (attack.isAttack(i, false)) {
                attack.clear(i);
            }
        }
        attack.set(pending, 0);
        work.setInt(-1, Work_Attack);
    } else if (work.getInt(Work_AttackLife) > 0) {
        work.decInt(Work_AttackLife);
        if (work.getInt(Work_AttackLife) <= 0) {
            for (int i = 0; i < 4; i++) {
                if (attack.isAttack(i, false)) {
                    attack.clear(i);
                }
            }
        }
    }
}

void ftDonkeyStatusUniqProcessFinalLoop::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x134) {
        static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(0, 0);
        int handle = moduleAccesser->getWorkManageModule().getInt(0x10000042);
        if (handle >= 0) {
            moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
            moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
        }
    }
}

ftDonkeyStatusUniqProcessFinalLoop g_ftDonkeyStatusUniqProcessFinalLoop;
