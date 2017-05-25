// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "PD_IA_TaskDefenseTargetCalc.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_IA_TaskDefenseTargetCalc : public UBTTaskNode
{
	GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp);
	
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		float APDefense = 3;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		float APDefenseCoward = 4;
	
};
