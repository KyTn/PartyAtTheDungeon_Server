// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */

#include "Structs/PD_ServerStructs.h"
 //forward declarations
class PD_PlayersManager;


class PATD_SERVER_API PD_GM_InterectionManager
{
public:
	PD_GM_InterectionManager();
	~PD_GM_InterectionManager();

	PD_PlayersManager* playerManager;

	
	void PlayersLogicTurn();

	void LogicTurnMovePhase();
	void LogicTurnAttackPhase();

	//Funciones para procesar cada orden individual
	void TickMovePhase(int tick);
	void TickAttackPhase(int tick);
	

};
