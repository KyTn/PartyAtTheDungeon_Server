// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_AI_DecoratorNewBehaviour.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"



bool UPD_AI_DecoratorNewBehaviour::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	UE_LOG(LogTemp, Log, TEXT("UPD_AI_DecoratorNewBehaviour:: Entrando a NewBehaviour: turnForGoal%d"), AIController->turnsForGoal);

	return AIController->turnsForGoal<=0;
}