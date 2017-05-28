// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "PD_IA_TaskFleeTargetCalc.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_IA_TaskFleeTargetCalc : public UBTTaskNode
{
		GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	bool CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp);
	
	
};
