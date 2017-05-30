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

	void AddNewPlayer(FString newID_Client, int player);
	int GetNumPlayers();


	//Funciones de modificacion de datos de players
	//void setTurnOrders(FStructTurnOrders* turnOrders,int player);

	//Funciones de consulta de datos de players
	bool AllPlayersReady();
	bool AllPlayersSendOrders();
	bool AnyPlayerDead();
	bool AllAnimationEnd();
	int GetIndexClientMaster();
	//Para encontrar un structPlayer por su idCliente, el idCliente es el que viene del movil, unico.
	StructPlayer* GetStructPlayerByIDClient(FString ID_ClientToSearch);

	//Para encontrar un structPlayer por el idCharacter del character que contiene el propio structPlayer, Pej: Player_00
	StructPlayer* GetStructPlayerByIDCharacter(FString ID_CharacterToSearch);

	int GetMaxLenghtActions(EActionPhase phase);
	int GetPlayerMaxLenghtActions(EActionPhase phase);
	//Funciones de consulta de mision y objetivos de la partida
	
	//FStructTurnOrders* getTurnOrders(int player);
	//Funcion de acceso directo al struct 
	StructPlayer* GetDataStructPlayer(int player);


	TArray<StructPlayer*> GetDataPlayers() { return dataPlayers;}

	PD_GM_LogicCharacter* GetCharacterByID(FString id);
	PD_GM_LogicCharacter* GetCharacterByIndex(int i);

#pragma region Map Already Instantiated FUNCTIONS

	bool Check_All_Players_Have_Map_Already_Instantiated();
	void Mark_Player_As_Map_Already_Instantiated(int player);

#pragma endregion

};
