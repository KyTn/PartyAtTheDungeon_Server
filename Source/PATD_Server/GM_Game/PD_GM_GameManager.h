// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Include de herencia (interfaz)
#include "NW_NetWorking/EventLayer/PD_NW_iEventObserver.h"


#include "Structs/PD_ServerStructs.h"
//forward declarations
class PD_PlayersManager;

/**
 * 
 */
class PATD_SERVER_API PD_GM_GameManager : public PD_NW_iEventObserver
{
public:
	PD_GM_GameManager();
	~PD_GM_GameManager();

	PD_PlayersManager* playerManager;

	//Tiene que heredar de observer y suscribirse a paquetes de juegos (ordenes).
	//Tiene un MapManager y un InteractionManager
	void HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);

	bool SuscribeToEvents(int inPlayer, UStructType inType);


	//Struct con el estado para la maquina de estados del gameManager
	StructGameState* structGameState;
	
	//Funciones de gestion del estado (maquina de estados)
	//Transiciones
	void UpdateState();
	//Acciones al empezar el estado
	void OnBeginState();
	//Control directo del estado, llama a OnBeginState
	void ChangeState(EGameState newState);

	void InitState();
};
