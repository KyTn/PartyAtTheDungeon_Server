// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */





 //forward declarations
class PD_PlayersManager;
class PD_GM_MapManager;

class PATD_SERVER_API PD_GM_InteractionsManager
{
private:
	LogicTurnInformation* ItemTurnInformation;
	LogicTurnInformation* MoveTurnInformation;
	LogicTurnInformation* AttackTurnInformation;

public:
	PD_GM_InteractionsManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager);
	~PD_GM_InteractionsManager();

	PD_PlayersManager* playersManager;
	PD_GM_MapManager* mapManager;

	void IntitializeTurnStates();
	
	void PlayersLogicTurn();

	void LogicTurnItemPhase();
	void LogicTurnMovePhase();
	void LogicTurnAttackPhase();

	//Funciones para procesar cada orden individual
	void TickItemPhase(int tick);
	void TickMovePhase(int tick);
	void TickAttackPhase(int tick);
};
