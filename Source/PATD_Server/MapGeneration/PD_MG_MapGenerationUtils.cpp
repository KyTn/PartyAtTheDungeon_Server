// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_MapGenerationUtils.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"

PD_MG_MapGenerationUtils::PD_MG_MapGenerationUtils()
{
}

PD_MG_MapGenerationUtils::~PD_MG_MapGenerationUtils()
{
}


#pragma region READ ROOMS FROM FILE


//TESTING : Content/DungeonTestingMaps/test1.rooms
bool PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfos(FString filepath, TArray<RoomTemplateInfo> roomTemplates) {

	roomTemplates.Empty();
	// Leamos el fichero
	FString FilePath = FPaths::GameDir() + *filepath;
	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *FilePath))
	{
		TArray<FString> fileSplitted;
		FileData.ParseIntoArray(fileSplitted, TEXT("START_ROOM"), true);

		/*
			Name: simple_Cuadrada
			id: sala_1
			tags:
			size_x: 8
			size_y: 8
			W W w w w w W W  
			W . . . . . . W
			w . . . . . . w
			w . . . . . . w
			w . . . . . . w
			W . . . . . . W
			W W w w w w W W
			
		*/

		for (int i = 0; i < fileSplitted.Num(); i++) {
			RoomTemplateInfo rti;
			FillRoomTemplateInfoWith(&rti, fileSplitted[i]);
			roomTemplates.Add(rti);

		}

	}


	return false;
}


void PD_MG_MapGenerationUtils::FillRoomTemplateInfoWith(RoomTemplateInfo *r, FString readedString) {

	TArray<FString> roomInfoSplitted;
	FString aux2 = "";
	
	readedString.ParseIntoArray(roomInfoSplitted, TEXT("\n"), true);

	// GET NAME
	//*aux = roomInfoSplitted.Pop();
	roomInfoSplitted[0].Split(": ", &aux2, &roomInfoSplitted[0]);
	r->NAME = roomInfoSplitted[0];

	// GET ID
	roomInfoSplitted[1].Split(": ", &aux2, &roomInfoSplitted[1]);
	r->ID = (uint32) FCString::Atoi(*(roomInfoSplitted[1]));

	// GET TAGS
	//roomInfoSplitted[1].Split(": ", aux2, &roomInfoSplitted[1]);
	ParseTags(r->TAGS, roomInfoSplitted[2]);

	// GET WIDTH
	roomInfoSplitted[3].Split(": ", &aux2, &roomInfoSplitted[3]);
	r->WIDTH = (uint32)FCString::Atoi(*(roomInfoSplitted[3]));

	// GET HEIGHT
	roomInfoSplitted[4].Split(": ", &aux2, &roomInfoSplitted[4]);
	r->HEIGHT = (uint32)FCString::Atoi(*(roomInfoSplitted[4]));

	// GET DATA
	for (uint32 i = 5; i < r->WIDTH; i++) {
		//TArray<TCHAR> ta = roomInfoSplitted[i].GetCharArray();
		r->DATA.Add(roomInfoSplitted[i].GetCharArray());
	}

}
void PD_MG_MapGenerationUtils::ParseTags(TArray<FString> tags, FString braquets) {
	tags.Empty();

	TArray<FString> s;
	braquets.ParseIntoArray(s, TEXT(","), true);

	if (s.Num() == 0) {
		tags.Add("None");
		return;
	}

	for (int i = 0; i < s.Num(); i++) {
		tags.Add(s[i]);
	}

}

#pragma endregion


bool PD_MG_MapGenerationUtils::GenerateRandomStaticMap(PD_MG_StaticMap* StaticMap) {

	return false;
}