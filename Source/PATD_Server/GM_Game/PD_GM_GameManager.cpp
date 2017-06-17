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
#include "MapInfo/PD_MM_MapInfo.h"
#include "Actors/Interactuables/PD_E_Interactuable.h"
#include "Actors/Interactuables/PD_E_Door.h"


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
#include "Actors/PD_E_Character.h"
#include "ServerCamera.h"
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
	
	if (playersManager->GetNumPlayers() >= inPlayer + 1)
	{
		if (!playersManager->GetDataStructPlayer(inPlayer)->isConnected) //si el que envia esta a no conectado
			playersManager->GetDataStructPlayer(inPlayer)->isConnected = true;
	}

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

		characterWhoPlayDefenseAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de defensa
		characterWhoPlayGetHurtAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de herida
		characterWhoPlayHealAnim.Empty();    //limpiamos los characters que se tienen que tienen que lanzar animacion de Curar

		if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases) {
			//Actualizar estados alteadores y de efectos en enemigos --- despues del turno de los players
			CheckAndUpdate_ActiveEffectsOnEnemies();
			CheckAndUpdate_AlteredStateOnEnemies();
			if(CheckWinGameConditions())
			{
				this->ChangeState(EGameState::EndOfTurn);
			}
			else 
			{
				this->ChangeState(EGameState::WaitingEnemiesOrders);
			}
		}
		

	}else if (structGameState->enumGameState == EGameState::WaitingEnemiesOrders) {

		//Transiciones de estados
		if (enemyManager->AllEnemiesHaveOrders()) {
			this->ChangeState(EGameState::ExecutingEnemiesTurn);
		}

	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {

		characterWhoPlayDefenseAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de defensa
		characterWhoPlayGetHurtAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de herida
		characterWhoPlayHealAnim.Empty();    //limpiamos los characters que se tienen que tienen que lanzar animacion de Curar

		if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases) {
			//Actualizar activeeffects and alteredState de los jugadores --- justo antes de actualizarlos en el server
			CheckAndUpdate_ActiveEffectsOnPlayers();
			CheckAndUpdate_AlteredStateOnPlayers();
			this->ChangeState(EGameState::EndOfTurn);
		}
	

	}else if (structGameState->enumGameState == EGameState::EndOfTurn) {

		characterWhoPlayDefenseAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de defensa
		characterWhoPlayGetHurtAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de herida
		characterWhoPlayHealAnim.Empty();    //limpiamos los characters que se tienen que tienen que lanzar animacion de Curar

		if (CheckWinGameConditions()) //Jugadores Ganan la partida
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn: GANAN PLAYERS"));
			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
			if (SGI)
			{
				MatchIsWinOrLost = 1; //Victoria
				SGI->UpdateState();
			}
		}
		else if (CheckLoseGameConditions()) //Jugadores pierden la partida
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn: PIERDEN PLAYERS"));
			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
			if (SGI)
			{
				MatchIsWinOrLost = 2; //Derrota

				SGI->UpdateState();
			}
		}
		else
		{
			//Transicion inmediata de estado
			this->ChangeState(EGameState::WaitingPlayerOrders);
		}
			
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

		//Popner la camara a patrullar
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
		FVector target = Cast<AServerCamera>(SGI->CameraServer)->GetPlayersAveragePosition();
		Cast<AServerCamera>(SGI->CameraServer)->InitPatrol(target);
		Cast<AServerCamera>(SGI->CameraServer)->LookAtPoint(target);
		Cast<AServerCamera>(SGI->CameraServer)->SetCameraOnView();

		//setar la vida en el widget de los jugadores
		for (int index_player = 0; index_player < playersManager->GetNumPlayers(); index_player++)
		{
			Cast<APD_E_Character>(playersManager->GetDataStructPlayer(index_player)->logic_Character->GetCharacterBP())->UpdateStringHP();
		}
		for (int index_enemy = 0; index_enemy < enemyManager->GetEnemies().Num(); index_enemy++)
		{
			Cast<APD_E_Character>(enemyManager->GetEnemies()[index_enemy]->GetCharacterBP())->UpdateStringHP();
		}
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

		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
		if (SGI)
		{
			Cast<AServerCamera>(SGI->CameraServer)->SetCameraOnView();

			//Popner la camara a patrullar
			SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
			//FVector target = Cast<AServerCamera>(SGI->CameraServer)->GetPlayersAveragePosition();
			//Cast<AServerCamera>(SGI->CameraServer)->InitPatrol(target);
			//Cast<AServerCamera>(SGI->CameraServer)->LookAtPoint(target);
		}

		
		enemyManager->newTurn();
		CreateEnemyOrders();
		//update life for widget
		for (int index_player = 0; index_player < playersManager->GetNumPlayers(); index_player++)
		{
			Cast<APD_E_Character>(playersManager->GetDataStructPlayer(index_player)->logic_Character->GetCharacterBP())->UpdateStringHP();
		}
		for (int index_enemy = 0; index_enemy < enemyManager->GetEnemies().Num(); index_enemy++)
		{
			Cast<APD_E_Character>(enemyManager->GetEnemies()[index_enemy]->GetCharacterBP())->UpdateStringHP();
		}
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
		UpdatePoints();
		//Limpiar que el player este parado por haber perdido una collision
		for (int i = 0; i < playersManager->GetNumPlayers(); i++)
		{
			playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(false); //limpiamos que se hayan quedado parados por collision
			Cast<APD_E_Character>(playersManager->GetDataStructPlayer(i)->logic_Character->GetCharacterBP())->StopAnimationParticleSystem();
		}
		for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
		{
			enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(false); //limpiamos que se hayan quedado parados por collision
			Cast<APD_E_Character>(enemyManager->GetEnemies()[i]->GetCharacterBP())->StopAnimationParticleSystem();

		}
		//Popner la camara a patrullar
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
		Cast<AServerCamera>(SGI->CameraServer)->SetCameraOnView();
		//FVector target = Cast<AServerCamera>(SGI->CameraServer)->GetPlayersAveragePosition();
		//Cast<AServerCamera>(SGI->CameraServer)->InitPatrol(target);
		//Cast<AServerCamera>(SGI->CameraServer)->LookAtPoint(target);

		//Hay que hacer lo necesario (borrar las cosas de este turno) para que se pueda recibir otro normalmente.
		for (StructPlayer* structPlayer : playersManager->GetDataPlayers()) {
			structPlayer->turnOrders->positionsToMove.Empty();
			structPlayer->turnOrders->actions.Empty();
			structPlayer->playerSendOrder = false;
			structPlayer->turnOrders->consumablesToConsume.Empty();
			structPlayer->turnOrders->interactuablesToInteract.Empty();

		}

		//Enviar a cliente actualizacion del mapa
		FStructUpdateTurn structUpdateTurn = GenerateStructUpdateTurn();

		//listOfRoomsInstiantate.Empty(); //limpiamos las salas para el siguiente turno

		//doorsOpened.Empty();
		//Envio a todos los clientes con el update del turno
		networkManager->SendNow(&structUpdateTurn);

		
		UpdateState();//transicion inmediata
		

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



