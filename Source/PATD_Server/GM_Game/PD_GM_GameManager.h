// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Include de herencia (interfaz)
#include "NW_NetWorking/EventLayer/PD_NW_iEventObserver.h"


//forward declarations

//Include de enums (no se pueden hacer forward declaration)
#include "Structs/PD_ServerEnums.h"
class PD_PlayersManager;
class PD_GM_MapManager;
class PD_GM_InteractionsManager;
struct StructGameState;
/**
 * 
 */
class PATD_SERVER_API PD_GM_GameManager : public PD_NW_iEventObserver
{
public:
	PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager);
	~PD_GM_GameManager();

	PD_PlayersManager* playersManager;
	PD_GM_InteractionsManager* interactionManager;
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
};