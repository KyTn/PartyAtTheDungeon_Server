// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_PlayersManager.h"

PD_PlayersManager::PD_PlayersManager()
{
}

PD_PlayersManager::~PD_PlayersManager()
{
}

//Funciones de gestion de conexion

void PD_PlayersManager::AddNewPlayer(FStructNewConnection* newClientConnection, int player) {
	StructPlayer* structPlayer = new StructPlayer();
	dataPlayers.Insert(structPlayer, player);
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
	
	for (StructPlayer* player : dataPlayers) {
		if (!player->readyMenu) {
			return false;
		}
	}
	return true;
}

bool PD_PlayersManager::AllPlayersSendOrders() {
	
	for (StructPlayer* player : dataPlayers) {
		if (!player->turnOrders) {
			return false;
		}
	}
	return true;
}

StructPlayer* PD_PlayersManager::getDataStructPlayer(int player) {
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