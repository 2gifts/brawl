#pragma once

int gmCheckChallengerToVsMelee();
void gmCheckHideCondition();
void gmCheckSoftList();
void gmActionOpenClearGet(int condition, int kind, int reward);
bool gmCheckTargetBreakHideElement();
bool checkAllstar(int condition);
bool checkBossBattle(int condition);

// HYPOTHESIS: the map has these in an anonymous namespace; they stay global until their callers are recovered, or they would be stripped.
bool checkSimple(int condition);
bool checkMeleeEtc(int condition);
bool checkTraining(int condition);
bool checkInfo_advFigure(int condition);
