#pragma once

#include "PD_NetStructs.h"
//Incluyendo este archivo en cualquier otro #include "Structs/PD_ServerStructs.h" tenemos todos los structs (porque hace include de los demas)







//Usado por: PlayersManager
struct StructPlayer {

	//Menu options and configuration
	bool readyMenu;

	//Ingame data
	FStructTurnOrders* turnOrders;


};


enum class EGameState {  WaitingPlayerOrders, ExecutingActionOrders };

//Usado por: GameManager
struct StructGameState {
	EGameState enumGameState;
};

enum class EServerState {WaitingClientMaster,WaitingStartGame,GameInProcess };
//Usado por: GameInstance -ServerManager-
struct StructServerState {
	EServerState enumServerState;
};