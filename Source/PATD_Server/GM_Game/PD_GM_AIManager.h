// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "PATD_Server/Actors/Enemies/MyAIClass.h"
struct FStructTurnOrders;
class PD_GM_MapManager;
class PD_GM_LogicCharacter;
class MyAIClass;
/**
 * 
 */
class PATD_SERVER_API PD_GM_AIManager
{
private:

	//IAHito* iaH;
	//APD_AIController* contr;
public:
	MyAIClass* MyAI;
	PD_GM_AIManager();
	~PD_GM_AIManager();

	FStructTurnOrders* AIExecEnemy(PD_GM_LogicCharacter* charac, PD_GM_MapManager* mngrMap);

	void StartCalculateAIEnemy(PD_GM_LogicCharacter* charac);
};
