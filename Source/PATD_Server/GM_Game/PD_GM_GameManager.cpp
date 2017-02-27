// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_GameManager.h"

//Includes de uso de objetos
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "LogicCharacter/PD_GM_PlayerLogicCharacter.h"
#include "Actors/Players/PD_PLY_Controller.h"

//Includes of forward declaration
#include "PD_PlayersManager.h"
#include "PD_GM_MapManager.h"
#include "PD_GM_InteractionsManager.h"
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums



PD_GM_GameManager::PD_GM_GameManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager)
{
	InitState();
	playersManager = inPlayersManager; 
	mapManager = inMapManager;
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

		if (structGameState->enumActionPhase == EActionPhase::EndTurn) {
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

	LogicTurnItemPhase();
	LogicTurnMovePhase();
	LogicTurnAttackPhase();
}

void PD_GM_GameManager::LogicTurnItemPhase() {
	ItemTurnInformation->CurrentState = InteractionStates::Working;
}

void PD_GM_GameManager::LogicTurnMovePhase() {
	MoveTurnInformation->CurrentState = InteractionStates::Working;
	//Calcular el numero de ticks a realizar (el de la lista mas larga) -Aqui o en el playerManager?- Yo creo que mejor en el playerManager
	int numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Move);
	//Ejecutar los ticks
	for (int i = 0; i < numTicks; i++) {
		LogicMoveTick(i);
	}
}

void PD_GM_GameManager::LogicTurnAttackPhase() {
	AttackTurnInformation->CurrentState = InteractionStates::Working;
	int numTicks = playersManager->GetMaxLenghtActions(EActionPhase::Attack);
	for (int i = 0; i < numTicks; i++) {
		LogicAttackTick(i);
	}
}


//Control de los Ticks

void PD_GM_GameManager::LogicItemTick(int tick) {

}

void PD_GM_GameManager::LogicMoveTick(int tick) {
	for (int i = 0; playersManager->GetNumPlayers(); i++) {

		
		StructPlayer* structPlayer = playersManager->GetDataStructPlayer(i);
		//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
		FStructOrderAction order = structPlayer->turnOrders->listMove[tick];
		//structPlayer->logicCharacter->MoveToLogicPosition(uint8 tick, TArray<FStructOrderAction> listMove);

	}
	//Comprobar que choques.//Resolucion de conflictos
}

void PD_GM_GameManager::LogicAttackTick(int tick) {

	for (int i = 0; playersManager->GetNumPlayers(); i++) {


		StructPlayer* structPlayer = playersManager->GetDataStructPlayer(i);
		//Controlar por si no tiene ordenes (el maximo tick es para la lista mas larga)
		FStructOrderAction order = structPlayer->turnOrders->listAttack[tick];
		//structPlayer->logicCharacter->ActionTo(PD_MG_LogicPosition targetPosition, uint32 action);

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
			structGameState->enumActionPhase = EActionPhase::EndTurn;
			//Fin del turno, hacer update de la maquina de estados del GameManager
			UpdateState();
		}
		else {
			VisualAttackTick();
		}
	}
}

void PD_GM_GameManager::VisualMoveTick(){ 
	
	//Todos a la vez
	for (int i = 0; playersManager->GetNumPlayers(); i++) {
			FStructOrderAction visualAction = playersManager->GetDataStructPlayer(i)->turnOrders->listMove.Pop();

			StructPlayer* structPlayer = playersManager->GetDataStructPlayer(i);
			PD_GM_PlayerLogicCharacter* logicCharacter = structPlayer->logicCharacter;
			PD_MG_LogicPosition* logicPosition = logicCharacter->getLogicPosition();
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
	int indexPlayer=playersManager->GetPlayerMaxLenghtActions(structGameState->enumActionPhase);
	FStructOrderAction visualAction= playersManager->GetDataStructPlayer(indexPlayer)->turnOrders->listAttack.Pop();
	//playersManager->GetDataStructPlayer(indexPlayer)->actorController->ActionTo();
		
	
}






void PD_GM_GameManager::OnAnimationEnd() {
	if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization) {
		/*if (playersManager->AllAnimationEnd()) {
			VisualTickControl();
		}*/ 
		//Ma�ana hacer el sistema del callback cuando este el controller subido.
	}
}