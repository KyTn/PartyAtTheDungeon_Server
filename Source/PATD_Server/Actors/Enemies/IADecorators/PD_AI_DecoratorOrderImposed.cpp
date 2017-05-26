// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_AI_DecoratorOrderImposed.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"



bool UPD_AI_DecoratorOrderImposed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	return AIController->orderImpossed;
}