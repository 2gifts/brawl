#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_anm.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gm/gm_global.h>
#include <gm/gm_result_info.h>
#include <gr/gr_madein.h>
#include <gr/ground.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

#include <st/st_result.h>

// HYPOTHESIS: ftOwner::setResultWinRotY (sora_melee 0x112C04, map); its name belongs to the ft_owner unit, so it stays a
// stand-in here.
extern "C" void fn_27_112C04(ftOwner* owner, float rotY);

stClassInfoImpl<Stages::Result, stResult> stResult::bss_loc_14;

stResult* stResult::create() {
    return new (Heaps::StageInstance) stResult;
}

stResult::stResult() : stMelee("stResult", Stages::Result), m_winningCharKind(0), m_winAnim(0), unk270(0), unkState274(0), unk278(0), unk2A0(0.0f) {
    for (int i = 0; i < 4; i++) {
        m_fighterRot[i] = 0.0f;
    }
}

stResult::~stResult() {
    releaseArchive();
    CameraController::getInstance()->changeInput(CameraController::AnimationLocked);
    // HYPOTHESIS: the animation controller is the current one after the switch above.
    static_cast<cmAnimationController*>(CameraController::getInstance()->m_currentController)->releaseScnAnmRes();
}

bool stResult::loading() {
    return true;
}

void stResult::createObj() {
    testStageParamInit(m_fileData, 10);
    addGround(grMadein::create(0, "", "grResultMainBg", Heaps::StageInstance));
    for (u32 i = 0, groundNum = getGroundNum(); i != groundNum; i++) {
        Ground* ground = getGround(i);
        if (ground) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        }
    }
    createCollision(m_fileData, 2, NULL);
    static_cast<grMadein*>(getGround(0))->initializeEntity();
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 0x64, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x1E);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 1, 0xFFFE)), 0);

    Vec3f positions[12];
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
        getGround(0)->getNodePosition(&positions[0], 0, "NoContest_b2N");
        getGround(0)->getNodePosition(&positions[1], 0, "NoContest_b1N");
        getGround(0)->getNodePosition(&positions[2], 0, "NoContest_b3N");
        getGround(0)->getNodePosition(&positions[3], 0, "NoContest_b1N");
        getGround(0)->getNodePosition(&positions[4], 0, "NoContest_b2N");
        getGround(0)->getNodePosition(&positions[5], 0, "NoContest_b3N");
        getGround(0)->getNodePosition(&positions[6], 0, "NoContest_a1N");
        getGround(0)->getNodePosition(&positions[7], 0, "NoContest_a2N");
        getGround(0)->getNodePosition(&positions[8], 0, "NoContest_a3N");
        getGround(0)->getNodePosition(&positions[9], 0, "NoContest_a4N");
    } else if (!resultInfo->m_isTeams) {
        getGround(0)->getNodePosition(&positions[0], 0, "Player1N");
        getGround(0)->getNodePosition(&positions[1], 0, "Player2aN");
        getGround(0)->getNodePosition(&positions[2], 0, "Player2bN");
        getGround(0)->getNodePosition(&positions[3], 0, "Player2cN");
        getGround(0)->getNodePosition(&positions[4], 0, "Player3aN");
        getGround(0)->getNodePosition(&positions[5], 0, "Player3bN");
        getGround(0)->getNodePosition(&positions[6], 0, "Player4N");
    } else {
        getGround(0)->getNodePosition(&positions[0], 0, "team_11N");
        getGround(0)->getNodePosition(&positions[1], 0, "team_12N");
        getGround(0)->getNodePosition(&positions[2], 0, "team_13N");
        getGround(0)->getNodePosition(&positions[3], 0, "team_14N");
        getGround(0)->getNodePosition(&positions[4], 0, "team_21N");
        getGround(0)->getNodePosition(&positions[5], 0, "team_22N");
        getGround(0)->getNodePosition(&positions[6], 0, "team_23N");
        getGround(0)->getNodePosition(&positions[7], 0, "team_24N");
        getGround(0)->getNodePosition(&positions[8], 0, "team_31N");
        getGround(0)->getNodePosition(&positions[9], 0, "team_32N");
        getGround(0)->getNodePosition(&positions[10], 0, "team_33N");
        getGround(0)->getNodePosition(&positions[11], 0, "team_34N");
    }
    for (int i = 0; i < 12; i++) {
        m_positions[i] = positions[i];
    }
}

