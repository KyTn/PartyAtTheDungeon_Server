// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_GameManager.h"

//Includes de uso de objetos
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_GenericController.h"
#include "Actors/Players/PD_CharacterController.h"
#include "Components/SplineComponent.h"
#include "../PD_ServerGameInstance.h"
#include"../ServerCamera.h"

//Includes of forward declaration
#include "PD_PlayersManager.h"
#include "PD_GM_MapManager.h"
#include "PD_GM_EnemyManager.h"
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "NW_Networking/PD_NW_NetworkManager.h"
#include "PD_GM_SplineManager.h"

#include "Actors/PD_TimerGame.h"
#include "Actors/Enemies/PD_AIController.h"
#include "Actors/PD_GenericController.h"

#include "Structs/PD_NetStructs.h"
PD_GM_GameManager::PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager, PD_NW_NetworkManager* inNetworkManager, APD_GM_SplineManager* inSplineManager, APD_TimerGame* inTimer)
{
	TotalPoints = 0;
	playersManager = inPlayersManager; 
	mapManager = inMapManager;
	splineManager = inSplineManager;
	mapManager->_GAMEMANAGER = this;
	enemyManager = new PD_GM_EnemyManager();
	networkManager = inNetworkManager;
	networkManager->RegisterObserver(this);
	structGameState = new StructGameState();
	structGamePhase = new StructGamePhase();
	timer = inTimer;
	timer->setGameManager(this);
	InitState();

}

PD_GM_GameManager::~PD_GM_GameManager()
{
}


#pragma region State Machine


void PD_GM_GameManager::InitState() {
	
	ChangeState(EGameState::Instantiate_Map);

}

void PD_GM_GameManager::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	
	if (structGameState->enumGameState == EGameState::Instantiate_Map) {
		// Si se recibe del servidor un Start_Match, ir a ese estado. 
		if (inEventType == UStructType::FStructClientMapAlreadyInstantiated) {
			// marcamos al player que ya ha instanciado el mapa
			playersManager->Mark_Player_As_Map_Already_Instantiated(inPlayer);

			// Si todos han instanciado el mapa, mandamos mensaje de pasar al Start_Match a los clientes
			if (playersManager->Check_All_Players_Have_Map_Already_Instantiated()) {
				Send_FStructClientStartMatchOnGM();
				ChangeState(EGameState::Start_Match);
			}
			
		}
	}
	else if (structGameState->enumGameState == EGameState::Start_Match) {
		
	}
	else if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) 
	{
		if (inEventType == UStructType::FStructTurnOrders) {
			FStructTurnOrders* turnStruct = (FStructTurnOrders*)inDataStruct;

			playersManager->GetDataStructPlayer(inPlayer)->turnOrders = turnStruct;
			playersManager->GetDataStructPlayer(inPlayer)->playerSendOrder = true;
			UpdateState();
		}
		else if (inEventType == UStructType::FStructLostConnection) {
			UpdateState();
		}
	}
	
	//if(playerManager->AllPlayersSendOrders())

}

bool PD_GM_GameManager::SuscribeToEvents(int inPlayer, UStructType inType) {
	if (inType == UStructType::FStructTurnOrders) return true;								
	else if (inType == UStructType::FStructClientCanGenerateOrders) return true;			
	else if (inType == UStructType::FStructClientMapAlreadyInstantiated) return true;		
	else if (inType == UStructType::FStructClientStartMatchOnGM) return true;				
	else if (inType == UStructType::FStructInstatiatePlayers) return true;					
	else if (inType == UStructType::FStructOrderMenu) return true;							
	else if (inType == UStructType::FStructUpdateTurn) return true;	
	else if (inType == UStructType::FStructLostConnection) return true;
	else return false;
}


void PD_GM_GameManager::UpdateState() {

	//Creo que es mas claro un if-else gigante que un switch gigante

	if (structGameState->enumGameState == EGameState::Instantiate_Map) {
		//Transicion inmediata de estado
		//this->ChangeState(EGameState::Start_Match);
		

	}else if (structGameState->enumGameState == EGameState::Start_Match) {
		//Transicion inmediata de estado
		// pero este estado está pensado para la presentacion y animación inicial del mapa y de la mision
		this->ChangeState(EGameState::WaitingPlayerOrders);
		
	}else if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
		//Transiciones de estados
		if (playersManager->AllPlayersSendOrders()) {
			this->ChangeState(EGameState::ExecutingPlayersTurn);
		}


	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {

		if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases) {
			//Actualizar estados alteadores y de efectos en enemigos --- despues del turno de los players
			CheckAndUpdate_ActiveEffectsOnEnemies();
			CheckAndUpdate_AlteredStateOnEnemies();
			CheckWinGameConditions();
			this->ChangeState(EGameState::WaitingEnemiesOrders);
		}
		

	}else if (structGameState->enumGameState == EGameState::WaitingEnemiesOrders) {

		//Transiciones de estados
		if (enemyManager->AllEnemiesHaveOrders()) {
			this->ChangeState(EGameState::ExecutingEnemiesTurn);
		}

	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {

		if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases) {
			//Actualizar activeeffects and alteredState de los jugadores --- justo antes de actualizarlos en el server
			CheckAndUpdate_ActiveEffectsOnPlayers();
			CheckAndUpdate_AlteredStateOnPlayers();
			this->ChangeState(EGameState::EndOfTurn);
		}
	

	}else if (structGameState->enumGameState == EGameState::EndOfTurn) {

			//Transicion inmediata de estado
			this->ChangeState(EGameState::WaitingPlayerOrders);


	}else{ //Caso indeterminado
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::UpdateState: WARNING: estado sin update"));
	}
}

