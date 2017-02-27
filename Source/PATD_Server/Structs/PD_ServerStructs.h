#pragma once
//Hacemos (#include "Structs/PD_ServerStructs.h") en el .cpp de las clases para usar los structs.
//Si los necesitamos en el .h los hacemos con forward declaration.
#include "PD_NetStructs.h"
#include "PD_ServerEnums.h"

//Usado por: PlayersManager
struct StructPlayer {

	//Menu options and configuration
	bool readyMenu;

	bool clientMaster;
	//Ingame data
	FStructTurnOrders* turnOrders;

	//PD_MG_LogicPosition* logicPosition;
//	PD_GM_GameManager* gameManager;
	//Puntero a Struct de caracteristicas del jugador.
	
	PD_GM_PlayerLogicCharacter* logicCharacter;
	APD_PLY_Controller* actorController;
	APD_PLY_GenericCharacter* actor;
	//Puntero a su fisicCharacter (actor de unreal)


};




//Usado por: GameManager
struct StructGameState {
	EGameState enumGameState;
	EActionPhase enumActionPhase;
	
};


//Usado por: GameInstance -ServerManager-
struct StructServerState {
	EServerState enumServerState;
	bool gameConfigurationDone;
};

//Estados en los que se puede encontrar cada Logic Turn

//Usado por: InteractionsManager
struct LogicTurnInformation {
	InteractionStates CurrentState;
};