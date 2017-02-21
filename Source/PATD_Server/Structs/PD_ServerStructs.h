#pragma once

#include "PD_NetStructs.h"







//Usado por: PlayersManager
struct StructPlayer {

	//Menu options and configuration
	bool readyMenu;

	//Ingame data
	FStructTurnOrders* turnOrders;


};


enum class EGameState {  WaitingPlayerOrders, ExecutingActionOrders, };

//Usado por: GameManager
struct StructGameState {
	EGameState enumGameState;
};