void PD_GM_GameManager::OnBeginState() {
	if (structGameState->enumGameState == EGameState::Instantiate_Map) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: Instantiate_Map"));
		//Update de los enemigos a traves del dinamic map
		
		mapManager->InstantiateMap();
		
		UpdateState();

	}
	else if (structGameState->enumGameState == EGameState::Start_Match) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: Start_Match"));


		UpdateState();

	} else if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: WaitingPlayerOrders"));
		FStructClientCanGenerateOrders clientGenerateOrders = FStructClientCanGenerateOrders();
		networkManager->SendNow(&clientGenerateOrders, -1);


		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
		FVector target = Cast<AServerCamera>(SGI->CameraServer)->GetPlayersAveragePosition();
		Cast<AServerCamera>(SGI->CameraServer)->InitPatrol(target);
		Cast<AServerCamera>(SGI->CameraServer)->LookAtPoint(target);

		UpdateState();

	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingPlayersLogic"));
		//PlayersLogicTurn();
		InitPhase();
		


	}
	/*else if (structGameState->enumGameState == EGameState::ExecutingPlayersT){
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingPlayersVisualization"));
		
		structGameState->enumActionPhase = EActionPhase::Move; //Empezamos en fase de mover y a partir de ahi lo controla el VisualTickControl
		VisualTickControl();
		

	}*/
	else if (structGameState->enumGameState == EGameState::WaitingEnemiesOrders) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: WaitingEnemiesOrders"));
		enemyManager->newTurn();
		CreateEnemyOrders();
		UpdateState();

	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingEnemiesLogic"));
		//PlayersLogicTurn();
		InitPhase();
		

	}
	/*else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingEnemiesVisualization"));
		structGameState->enumActionPhase = EActionPhase::Move; //Empezamos en fase de mover y a partir de ahi lo controla el VisualTickControl
		VisualTickControl();

	}*/else if (structGameState->enumGameState == EGameState::EndOfTurn) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn"));
		

		//Hay que hacer lo necesario (borrar las cosas de este turno) para que se pueda recibir otro normalmente.
		for (StructPlayer* structPlayer : playersManager->GetDataPlayers()) {
			structPlayer->turnOrders->positionsToMove.Empty();
			structPlayer->turnOrders->actions.Empty();
			structPlayer->playerSendOrder = false;
			//structPlayer->turnOrders->consumablesToConsume.Empty();
			//structPlayer->turnOrders->interactuablesToInteract.Empty();

		}

		//Enviar a cliente actualizacion del mapa
		FStructUpdateTurn structUpdateTurn;
		//Jugadores
		for (int iPlayers = 0; iPlayers < playersManager->GetDataPlayers().Num(); iPlayers++) {
			PD_GM_LogicCharacter* logicCharacter = playersManager->GetDataPlayers()[iPlayers]->logic_Character;
			FStructUpdateCharacter structUpdateCharacter;
			//Conversion de Struct a LogicPosition
			FStructLogicPosition logicPosition;
			logicPosition.positionX = logicCharacter->GetCurrentLogicalPosition().GetX();
			logicPosition.positionY = logicCharacter->GetCurrentLogicalPosition().GetY();

			structUpdateCharacter.currentCharacterPosition = logicPosition;
			structUpdateCharacter.ID_character = logicCharacter->GetIDCharacter();
			structUpdateTurn.listPlayerCharacters.Add(structUpdateCharacter);
		}
		//Enemigos
		for (int iEnemies = 0; iEnemies < enemyManager->GetEnemies().Num(); iEnemies++) {
			PD_GM_LogicCharacter* logicCharacter = enemyManager->GetEnemies()[iEnemies];
			FStructUpdateCharacter structUpdateCharacter;
			//Conversion de Struct a LogicPosition
			FStructLogicPosition logicPosition = FStructLogicPosition();
			logicPosition.positionX = logicCharacter->GetCurrentLogicalPosition().GetX();
			logicPosition.positionY = logicCharacter->GetCurrentLogicalPosition().GetY();

			structUpdateCharacter.currentCharacterPosition = logicPosition;
			//	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn: id Enemy:%s" ), *logicCharacter->GetIDCharacter());
			structUpdateCharacter.ID_character = logicCharacter->GetIDCharacter();
			structUpdateTurn.listEnemyCharacters.Add(structUpdateCharacter);
		}

		//Envio a todos los clientes con el update del turno
		networkManager->SendNow(&structUpdateTurn);



		if (CheckWinGameConditions()) //Jugadores Ganan la partida
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn: GANAN PLAYERS"));
		}
		else if (CheckLoseGameConditions()) //Jugadores pierden la partida
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn: PIERDEN PLAYERS"));
		}
		else
		{

			
			UpdateState();//transicion inmediata
		}

	}else //Caso indeterminado
	{
		//UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginState: WARNING: estado sin inicializacion"));
	}

}


void PD_GM_GameManager::ChangeState(EGameState newState) {
	structGameState->enumGameState = newState;
	OnBeginState();
}

#pragma endregion

#pragma region Send to Clients Functions

// Si todos los clientes han enviado un FStructClientMapAlreadyInstantiated, 
// se le envia un FStructClientStartMatchOnGM para que puedan pasar al Start_Match
bool PD_GM_GameManager::Send_FStructClientStartMatchOnGM() {
	FStructClientStartMatchOnGM msg = FStructClientStartMatchOnGM();
	return networkManager->SendNow(&msg, -1);
}

#pragma endregion



/*
//Funciones de gestion de acciones
void PD_GM_GameManager::IntitializeTurnStates() {
	ItemTurnInformation->CurrentState = InteractionStates::Ready;
	MoveTurnInformation->CurrentState = InteractionStates::Ready;
	AttackTurnInformation->CurrentState = InteractionStates::Ready;
}*/


void PD_GM_GameManager::CreateEnemyOrders() {
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CreateEnemyOrders, enemyID:%s"), *enemyManager->GetEnemies()[i]->GetIDCharacter());
		APD_AIController* controller = (APD_AIController*)enemyManager->GetEnemies()[i]->GetController();
		controller->StartAITurnCalcultion(mapManager);
	}
}

