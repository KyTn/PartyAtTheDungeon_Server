// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "PD_ServerGameInstance.generated.h"

//forward declarations
//class PD_NW_SocketManager;
class PD_NW_NetworkManager;

/**
*
*/

UCLASS()
class PATD_SERVER_API UPD_ServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

		//Networking
		//PD_NW_SocketManager* socketManager;
		

	
	//Functions ====
	//Funcion para inicializar entre otros el socketManager. 
	void InitializeNetworking();

	//Funcion para volver a poner todo adecuadamente despues de un travel.
	void InitializeAfterTravel();

public:
	PD_NW_NetworkManager* networkManager;

	int clientMasterIndex = -1;


	///CONSTANTES
	const int32 defaultServerPort = 8890;

	//Overwrites

	//Funcion que llama al inicializar 
	//Existe ya UWorld aqui, y GetTimerManager()??

	virtual void Init();

	virtual void Shutdown();

//	PD_NW_SocketManager* GetSocketManager();

	void LoadMap(FString mapName);

	void SetClientReady(int indexClient);

	bool CheckForAllClientReady();


	//Funciones que son llamadas desde BP / Funciones UTILES para el JUEGO
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void sendMap();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void InitServerActoWhenLoadMap();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	FString GetServerIP();
	
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	TArray<FString> GetPlayersConnected();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	TArray<bool> GetPlayersReady();
};
