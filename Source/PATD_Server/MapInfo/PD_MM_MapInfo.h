// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MapGeneration/PD_MG_LogicPosition.h"


// FORWARD DECLARATIONS
class PD_MG_LogicPosition;
class PD_MG_StaticMap;
class PD_GM_MapManager;




class PATD_SERVER_API PD_MM_Room
{
public:
	PD_MM_Room();
	PD_MM_Room(int idRoom);
	~PD_MM_Room();

	bool IsSpawnRoom;
	int IDRoom;
	
	TArray<PD_MG_LogicPosition> LogicPosInRoom;

	TMap<PD_MG_LogicPosition, AActor*> tiles;
	TMap<PD_MG_LogicPosition, AActor*> walls;
	TMap<PD_MG_LogicPosition, AActor*> interactuables;

	bool AddLogicPos(PD_MG_LogicPosition logpos);
	int GetIDRoom() { return IDRoom; };

	bool AddTile(PD_MG_LogicPosition logpos, AActor* tile);
	bool AddWall(PD_MG_LogicPosition logpos, AActor* wall);
	bool AddInteractuable(PD_MG_LogicPosition logpos, AActor* interactuable);

};


class PATD_SERVER_API PD_MM_MapInfo
{

public:
	PD_MM_MapInfo(PD_GM_MapManager* mM);
	~PD_MM_MapInfo();

	PD_GM_MapManager* mapManager;

	PD_MM_Room* SpawnRoom;
	TArray<PD_MG_LogicPosition> allLogicPos;
	TArray<PD_MM_Room> rooms;
	TMap<PD_MG_LogicPosition, PD_MM_Room> roomByLogPos;


	// Devuelve un puntero al Room que tenga ese logPos. True si existe. 
	bool RoomOf(PD_MG_LogicPosition logpos, PD_MM_Room *room);

	// Inicializa el vector de rooms dado un staticMap
	void CalculateRooms();



	bool AddWall(PD_MG_LogicPosition logpos, AActor* wall);

	bool AddTile(PD_MG_LogicPosition logpos, AActor* tile);

	bool AddInteractuable(PD_MG_LogicPosition logpos, AActor* interactuable);


};