void PD_GM_GameManager::CallbackEndCreateEnemyOrders(FString idCharacter, FStructTurnOrders* turnOrders) {

	//Asignar las ordenes al character del id
	int index =enemyManager->GetIndexByID(idCharacter);
	if (!turnOrders) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CallbackEndCreateEnemyOrders, enemyID:%s Index:%d ERROR no TURNORDERS"), *idCharacter, index);

	}

	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CallbackEndCreateEnemyOrders, enemyID:%s Index:%d NumMoves:%d"), *idCharacter, index, turnOrders->positionsToMove.Num());
	for (FStructLogicPosition pos : turnOrders->positionsToMove) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CallbackEndCreateEnemyOrders,Orders: x%d,y%d"), pos.positionX,pos.positionY);

	}
	enemyManager->AddActionTurn(turnOrders, index);
	//Comprobar si estan todas para pasar de estado
	UpdateState();


}

//Control de los turnos

void PD_GM_GameManager::PlayersLogicTurn() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::PlayersLogicTurn"));
	//Distincion para players o enemigos
	int numCharacters = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		numCharacters = playersManager->GetNumPlayers();
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		numCharacters = enemyManager->GetEnemies().Num();
	}


	//LogicTurnItemPhase(numCharacters);
	LogicTurnMovePhase(numCharacters);
	//LogicTurnAttackPhase(numCharacters);
}

void PD_GM_GameManager::LogicTurnItemPhase() {
	
}

void PD_GM_GameManager::LogicTurnMovePhase(int numCharacters) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnMovePhase"));

	//Distincion para players o enemigos
	//Calcular el numero de ticks a realizar (el de la lista mas larga)
	int numTicks = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Move);
		for (int i = 0; i < playersManager->GetNumPlayers(); i++)
		{
			playersManager->GetDataStructPlayer(i)->logic_Character->ClearMovingLogicalPosition(); //limpiar el array de moverse logicamente
		}
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		numTicks = enemyManager->GetMaxLenghtActions(EActionPhase::Move);
		for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
		{
			enemyManager->GetEnemies()[i]->ClearMovingLogicalPosition(); //limpiar el array de moverse logicamente
		}
	}

	

	//Ejecutar los ticks
	for (int i = 0; i < numTicks; i++) {
		//Solo ejecuta al LogicMoveTick si el numero total de movimientos es superior al tick correspodiente
		//Si no, lo que pasara es que intentara acceder en un tick superior a una posicion del array que no existe
			LogicMoveTick(i, numCharacters);

	}
}

void PD_GM_GameManager::LogicTurnAttackPhase() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase"));
	/*
	- Segun el turno de Players o Enemies, metemos al Tmap de Accions por turno, las acciones de TODOS los que toque, 
	para que luego en cada llamada de Visual Attack, se interprete solo 1
	- SI NO HACEMOS ESTO, no se van a poder ver las animaciones de todos los ataques que se hagan, solo el final
	(Y hombre, eso esta feo - Ahi esa animacion en una cama nido!)
	*/


	// 1) todos los bufos
	// 2) habilidades que causan estados
	// 3) habilidades que hacen que te muevas (cargas)
	// 4) ataques normales y otras habilidades 
	// 5) habilidades que hacen que muevan a los otros

	// de momento vamos a hacerlo de forma secuencial sin ordenar...

	index_IndividualActionsOnTurns = 0;

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		for (int index_players = 0; index_players < playersManager->GetNumPlayers(); index_players++)
		{
			for (int index_actions = 0; index_actions < playersManager->GetDataStructPlayer(index_players)->turnOrders->actions.Num(); index_actions++)
			{
				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase -  players -- adding action %d"), index_actions);

				FString id_player = playersManager->GetDataStructPlayer(index_players)->logic_Character->GetIDCharacter(); 
				individualActionOnTurns.Add(id_player, index_actions);
			}
		}
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		
		for (int index_enemies = 0; index_enemies < enemyManager->GetEnemies().Num(); index_enemies++)
		{
			for (int index_actions  = 0; index_actions < enemyManager->GetTurnOrders(index_enemies)->actions.Num(); index_actions++)
			{
				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase -  enemies -- adding action %d"), index_actions);

				FString id_enemy = enemyManager->GetEnemies()[index_enemies]->GetIDCharacter();
				individualActionOnTurns.Add(id_enemy, index_actions);
			}
		}
	}


	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase -  TOTAL  -- adding action %d"), individualActionOnTurns.Num());

	
	//VIEJO CODIGO
	/*
	//Distincion para players o enemigos
	//Calcular el numero de ticks a realizar (el de la lista mas larga)
	int numTicks = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		numTicks = enemyManager->GetMaxLenghtActions(EActionPhase::Attack);
	}
	
	
	for (int i = 0; i < numTicks; i++) {
			LogicAttackTick(i, numCharacters);
		
	}
	*/
}


//Control de los Ticks

void PD_GM_GameManager::LogicItemTick(int tick) {

}

void PD_GM_GameManager::LogicMoveTick(int tick, int numCharacters) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick"));

	for (int i = 0; i<numCharacters; i++) {
			//Distincion para players o enemigos
			TArray<FStructOrderAction> listMove; 
			PD_GM_LogicCharacter* logicCharacter=nullptr;
			if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
				if (!playersManager->GetDataStructPlayer(i)->logic_Character->GetIsStoppingByCollision() && playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove.Num() > tick)
				{
					
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : moviendo logic character"));

					playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(CheckAndManageCollisionWithPlayers(i, tick, numCharacters));

					playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(CheckAndManageCollisionWithEnemies(i, tick, numCharacters));

					playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(CheckAndManageCollisionWithMapElements(i, tick, numCharacters));

				}
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				
				if (!enemyManager->GetEnemies()[i]->GetIsStoppingByCollision() && enemyManager->getListTurnOrders()[i]->positionsToMove.Num() > tick)
				{
					enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(CheckAndManageCollisionWithPlayers(i, tick, numCharacters));

					enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(CheckAndManageCollisionWithEnemies(i, tick, numCharacters));

					enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(CheckAndManageCollisionWithMapElements(i, tick, numCharacters));

				}
			}
	
			
	}
	//Comprobar que choques.//Resolucion de conflictos
}