FStructUpdateTurn PD_GM_GameManager::GenerateStructUpdateTurn() {
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
		structUpdateCharacter.HPCurrent = logicCharacter->GetTotalStats()->HPCurrent;
		structUpdateCharacter.PointsCurrent = logicCharacter->GetTotalStats()->PointsCurrent;
		structUpdateCharacter.currentCharacterPosition = logicPosition;
		structUpdateCharacter.ID_character = logicCharacter->GetIDCharacter();
		structUpdateTurn.listPlayerCharacters.Add(structUpdateCharacter);

		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager:: player con id %s"), *structUpdateCharacter.ID_character);
	}
	//Enemigos
	for (int iEnemies = 0; iEnemies < enemyManager->GetEnemies().Num(); iEnemies++) {
		PD_GM_LogicCharacter* logicCharacter = enemyManager->GetEnemies()[iEnemies];
		FStructUpdateCharacter structUpdateCharacter;
		//Conversion de Struct a LogicPosition
		FStructLogicPosition logicPosition = FStructLogicPosition();
		logicPosition.positionX = logicCharacter->GetCurrentLogicalPosition().GetX();
		logicPosition.positionY = logicCharacter->GetCurrentLogicalPosition().GetY();
		structUpdateCharacter.HPCurrent = logicCharacter->GetTotalStats()->HPCurrent;

		structUpdateCharacter.currentCharacterPosition = logicPosition;
		//	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn: id Enemy:%s" ), *logicCharacter->GetIDCharacter());
		structUpdateCharacter.ID_character = logicCharacter->GetIDCharacter();
		structUpdateTurn.listEnemyCharacters.Add(structUpdateCharacter);
	}

	//Update de Salas del mapa

	structUpdateTurn.listOfRoomsInstiantate = listOfRoomsInstiantate;

	structUpdateTurn.listOfDoorOpend = doorsOpened;

	return structUpdateTurn;
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
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CreateEnemyOrders, num %d ----- enemyID:%s"), i, *enemyManager->GetEnemies()[i]->GetIDCharacter());
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
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		//numCharacters = playersManager->GetNumPlayers();
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		for (int iEnemy = 0; iEnemy < enemyManager->GetEnemies().Num(); iEnemy++) {
			FStructTurnOrders* turnOrder= enemyManager->GetTurnOrders(iEnemy);
			if (turnOrder)
			{
				if (turnOrder->consumablesToConsume.Num() > 0) {
					enemyManager->GetEnemies()[iEnemy]->ConsumeItem(turnOrder->consumablesToConsume[0]);
				}
			}
			
		}
	}
	
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
void PD_GM_GameManager::LogicTurnInteractablePhase()
{
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnInteractablePhase"));

	index_individualActionInteractablesOnTurns = 0;

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		for (int index_players = 0; index_players < playersManager->GetNumPlayers(); index_players++)
		{		
				for (int index_actions = 0; index_actions < playersManager->GetDataStructPlayer(index_players)->turnOrders->interactuablesToInteract.Num(); index_actions++)
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnInteractablePhase -  players -- adding interaction %d"), index_actions);

					FString id_player = playersManager->GetDataStructPlayer(index_players)->logic_Character->GetIDCharacter();
					int id_interc = playersManager->GetDataStructPlayer(index_players)->turnOrders->interactuablesToInteract[index_actions];
					individualActionInteractablesOnTurns.Add(id_player, id_interc);
				}
		}
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {

		for (int index_enemies = 0; index_enemies < enemyManager->GetEnemies().Num(); index_enemies++)
		{
				if (enemyManager->GetTurnOrders(index_enemies))
				{
					for (int index_actions = 0; index_actions < enemyManager->GetTurnOrders(index_enemies)->interactuablesToInteract.Num(); index_actions++)
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnInteractablePhase -  enemies -- adding interaction %d"), index_actions);

						FString id_enemy = enemyManager->GetEnemies()[index_enemies]->GetIDCharacter();
						int id_interc = enemyManager->GetTurnOrders(index_enemies)->interactuablesToInteract[index_actions];
						individualActionInteractablesOnTurns.Add(id_enemy, id_interc);
					}
				}
		}
	}


	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnInteractablePhase -  TOTAL  -- adding interaction %d"), individualActionInteractablesOnTurns.Num());

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
				if (enemyManager->getListTurnOrders().Num() > 0) // hay ordenes
				{
					if (enemyManager->GetTurnOrders(index_enemies))
					{
						for (int index_actions = 0; index_actions < enemyManager->GetTurnOrders(index_enemies)->actions.Num(); index_actions++)
						{
							UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase -  enemies -- adding action %d"), index_actions);

							FString id_enemy = enemyManager->GetEnemies()[index_enemies]->GetIDCharacter();
							individualActionOnTurns.Add(id_enemy, index_actions);
						}
					}
				}
		}
	}


	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase -  TOTAL  -- adding action %d"), individualActionOnTurns.Num());

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
				//	playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(CheckAndManageCollisionWithCharacters(i, tick, numCharacters));
					PD_MG_LogicPosition logicPosition = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[tick].positionX, playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[tick].positionY);
					playersManager->GetDataStructPlayer(i)->logic_Character->AddMovementLogicalPosition(logicPosition);
				}
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				
				if (enemyManager->getListTurnOrders()[i])
				{
					if (!enemyManager->GetEnemies()[i]->GetIsStoppingByCollision() && enemyManager->getListTurnOrders()[i]->positionsToMove.Num() > tick)
					{
						//enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(CheckAndManageCollisionWithCharacters(i, tick, numCharacters));
						PD_MG_LogicPosition logicPosition = PD_MG_LogicPosition(enemyManager->GetTurnOrders(i)->positionsToMove[tick].positionX, enemyManager->GetTurnOrders(i)->positionsToMove[tick].positionY);
						enemyManager->GetEnemies()[i]->AddMovementLogicalPosition(logicPosition);
					}
				}
			}			
	}
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
		if (playersManager->GetDataStructPlayer(i)->turnOrders)
		{
			for (int posMoving = 0; posMoving < playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove.Num(); posMoving++)
			{
				PD_MG_LogicPosition positionInMoving = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[posMoving].positionX,
					playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[posMoving].positionY);
				if (positionToCheck == positionInMoving)
					thereIsSomething = true;
			}
		}
	}
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		if (positionToCheck == enemyManager->GetEnemies()[i]->GetCurrentLogicalPosition())
			thereIsSomething = true;
		if (enemyManager->getListTurnOrders().Num() > 0)
		{
			for (int posMoving = 0; posMoving < enemyManager->GetTurnOrders(i)->positionsToMove.Num(); posMoving++)
			{
				PD_MG_LogicPosition positionInMoving = PD_MG_LogicPosition(enemyManager->GetTurnOrders(i)->positionsToMove[posMoving].positionX,
					enemyManager->GetTurnOrders(i)->positionsToMove[posMoving].positionY);
				if (positionToCheck == positionInMoving)
					thereIsSomething = true;
			}
		}
		
	}

	return thereIsSomething;

}

