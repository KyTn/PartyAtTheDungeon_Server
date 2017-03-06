// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PATD_Server/GM_Game/PD_GM_MapManager.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PD_EnemyController.h"


bool APD_EnemyController::MoveTo(float x, float y)
{
	FVector newPosition = FVector(x, y, 0);

	MoveToLocation(newPosition, -1.0f, true, false, false, true, 0, true);

	return true;
}

bool APD_EnemyController::ActionTo(float x, float y, uint8 id_action)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "AL ATAQUERRRRRR !!!!!");

	return true;

}

bool APD_EnemyController::Animate(uint8 typeAnimation)
{
	return true;
}
