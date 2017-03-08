// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"

class PD_GM_LogicCharacter;
class PD_GM_MapManager;
class APD_E_Character;

class PATD_SERVER_API MyAIClass
{
private:
	PD_GM_MapManager* mapMng;
	FStructTurnOrders* actions;
	PD_MG_LogicPosition currentPos;
	PD_MG_LogicPosition ActionPos;
	TArray<bool> aviablePos;
	APD_E_Character* ch;

	bool PlayersNear;
	bool Attacked;
	int AP;

public:
	MyAIClass();
	~MyAIClass();


	void HStartTurnZombie(PD_GM_MapManager* refMap, PD_MG_LogicPosition pos);

	FStructTurnOrders* HGetActions() { return actions; }

	void HArePlayersNear();

	void AviablePosition();

	void HAddAttack();

	void HAddMove();



};
