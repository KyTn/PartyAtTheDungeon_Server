// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


struct FStructTurnOrders;
class PD_GM_MapManager;
class PD_GM_LogicCharacter;
/**
 * 
 */
class PATD_SERVER_API PD_GM_AIManager
{
public:
	PD_GM_AIManager();
	~PD_GM_AIManager();

	FStructTurnOrders* AIExecEnemy(PD_GM_LogicCharacter* charac, PD_GM_MapManager* mngrMap);

};
