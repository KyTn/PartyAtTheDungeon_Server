// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "PD_IA_TaskAttackTargetCalc.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_IA_TaskAttackTargetCalc : public UBTTaskNode
{
	GENERATED_BODY()
	
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	bool CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp);


	
};
