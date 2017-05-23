// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_MapGenerator.h"
#include "PD_MG_MapGenerationUtils.h"
#include "Structs/PD_NetStructs.h"


PD_MG_MapGenerator::PD_MG_MapGenerator()
{
}

PD_MG_MapGenerator::~PD_MG_MapGenerator()
{
}
void PD_MG_MapGenerator::Init()
{
	mgUtils = PD_MG_MapGenerationUtils(this);
	roomTemplateArray = TArray<RoomTemplateInfo>();
	Map_RoomTemplate_By_MapSkinsType = TMap<MapSkinType, TArray<RoomTemplateInfo*>>();
	//UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - reading room templates from file ..."));
	if (mgUtils.ReadAndPrepareRoomTemplateInfosFromFile(FPaths::GameDir()+"Content/DungeonTestingMaps/test1.rooms", roomTemplateArray)) {
		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - %d room templates readed!"), roomTemplateArray.Num());
		Mapping_RoomTemplateInfo();
	}
	else {

		//UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - a problem has ocurred while reading room template file. Reading from hardcoded templates ..."));

		mgUtils.GetPreloadedData(roomTemplateArray);

		//UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - %d room templates added from hardcoded templates!"), roomTemplateArray.Num());

	}
	/*
	for (int i = 0; i < roomTemplateArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - %s"), *roomTemplateArray[i].NAME);
	}
	*/
}



FString PD_MG_MapGenerator::GenerateProceduralMap_v01(PD_MatchConfigManager * MapManConfig, int numPlayers) {

	MapProceduralInfo map = MapProceduralInfo(250,250);
	mgUtils.GenerateRandomStaticMap_v01(map, roomTemplateArray, 250, 250, MapManConfig, numPlayers);
	FString s = map.ToString();
	s.Append(mgUtils.EnemiesGeneration_v01(map));
	s.Append("\n0");
	return s;
}



FStructMapData * PD_MG_MapGenerator::GenerateProcedural_FStructMapData_v02(PD_MatchConfigManager * MapManConfig, int numPlayers) {

	FStructMapData * mapData = new FStructMapData();
	MapProceduralInfo map = MapProceduralInfo(mapData, 250, 250);
	mgUtils.GenerateRandomStaticMap_v02(map, roomTemplateArray, 250, 250, MapManConfig, numPlayers);
	
	
	return mapData;
}


void PD_MG_MapGenerator::Mapping_RoomTemplateInfo()
{
	for (int i = 0; i < roomTemplateArray.Num(); i++) {
		for (int j = 0; j < roomTemplateArray[i].TAGS.Num(); j++) {

			if (Map_RoomTemplate_By_MapSkinsType.Contains(roomTemplateArray[i].TAGS[j]))
			{
				Map_RoomTemplate_By_MapSkinsType[roomTemplateArray[i].TAGS[j]].Add(&(roomTemplateArray[i]));
			}
			else
			{
				TArray<RoomTemplateInfo*> t = TArray<RoomTemplateInfo*>();
				Map_RoomTemplate_By_MapSkinsType.Add(roomTemplateArray[i].TAGS[j], t);
				Map_RoomTemplate_By_MapSkinsType[roomTemplateArray[i].TAGS[j]].Add(&(roomTemplateArray[i]));
			}
		}
	}
}