//Funciones de CHOQUE
bool  PD_GM_GameManager::CollisionCheckIsLogicCharacterInPosition(PD_MG_LogicPosition positionToCheck)
{
	//Devuelve true si hay algo en esa posicion - False si esta libre
	bool thereIsSomething = false;

	for (int i = 0; i < playersManager->GetNumPlayers(); i++) //Check For Players
	{
		if (positionToCheck == playersManager->GetDataStructPlayer(i)->logic_Character->GetCurrentLogicalPosition())
			thereIsSomething = true;
		else if (playersManager->GetCharacterByIndex(i)->GetController()
			&& playersManager->GetCharacterByIndex(i)->GetController()->GetSpline()
			&& playersManager->GetCharacterByIndex(i)->GetController()->GetSpline()->GetSplineComponent()->GetNumberOfSplinePoints() > 0) 
		{
			FVector lastPosition = playersManager->GetCharacterByIndex(i)->GetController()->GetSpline()->GetSplineComponent()->GetLocationAtSplinePoint(playersManager->GetCharacterByIndex(i)->GetController()->GetSpline()->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
			
			
			PD_MG_LogicPosition positionInMoving = mapManager->WorldToLogicPosition(lastPosition);
			if (positionToCheck == positionInMoving) {
				thereIsSomething = true;
			}

		}
		
	}
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		if (positionToCheck == enemyManager->GetEnemies()[i]->GetCurrentLogicalPosition())
			thereIsSomething = true;

		else if (enemyManager->GetCharacterByIndex(i)->GetController()
			&& enemyManager->GetCharacterByIndex(i)->GetController()->GetSpline()
			&& enemyManager->GetCharacterByIndex(i)->GetController()->GetSpline()->GetSplineComponent()->GetNumberOfSplinePoints() > 0)
		{
			FVector lastPosition = enemyManager->GetCharacterByIndex(i)->GetController()->GetSpline()->GetSplineComponent()->GetLocationAtSplinePoint(enemyManager->GetCharacterByIndex(i)->GetController()->GetSpline()->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);


			PD_MG_LogicPosition positionInMoving = mapManager->WorldToLogicPosition(lastPosition);
			if (positionToCheck == positionInMoving) {
				thereIsSomething = true;
			}

		}

	}

	return thereIsSomething;

}