//Funciones de CHOQUE
bool  PD_GM_GameManager::CheckIsLogicCharacterInPosition(PD_MG_LogicPosition positionToCheck)
{
	//Devuelve true si hay algo en esa posicion - False si esta libre
	bool thereIsSomething = false;

	for (int i = 0; i < playersManager->GetNumPlayers(); i++) //Check For Players
	{
		if (positionToCheck == playersManager->GetDataStructPlayer(i)->logic_Character->GetCurrentLogicalPosition())
			thereIsSomething = true;
	}
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		if (positionToCheck == enemyManager->GetEnemies()[i]->GetCurrentLogicalPosition())
			thereIsSomething = true;
	}

	return thereIsSomething;

}


bool  PD_GM_GameManager::CheckAndManageCollisionWithPlayers(int indexDataPlayers, int tick, int numCharacters)
{
	/*
		Comprueba si ha habido choque con los otros jugadores durante la fase de movimiento
	1 - Comprueba su posicion con todos los otros jugadores en el tick
	2 - Si ha habido chocque, comprobar quien sale perdiendo. 
	3 - Actualizar las posiciones logicas respecto a esto -> movingLogicalPos.Add(nuevaposicion);
	4 - Actualizar al otro player su bool isStoppingPlayer
	5 - Seguir comprobando hasta que no queden jugadores.
	*/
	PD_MG_LogicPosition LogicPosPlayerToCheck;
	PD_MG_LogicPosition LogicPosOtherPlayerToCheck;
	bool isCollisionOrCollisionLost = false;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		  LogicPosPlayerToCheck = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionX,
			playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionY);
		  UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers %d %d"), playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionX
		  , playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionY);

	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		LogicPosPlayerToCheck = PD_MG_LogicPosition(enemyManager->getListTurnOrders()[indexDataPlayers]->positionsToMove[tick].positionX,
			enemyManager->getListTurnOrders()[indexDataPlayers]->positionsToMove[tick].positionY);
	}
	

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) //Los jugadores comprueban su posicion con el resto de jugadores en ese mismo tick
	{
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers 1"));

		for (int i = 0; i < numCharacters; i++)
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers 2"));

			if (indexDataPlayers != i && (playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove.Num()) > tick  )
			{
				LogicPosOtherPlayerToCheck = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[tick].positionX,
					playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[tick].positionY);
				if (LogicPosPlayerToCheck == LogicPosOtherPlayerToCheck) //Si las dos son iguales, se va a producir una collision -> se comprueba quien gana
				{
					if (playersManager->GetDataStructPlayer(indexDataPlayers)->logic_Character->GetTotalStats()->CH >=
						playersManager->GetDataStructPlayer(i)->logic_Character->GetTotalStats()->CH)
					{
						//El Jugador a comprobar gana el choque, El otro se pone su variable isStoppingByCollision a true y se busca una posicion a la que ir
						playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(true);

						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Adyacents_To(LogicPosOtherPlayerToCheck);
						for (int j = 0; j < possibleNewPositionToMove.Num(); j++)
						{
							if (!CheckIsLogicCharacterInPosition(possibleNewPositionToMove[j]))
								LogicPosOtherPlayerToCheck = possibleNewPositionToMove[j]; 
							break;
						}

						playersManager->GetDataStructPlayer(i)->logic_Character->AddMovementLogicalPosition(LogicPosOtherPlayerToCheck);

					}
					else
					{
						//El Jugador a comprobar pierde el choque, se setea la variable isCollisionOrCollisionLost a true y se da una nueva posicion del mapa a la que se desplaza el jugador
						isCollisionOrCollisionLost = true;
						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Adyacents_To(LogicPosPlayerToCheck);
						for (int j = 0; j < possibleNewPositionToMove.Num(); j++)
						{
							if (!CheckIsLogicCharacterInPosition(possibleNewPositionToMove[j]))
								LogicPosPlayerToCheck = possibleNewPositionToMove[j];
							break;
						}
					}
				}
			}
			
		}
		//Añadir al Array movingLogicalPosition la posicion que se esta comprobando para ver si se mueve ahi u es otra diferente por el choque
		playersManager->GetDataStructPlayer(indexDataPlayers)->logic_Character->AddMovementLogicalPosition(LogicPosPlayerToCheck);
		return isCollisionOrCollisionLost;
	}

	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) 
	{
		//A priori cuando es el turno de movimiento de los enemigos, no se sabe cuando pueden chocar con un player, para que sea correcto
		//los desplazamientos de los jugadores-enemigos, este se tiene que hacer cuando collision REALMENTE-- en el apartado logico no se puede hacer nada
		//A PRIORI!
		
		//Esto al menos para añadir la posicion, igual que hacemos al final para los players 
		enemyManager->GetEnemies()[indexDataPlayers]->AddMovementLogicalPosition(LogicPosPlayerToCheck);
	}
	
	return isCollisionOrCollisionLost;
}