void stResult::update(float deltaFrame) {
    switch (unkState274) {
        case 0:
            if (unk270 == 1) {
                for (int i = 0; i < 51; i++) {
                    if (m_winningCharKind == fighterResultData[i]._00) {
                        unk278 = i;
                        break;
                    }
                }
                CameraController::getInstance()->changeInput(CameraController::AnimationLocked);
                u32 isWidescreen = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
                int base = isWidescreen ? 6 : 0;
                u32 rand = randi(6);
                if (rand >= 5) {
                    rand = 5;
                }
                int anim = base + rand + 1;
                switch (m_winningCharKind) {
                    case Character_Falco:
                        if (m_winAnim == 1) {
                            if (isWidescreen) {
                                anim = 13;
                            } else {
                                anim = 14;
                            }
                        }
                        break;
                    case Character_SZeroSuit:
                        if (m_winAnim == 1) {
                            if (isWidescreen) {
                                anim = 15;
                            } else {
                                anim = 16;
                            }
                        }
                        break;
                    case Character_Ike:
                        if (m_winAnim == 2) {
                            if (isWidescreen) {
                                anim = 17;
                            } else {
                                anim = 18;
                            }
                        }
                        break;
                    case Character_Snake:
                        if (m_winAnim == 2) {
                            anim = isWidescreen ? 19 : 20;
                        }
                        break;
                }
                gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
                if (resultInfo->m_isTeams) {
                    anim = isWidescreen ? 23 : 24;
                }
                if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
                    anim = isWidescreen ? 21 : 22;
                }
                nw4r::g3d::ResFile scnFile(m_fileData->getData(Data_Type_Scene, anim, 0xFFFE));
                if (scnFile.IsValid() && scnFile.HasResAnmScn()) {
                    nw4r::g3d::ResAnmScn scn = scnFile.GetResAnmScn(0);
                    cmAnimationController* controller = static_cast<cmAnimationController*>(CameraController::getInstance()->m_currentController);
                    controller->setScnAnmRes(scn, 0);
                }
                static_cast<grMadein*>(getGround(0))->startEntityAutoLoop();
                unkState274++;
            } else {
                return;
            }
            {
                int playerCount = 0;
                gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
                for (int i = 0; i < 4; i++) {
                    Vec3f pos;
                    if (getPlayerPosition(i, &pos)) {
                        playerCount++;
                    }
                }
                if (!resultInfo->m_isTeams) {
                    float offsets[5] = {0.0f, 0.0f, -7.0f, -1.0f, 4.0f};
                    unk288 = offsets[playerCount];
                } else {
                    float offsets[5] = {0.0f, 0.0f, 0.0f, 0.0f, 8.5f};
                    unk288 = offsets[playerCount];
                }
                unk28C = 0.0f;
                float rots[5] = {0.0f, 0.0f, -7.0f, 0.0f, 8.0f};
                m_fighterRot[0] = rots[playerCount];
                if (g_GameGlobal->m_resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
                    unk288 = 0.0f;
                }
                float camY = fighterResultData[unk278].m_camY[m_winAnim];
                unk27C.m_x = 0.0f;
                unk27C.m_y = camY;
                unk27C.m_z = 0.0f;
                gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
                camera->unkFC = 0;
                camera->m_0x9C = unk27C;
                unk2A0 = deltaFrame / 60.0f;
            }
        case 1:
            unk28C += unk2A0;
            unk2A0 *= 0.985f;
            if (unk28C >= 1.0f) {
                unk28C = 1.0f;
            }
            unk27C.m_x = unk288 * unk28C;
            gfCameraManager::getManager()->getCamera(0)->m_0x9C = unk27C;
    }
    for (int i = 0; i < 4; i++) {
        float rot = m_fighterRot[i];
        int entryId = g_ftManager->getEntryId(i);
        if (entryId != -1) {
            fn_27_112C04(g_ftManager->getOwner(entryId), rot);
        }
    }
}

void stResult::getFighterStartPos(Vec3f* out, int ftIndex) {
    if (static_cast<u32>(ftIndex) < 12) {
        *out = m_positions[ftIndex];
    }
}
