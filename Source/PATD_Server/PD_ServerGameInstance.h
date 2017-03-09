// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Include de herencia (interfaz)
#include "NW_NetWorking/EventLayer/PD_NW_iEventObserver.h"



//forward declarations
//Include de enums (no se pueden hacer forward declaration)
#include "Structs/PD_ServerEnums.h"
struct StructServerState;
class PD_NW_NetworkManager;
class PD_PlayersManager;
class PD_MG_MapParser;
class AMapInstantiatorActor;
class PD_GM_MapManager;
class PD_GM_GameManager;
class PD_GM_EnemyManager;
//Includes de unreal
#include "Engine/GameInstance.h"
#include "LevelsNameDictionary.h"
#include "PD_ServerGameInstance.generated.h"




/**
*
*/
	
UCLASS()															//Interfaz observer para reaccionar a eventos del netmanager
class PATD_SERVER_API UPD_ServerGameInstance : public UGameInstance, public PD_NW_iEventObserver
	


{
	GENERATED_BODY()
	void InitializeNetworking();
		
	FString serverIP;
	FString serverName;
	

public:

	//Para tener los nombres de los niveles - diferenciar ejecución en editor o en ejecutable
	LevelsNameDictionary levelsNameDictionary;
	//PD_MG_Map*  LOGIC_MAP;
	
	PD_NW_NetworkManager* networkManager;
	PD_PlayersManager* playersManager;
	PD_MG_MapParser* mapParser;
	PD_GM_MapManager* mapManager;
	PD_GM_GameManager* gameManager;
	// PD_GM_EnemyManager* enemyManager; no tiene sentido un enemymanager en el instance. Está en el GameManager
	///CONSTANTES
	const int32 defaultServerPort = 8890;

	FString mapPath=  /*FPaths::GameDir() +*/ "Content/DungeonTestingMaps/test5.dungeon";
	int mapX = 0; //Ancho del mapa
	int mapY = 0; // Largo del mapa

	//Funciones para obtener managers
	PD_GM_GameManager* getGameManager();


	//Overwrites - GameInstance
	virtual void Init();
	virtual void Shutdown();
	//Overwrites - iEventObserver
	virtual	bool SuscribeToEvents(int inPlayer, UStructType inType);
	virtual void HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType); //Aqui controlamos la mayoria de entradas.

	// --- Handlers --- //
	void HandleEvent_NewConnection(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType, bool isMasterClient);
	void HandleEvent_ConfigMatch(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);
	void HandleEvent_ConfigMatchDone(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);
	void HandleEvent_PlayerReady(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType);
	void HandleEvent_LoadPlayerInfo(FStructGeneric* inDataStruct, int inPlayer);


	//Carga de mapa
	void LoadMap(FString mapName);
	//Callback cuando el mapa este cargado (Lo llama el estado GameStateInitializer en su beginPlay)
	void OnLoadedLevel();

	//Inicializa la variable privada ip y el serverName
	void InitializeServerAddress();



	//=========
	//Funciones de gestion del estado (maquina de estados)
	//=========

	//Struct con el estado del server
	StructServerState* structServerState;

	//Funciones de configuracion de la maquina
	//Transiciones
	void UpdateState();
	//Acciones al empezar el estado
	void OnBeginState();
	//Funciones auxiliares
	//Control directo del estado, llama a OnBeginState
	void ChangeState(EServerState newState);
	//void InitState();

	//=========
	//Funciones que son llamadas desde BP / Funciones UTILES para el JUEGO
	//=========

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		FString GetServerIP();
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		FString GetServerName();
	
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	TArray<FString> GetPlayersConnected();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	TArray<bool> GetPlayersReady();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void GetMapsize(float &SizemapX, float &SizemapY);

};
