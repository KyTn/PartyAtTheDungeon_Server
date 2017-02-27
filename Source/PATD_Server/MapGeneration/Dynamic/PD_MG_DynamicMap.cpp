// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_DynamicMap.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"

PD_MG_DynamicMap::PD_MG_DynamicMap()
{
	//_rooms = new TArray<PD_MG_Room*>();
	//_LogicPositionsRefs = new TArray<PD_MG_LogicPosition*>();
}

PD_MG_DynamicMap::~PD_MG_DynamicMap()
{
}

void PD_MG_DynamicMap::AddNewEnemy(uint32 x, uint32 y, PD_GM_LogicCharacter* ch, EEnemiesType type) {

	PD_MG_LogicPosition* lp = new PD_MG_LogicPosition(x, y);
	_LogicPositionsRefs.Add(lp);
	_xymap.Add(*lp, type);
	TArray<	PD_GM_LogicCharacter*> chs;
	chs.Add(ch);
	enemies.Add(*lp,chs);
}

void PD_MG_DynamicMap::UpdateActor(AActor* actor, PD_MG_LogicPosition* lp) {

/*	TArray<	PD_GM_LogicCharacter*> chs = enemies[*lp];
	PD_GM_IALogicCharacter* ch = chs[0];
	ch->UpdateCharacter(actor);
	chs.Reset();
	chs.Add(ch);
	enemies.Emplace(*lp,chs);
	*/
}

bool PD_MG_DynamicMap::Clear() { return false; }