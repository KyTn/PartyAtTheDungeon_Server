// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_PlayersManager.h"

//Includes de uso
#include "Actors/PD_GenericController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"

//Include of forward declaration
#include "Structs/PD_ServerStructs.h"//Para todos los structs


PD_PlayersManager::PD_PlayersManager()
{
}

PD_PlayersManager::~PD_PlayersManager()
{
}

//Funciones de gestion de conexion

void PD_PlayersManager::AddNewPlayer(FStructNewConnection* newClientConnection, int player) {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::AddNewPlayer"));
	StructPlayer* structPlayer = new StructPlayer();
	structPlayer->ID_PLAYER = player;
	dataPlayers.Add(structPlayer);
}

int PD_PlayersManager::GetNumPlayers() {
	return dataPlayers.Num();
}

//Funciones de modificacion de datos de players
/*
void PD_PlayersManager::setTurnOrders(FStructTurnOrders* turnOrders, int player) {
	CheckPlayerIndex(player);
	dataPlayers[player]->turnOrders = turnOrders;
}*/


//Funciones de consulta de datos de players






bool PD_PlayersManager::AllPlayersReady() {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::AllPlayersReady"));
	for (StructPlayer* player : dataPlayers) {
		if (!player->readyMenu) {
			return false;
		}
	}
	return true;
}

bool PD_PlayersManager::AllPlayersSendOrders() {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::AllPlayersSendOrders"));
	for (StructPlayer* player : dataPlayers) {
		if (!player->turnOrders) {
			return false;
		}
	}
	return true;
}

bool PD_PlayersManager::AnyPlayerDead() {

	for (StructPlayer* player : dataPlayers) {
		if (0 <= 0)	//Aqui habria que coger el struct de stats de cada jugador y comprobar si la vida es menor o igual a 0
			return true;
	}

	return false;
}



bool  PD_PlayersManager::AllAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::AllAnimationEnd"));
	for (StructPlayer* player : dataPlayers) {
		if (!player->logic_Character->GetController()->IsAtAnimation()) {
			return false;
		}
	}
	return true;
}

int PD_PlayersManager::GetIndexClientMaster() {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetIndexClientMaster"));
	for (int i = 0; i < dataPlayers.Num();i++) {
		StructPlayer* player = dataPlayers[i];
		if (player->clientMaster) return i;
	}
	return -1;
}

//Devuelve el numero de acciones de la lista mas larga
int PD_PlayersManager::GetMaxLenghtActions(EActionPhase phase) {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetMaxLenghtActions "));
	int indexplayer = this->GetPlayerMaxLenghtActions(phase);
	
	if (indexplayer != -1) {
		if (phase == EActionPhase::Move) {
			return this->GetDataStructPlayer(indexplayer)->turnOrders->listMove.Num();
		}
		else if (phase == EActionPhase::Attack) {
			return this->GetDataStructPlayer(indexplayer)->turnOrders->listAttack.Num();
		}
	}
	return 0;


}
//Devuelve el index del jugador con la lista de acciones mas larga
int PD_PlayersManager::GetPlayerMaxLenghtActions(EActionPhase phase) {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetPlayerMaxLenghtActions"));
	int numTicks = -1;
	int indexPlayer=-1;

	for (int i = 0; i < this->GetNumPlayers(); i++) {
		TArray<FStructOrderAction> listActions;
		if (phase == EActionPhase::Move) {
			listActions = this->GetDataStructPlayer(i)->turnOrders->listMove;
		}
		else if (phase == EActionPhase::Attack) {
			listActions = this->GetDataStructPlayer(i)->turnOrders->listAttack;
		}

		UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetPlayerMaxLenghtActions : Phase: % d  :index: %d  :lenght: %d"), static_cast<uint8>(phase), i, listActions.Num());

		if (numTicks < (listActions.Num())) {
			numTicks = listActions.Num();
			indexPlayer = i;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("PD_PlayersManager::GetPlayerMaxLenghtActions: devuelto %d"), indexPlayer);

	return indexPlayer;
}

//Funcion de acceso directo al struct
StructPlayer* PD_PlayersManager::GetDataStructPlayer(int player) {
	
	CheckPlayerIndex(player);
	return dataPlayers[player];
}




//Funciones private
bool PD_PlayersManager::CheckPlayerIndex(int player) {
	
	if (dataPlayers.IsValidIndex(player) && dataPlayers[player]) {
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: CheckPlayerState ERROR: El indice de player no es valido"));
		return false;
	}
}

