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

//Includes of forward declaration
#include "PD_PlayersManager.h"
#include "PD_GM_MapManager.h"
#include "PD_GM_EnemyManager.h"
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "NW_Networking/PD_NW_NetworkManager.h"
#include "PD_GM_SplineManager.h"


PD_GM_GameManager::PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager, PD_NW_NetworkManager* inNetworkManager, APD_GM_SplineManager* inSplineManager)
{
	
	playersManager = inPlayersManager; 
	mapManager = inMapManager;
	splineManager = inSplineManager;
	mapManager->_GAMEMANAGER = this;
	enemyManager = new PD_GM_EnemyManager();
	networkManager = inNetworkManager;
	networkManager->RegisterObserver(this);
	structGameState = new StructGameState();
	structGamePhase = new StructGamePhase();
	InitState();

}

PD_GM_GameManager::~PD_GM_GameManager()
{
}

void PD_GM_GameManager::InitState() {
	
	ChangeState(EGameState::Instantiate_Map);

}

void PD_GM_GameManager::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	/*
	if (structGameState->enumGameState == EGameState::Instantiate_Map) {
		// Si se recibe del servidor un Start_Match, ir a ese estado. 
		if (inEventType == UStructType::FStructClientStartMatchOnGM) {
			ChangeState(EGameState::Start_Match);
		}
	}
	else if (structGameState->enumGameState == EGameState::Start_Match) {
		if (inEventType == UStructType::FStructClientCanGenerateOrders) {
			ChangeState(EGameState::WaitingPlayerOrders);
		}
	}
	else*/ 
	if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) 
	{
		FStructTurnOrders* turnStruct = (FStructTurnOrders*)inDataStruct;

		playersManager->GetDataStructPlayer(inPlayer)->turnOrders=turnStruct;
		UpdateState();
	}
	
	//if(playerManager->AllPlayersSendOrders())

}

bool PD_GM_GameManager::SuscribeToEvents(int inPlayer, UStructType inType) {
	if (inType == UStructType::FStructTurnOrders) return true; //Suscrito a TurnOrders
	else return false;
}


void PD_GM_GameManager::UpdateState() {

	//Creo que es mas claro un if-else gigante que un switch gigante

	if (structGameState->enumGameState == EGameState::Instantiate_Map) {
		//Transicion inmediata de estado

		this->ChangeState(EGameState::Start_Match);
		

	}else if (structGameState->enumGameState == EGameState::Start_Match) {
		//Transicion inmediata de estado
		

		this->ChangeState(EGameState::WaitingPlayerOrders);
		
	}else if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
		
		//Transiciones de estados
		if (playersManager->AllPlayersSendOrders()) {
			this->ChangeState(EGameState::ExecutingPlayersTurn);
		}

	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {

		if (structGamePhase->enumGamePhase == EServerPhase::EndAllPhases) {
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
		FStructClientCanGenerateOrders clientGenerateOrders = FStructClientCanGenerateOrders();
		networkManager->SendNow(&clientGenerateOrders, -1);

		UpdateState();

	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingPlayersLogic"));
		PlayersLogicTurn();
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
		PlayersLogicTurn();
		InitPhase();
		

	}
	/*else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingEnemiesVisualization"));
		structGameState->enumActionPhase = EActionPhase::Move; //Empezamos en fase de mover y a partir de ahi lo controla el VisualTickControl
		VisualTickControl();

	}*/else if (structGameState->enumGameState == EGameState::EndOfTurn) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: EndOfTurn"));
		
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
			UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: id Enemy:%s" ), *logicCharacter->GetIDCharacter());
			structUpdateCharacter.ID_character = logicCharacter->GetIDCharacter();
			structUpdateTurn.listEnemyCharacters.Add(structUpdateCharacter);
		}
		 
		//Envio a todos los clientes con el update del turno
		networkManager->SendNow(&structUpdateTurn);
		//Hay que hacer lo necesario (borrar las cosas de este turno) para que se pueda recibir otro normalmente.
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

/*
//Funciones de gestion de acciones
void PD_GM_GameManager::IntitializeTurnStates() {
	ItemTurnInformation->CurrentState = InteractionStates::Ready;
	MoveTurnInformation->CurrentState = InteractionStates::Ready;
	AttackTurnInformation->CurrentState = InteractionStates::Ready;
}*/


void PD_GM_GameManager::CreateEnemyOrders() {
	for (int i = 0; i < enemyManager->GetEnemies().Num(); i++) {
		APD_AIController* controller = (APD_AIController*)enemyManager->GetEnemies()[i]->GetController();
		controller->StartAITurnCalcultion(mapManager, enemyManager->GetEnemies()[i]);
	}
}

void PD_GM_GameManager::CallbackEndCreateEnemyOrders(FString idCharacter, FStructTurnOrders* turnOrders) {
	//Asignar las ordenes al character del id
	int index =enemyManager->GetIndexByID(idCharacter);
	enemyManager->getListTurnOrders()[index] = turnOrders;
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
	LogicTurnAttackPhase(numCharacters);
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
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		numTicks = enemyManager->GetMaxLenghtActions(EActionPhase::Move);
	}

	



	//Ejecutar los ticks
	for (int i = 0; i < numTicks; i++) {
		//Solo ejecuta al LogicMoveTick si el numero total de movimientos es superior al tick correspodiente
		//Si no, lo que pasara es que intentara acceder en un tick superior a una posicion del array que no existe
			LogicMoveTick(i, numCharacters);

	}
}

