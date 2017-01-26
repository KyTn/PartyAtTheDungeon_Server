// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_MG_StaticMap.h"
#include "PATD_MG_LogicPosition.h"

PATD_MG_StaticMap::PATD_MG_StaticMap()
{
	_rooms = new TArray<PATD_MG_Room*>();
	_LogicPositionsRefs = new TArray<PATD_MG_LogicPosition*>();
}

PATD_MG_StaticMap::~PATD_MG_StaticMap()
{
}


bool PATD_MG_StaticMap::AddNewLogicPosition(uint32 x, uint32 y)
{
	PATD_MG_LogicPosition* lp = new PATD_MG_LogicPosition(x, y);
	_LogicPositionsRefs->Add(lp);

	return false;
}
bool PATD_MG_StaticMap::AddLogicPosition(PATD_MG_LogicPosition* ref) { return false; }
