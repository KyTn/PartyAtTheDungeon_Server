// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_DynamicMap.h"
#include "PD_MG_LogicPosition.h"
PD_MG_DynamicMap::PD_MG_DynamicMap()
{
	//_rooms = new TArray<PD_MG_Room*>();
	//_LogicPositionsRefs = new TArray<PD_MG_LogicPosition*>();
}

PD_MG_DynamicMap::~PD_MG_DynamicMap()
{
}


PD_MG_LogicPosition* PD_MG_DynamicMap::AddNewLogicPosition(uint32 x, uint32 y, TCHAR c)
{
	PD_MG_LogicPosition* lp = new PD_MG_LogicPosition(x, y);

	_LogicPositionsRefs.Add(lp);
	_xymap.Add(*lp, c);

	return lp;
}

void PD_MG_DynamicMap::AddEnemyDictionary(TCHAR key, FString value) {
	enemies.Add(key,value);
}

FString PD_MG_DynamicMap::GetEnemy(TCHAR key) {
	FString idEnemy = enemies[key];
	return idEnemy;
}

bool PD_MG_DynamicMap::IsEnemy(TCHAR key) {
	return enemies.Contains(key);
}


bool PD_MG_DynamicMap::Clear() { return false; }