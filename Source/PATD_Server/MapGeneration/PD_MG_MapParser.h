// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PATD_Server/MapGeneration/ParserActor.h"


#pragma region Class_References

class PD_MG_StaticMap;

#pragma endregion 


class PATD_SERVER_API PD_MG_MapParser
{
private:
	PD_MG_StaticMap* Parsing_v_0_1(TArray<FString> fileReaded, PD_MG_StaticMap* staticMapRef);

	uint32 ReadRawMap(TArray<FString> fileReaded, uint32 firstIndex, PD_MG_StaticMap* staticMapRef);
public:
	PD_MG_MapParser();
	~PD_MG_MapParser();

	PD_MG_StaticMap* StartParsingFromFile(FString* filepath);
	PD_MG_StaticMap* StartParsingFromFile(FString* filepath, PD_MG_StaticMap*  staticMapRef);

	void InstantiateStaticMap(AParserActor* parserActor);
};
