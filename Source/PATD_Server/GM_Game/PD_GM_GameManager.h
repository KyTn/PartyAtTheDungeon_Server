// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Include de herencia (interfaz)
#include "NW_NetWorking/EventLayer/PD_NW_iEventObserver.h"


//forward declarations

//Include de enums (no se pueden hacer forward declaration)
#include "Structs/PD_ServerEnums.h"
class PD_PlayersManager;
class PD_GM_MapManager;
class PD_GM_EnemyManager;
class PD_NW_NetworkManager;
class APD_GM_SplineManager;
class APD_TimerGame;
//class PD_GM_InteractionsManager;
struct StructGameState;
struct LogicTurnInformation;

#include "Components/SplineComponent.h"


/**
 * 
 */
class PATD_SERVER_API PD_GM_GameManager : public PD_NW_iEventObserver
{

private:
	/*LogicTurnInformation* ItemTurnInformation;
	LogicTurnInformation* MoveTurnInformation;
	LogicTurnInformation* AttackTurnInformation;*/
	//void IntitializeTurnStates();
public:
	PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager, PD_NW_NetworkManager* inNetworkManager, APD_GM_SplineManager* inSplineManager, APD_TimerGame* inTimer);
	~PD_GM_GameManager();

	PD_NW_NetworkManager* networkManager;
	PD_PlayersManager* playersManager;
	PD_GM_MapManager* mapManager;
	PD_GM_EnemyManager* enemyManager;

	APD_GM_SplineManager* splineManager;

	APD_TimerGame* timer;

	const float timeWaitingPhases = 3;

	int TotalPoints;

	//Tiene que heredar de observer y suscribirse a paquetes de juegos (ordenes).
	//Tiene un MapManager y un InteractionManager
	void HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);

	bool SuscribeToEvents(int inPlayer, UStructType inType);


	//Struct con el estado para la maquina de estados del gameManager
	StructGameState* structGameState;
	StructGamePhase* structGamePhase;
	//Funciones de gestion del estado (maquina de estados)


#pragma region State Machine

	//Funciones de configuracion de la maquina
	//Transiciones
	void UpdateState();
	//Acciones al empezar el estado
	void OnBeginState();

	//Funciones auxiliares
	//Control directo del estado, llama a OnBeginState
	void ChangeState(EGameState newState);
	

	void InitState();


#pragma endregion


#pragma region Send to Clients Functions

	// Si todos los clientes han enviado un FStructClientMapAlreadyInstantiated, 
	// se le envia un FStructClientStartMatchOnGM para que puedan pasar al Start_Match
	bool Send_FStructClientStartMatchOnGM();

#pragma endregion



	
#pragma region Phase Machine 

	void UpdatePhase();
	void OnBeginPhase();
	void ChangePhase(EServerPhase newPhase);
	void InitPhase();

#pragma endregion

	//Funciones para crear las ordenes de los enemigos
	void CreateEnemyOrders();
	void CallbackEndCreateEnemyOrders(FString idCharacter, FStructTurnOrders* turnOrders);


	//Funciones de gestion de acciones logicas
	void PlayersLogicTurn();

	void LogicTurnItemPhase();
	void LogicTurnMovePhase(int numCharacters);
	void LogicTurnAttackPhase(int numCharacters);

	//Funciones para procesar cada accion individual
	void LogicMoveTick(int tick, int numCharacters);
	void LogicAttackTick(int tick, int numCharacters);
	void LogicItemTick(int tick);

	//Funciones para gestionar el choque - devuelve si se ha chocado y a perdido (true) o no ha chocado o no ha perdido éste (false)
	bool  CheckAndManageCollisionWithPlayers(int indexDataPlayers, int tick, int numCharacters);
	bool  CheckAndManageCollisionWithEnemies(int indexDataPlayers, int tick, int numCharacters);
	bool  CheckAndManageCollisionWithMapElements(int indexDataPlayers, int tick, int  numCharacters);
	bool  CheckIsLogicCharacterInPosition(PD_MG_LogicPosition positionToCheck);

	void CallbackAttack();

	//Funciones de gestion de visualizacion (visualization manager)
	//Esto lo llamara la parte visual (controller o actor) cuando finalize su animacion, para cambiar de estado o hacer lo que coresponda
	void OnAnimationEnd();

	void OnCameraEndMove();

	void OnTimerEnd();
	void VisualTickControl();
	void VisualMoveTick();
	void VisualAttackTick();

	bool CheckWinGameConditions();

	bool CheckLoseGameConditions();

	//Función del sistema de puntuación
	void UpdatePoints(PD_GM_LogicCharacter* player, PD_GM_LogicCharacter* enemy);//Recibe el jugador y el enemigo, de tal manera que ve los puntos que vale el enemigo para sumarselos

};
