// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class PD_GM_MapManager;
#include "stlastar.h" // See header for copyright and usage information
/**
 * 
 */
class PATD_SERVER_API PD_GM_Pathfinder
{
	PD_GM_MapManager* mapManager;
	
public:
	PD_GM_Pathfinder(PD_GM_MapManager* inMapManager);
	~PD_GM_Pathfinder();

	TArray<PD_MG_LogicPosition> getPathFromTo(PD_MG_LogicPosition posFrom, PD_MG_LogicPosition posTo);
};
