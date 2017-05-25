// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "Structs/PD_ServerEnums.h"
#include "PD_IA_DecoratorCheckBehaviour.generated.h"

/**
 * 
 */

UCLASS()
class PATD_SERVER_API UPD_IA_DecoratorCheckBehaviour : public UBTDecorator
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, Category = Condition)
		TEnumAsByte<EIABehaviour> behaviour;
	

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
