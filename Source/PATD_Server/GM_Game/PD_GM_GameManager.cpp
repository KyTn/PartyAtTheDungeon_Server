// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_GameManager.h"

//Includes of forward declaration
#include "PD_PlayersManager.h"



PD_GM_GameManager::PD_GM_GameManager()
{
	InitState();
}

PD_GM_GameManager::~PD_GM_GameManager()
{
}


void PD_GM_GameManager::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	if (structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
		FStructTurnOrders* turnStruct = (FStructTurnOrders*)inDataStruct;
		playerManager->getDataStructPlayer(inPlayer)->turnOrders=turnStruct;
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
		if (playerManager->AllPlayersSendOrders()) {
			this->ChangeState(EGameState::ExecutingActionOrders);
		}

	}
	else //Caso indeterminado
	{
	
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::UpdateState: WARNING: estado sin update"));


	}
}

void PD_GM_GameManager::OnBeginState() {


	if (structGameState->enumGameState == EGameState::ExecutingActionOrders) {

		//Llamar a realizar el turno
	}
	else //Caso indeterminado
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