bool  PD_GM_GameManager::CheckAndManageCollisionWithEnemies(int indexDataPlayers, int tick, int  numCharacters)
{
	PD_MG_LogicPosition LogicPosPlayerToCheck;
	PD_MG_LogicPosition LogicPosOtherPlayerToCheck;
	bool isCollisionOrCollisionLost = false;

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		LogicPosPlayerToCheck = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionX,
			playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionY);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		LogicPosPlayerToCheck = PD_MG_LogicPosition(0, 0);
		PD_MG_LogicPosition(enemyManager->getListTurnOrders()[indexDataPlayers]->positionsToMove[tick].positionX,
			enemyManager->getListTurnOrders()[indexDataPlayers]->positionsToMove[tick].positionY);
	}

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) 
	{
		//Al igual que en el metodo CheckPlayers -> en el turno de los jugadores es dificil decir cuando un enemigo con el que se ha producdido 
		//un choque se tiene que mover-> hay que hacerlo cuando se producza realmente el choque
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn)
	{
		//Cuando se chequean los enemigos, en la fase de movimiento todos se estan moviendo a la vez, hay que comprobar el tick de cada enemigo
		for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
		{
			if (indexDataPlayers != i && (enemyManager->getListTurnOrders()[i]->positionsToMove.Num()) > tick)
			{
				LogicPosOtherPlayerToCheck = PD_MG_LogicPosition(enemyManager->getListTurnOrders()[i]->positionsToMove[tick].positionX,
					enemyManager->getListTurnOrders()[i]->positionsToMove[tick].positionY);
				if (LogicPosPlayerToCheck == LogicPosOtherPlayerToCheck) //Si las dos son iguales, se va a producir una collision -> se comprueba quien gana
				{
					if (enemyManager->GetEnemies()[indexDataPlayers]->GetTotalStats()->CH >=
						enemyManager->GetEnemies()[i]->GetTotalStats()->CH)
					{
						//El Enemigo char a comprobar gana el choque, El otro se pone su variable isStoppingByCollision a true y se busca una posicion a la que ir
						enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(true);

						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Adyacents_To(LogicPosOtherPlayerToCheck);
						for (int j = 0; j < possibleNewPositionToMove.Num(); j++)
						{
							if (!CheckIsLogicCharacterInPosition(possibleNewPositionToMove[j]))
								LogicPosOtherPlayerToCheck = possibleNewPositionToMove[j];
							break;
						}

						enemyManager->GetEnemies()[i]->AddMovementLogicalPosition(LogicPosOtherPlayerToCheck);

					}
					else
					{
						//El Enemigo char a comprobar pierde el choque, se setea la variable isCollisionOrCollisionLost a true y se da una nueva posicion del mapa a la que se desplaza el jugador
						isCollisionOrCollisionLost = true;
						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Adyacents_To(LogicPosPlayerToCheck);
						for (int j = 0; j < possibleNewPositionToMove.Num(); j++)
						{
							if (!CheckIsLogicCharacterInPosition(possibleNewPositionToMove[j]))
								LogicPosPlayerToCheck = possibleNewPositionToMove[j];
							break;
						}
					}
				}
			}
		}
	}
	
	return isCollisionOrCollisionLost;
}



bool  PD_GM_GameManager::CheckAndManageCollisionWithMapElements(int indexDataPlayers, int tick, int  numCharacters)
{
	return false;
}



void PD_GM_GameManager::LogicAttackTick(int tick,int numCharacters) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicAttackTick"));


	for (int i = 0; i<numCharacters; i++) {

			//Distincion para players o enemigos
			TArray<FStructOrderAction> listAttack;
			PD_GM_LogicCharacter* logicCharacter = nullptr;
			if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
				if (playersManager->GetDataStructPlayer(i)->turnOrders->actions.Num() > tick)
				{
					//listAttack = playersManager->GetDataStructPlayer(i)->turnOrders->listAttack;
					logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
					//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
					//FStructOrderAction order = listAttack[tick];
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : atacando logic character"));
					//logicCharacter->ActionTo(order);
				}
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				//listAttack = enemyManager->GetTurnOrders(i)->listAttack;
				logicCharacter = enemyManager->GetEnemies()[i];
				//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
				//FStructOrderAction order = listAttack[tick];
				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : atacando logic character"));
				//logicCharacter->ActionTo(order);
			}


			//Logica del attack tick


			
	}


}
/*
void PD_GM_GameManager::VisualTickControl() {
	

	//Distincion para players o enemigos
	int maxLengthAction = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) {
		maxLengthAction = playersManager->GetMaxLenghtActions(structGameState->enumActionPhase);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
		maxLengthAction = enemyManager->GetMaxLenghtActions(structGameState->enumActionPhase);

	}
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualTickControl : Phase: % d  :maxLengthAction: %d"),  static_cast<uint8>(structGameState->enumActionPhase), maxLengthAction);
	//Logica del VisualTickControl

	if (structGameState->enumActionPhase == EActionPhase::Move) {
		if (maxLengthAction == 0) { //No hay mas acciones de mover
			structGameState->enumActionPhase = EActionPhase::Attack;
			VisualTickControl();//Llamar otra vez para que actualice el estado
		}
		else {
			VisualMoveTick();
		}
	} else if (structGameState->enumActionPhase == EActionPhase::Attack) {
		if (maxLengthAction == 0) { //No hay mas acciones de atacar
			structGameState->enumActionPhase = EActionPhase::EndPhase;
			//Fin del turno, hacer update de la maquina de estados del GameManager
			UpdateState();
		}
		else {
			VisualAttackTick();
		}
	}
}*/

void PD_GM_GameManager::VisualMoveTick() {
	//Array para guardar las nuevas posiciones de destinos y calcular el movimiento de la camara en la visualizacion
	TArray<PD_MG_LogicPosition> newPositionsToMove = TArray<PD_MG_LogicPosition>();

	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick"));
	int players = -1;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn)
		players = playersManager->GetNumPlayers();
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn)
		players = enemyManager->GetEnemies().Num();

	//Todos a la vez
	PD_GM_LogicCharacter* logicCharacter = nullptr;

	for (int i = 0; i < players; i++) {
		//Distincion para players o enemigos
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
			logicCharacter = enemyManager->GetEnemies()[i];
		}

		if (logicCharacter->GetMovingLogicalPosition().Num() > 0) //Si hay posiciones en este array, quiere decir que se tiene que mover
		{
			//Set un spline para dicho character
			if (!logicCharacter->GetController()->GetSpline())
			{
				logicCharacter->GetController()->SetSpline(splineManager->GetSpline());
			}

			TArray<FVector> positionsToMove = TArray<FVector>();
			//positionsToMove.Add(mapManager->LogicToWorldPosition(logicCharacter->GetCurrentLogicalPosition())); //Add the current poisition to start moving
			for (int j = 0; j < logicCharacter->GetMovingLogicalPosition().Num(); j++)
			{
				FVector v = mapManager->LogicToWorldPosition(logicCharacter->GetMovingLogicalPosition()[j]);
			v.Z = logicCharacter->GetCharacterBP()->GetActorLocation().Z;
				//v.Z = 45.0f;
				positionsToMove.Add(v);
				//positionsToMove.Add(mapManager->LogicToWorldPosition(logicCharacter->GetMovingLogicalPosition()[j]));
			}

			logicCharacter->MoveToPhysicalPosition(positionsToMove);

			//Actualizar la currentLogicPosition con el ultima posicion del array movingLogicalPosition
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick %d"), logicCharacter->GetMovingLogicalPosition().Num());
			if (logicCharacter->GetMovingLogicalPosition().Num() > 0) {
				logicCharacter->SetCurrentLogicalPosition(logicCharacter->GetMovingLogicalPosition()[logicCharacter->GetMovingLogicalPosition().Num() - 1]);
			}
		}
		
	}

	
}

