// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "PD_ServerGameInstance.generated.h"

/**
*
*/
//forward declarations
class PD_NW_SocketManager;
UCLASS()
class PATD_SERVER_API UPD_ServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

		//Networking
		PD_NW_SocketManager* socketManager;



	//Functions ====
	//Funcion para inicializar entre otros el socketManager. 
	void initializeNetworking();

	//Funcion para volver a poner todo adecuadamente despues de un travel.
	void initializeAfterTravel();

public:
	//Overwrites

	//Funcion que llama al inicializar 
	//Existe ya UWorld aqui, y GetTimerManager()??

	virtual void Init();

	//void Shutdown();

	PD_NW_SocketManager* GetSocketManager();


};
