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
//	interactionManager = new PD_GM_InteractionsManager(inPlayersManager, inMapManager);
}

PD_GM_GameManager::~PD_GM_GameManager()
{
}


void PD_GM_GameManager::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
		FStructTurnOrders* turnStruct = (FStructTurnOrders*)inDataStruct;
		playersManager->getDataStructPlayer(inPlayer)->turnOrders=turnStruct;
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
		

	}
	else //Caso indeterminado
	{
	
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::UpdateState: WARNING: estado sin update"));


	}
}

void PD_GM_GameManager::OnBeginState() {


	if (structGameState->enumGameState == EGameState::ExecutingPlayersLogic) {
		PlayersLogicTurn();
		//Inicializar el tickVisual
		structGameState->visualTick = 0;
		

		/*TArray<FStructOrderAction> listActions;
		if (structGameState->enumActionPhase == EActionPhase::Move) {
			listActions = playersManager->getDataStructPlayer(i)->turnOrders->listMove;
		}
		else if (structGameState->enumActionPhase == EActionPhase::Attack) {
			listActions = playersManager->getDataStructPlayer(i)->turnOrders->listAttack;
		}

		int numTicks = 0;
		for (int i = 0; playersManager->GetNumPlayers(); i++) {
			if (numTicks < (listActions.Num())) {
				numTicks = listActions.Num();
			};
		}

		LogicTick();
		*/
		//Llamar a realizar el turno
	}else if (structGameState->enumGameState == EGameState::ExecutingPlayersVisualization){

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
	int numTicks = 0;
	for (int i = 0; playersManager->GetNumPlayers(); i++) {
		if (numTicks < (playersManager->getDataStructPlayer(i)->turnOrders->listMove.Num())) {
			numTicks = playersManager->getDataStructPlayer(i)->turnOrders->listMove.Num();
		};
	}
	//Ejecutar los ticks
	for (int i = 0; i < numTicks; i++) {
		TickMovePhase(i);
	}
}

void PD_GM_GameManager::LogicTurnAttackPhase() {
	AttackTurnInformation->CurrentState = InteractionStates::Working;
}


//Control de los Ticks

void PD_GM_GameManager::TickItemPhase(int tick) {

}

void PD_GM_GameManager::TickMovePhase(int tick) {
	for (int i = 0; playersManager->GetNumPlayers(); i++) {

		//Resolucion de conflictos? choques etc.
		StructPlayer* structPlayer = playersManager->getDataStructPlayer(i);
		FStructOrderAction order = structPlayer->turnOrders->listMove[tick];
		structPlayer->logicCharacter->ProcessMoveOrder(order);
		structPlayer->turnOrders->listMove.Remove(order); //O marcar como consumida

		//Llamada a moverse actor provisional
		//Aqui en realidad no tendriamos la posicion final en el logicCharacter, pues podria haber choques 
		//al moverse los siguientes personajes.
		PD_GM_PlayerLogicCharacter* logicCharacter = structPlayer->logicCharacter;
		PD_MG_LogicPosition* logicPosition = logicCharacter->getLogicPosition(); 
		FVector* realPosition = mapManager->LogicToWorldPosition(logicPosition);
		playersManager->getDataStructPlayer(i)->actorController->Move(realPosition->X, realPosition->Y);
	}
	//Comprobar que choques.
}

void PD_GM_GameManager::TickAttackPhase(int tick) {

}




void PD_GM_GameManager::VisualTurnMovePhase() {
	if (structGameState->enumActionPhase == EActionPhase::Move) {
	
	}
	)
}


void PD_GM_GameManager::OnAnimationEnd() {
	if (structGameState->enumGameState == EGameState::ExecutingActionOrders) {

	}
}