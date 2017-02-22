// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Include de herencia (interfaz)
#include "NW_NetWorking/EventLayer/PD_NW_iEventObserver.h"

//Include de structs
#include "Structs/PD_ServerStructs.h"

//Includes de unreal
#include "Engine/GameInstance.h"
#include "LevelsNameDictionary.h"
#include "PD_ServerGameInstance.generated.h"




//forward declarations
//class PD_NW_SocketManager;
class PD_NW_NetworkManager;
class PD_MG_Map;
class AParserActor;
class PD_PlayersManager;
/**
*
*/
	
UCLASS()															//Interfaz observer para reaccionar a eventos del netmanager
class PATD_SERVER_API UPD_ServerGameInstance : public UGameInstance, public PD_NW_iEventObserver
	


{

	//Aqui deberiamos dejar solo las funciones de inicializacion de managers y de comunicacion con interfaz
	//El gameinstance sera una especie de gestor de interfaces de menu.
	//Tendria el ServerManager que seria el encargado de todo el switch gigante, el estado del servidor y la gestion de conexiones.
	
	//Ademas convendria hacer ya una limpieza para poder tener en cuenta los nuevos comentarios y no los viejos.

	

	GENERATED_BODY()

		//Networking
		//PD_NW_SocketManager* socketManager;
		

	
	//Functions ====
	//Funcion para inicializar entre otros el socketManager. 
	void InitializeNetworking();
		
	//Funcion para volver a poner todo adecuadamente despues de un travel.
	void InitializeAfterTravel();
	
	FString serverIP;
	FString serverName;
	

public:

	//Para tener los nombres de los niveles - diferenciar ejecución en editor o en ejecutable
	LevelsNameDictionary levelsNameDictionary;
	PD_MG_Map*  LOGIC_MAP;
	AParserActor* parserActor;
	PD_NW_NetworkManager* networkManager;

	int clientMasterIndex = -1;
	TArray<bool>* ready;
	int numPlayers = 0;
	///CONSTANTES
	const int32 defaultServerPort = 8890;
	//TArray <uint8> ip = TArray<uint8>();

	PD_PlayersManager* playersManager;

	//Overwrites

	//Funcion que llama al inicializar 
	//Existe ya UWorld aqui, y GetTimerManager()??

	virtual void Init();

	virtual void Shutdown();

//	PD_NW_SocketManager* GetSocketManager();

	void LoadMap(FString mapName);

	void SetClientReady(int indexClient);

	bool CheckForAllClientReady();

	//Inicializa la variable privada ip y el serverName
	void InitializeServerAddress();

	//Funciones del eventObserver, para determinar que eventos recibimos y para procesarlos

	virtual	bool SuscribeToEvents(int inPlayer, UStructType inType);
	virtual void HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);

	//=========
	//Funciones de gestion del estado (maquina de estados)
	//=========
	//Esquema
	/*
	el clientMaster se conecta al servidor
		Los clientes se meten ahora.
		el clientMaster configura y da ok.
		les sale a todos para elegir su personaje.
		Aqui no se puede conectar nadie mas.
		Lo crean y le dan a ready.
		*/
	//Struct con el estado para la maquina de estados del gameManager
	StructServerState* structGameState;

	//Funciones de configuracion de la maquina
	//Transiciones
	void UpdateState();
	//Acciones al empezar el estado
	void OnBeginState();

	//Funciones auxiliares
	//Control directo del estado, llama a OnBeginState
	void ChangeState(EServerState newState);

	void InitState();


	//=========
	//Funciones que son llamadas desde BP / Funciones UTILES para el JUEGO
	//=========
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void sendMap();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void InitServerActoWhenLoadMap();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void InitGameMap();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		FString GetServerIP();
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		FString GetServerName();
	
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	TArray<FString> GetPlayersConnected();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	TArray<bool> GetPlayersReady();


};
