// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MapInfo/MapInstantiation/MapInstantiatorActor.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"

// FORWARD DECLARATIONS
class PD_MG_LogicPosition;
class PD_MM_MapInfo;
class PD_MG_StaticMap;
class PD_MG_DynamicMap;
class PD_GM_GameManager;
class PD_GM_EnemyManager;
class PD_PlayersManager;
class APD_GenericController;
class PD_MM_DoorInfo;
class PD_MM_InteractuableInfo;
class APD_E_Interactuable;

class PATD_SERVER_API PD_GM_MapManager
{
private:

	

	
	// Dada una posici�n l�gica, devuelve el GenericCharacter que est� en esa posici�n, 0 si no hay ninguno. 
	//bool getGenericCharacterAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter);

public:
	PD_GM_MapManager();
	~PD_GM_MapManager();

	int index_enemies_forID = 0;

	void Init();

	AMapInstantiatorActor* instantiator;
	PD_GM_GameManager* _GAMEMANAGER;

	PD_MM_MapInfo* MapInfo;
	PD_MG_DynamicMap* DynamicMapRef;


#pragma region GET RAW INFO FROM STATIC

	bool IsLogicPositionAWall(PD_MG_LogicPosition logpos);
	bool IsLogicPositionATile(PD_MG_LogicPosition logpos);
	bool IsLogicPositionAProp(PD_MG_LogicPosition logpos);
	bool IsLogicPositionADoor(PD_MG_LogicPosition logpos);
	//bool IsLogicPositionASpawn(PD_MG_LogicPosition logpos);
	bool IsLogicPositionAEnemy(PD_MG_LogicPosition logpos);


#pragma endregion



#pragma region GET INFO OF THE MAP


	// Dada una posici�n l�gica, devuelve el GenericCharacter que est� en esa posici�n, que adem�s es un Player. 0 si no hay ninguno. 
	 bool IsTherePlayer(uint32 x, uint32 y);
	// Dada una posici�n l�gica, devuelve el GenericCharacter que est� en esa posici�n, que adem�s es un Enemy. 0 si no hay ninguno. 
	//bool getEnemyAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter);
	TArray<PD_MG_LogicPosition> GetSpawnPoints();

	// Esta funcion devolver� un Interactuable
	AActor* getInteractuableAt(PD_MG_LogicPosition logpos);

	//Dada un LogicPosition, te da la posici�n en el mundo.
	const FVector LogicToWorldPosition(PD_MG_LogicPosition pos);

	//Dada la posici�n en el mundo, te devuelve la posici�n l�gica que estar�a asignada a posici�n en el mundo, exista o no en el mapa. 
	PD_MG_LogicPosition WorldToLogicPosition(FVector pos);

	TArray<PD_MG_LogicPosition> Get_LogicPosition_Adyacents_To(PD_MG_LogicPosition logPos);

	TArray<PD_MG_LogicPosition> Get_LogicPosition_Diagonals_And_Adyacents_To(PD_MG_LogicPosition logPos);

	TArray<PD_MG_LogicPosition> GetAllTilesInRange(float range, PD_MG_LogicPosition logPos);
#pragma endregion


#pragma region INSTANTIATE MAP

	void InstantiateMap();
	void InstantiateRoomAndAdj(uint8 id);
	void InstantiateMapElementBySkin(MapSkinType mapSkin, StaticMapElement element, PD_MG_LogicPosition lp);
	void InstantiateWallAt(PD_MG_LogicPosition lp);
	void InstantiateWallBySkin(PD_MG_LogicPosition lp);
	void InstantiateStraigthWallBySkin(PD_MG_LogicPosition lp, bool rotate);
	void InstantiateDoor(PD_MG_LogicPosition lp, PD_MM_DoorInfo* doorInfo);
	APD_E_Interactuable* InstantiateInteractuable(PD_MG_LogicPosition lp, PD_MM_InteractuableInfo* interInfo);
	void InstantiateDynamicMap();
	void InstantiateEnemies();

#pragma endregion

};

