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

void PD_PlayersManager::AddNewPlayer(FString newID_Client, int player) {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::AddNewPlayer"));
	StructPlayer* structPlayer = new StructPlayer();
	structPlayer->logic_Character = new PD_GM_LogicCharacter();
	structPlayer->ID_Client = newID_Client;
	structPlayer->ID_PLAYER = player;
	structPlayer->isConnected = true;
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
	bool checkAllPlayerSendData = true;
	int numDesconected = 0;
	for (StructPlayer* player : dataPlayers) {
		if (player->isConnected) {
			checkAllPlayerSendData = checkAllPlayerSendData && player->playerSendOrder;
		}
		else {
			numDesconected++;
		}
		
	}

	if (numDesconected == dataPlayers.Num()) return false;
	return checkAllPlayerSendData;
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
		if (player->logic_Character->GetController()->IsAtAnimation()) {
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


StructPlayer* PD_PlayersManager::GetStructPlayerByIDClient(FString ID_ClientToSearch)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDClient : buscando a %s"), *ID_ClientToSearch);

	for (int i = 0; i < dataPlayers.Num(); i++) {
	//	UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDClient : probando con %s"), *dataPlayers[i]->ID_Client);

		if (dataPlayers[i]->ID_Client == ID_ClientToSearch) {
		//	UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDClient : acertando con %s"), *dataPlayers[i]->ID_Client);
			return dataPlayers[i];
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDClient ERROR: No se ha encontrado cliente con id %s"), *ID_ClientToSearch);
	return nullptr;
}

StructPlayer* PD_PlayersManager::GetStructPlayerByIDCharacter(FString ID_CharacterToSearch)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDCharacter : buscando a %s"), *ID_CharacterToSearch);

	for (int i = 0; i < dataPlayers.Num(); i++) {
		//UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDCharacter : probando con %s"), *dataPlayers[i]->logic_Character->GetIDCharacter());

		if (dataPlayers[i]->logic_Character->GetIDCharacter() == ID_CharacterToSearch) {
			//UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDCharacter : acertando con %s"), *dataPlayers[i]->logic_Character->GetIDCharacter());
			return dataPlayers[i];
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetStructPlayerByIDCharacter ERROR: No se ha encontrado character de player con id %s"), *ID_CharacterToSearch);
	return nullptr;
}



//Devuelve el numero de acciones de la lista mas larga
int PD_PlayersManager::GetMaxLenghtActions(EActionPhase phase) {
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetMaxLenghtActions "));
	int indexplayer = this->GetPlayerMaxLenghtActions(phase);
	
	if (indexplayer != -1) {
		if (phase == EActionPhase::Move) {
			return this->GetDataStructPlayer(indexplayer)->turnOrders->positionsToMove.Num();
		}
		else if (phase == EActionPhase::Attack) {
			return this->GetDataStructPlayer(indexplayer)->turnOrders->actions.Num();
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
		int listActions = 0;
		if (phase == EActionPhase::Move) {
			listActions = this->GetDataStructPlayer(i)->turnOrders->positionsToMove.Num();
		}
		else if (phase == EActionPhase::Attack) {
			listActions = this->GetDataStructPlayer(i)->turnOrders->actions.Num();
		}

		UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetPlayerMaxLenghtActions : Phase: % d  :index: %d  :lenght: %d"), static_cast<uint8>(phase), i, listActions);

		if (numTicks < (listActions)) {
			numTicks = listActions;
			indexPlayer = i;
		}
		UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::GetPlayerMaxLenghtActions : : % d  :index: %d  :lenght: %d"), static_cast<uint8>(phase), i, listActions);

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
		UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: CheckPlayerState ERROR: El indice %d de player no es valido"),player);
		return false;
	}
}

PD_GM_LogicCharacter* PD_PlayersManager::GetCharacterByID(FString id) {
	for (int i = 0; i < dataPlayers.Num(); i++) {
		if (dataPlayers[i]->logic_Character->GetIDCharacter() == id) {
			UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetCharacterByID : probando con %s"), *dataPlayers[i]->logic_Character->GetIDCharacter());
			return dataPlayers[i]->logic_Character;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("PlayerManager:: GetCharacterByID ERROR: No se ha encontrado character con id %s"), *id);
	return nullptr;
}
PD_GM_LogicCharacter* PD_PlayersManager::GetCharacterByIndex(int i) {
	if (CheckPlayerIndex(i)) {
		return dataPlayers[i]->logic_Character;
	}else {
		return nullptr;
	}
	
}

#pragma region Map Already Instantiated FUNCTIONS

bool PD_PlayersManager::Check_All_Players_Have_Map_Already_Instantiated() {

	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::Check_All_Players_Have_Map_Already_Instantiated()"));


	for (int i = 0; i < dataPlayers.Num(); i++) 
	{
		UE_LOG(LogTemp, Warning, TEXT("mapAlreadyInstantiated for player %d is  %s"), dataPlayers[i]->ID_PLAYER,(dataPlayers[i]->mapAlreadyInstantiated ? TEXT("True") : TEXT("False")));
		if (!dataPlayers[i]->mapAlreadyInstantiated)
		{

			UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::Check_All_Players_Have_Map_Already_Instantiated() player with no map instantiated found! ID_PLAYER=%d"), dataPlayers[i]->ID_PLAYER);
			return false;
		}
	}
	return true;

}


void PD_PlayersManager::Mark_Player_As_Map_Already_Instantiated(int player) {

	/*for (int i = 0; i < dataPlayers.Num(); i++) {
		if (dataPlayers[i]->ID_PLAYER == player) {
			dataPlayers[i]->mapAlreadyInstantiated = true;
			UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::Mark_Player_As_Map_Already_Instantiated() Update mapAlreadyInstantiated on ID_PLAYER=%d"), dataPlayers[i]->ID_PLAYER);

		}
	}
	*/
	dataPlayers[player]->mapAlreadyInstantiated = true;
	UE_LOG(LogTemp, Log, TEXT("PD_PlayersManager::Mark_Player_As_Map_Already_Instantiated() Update mapAlreadyInstantiated on ID_PLAYER=%d"), dataPlayers[player]->ID_PLAYER);

}

#pragma endregion