// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PD_MG_MapGenerationUtils.h"
/**
 * 
 */
class PATD_SERVER_API PD_MG_MapGenerator
{
public:

	PD_MG_MapGenerationUtils mgUtils;
	TArray<RoomTemplateInfo> roomTemplateArray;

	PD_MG_MapGenerator();
	~PD_MG_MapGenerator();

	void Init();

	FString GenerateProceduralMap();

};
