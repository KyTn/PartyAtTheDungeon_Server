// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_StaticMap.h"
#include "PD_MG_LogicPosition.h"

PD_MG_StaticMap::PD_MG_StaticMap()
{
	_rooms = new TArray<PD_MG_Room*>();
	_LogicPositionsRefs = new TArray<PD_MG_LogicPosition*>();
}

PD_MG_StaticMap::~PD_MG_StaticMap()
{
}


bool PD_MG_StaticMap::AddNewLogicPosition(uint32 x, uint32 y)
{
	PD_MG_LogicPosition* lp = new PD_MG_LogicPosition(x, y);
	_LogicPositionsRefs->Add(lp);

	return false;
}
bool PD_MG_StaticMap::AddLogicPosition(PD_MG_LogicPosition* ref) { return false; }
