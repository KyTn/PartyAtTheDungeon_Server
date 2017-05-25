// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "PD_AI_DecoratorNewBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_AI_DecoratorNewBehaviour : public UBTDecorator
{
	GENERATED_BODY()
	
	
		virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