bool  PD_GM_GameManager::CheckAndManageCollisionWithCharacters(int indexDataPlayers, int tick, int numCharacters)
{
	/*
		Comprueba si ha habido choque con los otros jugadores durante la fase de movimiento
	1 - Comprueba su posicion con todos los otros jugadores en el tick
	2 - Si ha habido chocque, comprobar quien sale perdiendo. 
	3 - Actualizar las posiciones logicas respecto a esto -> movingLogicalPos.Add(nuevaposicion);
	4 - Actualizar al otro player su bool isStoppingPlayer
	5 - Seguir comprobando hasta que no queden jugadores.
	*/
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters "));

	PD_MG_LogicPosition LogicPosPlayerToCheck;
	PD_MG_LogicPosition LogicPosOtherPlayerToCheck;
	bool isCollisionOrCollisionLost = false;

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		  LogicPosPlayerToCheck = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionX,
			playersManager->GetDataStructPlayer(indexDataPlayers)->turnOrders->positionsToMove[tick].positionY);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		LogicPosPlayerToCheck = PD_MG_LogicPosition(enemyManager->getListTurnOrders()[indexDataPlayers]->positionsToMove[tick].positionX,
			enemyManager->getListTurnOrders()[indexDataPlayers]->positionsToMove[tick].positionY);
	}
	

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) //Los jugadores comprueban su posicion con el resto de jugadores en ese mismo tick
	{
		//UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers 1"));

		for (int i = 0; i < numCharacters; i++)
		{
			//UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers 2"));

			if (indexDataPlayers != i && (playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove.Num()) > tick  )
			{
				LogicPosOtherPlayerToCheck = PD_MG_LogicPosition(playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[tick].positionX,
					playersManager->GetDataStructPlayer(i)->turnOrders->positionsToMove[tick].positionY);
				if (LogicPosPlayerToCheck == LogicPosOtherPlayerToCheck) //Si las dos son iguales, se va a producir una collision -> se comprueba quien gana
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters -- se va a producir un choque entre %s y %s"), *playersManager->GetDataStructPlayer(indexDataPlayers)->logic_Character->GetIDCharacter(),
						*playersManager->GetDataStructPlayer(i)->logic_Character->GetIDCharacter());

					if (playersManager->GetDataStructPlayer(indexDataPlayers)->logic_Character->GetTotalStats()->CH >=
						playersManager->GetDataStructPlayer(i)->logic_Character->GetTotalStats()->CH)
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters --Jugador que estaba comprobando GANA el choque"));
						//El Jugador a comprobar gana el choque, El otro se pone su variable isStoppingByCollision a true y se busca una posicion a la que ir
						playersManager->GetDataStructPlayer(i)->logic_Character->SetIsStoppingByCollision(true);

						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Diagonals_And_Adyacents_To(LogicPosOtherPlayerToCheck);
						for (int32 j = 0; j < possibleNewPositionToMove.Num(); j++) //Shuffel de posibles posiciones para mover
						{
							int index = rand() % (j + 1);
							possibleNewPositionToMove.Swap(j, index);
						}
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
						UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters --Jugador que estaba comprobando PIERDE el choque"));
						//El Jugador a comprobar pierde el choque, se setea la variable isCollisionOrCollisionLost a true y se da una nueva posicion del mapa a la que se desplaza el jugador
						isCollisionOrCollisionLost = true;
						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Diagonals_And_Adyacents_To(LogicPosPlayerToCheck);
						for (int32 j = 0; j < possibleNewPositionToMove.Num(); j++) //Shuffel de posibles posiciones para mover
						{
							int index = rand() % (j + 1);
							possibleNewPositionToMove.Swap(j, index);
						}

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
		//UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers 1"));

		for (int i = 0; i < numCharacters; i++)
		{
			//UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithPlayers 2"));

			if (indexDataPlayers != i && (enemyManager->GetTurnOrders(i)->positionsToMove.Num()) > tick)
			{
				LogicPosOtherPlayerToCheck = PD_MG_LogicPosition(enemyManager->GetTurnOrders(i)->positionsToMove[tick].positionX,
					enemyManager->GetTurnOrders(i)->positionsToMove[tick].positionY);
				if (LogicPosPlayerToCheck == LogicPosOtherPlayerToCheck) //Si las dos son iguales, se va a producir una collision -> se comprueba quien gana
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters -- ENEMIGOS se va a producir un choque entre %s y %s"), *enemyManager->GetEnemies()[indexDataPlayers]->GetIDCharacter(),
						*enemyManager->GetEnemies()[i]->GetIDCharacter());
					if (enemyManager->GetEnemies()[indexDataPlayers]->GetTotalStats()->CH >=
						enemyManager->GetEnemies()[i]->GetTotalStats()->CH)
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters --Enemigo que estaba comprobando GANA el choque"));

						//El Jugador a comprobar gana el choque, El otro se pone su variable isStoppingByCollision a true y se busca una posicion a la que ir
						enemyManager->GetEnemies()[i]->SetIsStoppingByCollision(true);

						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Diagonals_And_Adyacents_To(LogicPosOtherPlayerToCheck);
						for (int32 j = 0; j < possibleNewPositionToMove.Num(); j++) //Shuffel de posibles posiciones para mover
						{
							int index = rand() % (j + 1);
							possibleNewPositionToMove.Swap(j, index);
						}

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
						UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::CheckAndManageCollisionWithCharacters --Enemigo que estaba comprobando PIERDE el choque"));

						//El Jugador a comprobar pierde el choque, se setea la variable isCollisionOrCollisionLost a true y se da una nueva posicion del mapa a la que se desplaza el jugador
						isCollisionOrCollisionLost = true;
						TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapManager->Get_LogicPosition_Diagonals_And_Adyacents_To(LogicPosPlayerToCheck);
						for (int32 j = 0; j < possibleNewPositionToMove.Num(); j++) //Shuffel de posibles posiciones para mover
						{
							int index = rand() % (j + 1);
							possibleNewPositionToMove.Swap(j, index);
						}

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
		enemyManager->GetEnemies()[indexDataPlayers]->AddMovementLogicalPosition(LogicPosPlayerToCheck);
		return isCollisionOrCollisionLost;

	}
	
	return isCollisionOrCollisionLost;
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
		if (!logicCharacter->GetIsPlayer())
			positionsToMove.Add(mapManager->LogicToWorldPosition(logicCharacter->GetCurrentLogicalPosition())); //Add the current poisition to start moving
		
		for (int j = 0; j < logicCharacter->GetMovingLogicalPosition().Num(); j++)
		{
			FVector v = mapManager->LogicToWorldPosition(logicCharacter->GetMovingLogicalPosition()[j]);
			//v.Z = logicCharacter->GetCharacterBP()->GetActorLocation().Z;
			v.Z = 10.0f;
			positionsToMove.Add(v);
		}

		logicCharacter->MoveToPhysicalPosition(positionsToMove);
	}
	else {
		logicCharacter->GetController()->OnAnimationEnd();
	}

}


}


