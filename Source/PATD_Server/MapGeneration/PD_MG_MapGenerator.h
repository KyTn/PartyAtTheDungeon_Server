// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PD_MG_MapGenerationUtils.h"
#include "Structs/PD_NetStructs.h"



/**
 * 
 */
class PD_MatchConfigManager;

class PATD_SERVER_API PD_MG_MapGenerator
{
public:

	TMap<MapSkinType, TArray<RoomTemplateInfo*>> Map_RoomTemplate_By_MapSkinsType;

	PD_MG_MapGenerationUtils mgUtils;
	TArray<RoomTemplateInfo> roomTemplateArray;

	PD_MG_MapGenerator();
	~PD_MG_MapGenerator();

	void Init();

	FString GenerateProceduralMap_v01(PD_MatchConfigManager * MapManConfig, int numPlayers);
	FStructMapData* GenerateProcedural_FStructMapData_v02(PD_MatchConfigManager * MapManConfig, int numPlayers);
	void Mapping_RoomTemplateInfo();
};
