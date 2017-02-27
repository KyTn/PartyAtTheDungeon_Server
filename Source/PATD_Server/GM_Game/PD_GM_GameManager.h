// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Include de herencia (interfaz)
#include "NW_NetWorking/EventLayer/PD_NW_iEventObserver.h"


//forward declarations

//Include de enums (no se pueden hacer forward declaration)
#include "Structs/PD_ServerEnums.h"
class PD_PlayersManager;
class PD_GM_MapManager;
//class PD_GM_InteractionsManager;
struct StructGameState;
struct LogicTurnInformation;


/**
 * 
 */
class PATD_SERVER_API PD_GM_GameManager : public PD_NW_iEventObserver
{

private:
	LogicTurnInformation* ItemTurnInformation;
	LogicTurnInformation* MoveTurnInformation;
	LogicTurnInformation* AttackTurnInformation;
public:
	PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager);
	~PD_GM_GameManager();

	PD_PlayersManager* playersManager;
	PD_GM_MapManager* mapManager;
	//Tiene que heredar de observer y suscribirse a paquetes de juegos (ordenes).
	//Tiene un MapManager y un InteractionManager
	void HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);

	bool SuscribeToEvents(int inPlayer, UStructType inType);


	//Struct con el estado para la maquina de estados del gameManager
	StructGameState* structGameState;
		
	//Funciones de gestion del estado (maquina de estados)

	//Funciones de configuracion de la maquina
	//Transiciones
	void UpdateState();
	//Acciones al empezar el estado
	void OnBeginState();

	//Funciones auxiliares
	//Control directo del estado, llama a OnBeginState
	void ChangeState(EGameState newState);
	void InitState();


	//Funciones de gestion de acciones logicas

	void IntitializeTurnStates();

	void PlayersLogicTurn();

	void LogicTurnItemPhase();
	void LogicTurnMovePhase();
	void LogicTurnAttackPhase();

	//Funciones para procesar cada accion individual
	void LogicMoveTick(int tick);
	void LogicAttackTick(int tick);
	void LogicItemTick(int tick);



	//Funciones de gestion de visualizacion (visualization manager)
	//Esto lo llamara la parte visual (controller o actor) cuando finalize su animacion, para cambiar de estado o hacer lo que coresponda
	void OnAnimationEnd();

	void VisualTickControl();
	void VisualMoveTick();
	void VisualAttackTick();
};