void PD_GM_GameManager::LogicTurnAttackPhase(int numCharacters) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicTurnAttackPhase"));

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
				if (playersManager->GetDataStructPlayer(i)->turnOrders->listMove.Num() > tick)
				{
					listMove = playersManager->GetDataStructPlayer(i)->turnOrders->listMove;
					logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
					//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
					FStructOrderAction* order = &listMove[tick];

					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : moviendo logic character"));

					logicCharacter->MoveToLogicPosition(order);
				}
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				listMove = enemyManager->GetTurnOrders(i)->listMove;
				logicCharacter = enemyManager->GetEnemies()[i];
				//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
				FStructOrderAction* order = &listMove[tick];

				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : moviendo logic character"));

				logicCharacter->MoveToLogicPosition(order);
			}
	
			
		
		///TODO ALVARO
		//structPlayer->logicCharacter->ProcessMoveOrder(order);

		//structPlayer->turnOrders->listMove.Remove(order); //O marcar como consumida

		//Llamada a moverse actor provisional
		//Aqui en realidad no tendriamos la posicion final en el logicCharacter, pues podria haber choques 
		//al moverse los siguientes personajes.
		
		///TODO ALVARO
		//PD_GM_PlayerLogicCharacter* logicCharacter = structPlayer->logicCharacter;
		//PD_MG_LogicPosition* logicPosition = logicCharacter->getLogicPosition(); 
		//FVector* realPosition = mapManager->LogicToWorldPosition(logicPosition);
		
		///TODO ALVARO
		//playersManager->getDataStructPlayer(i)->actorController->Move(realPosition->X, realPosition->Y);

	}
	//Comprobar que choques.//Resolucion de conflictos
}

void PD_GM_GameManager::LogicAttackTick(int tick,int numCharacters) {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicAttackTick"));


	for (int i = 0; i<numCharacters; i++) {

			//Distincion para players o enemigos
			TArray<FStructOrderAction> listAttack;
			PD_GM_LogicCharacter* logicCharacter = nullptr;
			if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
				if (playersManager->GetDataStructPlayer(i)->turnOrders->listAttack.Num() > tick)
				{
					listAttack = playersManager->GetDataStructPlayer(i)->turnOrders->listAttack;
					logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
					//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
					FStructOrderAction order = listAttack[tick];
					UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : atacando logic character"));
					logicCharacter->ActionTo(order);
				}
			}
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
				listAttack = enemyManager->GetTurnOrders(i)->listAttack;
				logicCharacter = enemyManager->GetEnemies()[i];
				//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
				FStructOrderAction order = listAttack[tick];
				UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::LogicMoveTick : atacando logic character"));
				logicCharacter->ActionTo(order);
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
	for (int i = 0; i<players; i++) {
		//Distincion para players o enemigos
		TArray<FStructOrderAction>* listMove=nullptr;
		PD_GM_LogicCharacter* logicCharacter=nullptr;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
			listMove = &playersManager->GetDataStructPlayer(i)->turnOrders->listMove;
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
			listMove = &enemyManager->GetTurnOrders(i)->listMove;
			logicCharacter = enemyManager->GetEnemies()[i];
		}

		
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick : lenght before Pop:%d "), listMove->Num());

		FStructOrderAction visualAction = listMove->Pop();
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick : lenght after Pop:%d "), listMove->Num());

		//Se añade al vector la nueva posicion a la que moverse ------- Cambiar cuando se pase varias posiciones....
		newPositionsToMove.Add(PD_MG_LogicPosition(visualAction.targetLogicPosition.positionX, visualAction.targetLogicPosition.positionY));

		//Peta al no tener actor ni controller
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::VisualMoveTick -->  Posicion Jugador: %d %d"), logicCharacter->GetCurrentLogicalPosition().GetX(), logicCharacter->GetCurrentLogicalPosition().GetY());
		
		logicCharacter->GetController()->SetSpline(splineManager->GetSpline());
		
		logicCharacter->MoveToPhysicalPosition(PD_MG_LogicPosition(visualAction.targetLogicPosition.positionX, visualAction.targetLogicPosition.positionY));

	}

	//Se llama a la Camara Server para que actulice su posicion - Se usa el SplineManager que es un actor para coger la referencia del Game Instance
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(splineManager->GetGameInstance());
	if (SGI)
	{
		//Pasas todo el Array de posiciones logicas a fisicas para el meotodo MOVER
		TArray<FVector> targetPositions = TArray<FVector>();
		for (int j = 0; j < newPositionsToMove.Num(); j++)
		{
			targetPositions.Add(mapManager->LogicToWorldPosition(newPositionsToMove[j]));
		}

		//Añades todas las posiciones de los jugadores del turno anterior al Array
		for (int i = 0; i < playersManager->GetNumPlayers(); i++)
		{
			targetPositions.Add(playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->GetActorLocation());
		}

		UE_LOG(LogTemp, Log, TEXT("Camera MOve from GameManager"));
		SGI->Camera_MoveInMovementPhase(targetPositions);
		SGI->Camera_ZoomInMovementPhase(targetPositions);
	}

}