void PD_GM_GameManager::VisualInteractbaleTick(FString id_char, int id_interact)
{
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualInteractbaleTick -- idInteract %d"), id_interact);

	/*
	- Para usar el interactuable
	 1. Animacion de uso de interactuable
	 2. Encontrar el interactuable por ID
	 3. Conseguir su tipo de interactuable
	 4. Hacer un Case
		4.1 Si es una puerta: Acceder a las id de roomA y roomB, instanciarlas ambas, instanciar enemigos
	*/

	StaticMapElement typeInteract;

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn)
	{
		PD_GM_LogicCharacter* logic_char = playersManager->GetCharacterByID(id_char);

		if (mapManager->MapInfo->interactuableInfoInMap.Num() > 0 )
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualInteractbaleTick -- Hay info interactublaInfoMap"));

			for (int i = 0; i < mapManager->MapInfo->interactuableInfoInMap.Num(); i++)
			{
				if (mapManager->MapInfo->interactuableInfoInMap[i]->IDInteractuable == id_interact) //encuentra el interactuable
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualInteractbaleTick -- Ha encontrado el interactuable"));

					typeInteract = mapManager->MapInfo->interactuableInfoInMap[i]->type;
				}
			}

			switch (StaticMapElement(typeInteract))
			{
			case StaticMapElement::PROP_CHEST:
				break;
			case StaticMapElement::DOOR:
			{
				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualInteractbaleTick -- Es una puerta"));

				int idRoomA = mapManager->MapInfo->doorInfoByID[id_interact]->room_ConnA->IDRoom;
				int idRoomB = mapManager->MapInfo->doorInfoByID[id_interact]->room_ConnB->IDRoom;

				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualInteractbaleTick -- room A es: %d"), idRoomA);
				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualInteractbaleTick -- room B es: %d"), idRoomB);


				mapManager->InstantiateRoomAndAdj(idRoomA);
				mapManager->InstantiateRoomAndAdj(idRoomB);
				
				//metemos en la lista de habitaciones instanciadas las que se van a instanciar
				listOfRoomsInstiantate.Add(idRoomA);
				listOfRoomsInstiantate.Add(idRoomB);

				mapManager->InstantiateEnemies();

				APD_E_Door* doorOpend = nullptr;
				doorOpend = mapManager->MapInfo->doorActorByID[id_interact];
				if (doorOpend)
				{
					doorOpend->IsDoorOpen = true;
					doorsOpened.Add(doorOpend->ID_Interactuable);
					doorOpend->Interact(nullptr);
					//doorOpend->SetActorHiddenInGame(true);
					logic_char->GetController()->UpdateRotationCharacterToEnemy(doorOpend->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar
					logic_char->UseInteractable();

				}
				break;
			}
			case StaticMapElement::LEVEL:
				break;
			case StaticMapElement::PRESURE_PLATE:
				break;
			case StaticMapElement::LARGE_CHEST:
				break;
			case StaticMapElement::SMALL_CHEST:
				break;
			default:
				break;
			}
		}
	}

	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn)
	{

	}
}



