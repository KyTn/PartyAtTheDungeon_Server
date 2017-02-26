// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"


#pragma region Class_References

class PD_MG_StaticMap;
class PD_MG_DynamicMap;

#pragma endregion 


class PATD_SERVER_API PD_MG_MapParser
{
private:
	PD_MG_StaticMap* Parsing_v_0_1(TArray<FString> fileReaded, PD_MG_StaticMap* staticMapRef, PD_MG_DynamicMap* DynamicMapRef);

	uint32 ReadRawMap(TArray<FString> fileReaded, uint32 firstIndex, PD_MG_StaticMap* staticMapRef);

	uint32 ReadTypesEnemies(TArray<FString> fileReaded, uint32 nextIndexRead, PD_MG_DynamicMap* dynamicMapRef);

	void ReadEnemiesMap(TArray<FString> fileReaded, uint32 nextIndexRead, PD_MG_DynamicMap* dynamicMapRef);
public:
	PD_MG_MapParser();
	~PD_MG_MapParser();

	PD_MG_StaticMap* StartParsingFromFile(FString* filepath);
	PD_MG_StaticMap* StartParsingFromFile(FString* filepath, PD_MG_StaticMap*  staticMapRef, PD_MG_DynamicMap* DynamicMapRef);

};
