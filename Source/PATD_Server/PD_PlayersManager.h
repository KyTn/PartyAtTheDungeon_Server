// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Incluyendo #include "Structs/PD_ServerStructs.h" tenemos todos los structs (porque hace include de los demas)
#include "Structs/PD_ServerStructs.h"

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

	void AddNewPlayer(FStructNewConnection* newClientConnection);



	//Funciones de modificacion de datos de players
	void setTurnOrders(FStructTurnOrders* turnOrders,int player);

	//Funciones de consulta de datos de players
	bool AllPlayersReady();
	bool AllPlayersSendOrders();

	


};