void PD_GM_GameManager::VisualAttackTick() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick"));

	//Distincion para players o enemigos
	TArray<FStructOrderAction>* listAttack=nullptr;
	int indexCharacter;
	PD_GM_LogicCharacter* logicCharacter=nullptr;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
		indexCharacter = playersManager->GetPlayerMaxLenghtActions(EActionPhase::Attack);
		listAttack = &playersManager->GetDataStructPlayer(indexCharacter)->turnOrders->listAttack;
		logicCharacter = playersManager->GetDataStructPlayer(indexCharacter)->logic_Character;
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
		indexCharacter = enemyManager->GetEnemyMaxLenghtActions(EActionPhase::Attack);
		listAttack = &enemyManager->GetTurnOrders(indexCharacter)->listAttack;
		logicCharacter = enemyManager->GetEnemies()[indexCharacter];
	}

	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick despues de players/enemigos"));

	//Uno a uno
	
	FStructOrderAction visualAction = listAttack->Pop();
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick 0"));
	PD_MG_LogicPosition logicPosition = PD_MG_LogicPosition(visualAction.targetLogicPosition.positionX, visualAction.targetLogicPosition.positionY);
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick 1"));
	FVector physicPosition=mapManager->LogicToWorldPosition(logicPosition);
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick 2"));
	//Peta al no tener actor ni controller
    Cast<APD_CharacterController>(logicCharacter->GetController())->ActionTo(physicPosition.X, physicPosition.Y, visualAction.orderType);
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick 3"));

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
			//Aqui deberia estar en un estado de fase que sea tick (no INI)
			UpdatePhase();
		}

}


#pragma region GM PHASE MACHINE 

void PD_GM_GameManager::InitPhase() {
	ChangePhase(EServerPhase::MoveIni);

}

// Esta funcion cambia el estado actual al pasado por parametros y llama al OnBeginState del mismo.
void PD_GM_GameManager::ChangePhase(EServerPhase newPhase)
{
	structGamePhase->enumGamePhase = newPhase;
	OnBeginPhase();
}

void PD_GM_GameManager::UpdatePhase()
{
	if (structGamePhase->enumGamePhase == EServerPhase::MoveIni)
	{
		//Distincion para players o enemigos
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
		else {
			ChangePhase(EServerPhase::AttackIni);
		}

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveTick)
	{
		//Distincion para players o enemigos
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
		else {
			ChangePhase(EServerPhase::AttackIni);
		}
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackIni)
	{
		//Distincion para players o enemigos
		int maxLengthAction = 0;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
			maxLengthAction = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
			maxLengthAction = enemyManager->GetMaxLenghtActions(EActionPhase::Attack);
		}

		if (maxLengthAction != 0) {
			ChangePhase(EServerPhase::AttackTick); //Hay acciones por lo que volvemos a repetir el tick.
		}
		else {
			ChangePhase(EServerPhase::EndAllPhases);
		}

		
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackTick)
	{
		//Distincion para players o enemigos
		int maxLengthAction = 0;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersTurn) {
			maxLengthAction = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesTurn) {
			maxLengthAction = enemyManager->GetMaxLenghtActions(EActionPhase::Attack);
		}

		if (maxLengthAction != 0) {
			ChangePhase(EServerPhase::AttackTick); //Hay acciones por lo que volvemos a repetir el tick.
		}
		else {
			ChangePhase(EServerPhase::EndAllPhases);
		}
	
	}
	else //Caso indeterminado
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::UpdatePhase: WARNING: estado sin inicializacion"));
	}

}

void PD_GM_GameManager::OnBeginPhase()
{
	if (structGamePhase->enumGamePhase == EServerPhase::MoveIni)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: MoveIni"));
		//Limpiar todos los splines del splineManager para que todos los instanciados esten disponibles
		for (int i = 0; i < splineManager->splinesPool.Num(); i++)
		{
			splineManager->splinesPool[i]->RemovePoints();
		}

		//Llamar al procceso del movimiento logico
		UpdatePhase();
	}
	else if (structGamePhase->enumGamePhase == EServerPhase::MoveTick)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: MoveTick"));
		VisualMoveTick();

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackIni)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: AttackIni"));
		//Llamar al procceso del ataque logico
		UpdatePhase();

	}
	else if (structGamePhase->enumGamePhase == EServerPhase::AttackTick)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginPhase: AttackTick"));
		VisualAttackTick();

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