void PD_GM_GameManager::VisualAttackTick(FString id_char, int index_action) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick"));

	//Distincion para players o enemigos
	//	TArray<FStructTargetToAction> listAttack;
	//int indexCharacter;
	//PD_GM_LogicCharacter* logicCharacter=nullptr;

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) 
	{
	
		if (playersManager->GetNumPlayers() > 0 )
		{
			PD_GM_LogicCharacter* logic_char = playersManager->GetCharacterByID(id_char);
	
			if (playersManager->GetStructPlayerByIDClient(id_char))
			{
				if (playersManager->GetStructPlayerByIDClient(id_char)->turnOrders->actions.Num() > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick - Si hay acciones que hacer"));

					FStructTargetToAction action = playersManager->GetStructPlayerByIDClient(id_char)->turnOrders->actions[index_action];
					logic_char->ActionTo(action);
				}
			}
			
			
//			playersManager->GetCharacterByID(id_char)->ActionTo(playersManager->GetStructPlayerByIDClient(id_char)->turnOrders->actions[index_action]);
		}

	}

	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) 
	{
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick enemigos"));

		int index_enemy = enemyManager->GetIndexByID(id_char);

		enemyManager->GetCharacterByID(id_char)->ActionTo(
			enemyManager->GetTurnOrders(index_enemy)->actions[index_action]);

	}

	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick despues de players/enemigos"));

}	


/*
void PD_GM_GameManager::CallbackMoveEnd() {
	//Mirar si todos han terminado de moverse y en ese caso pasar al siguiente tick
}

void PD_GM_GameManager::CallbackAnimEnd() {
	//mirar si todos han terminado su animacion y en ese caso pasar al siguiente tick
}

void PD_GM_GameManager::CallbackPlaySecondaryAnims() {
	//lanzar las animaciones secundarias. 
}
*/

void PD_GM_GameManager::OnAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnAnimationEnd"));

		if (playersManager->AllAnimationEnd() && enemyManager->AllAnimationEnd()) {
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnAnimationEnd: TRUE"));
			//Aqui deberia estar en un estado de fase que sea tick (no INI)

			
			if ((structGamePhase->enumGamePhase == EServerPhase::AttackTick))
			{
				index_IndividualActionsOnTurns++;

				UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: index - %d"), index_IndividualActionsOnTurns);

				UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: totalindex - %d"), individualActionOnTurns.Num());

				if (index_IndividualActionsOnTurns < individualActionOnTurns.Num())
				{
					TArray<FString> id_charactersOnArray;
					individualActionOnTurns.GenerateKeyArray(id_charactersOnArray);

					TArray<int> index_totalActions;
					individualActionOnTurns.GenerateValueArray(index_totalActions);

					FString id_char = id_charactersOnArray[index_IndividualActionsOnTurns];
					int index_actionOfChar = index_totalActions[index_IndividualActionsOnTurns];

					UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: lanzando accion de caract :%s , index  - %d"),*id_char, index_actionOfChar);

					VisualAttackTick(id_char, index_actionOfChar);
				}
				else
				{
					UpdatePhase();

				}
			}
			else 
			{
				UpdatePhase();

			}
			

		}

}

void PD_GM_GameManager::OnCameraEndMove() {
		UpdatePhase();
}

void PD_GM_GameManager::OnTimerEnd() {
	UpdatePhase();
}



#pragma region GM PHASE MACHINE 

void PD_GM_GameManager::InitPhase() {
	ChangePhase(EServerPhase::StartPhases);

}

// Esta funcion cambia el estado actual al pasado por parametros y llama al OnBeginState del mismo.
void PD_GM_GameManager::ChangePhase(EServerPhase newPhase)
{
	structGamePhase->enumGamePhase = newPhase;
	OnBeginPhase();
}

