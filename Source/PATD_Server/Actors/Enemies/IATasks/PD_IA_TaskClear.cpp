// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskClear.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "MapGeneration/PD_MG_LogicPosition.h"


EBTNodeResult::Type UPD_IA_TaskClear::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	AIController->turnsForGoal--;
	AIController->turnTargetCharacter=nullptr;
	AIController->turnNumAttacks = 0;
//	AIController->turnTargetPosition = PD_MG_LogicPosition(0,0,0);

	AIController->EndAITurnCalculation();

	return EBTNodeResult::Succeeded;

}