// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_DecoratorCheckBehaviour.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"


bool UPD_IA_DecoratorCheckBehaviour::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	return AIController->selectedBehaviour == this->behaviour;
	
	//this->GetGameplayTaskOwner(this);
}