void PD_GM_GameManager::UpdatePhase()
{
	if (structGamePhase->enumGamePhase == EServerPhase::StartPhases)
	{
		
			ChangePhase(EServerPhase::ConsumableIni);
		
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::ConsumableIni)
	{
		if (!timer->isTimerRunning()) {
			ChangePhase(EServerPhase::ConsumableCamera);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::ConsumableCamera)
	{
		if (!structGamePhase->cameraMoving) {
			ChangePhase(EServerPhase::ConsumableTick);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::ConsumableTick)
	{
		ChangePhase(EServerPhase::MoveIni);
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveIni)
	{
		if (!timer->isTimerRunning()) {
			//Distincion para players o enemigos
			int maxLengthAction = 0;
			if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
				maxLengthAction = playersManager->GetMaxLenghtActions(EActionPhase::Move);
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				maxLengthAction = enemyManager->GetMaxLenghtActions(EActionPhase::Move);
			}

			if (maxLengthAction != 0) {
				ChangePhase(EServerPhase::MoveCamera); //Hay acciones por lo que volvemos a repetir el tick.
			}
			else {
				ChangePhase(EServerPhase::InteractionIni);
			}
		}

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveCamera)
	{
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
		if (Cast<AServerCamera>(SGI->CameraServer)->GetMoveState() == ECameraMoveState::EndMoving) {

			ChangePhase(EServerPhase::MoveTick);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveTick)
	{
		/*//Distincion para players o enemigos
		int maxLengthAction = 0;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
			maxLengthAction = playersManager->GetMaxLenghtActions(EActionPhase::Move);
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
			maxLengthAction = enemyManager->GetMaxLenghtActions(EActionPhase::Move);
		}

		if (maxLengthAction != 0) {
			ChangePhase(EServerPhase::MoveTick); //Hay acciones por lo que volvemos a repetir el tick.
		}
		else {*/
			ChangePhase(EServerPhase::InteractionIni);
	//	}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::InteractionIni)
	{
		if (!timer->isTimerRunning()) {
			ChangePhase(EServerPhase::InteractionCamera);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::InteractionCamera)
	{
		if (!structGamePhase->cameraMoving) {
			ChangePhase(EServerPhase::InteractionTick);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::InteractionTick)
	{
		ChangePhase(EServerPhase::AttackIni);
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackIni)
	{
		if (!timer->isTimerRunning()) {
			//Distincion para players o enemigos
			int maxLengthAction = 0;
			if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
				maxLengthAction = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				maxLengthAction = enemyManager->GetMaxLenghtActions(EActionPhase::Attack);
			}

			if (maxLengthAction != 0) {
				ChangePhase(EServerPhase::AttackCamera); //Hay acciones por lo que volvemos a repetir el tick.
			}
			else {
				ChangePhase(EServerPhase::EndAllPhases);
			}
		}

		
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackCamera)
	{
		if (!structGamePhase->cameraMoving) {
			ChangePhase(EServerPhase::AttackTick);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackTick)
	{
		//Distincion para players o enemigos
	/*	int maxLengthAction = 0;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
			maxLengthAction = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
			maxLengthAction = enemyManager->GetMaxLenghtActions(EActionPhase::Attack);
		}

		if (maxLengthAction != 0) {
			ChangePhase(EServerPhase::AttackTick); //Hay acciones por lo que volvemos a repetir el tick.
		}
		else {*/
			ChangePhase(EServerPhase::EndAllPhases);
		//}
	
	}
	else //Caso indeterminado
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::UpdatePhase: WARNING: estado sin inicializacion"));
	}

}

void PD_GM_GameManager::OnBeginPhase()
{
	if (structGamePhase->enumGamePhase == EServerPhase::StartPhases)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: StartPhases"));
		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::ConsumableIni)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: ConsumableIni"));
		
		timer->InitTimer(timeWaitingPhases);
		GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Red, FString::Printf(TEXT("Cartel de Inicio de consumible")));

		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::ConsumableCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: ConsumableCamera"));

		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::ConsumableTick)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: ConsumableTick"));
		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveIni)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: MoveIni"));
		//Limpiar todos los splines del splineManager para que todos los instanciados esten disponibles
		for (int i = 0; i < splineManager->splinesPool.Num(); i++)
		{
			splineManager->splinesPool[i]->RemovePoints();
		}

		//Llamar al procceso del movimiento logico
		PlayersLogicTurn();

		timer->InitTimer(timeWaitingPhases);
		GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Red, FString::Printf(TEXT("Cartel de Inicio de movimiento")));

		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: MoveCamera"));
		///FUNCION DE CAMARA
		int players = -1;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn)
			players = playersManager->GetNumPlayers();
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn)
			players = enemyManager->GetEnemies().Num();
		//Se llama a la Camara Server para que actulice su posicion - Se usa el SplineManager que es un actor para coger la referencia del Game Instance
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
		if (SGI)
		{
			//Pasas todo el Array de posiciones logicas a fisicas para el meotodo MOVER
			TArray<FVector> targetPositions = TArray<FVector>();
			for (int k = 0; k < players; k++)
			{
				if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
					//targetPositions.Add(mapManager->LogicToWorldPosition(
						//playersManager->GetDataPlayers()[k]->logic_Character->GetMovingLogicalPosition()[logicCharacter->GetMovingLogicalPosition().Num() - 1]));
					//	playersManager->GetDataPlayers()[k]->logic_Character->GetCurrentLogicalPosition()));
					if (playersManager->GetDataPlayers()[k]->turnOrders->positionsToMove.Num() > 0) {
						FStructLogicPosition pos = playersManager->GetDataPlayers()[k]->turnOrders->positionsToMove[playersManager->GetDataPlayers()[k]->turnOrders->positionsToMove.Num() - 1];

						targetPositions.Add(mapManager->LogicToWorldPosition(PD_MG_LogicPosition(pos.positionX, pos.positionY)));
						UE_LOG(LogTemp, Log, TEXT("Camera MOve adding logic: %s"), *(mapManager->LogicToWorldPosition(PD_MG_LogicPosition(pos.positionX, pos.positionY)).ToString()));

					}
				}
				if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
					targetPositions.Add(mapManager->LogicToWorldPosition(
						//enemyManager->GetEnemies()[k]->GetMovingLogicalPosition()[logicCharacter->GetMovingLogicalPosition().Num() - 1]));
						enemyManager->GetEnemies()[k]->GetCurrentLogicalPosition()));


				}
			}

			//Añades todas las posiciones de los jugadores del turno anterior al Array
			for (int z = 0; z < players; z++)
			{
				if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
					targetPositions.Add(playersManager->GetDataPlayers()[z]->logic_Character->GetCharacterBP()->GetActorLocation());
					UE_LOG(LogTemp, Log, TEXT("Camera MOve adding: %s"), *(playersManager->GetDataPlayers()[z]->logic_Character->GetCharacterBP()->GetActorLocation().ToString()));

				}
				if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
					targetPositions.Add(enemyManager->GetEnemies()[z]->GetCharacterBP()->GetActorLocation());
				}
			}

			//SGI->targetPositionsToCenterCamera = targetPositions;

			UE_LOG(LogTemp, Log, TEXT("Camera MOve from GameManager %d"), targetPositions.Num());
			FVector target = Cast<AServerCamera>(SGI->CameraServer)->FindAvaragePosition(targetPositions);

			Cast<AServerCamera>(SGI->CameraServer)->LookAtPoint(target);
			Cast<AServerCamera>(SGI->CameraServer)->MoveTo(FVector(target.X, target.Y,1000));

			FOutputDeviceNull ar;
			const FString command = FString::Printf(TEXT("ManageZoomWithTargetPositions")); //Funcion en BP de ServerCamera_GamePlay
																							//Cast<AServerCamera>(SGI->CameraServer)->CallFunctionByNameWithArguments(*command, ar, NULL, true);
		}

		UpdatePhase();

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveTick)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: MoveTick"));
		VisualMoveTick();

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::InteractionIni)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: InteractionIni"));
		timer->InitTimer(timeWaitingPhases);
		GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Red, FString::Printf(TEXT("Cartel de Inicio de interaccion")));

		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::InteractionCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: InteractionCamera"));
		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::InteractionTick)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: InteractionTick"));
		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackIni)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: AttackIni"));
		//Llamar al procceso del ataque logico

		timer->InitTimer(timeWaitingPhases);
		GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Red, FString::Printf(TEXT("Cartel de Inicio de ataque")));

		individualActionOnTurns.Empty(); //limpiar siempre el Tmap(), por lo que pueda pasr

		LogicTurnAttackPhase(); //va a calcular las acciones de TODOS los PLAYERS O ENEMIGOS de ese turno

		UpdatePhase();

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: AttackCamera"));
		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackTick)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: AttackTick"));
		
		//el index_IndividualActionsOnTurns es 0

		if (individualActionOnTurns.Num() > 0)
		{
			TArray<FString> id_charactersOnArray;
			individualActionOnTurns.GenerateKeyArray(id_charactersOnArray);

			TArray<int> index_totalActions;
			individualActionOnTurns.GenerateValueArray(index_totalActions);

			FString id_char = id_charactersOnArray[0]; //siempre el primero
			int index_actionOfChar = index_totalActions[0]; //siempre el primero

			VisualAttackTick(id_char, index_actionOfChar);
		}
		
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: EndAllPhases"));
		UpdateState(); //Devolvemos el flujo a la maquina de estados del game manager (no de fases)
	}else //Caso indeterminado
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: WARNING: estado sin inicializacion"));
	}

}
#pragma endregion

