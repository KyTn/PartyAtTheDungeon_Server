// Fill out your copyright notice in the Description page of Project Settings.

#pragma once




//forward declaration
#include "Structs/PD_ServerEnums.h"
struct FStructNewConnection;
struct StructPlayer;

/**
 * 
 */
class PATD_SERVER_API PD_PlayersManager
{
private:
	TArray<StructPlayer*> dataPlayers;


	bool CheckPlayerIndex(int player);

public:
	PD_PlayersManager();
	~PD_PlayersManager();

	

	//Funciones de gestion de conexion

	void AddNewPlayer(FStructNewConnection* newClientConnection, int player);
	int GetNumPlayers();


	//Funciones de modificacion de datos de players
	//void setTurnOrders(FStructTurnOrders* turnOrders,int player);

	//Funciones de consulta de datos de players
	bool AllPlayersReady();
	bool AllPlayersSendOrders();
	bool AnyPlayerDead();
	bool AllAnimationEnd();
	int GetIndexClientMaster();

	int GetMaxLenghtActions(EActionPhase phase);
	int GetPlayerMaxLenghtActions(EActionPhase phase);
	//Funciones de consulta de mision y objetivos de la partida
	
	//FStructTurnOrders* getTurnOrders(int player);
	//Funcion de acceso directo al struct 
	StructPlayer* GetDataStructPlayer(int player);

	


};
