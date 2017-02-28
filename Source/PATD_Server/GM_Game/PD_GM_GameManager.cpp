// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_GameManager.h"

//Includes de uso de objetos
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "LogicCharacter/PD_GM_LogicCharacter.h"
//#include "Actors/Players/PD_PLY_Controller.h"

//Includes of forward declaration
#include "PD_PlayersManager.h"
#include "PD_GM_MapManager.h"
#include "PD_GM_EnemyManager.h"
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums



PD_GM_GameManager::PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager, PD_GM_EnemyManager* inEnemyMan)
{
	InitState();
	playersManager = inPlayersManager; 
	mapManager = inMapManager;
	enemyManager = inEnemyMan;
//	interactionManager = new PD_GM_InteractionsManager(inPlayersManager, inMapManager);
}

PD_GM_GameManager::~PD_GM_GameManager()
{
}


void PD_GM_GameManager::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
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
	//ESTADO: WaitingActionOrders
	if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {

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
		}

	}else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {

		//Transicion inmediata de estado
		this->ChangeState(EGameState::ExecutingPlayersVisualization);


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


	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		PlayersLogicTurn();
		UpdateState(); //transicion inmediata
		
		
	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization){
		
		structGameState->enumActionPhase = EActionPhase::Move; //Empezamos en fase de mover y a partir de ahi lo controla el VisualTickControl
		VisualTickControl();
		

	}else if (structGameState->enumGameState == EGameState::EndOfTurn) {

		//Enviar a cliente actualizacion del mapa
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


void PD_GM_GameManager::InitState() {
	structGameState = new StructGameState();
	structGameState->enumGameState = EGameState::WaitingPlayerOrders;

}

//Funciones de gestion de acciones
void PD_GM_GameManager::IntitializeTurnStates() {
	ItemTurnInformation->CurrentState = InteractionStates::Ready;
	MoveTurnInformation->CurrentState = InteractionStates::Ready;
	AttackTurnInformation->CurrentState = InteractionStates::Ready;
}

//Control de los turnos

void PD_GM_GameManager::PlayersLogicTurn() {
	//Distincion para players o enemigos
	int numCharacters = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		numCharacters = playersManager->GetNumPlayers();
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
		numCharacters = playersManager->GetNumPlayers();
	}


	//LogicTurnItemPhase(numCharacters);
	LogicTurnMovePhase(numCharacters);
	LogicTurnAttackPhase(numCharacters);
}

void PD_GM_GameManager::LogicTurnItemPhase() {
	
}

void PD_GM_GameManager::LogicTurnMovePhase(int numCharacters) {
	
	//Distincion para players o enemigos
	//Calcular el numero de ticks a realizar (el de la lista mas larga)
	int numTicks = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Move);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Move);
	}

	



	//Ejecutar los ticks
	for (int i = 0; i < numTicks; i++) {
		LogicMoveTick(i, numCharacters);

	}
}

void PD_GM_GameManager::LogicTurnAttackPhase(int numCharacters) {
	//Distincion para players o enemigos
	//Calcular el numero de ticks a realizar (el de la lista mas larga)
	int numTicks = 0;
	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
	}
	else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
		numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
	}



	
	for (int i = 0; i < numTicks; i++) {
		LogicAttackTick(i, numCharacters);
	}
}


//Control de los Ticks

void PD_GM_GameManager::LogicItemTick(int tick) {

}

void PD_GM_GameManager::LogicMoveTick(int tick, int numCharacters) {
		
	for (int i = 0; numCharacters; i++) {


		//Distincion para players o enemigos
		TArray<FStructOrderAction> listMove;
		PD_GM_LogicCharacter* logicCharacter;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
			listMove = playersManager->GetDataStructPlayer(i)->turnOrders->listMove;
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}
		else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
			listMove = playersManager->GetDataStructPlayer(i)->turnOrders->listMove;
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}


		
		
		//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
		FStructOrderAction order = listMove[tick];
//		logicCharacter->MoveToLogicPosition(order);


		
		
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


	for (int i = 0; numCharacters; i++) {

		//Distincion para players o enemigos
		TArray<FStructOrderAction> listAttack;
		PD_GM_LogicCharacter* logicCharacter;
		if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
			listAttack= playersManager->GetDataStructPlayer(i)->turnOrders->listAttack; 
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}else if (structGameState->enumGameState == EGameState::ExecutingEnemiesLogic) {
			listAttack = playersManager->GetDataStructPlayer(i)->turnOrders->listAttack;
			logicCharacter = playersManager->GetDataStructPlayer(i)->logic_Character;
		}

	
		//Logica del attack tick

		
		//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
		FStructOrderAction order = listAttack[tick];
		//logicCharacter->ActionTo(PD_MG_LogicPosition targetPosition, uint32 action);

	}


}
void PD_GM_GameManager::VisualTickControl() {

	if (structGameState->enumActionPhase == EActionPhase::Move) {
		if (playersManager->GetMaxLenghtActions(structGameState->enumActionPhase) == 0) { //No hay mas acciones de mover
			structGameState->enumActionPhase = EActionPhase::Attack;
		}
		else {
			VisualMoveTick();
		}
	}

	if (structGameState->enumActionPhase == EActionPhase::Attack) {
		if (playersManager->GetMaxLenghtActions(structGameState->enumActionPhase) == 0) { //No hay mas acciones de atacar
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

	//Todos a la vez
	for (int i = 0; playersManager->GetNumPlayers(); i++) {
		FStructOrderAction visualAction = playersManager->GetDataStructPlayer(i)->turnOrders->listMove.Pop();

		StructPlayer* structPlayer = playersManager->GetDataStructPlayer(i);
		PD_GM_LogicCharacter* logicCharacter = structPlayer->logic_Character;
		PD_MG_LogicPosition* logicPosition = logicCharacter->GetCurrentLogicalPosition();
		//Esto solo esta para pruebas, pues luego la visualizacion no tendra que lidiar con ordenesLogicas (sino con sus propias ordenes preferiblemente)
		if (visualAction.targetDirection == 1)logicPosition->SetY(logicPosition->GetY() + 1);
		else if (visualAction.targetDirection == 2)logicPosition->SetY(logicPosition->GetY() - 1);
		else if (visualAction.targetDirection == 3)logicPosition->SetX(logicPosition->GetX() + 1);
		else if (visualAction.targetDirection == 4)logicPosition->SetX(logicPosition->GetX() - 1);


		FVector* realPosition = mapManager->LogicToWorldPosition(logicPosition);
		//playersManager->GetDataStructPlayer(i)->actorController->MoveTo(realPosition->X, realPosition->Y);

	}
}

void PD_GM_GameManager::VisualAttackTick() {
	//Uno a uno
	int indexPlayer = playersManager->GetPlayerMaxLenghtActions(structGameState->enumActionPhase);
	FStructOrderAction visualAction = playersManager->GetDataStructPlayer(indexPlayer)->turnOrders->listAttack.Pop();
	//playersManager->GetDataStructPlayer(indexPlayer)->actorController->ActionTo();


}




void PD_GM_GameManager::OnAnimationEnd() {
	if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) {
		/*if (playersManager->AllAnimationEnd()) {
		VisualTickControl();
		}*/
		//Mañana hacer el sistema del callback cuando este el controller subido.
	}

}