void PD_GM_GameManager::VisualAttackTick(FString id_char, int index_action) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick"));

	characterWhoPlayDefenseAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de defensa
	characterWhoPlayGetHurtAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de herida
	characterWhoPlayHealAnim.Empty();    //limpiamos los characters que se tienen que tienen que lanzar animacion de Curar

	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) 
	{
	
		if (playersManager->GetNumPlayers() > 0 )
		{
			PD_GM_LogicCharacter* logic_char = playersManager->GetCharacterByID(id_char);
	
			if (playersManager->GetStructPlayerByIDCharacter(id_char))
			{
				if (playersManager->GetStructPlayerByIDCharacter(id_char)->turnOrders->actions.Num() > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick - Si hay acciones que hacer"));

					Cast<APD_E_Character>(logic_char->GetCharacterBP())->SetCharacterCameraOnView();

					FStructTargetToAction action = playersManager->GetStructPlayerByIDCharacter(id_char)->turnOrders->actions[index_action];
					logic_char->ActionTo(action);
				}
			}			
		}

	}

	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) 
	{

		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick enemigos"));

		if (enemyManager->GetEnemies().Num() > 0)
		{
			int index_enemy = enemyManager->GetIndexByID(id_char);

			if (enemyManager->GetTurnOrders(index_enemy)->actions.Num() > 0 && !enemyManager->GetCharacterByID(id_char)->GetIsDead())
			{
				Cast<APD_E_Character>(enemyManager->GetCharacterByID(id_char)->GetCharacterBP())->SetCharacterCameraOnView();

				enemyManager->GetCharacterByID(id_char)->ActionTo(
					enemyManager->GetTurnOrders(index_enemy)->actions[index_action]);
			}
		}
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

			CheckIfCharacterIsDead_ForHidden(); //Poner en invisible a los que han muerto en un turno (se destruiran despues)

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
					UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: UpdatePhase Desde Ataque"));

					UpdatePhase();

				}
			}
			else if ((structGamePhase->enumGamePhase == EServerPhase::InteractionTick))
			{
				index_individualActionInteractablesOnTurns++;

				UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: index - %d"), index_IndividualActionsOnTurns);

				UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: totalindex - %d"), individualActionOnTurns.Num());

				if (index_individualActionInteractablesOnTurns < individualActionInteractablesOnTurns.Num())
				{
					TArray<FString> id_charactersOnArray;
					individualActionOnTurns.GenerateKeyArray(id_charactersOnArray);

					TArray<int> index_totalActions;
					individualActionOnTurns.GenerateValueArray(index_totalActions);

					FString id_char = id_charactersOnArray[index_IndividualActionsOnTurns];
					int id_interact = index_totalActions[index_IndividualActionsOnTurns];

					UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: lanzando interactuable de caract :%s , id  - %d"), *id_char, id_interact);

					VisualInteractbaleTick(id_char, id_interact);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: UpdatePhase Desde interaccion"));

					UpdatePhase();
				}
			}
			else 
			{
				UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnAnimationEnd: UpdatePhase Desde movimiento o consumable"));

				UpdatePhase();

			}
			

		}

}

