// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "LevelsNameDictionary.h"

LevelsNameDictionary::LevelsNameDictionary()
{
}

LevelsNameDictionary::~LevelsNameDictionary()
{
}

FString LevelsNameDictionary::GetMapName(int lvl)
{
	FString mapName = "";
	switch (lvl)
	{
	case 1:
	
		mapName =  name_LVL_1;
		break;
	case 2:
		
		mapName = name_LVL_2;
		break;
	case 3:
	
		mapName = name_LVL_3;
		break;
	case 4:
	
		mapName = name_LVL_4;
		break;
	case 5:

		mapName = name_LVL_5;
		break;
	}
	return mapName;
}