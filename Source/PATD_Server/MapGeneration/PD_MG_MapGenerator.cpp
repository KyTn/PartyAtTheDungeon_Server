// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_MapGenerator.h"
#include "PD_MG_MapGenerationUtils.h"


PD_MG_MapGenerator::PD_MG_MapGenerator()
{
}

PD_MG_MapGenerator::~PD_MG_MapGenerator()
{
}
void PD_MG_MapGenerator::Init()
{
	mgUtils = PD_MG_MapGenerationUtils();

	UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - reading room templates from file ..."));
	if (mgUtils.ReadAndPrepareRoomTemplateInfosFromFile(FPaths::GameDir()+"Content/DungeonTestingMaps/test1.rooms", roomTemplateArray)) {
		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - %d room templates readed!"), roomTemplateArray.Num());
	}
	else {

		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - a problem has ocurred while reading room template file. Reading from hardcoded templates ..."));

		mgUtils.GetPreloadedData(roomTemplateArray);

		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - %d room templates added from hardcoded templates!"), roomTemplateArray.Num());

	}
	
	for (int i = 0; i < roomTemplateArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerator::Init - %s"), *roomTemplateArray[i].NAME);
	}

}



FString PD_MG_MapGenerator::GenerateProceduralMap(PD_MatchConfigManager * MapManConfig, int numPlayers) {
	MapProceduralInfo map = MapProceduralInfo(100,100);
	mgUtils.GenerateRandomStaticMap(map, roomTemplateArray, 100, 100, MapManConfig, numPlayers);
	FString s = map.ToString();
	s.Append(mgUtils.EnemiesGeneration(map));
	s.Append("\n0");
	return s;
}