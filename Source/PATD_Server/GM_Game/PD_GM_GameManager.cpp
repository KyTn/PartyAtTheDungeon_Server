// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_GameManager.h"

//Includes de uso de objetos
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_GenericController.h"
#include "Actors/Players/PD_CharacterController.h"

//Includes of forward declaration
#include "PD_PlayersManager.h"
#include "PD_GM_MapManager.h"
#include "PD_GM_EnemyManager.h"
#include "PD_GM_AIManager.h"
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "NW_Networking/PD_NW_NetworkManager.h"


PD_GM_GameManager::PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager, PD_NW_NetworkManager* inNetworkManager)
{
	
	playersManager = inPlayersManager; 
	mapManager = inMapManager;
	mapManager->_GAMEMANAGER = this;
	enemyManager = new PD_GM_EnemyManager();
	AIManager = new PD_GM_AIManager();
	InitState();
	networkManager = inNetworkManager;
	networkManager->RegisterObserver(this);
//	interactionManager = new PD_GM_InteractionsManager(inPlayersManager, inMapManager);
}

PD_GM_GameManager::~PD_GM_GameManager()
{
}

void PD_GM_GameManager::InitState() {
	structGameState = new StructGameState();
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
			this->ChangeState(EGameState::ExecutingPlayersLogic);
		}

	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {

		//Transicion inmediata de estado
		this->ChangeState(EGameState::ExecutingPlayersVisualization);
		

	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) {

		if (structGameState->enumActionPhase == EActionPhase::EndPhase) {
			this->ChangeState(EGameState::ExecutingEnemiesLogic); 
		//Salto de pruebas a end of turn para no hacer la logica del enemigo que aun peta.
			//this->ChangeState(EGameState::EndOfTurn);
		}

	}else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {

		//Transicion inmediata de estado
		this->ChangeState(EGameState::ExecutingEnemiesVisualization);


	}else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {

		if (structGameState->enumActionPhase == EActionPhase::EndPhase) {
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

	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingPlayersLogic"));
		PlayersLogicTurn();
		UpdateState(); //transicion inmediata


	}
	else if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization){
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingPlayersVisualization"));
		
		structGameState->enumActionPhase = EActionPhase::Move; //Empezamos en fase de mover y a partir de ahi lo controla el VisualTickControl
		VisualTickControl();
		

	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingEnemiesLogic"));
		enemyManager->newTurn();
		CreateEnemyOrders();
		PlayersLogicTurn();
		UpdateState(); //transicion inmediata

	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: ExecutingEnemiesVisualization"));
		structGameState->enumActionPhase = EActionPhase::Move; //Empezamos en fase de mover y a partir de ahi lo controla el VisualTickControl
		VisualTickControl();
		UpdateState();

	}else if (structGameState->enumGameState == EGameState::EndOfTurn) {
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
		enemyManager->AddActionTurn(AIManager->AIExecEnemy(enemyManager->GetEnemies()[i],mapManager));
	}
}


//Control de los turnos

void PD_GM_GameManager::PlayersLogicTurn() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::PlayersLogicTurn"));
	//Distincion para players o enemigos
	int numCharacters = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		numCharacters = playersManager->GetNumPlayers();
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
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
	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Move);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
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
	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
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
			if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
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
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
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
			if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
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
			else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
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
}

void PD_GM_GameManager::VisualMoveTick() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick"));
	int players = -1;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) 
		players = playersManager->GetNumPlayers();
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization)
		players = enemyManager->GetEnemies().Num();
	//Todos a la vez
	for (int i = 0; i<players; i++) {
		//Distincion para players o enemigos
		TArray<FStructOrderAction>* listMove=nullptr;
		PD_GM_LogicCharacter* logicCharacter=nullptr;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) {
			listMove = &playersManager->GetDataStructPlayer(i)->turnOrders->listMove;
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
			listMove = &enemyManager->GetTurnOrders(i)->listMove;
			logicCharacter = enemyManager->GetEnemies()[i];
		}


		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick : lenght before Pop:%d "), listMove->Num());

		FStructOrderAction visualAction = listMove->Pop();
		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualMoveTick : lenght after Pop:%d "), listMove->Num());

		//Peta al no tener actor ni controller
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::VisualMoveTick -->  Posicion Jugador: %d %d"), logicCharacter->GetCurrentLogicalPosition().GetX(), logicCharacter->GetCurrentLogicalPosition().GetY());
		logicCharacter->MoveToPhysicalPosition(logicCharacter->GetCurrentLogicalPosition());
		
		

		//Esto solo esta para pruebas, pues luego la visualizacion no tendra que lidiar con ordenesLogicas (sino con sus propias ordenes preferiblemente)
		/*if (visualAction.targetDirection == 1)currentLogicPosition->SetY(currentLogicPosition->GetY() + 1);
		else if (visualAction.targetDirection == 2)currentLogicPosition->SetY(currentLogicPosition->GetY() - 1);
		else if (visualAction.targetDirection == 3)currentLogicPosition->SetX(currentLogicPosition->GetX() + 1);
		else if (visualAction.targetDirection == 4)currentLogicPosition->SetX(currentLogicPosition->GetX() - 1);
		*/

		

	}
}

void PD_GM_GameManager::VisualAttackTick() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::VisualAttackTick"));

	//Distincion para players o enemigos
	TArray<FStructOrderAction>* listAttack=nullptr;
	int indexCharacter;
	PD_GM_LogicCharacter* logicCharacter=nullptr;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) {
		indexCharacter = playersManager->GetPlayerMaxLenghtActions(structGameState->enumActionPhase);
		listAttack = &playersManager->GetDataStructPlayer(indexCharacter)->turnOrders->listAttack;
		logicCharacter = playersManager->GetDataStructPlayer(indexCharacter)->logic_Character;
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
		indexCharacter = enemyManager->GetEnemyMaxLenghtActions(structGameState->enumActionPhase);
		listAttack = &enemyManager->GetTurnOrders(indexCharacter)->listAttack;
		logicCharacter = enemyManager->GetEnemies()[indexCharacter];
	}

	//Uno a uno
	
	FStructOrderAction visualAction = listAttack->Pop();
	PD_MG_LogicPosition logicPosition = PD_MG_LogicPosition(visualAction.targetLogicPosition.positionX, visualAction.targetLogicPosition.positionY);
	FVector physicPosition=mapManager->LogicToWorldPosition(logicPosition);

	//Peta al no tener actor ni controller
    Cast<APD_CharacterController>(logicCharacter->GetController())->ActionTo(physicPosition.X, physicPosition.Y, visualAction.orderType);

}




void PD_GM_GameManager::OnAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnAnimationEnd"));

	if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization || structGameState->enumGameState == EGameState::ExecutingEnemiesVisualization) {
		//Falta hacer un caso igual para los enemigos.
		if (playersManager->AllAnimationEnd()) {
			VisualTickControl();
		}
		
	}

}