void PD_GM_GameManager::PlayAnimationOnCharacters_HurtDefenseHeal()
{
	UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::PlayAnimationOnCharacters_HurtDefenseHeal: Entrando"));

	/*for (int character = 0; character < characterWhoPlayDefenseAnim.Num(); character++)
	{
		Cast<APD_E_Character>(characterWhoPlayDefenseAnim[character]->GetCharacterBP())->UpdateStateActionOnChar();

		characterWhoPlayDefenseAnim[character]->GetController()->Animation_DefenseChar((int)ActiveSkills::Defense);
	}*/
	while (characterWhoPlayDefenseAnim.Num() > 0) {
		Cast<APD_E_Character>(characterWhoPlayDefenseAnim[0]->GetCharacterBP())->UpdateStateActionOnChar();

		characterWhoPlayDefenseAnim[0]->GetController()->Animation_DefenseChar((int)ActiveSkills::Defense);

		characterWhoPlayDefenseAnim.RemoveAt(0);
		//characterWhoPlayDefenseAnim.Pop

	}
	/*
	for (int character = 0; character < characterWhoPlayGetHurtAnim.Num(); character++)
	{
		Cast<APD_E_Character>(characterWhoPlayGetHurtAnim[character]->GetCharacterBP())->UpdateStringHP();
		Cast<APD_E_Character>(characterWhoPlayGetHurtAnim[character]->GetCharacterBP())->UpdateStateActionOnChar();
		if (characterWhoPlayGetHurtAnim[character]->GetTotalStats()->HPCurrent <= 0.0f)
			characterWhoPlayGetHurtAnim[character]->GetController()->Animation_DeathChar((int)ActiveSkills::GetHurt);
		else
			characterWhoPlayGetHurtAnim[character]->GetController()->Animation_GetHurt((int)ActiveSkills::GetHurt);
	}*/
	while (characterWhoPlayGetHurtAnim.Num() > 0) {
		Cast<APD_E_Character>(characterWhoPlayGetHurtAnim[0]->GetCharacterBP())->UpdateStringHP();
		Cast<APD_E_Character>(characterWhoPlayGetHurtAnim[0]->GetCharacterBP())->UpdateStateActionOnChar();
		if (characterWhoPlayGetHurtAnim[0]->GetTotalStats()->HPCurrent <= 0.0f)
			characterWhoPlayGetHurtAnim[0]->GetController()->Animation_DeathChar((int)ActiveSkills::GetHurt);
		else
			characterWhoPlayGetHurtAnim[0]->GetController()->Animation_GetHurt((int)ActiveSkills::GetHurt);

		characterWhoPlayGetHurtAnim.RemoveAt(0);
	}
	for (int character = 0; character < characterWhoPlayHealAnim.Num(); character++)
	/*{
		Cast<APD_E_Character>(characterWhoPlayHealAnim[character]->GetCharacterBP())->UpdateStringHP();
		Cast<APD_E_Character>(characterWhoPlayHealAnim[character]->GetCharacterBP())->UpdateStateActionOnChar();
		characterWhoPlayHealAnim[character]->GetController()->Animation_GetHurt((int)ActiveSkills::WhoHeal);
	}*/
	while (characterWhoPlayHealAnim.Num() > 0) {
		Cast<APD_E_Character>(characterWhoPlayHealAnim[0]->GetCharacterBP())->UpdateStringHP();
		Cast<APD_E_Character>(characterWhoPlayHealAnim[0]->GetCharacterBP())->UpdateStateActionOnChar();
		characterWhoPlayHealAnim[0]->GetController()->Animation_GetHurt((int)ActiveSkills::WhoHeal);

		characterWhoPlayHealAnim.RemoveAt(0);
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
		//if (Cast<AServerCamera>(SGI->CameraServer)->GetMoveState() == ECameraMoveState::EndMoving) {
			ChangePhase(EServerPhase::MoveTick);
		//}
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
		
		//GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Orange, FString::Printf(TEXT("Cartel de Inicio de consumible")));

		if (IsThereAnyConsumableOrders())
			timer->InitTimer(timeWaitingPhases);

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
		LogicTurnItemPhase();
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

		//GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Green, FString::Printf(TEXT("Cartel de Inicio de movimiento")));

		if (IsThereAnyMovementOrder())
			timer->InitTimer(timeWaitingPhases);
		
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
					//targetPositions.Add(mapManager->LogicToWorldPosition(
						//enemyManager->GetEnemies()[k]->GetMovingLogicalPosition()[logicCharacter->GetMovingLogicalPosition().Num() - 1]));
					//	enemyManager->GetEnemies()[k]->GetCurrentLogicalPosition()));
					if (enemyManager->GetTurnOrders(k)->positionsToMove.Num() > 0) {
						FStructLogicPosition pos = enemyManager->GetTurnOrders(k)->positionsToMove[enemyManager->GetTurnOrders(k)->positionsToMove.Num() - 1];
					
					targetPositions.Add(mapManager->LogicToWorldPosition(PD_MG_LogicPosition(pos.positionX, pos.positionY)));
					UE_LOG(LogTemp, Log, TEXT("Camera MOve adding logic: %s"), *(mapManager->LogicToWorldPosition(PD_MG_LogicPosition(pos.positionX, pos.positionY)).ToString()));
					}

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
					if (enemyManager->GetTurnOrders(z)->positionsToMove.Num() > 0) { //Solo metemos las posiciones  de los enemigos que se mueven este turno.
						targetPositions.Add(enemyManager->GetEnemies()[z]->GetCharacterBP()->GetActorLocation());
					}
				}
			}

			//SGI->targetPositionsToCenterCamera = targetPositions;

			UE_LOG(LogTemp, Log, TEXT("Camera MOve from GameManager %d"), targetPositions.Num());
			FVector target = Cast<AServerCamera>(SGI->CameraServer)->FindAvaragePosition(targetPositions);

			//Cast<AServerCamera>(SGI->CameraServer)->MoveTo(FVector(target.X, target.Y,1000));
			//Cast<AServerCamera>(SGI->CameraServer)->MoveToPositions(targetPositions);

			Cast<AServerCamera>(SGI->CameraServer)->InitPatrolPositions(targetPositions);
			Cast<AServerCamera>(SGI->CameraServer)->LookAtPoint(target);
			Cast<AServerCamera>(SGI->CameraServer)->SetCameraOnView();

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
		//liberar a cualquier character de su collision
		for (int index_player = 0; index_player < playersManager->GetNumPlayers(); index_player++)
		{
			playersManager->GetDataStructPlayer(index_player)->logic_Character->SetIsStoppingByCollision(false);
		}
		for (int index_enemy = 0; index_enemy < enemyManager->GetEnemies().Num(); index_enemy++)
		{
			enemyManager->GetEnemies()[index_enemy]->SetIsStoppingByCollision(false);
		}

		individualActionInteractablesOnTurns.Empty(); //limpiar siempre el Tmap(), por lo que pueda pasr

		LogicTurnInteractablePhase(); //va a calcular las acciones de TODOS los PLAYERS O ENEMIGOS de ese turno

		//GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Yellow, FString::Printf(TEXT("Cartel de Inicio de interaccion")));

		if (IsThereAnyInteractbaleOrder())
			timer->InitTimer(timeWaitingPhases);
		
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

		if (individualActionInteractablesOnTurns.Num() > 0)
		{
			TArray<FString> id_charactersOnArray;
			individualActionInteractablesOnTurns.GenerateKeyArray(id_charactersOnArray);

			TArray<int> index_totalActions;
			individualActionInteractablesOnTurns.GenerateValueArray(index_totalActions);

			FString id_char = id_charactersOnArray[0]; //siempre el primero
			int id_interact = index_totalActions[0]; //siempre el primero

			VisualInteractbaleTick(id_char, id_interact);
		}
		else 
		{
			UpdatePhase();
		}

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackIni)
	{
		individualActionOnTurns.Empty(); //limpiar siempre el Tmap(), por lo que pueda pasr

		LogicTurnAttackPhase(); //va a calcular las acciones de TODOS los PLAYERS O ENEMIGOS de ese turno

		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: AttackIni"));
		//Llamar al procceso del ataque logico

		//GEngine->AddOnScreenDebugMessage(-1, timeWaitingPhases, FColor::Red, FString::Printf(TEXT("Cartel de Inicio de ataque")));

		if (IsThereAnyAttackOrder())
			timer->InitTimer(timeWaitingPhases);

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
		else 
		{
			UpdatePhase();
		}

		
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: EndAllPhases"));
		characterWhoPlayDefenseAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de defensa
		characterWhoPlayGetHurtAnim.Empty(); //limpiamos los characters que se tienen que tienen que lanzar animacion de herida
		characterWhoPlayHealAnim.Empty();    //limpiamos los characters que se tienen que tienen que lanzar animacion de Curar

		UpdateState(); //Devolvemos el flujo a la maquina de estados del game manager (no de fases)
	}else //Caso indeterminado
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: WARNING: estado sin inicializacion"));
	}

}



