// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PATD_Server.h"

/// FORWARD DECLARATIONS
class PD_MG_StaticMap;


struct RoomTemplateInfo {

	FString NAME;
	uint32 ID;
	TArray<FString> TAGS;
	uint32 WIDTH;
	uint32 HEIGHT;
	TArray<TArray<TCHAR>> DATA;

};


class PATD_SERVER_API PD_MG_MapGenerationUtils
{

public:
	PD_MG_MapGenerationUtils();
	~PD_MG_MapGenerationUtils();

	bool ReadAndPrepareRoomTemplateInfos(FString filepath, TArray<RoomTemplateInfo> roomTemplates);
	bool GenerateRandomStaticMap(PD_MG_StaticMap* StaticMap);

private:
	void FillRoomTemplateInfoWith(RoomTemplateInfo *r, FString readedString);
	void ParseTags(TArray<FString> tags, FString braquets);


};
