#pragma once
//Hacemos (#include "Structs/PD_ServerStructs.h") en el .cpp de las clases para usar los structs.
//Si los necesitamos en el .h los hacemos con forward declaration.
#include "PD_NetStructs.h"
#include "PD_ServerEnums.h"

class PD_GM_LogicCharacter;

//Usado por: PlayersManager
struct StructPlayer {

	// indica el player que es
	int ID_PLAYER;

	//Menu options and configuration
	bool readyMenu;

	bool clientMaster;
	//Ingame data
	FStructTurnOrders* turnOrders;

	//PD_MG_LogicPosition* logicPosition;
//	PD_GM_GameManager* gameManager;
	//Puntero a Struct de caracteristicas del jugador.
	

	FStructCharacter* player_character; //Envio de la informacion LOGICA del personaje
	FStructUpdateCharacter* update_character;
	PD_GM_LogicCharacter* logic_Character;

	StructPlayer() {
		clientMaster = false;
		readyMenu = false; //�que es esto?
		turnOrders = new FStructTurnOrders();
		player_character = new FStructCharacter();
		update_character = new FStructUpdateCharacter();
	}

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