bool  PD_GM_GameManager::IsThereAnyConsumableOrders()
{
	return false;
}
bool  PD_GM_GameManager::IsThereAnyMovementOrder()
{
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) 
	{
		for (int index_players = 0; index_players < playersManager->GetNumPlayers(); index_players++)
		{
			if (playersManager->GetDataStructPlayer(index_players)->turnOrders->positionsToMove.Num() > 0)
				return true;
		}

	}
	if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) 
	{
		for (int index_enemies = 0; index_enemies < enemyManager->GetEnemies().Num(); index_enemies++)
		{
			if (enemyManager->GetTurnOrders(index_enemies))
			{
				if (enemyManager->GetTurnOrders(index_enemies)->positionsToMove.Num() > 0)
					return true;
			}
			
		}
	}

	return false;
}
bool  PD_GM_GameManager::IsThereAnyInteractbaleOrder()
{
	if (individualActionInteractablesOnTurns.Num() > 0)
		return true;
	else
		return false;
}
bool  PD_GM_GameManager::IsThereAnyAttackOrder()
{
	if (individualActionOnTurns.Num() > 0)
		return true;
	else
		return false;
}

#pragma endregion

int PD_GM_GameManager::getServerPhase()
{
	return (int)structGamePhase->enumGamePhase;
}

int PD_GM_GameManager::getGameMngrGameState()
{
	return (int)structGameState->enumGameState;
}

void PD_GM_GameManager::CheckIfCharacterIsDead_ForHidden()
{
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		if (enemyManager->GetEnemies()[i]->GetTotalStats()->HPCurrent <= 0)
		{
			enemyManager->GetEnemies()[i]->GetCharacterBP()->SetActorHiddenInGame(true);
			Cast<APD_E_Character>(enemyManager->GetEnemies()[i]->GetCharacterBP())->Widget->SetHiddenInGame(true);
			FVector newPositionToDie = enemyManager->GetEnemies()[i]->GetCharacterBP()->GetActorLocation();
			newPositionToDie.Z = newPositionToDie.Z - 300.0f;
			enemyManager->GetEnemies()[i]->GetCharacterBP()->SetActorLocation(newPositionToDie);
			enemyManager->GetEnemies()[i]->SetIsDead(true);
		}
	}

	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		if (playersManager->GetDataPlayers()[i]->logic_Character->GetTotalStats()->HPCurrent <= 0)
		{
			playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->SetActorHiddenInGame(true);
			Cast<APD_E_Character>(playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP())->Widget->SetHiddenInGame(true);
			FVector newPositionToDie = playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->GetActorLocation();
			newPositionToDie.Z = newPositionToDie.Z - 300.0f;
			playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->SetActorLocation(newPositionToDie);
			playersManager->GetDataPlayers()[i]->logic_Character->SetIsDead(true);
		}
	}

}

bool PD_GM_GameManager::CheckWinGameConditions()
{
	int enemiesDied = 0;
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++)
	{
		if (enemyManager->GetEnemies()[i]->GetTotalStats()->HPCurrent <= 0)
		{
			if (enemyManager->GetEnemies()[i]->GetTypeCharacter() == ECharacterType::OrcBoss) 
			{
				return true;
			}
			else 
			{
				enemiesDied++;
				enemyManager->DeleteEnemy(enemyManager->GetEnemies()[i]);
			}
		}

	}
	return false;
}

bool PD_GM_GameManager::CheckLoseGameConditions()
{
	bool playersLose = false;

	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		if (playersManager->GetDataPlayers()[i]->logic_Character->GetTotalStats()->HPCurrent <= 0)
		{
			playersManager->GetDataPlayers()[i]->logic_Character->GetController()->Animation_DeathChar((int)ActiveSkills::GetHurt);
			playersLose = true;
		}

	}

	return playersLose;
}

#pragma region POINTSSYSTEM

void PD_GM_GameManager::UpdatePoints() {/// valdría con pasarles el id por ejemplo
	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
			TotalPoints += playersManager->GetDataPlayers()[i]->logic_Character->GetTotalStats()->PointsCurrent;
	}
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