bool PD_GM_GameManager::CheckWinGameConditions()
{
	int enemiesDied = 0;

	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		if (enemyManager->GetEnemies()[i]->GetTotalStats()->HPCurrent <= 0)
		{
			enemiesDied++;
			enemyManager->GetEnemies()[i]->GetCharacterBP()->SetActorHiddenInGame(true);
		}
		
	}
	if (enemyManager->GetEnemies().Num() == enemiesDied)
		return true; //all enemies died
	else
		return false;
}

bool PD_GM_GameManager::CheckLoseGameConditions()
{
	bool playersLose = false;

	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		if (playersManager->GetDataPlayers()[i]->logic_Character->GetTotalStats()->HPCurrent <= 0)
			playersLose = true;
	}

	return playersLose;
}

#pragma region POINTSSYSTEM

void PD_GM_GameManager::UpdatePoints(PD_GM_LogicCharacter* player, PD_GM_LogicCharacter* enemy) {/// valdría con pasarles el id por ejemplo
	player->SetPoints(player->GetPoints() + enemy->GetPoints());
	TotalPoints += enemy->GetPoints();
}

#pragma endregion

#pragma region CHECK ACTIVEEFFECTS AND ALTEREDSTATE

//Funciones para comprobar los ActiveEffects y los AlteredState de enemigos y jugadores;
void PD_GM_GameManager::CheckAndUpdate_ActiveEffectsOnPlayers()
{
	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		for (auto& Elem : playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterState()->activeEffectsOnCharacter)
		{
			int id_af = Elem.Key;
			int cd_af = Elem.Value;
			cd_af--;
			if (cd_af <= 0)
				playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterState()->activeEffectsOnCharacter.Remove(id_af);
			else
				playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterState()->activeEffectsOnCharacter[id_af] = cd_af;
		}
	}
}

void PD_GM_GameManager::CheckAndUpdate_ActiveEffectsOnEnemies()
{
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		for (auto& Elem : enemyManager->GetEnemies()[i]->GetCharacterState()->activeEffectsOnCharacter)
		{
			int id_af = Elem.Key;
			int cd_af = Elem.Value;
			cd_af--;
			if (cd_af <= 0)
				enemyManager->GetEnemies()[i]->GetCharacterState()->activeEffectsOnCharacter.Remove(id_af);
			else
				enemyManager->GetEnemies()[i]->GetCharacterState()->activeEffectsOnCharacter[id_af] = cd_af;
		}
	}
}

void PD_GM_GameManager::CheckAndUpdate_AlteredStateOnPlayers()
{
	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		for (auto& Elem : playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterState()->alteredCharacterState)
		{
			int id_af = Elem.Key;
			int cd_af = Elem.Value;
			cd_af--;
			if (cd_af <= 0)
				playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterState()->alteredCharacterState.Remove(id_af);
			else
				playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterState()->alteredCharacterState[id_af] = cd_af;
		}
	}
}

void PD_GM_GameManager::CheckAndUpdate_AlteredStateOnEnemies()
{
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		for (auto& Elem : enemyManager->GetEnemies()[i]->GetCharacterState()->alteredCharacterState)
		{
			int id_af = Elem.Key;
			int cd_af = Elem.Value;
			cd_af--;
			if (cd_af <= 0)
				enemyManager->GetEnemies()[i]->GetCharacterState()->alteredCharacterState.Remove(id_af);
			else
				enemyManager->GetEnemies()[i]->GetCharacterState()->alteredCharacterState[id_af] = cd_af;
		}
	}
}

#pragma endregion