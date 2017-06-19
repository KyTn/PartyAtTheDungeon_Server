// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "PD_IA_TaskSwindler_TargetCalc.generated.h"


/**
*
*/
UCLASS()
class PATD_SERVER_API UPD_IA_TaskSwindler_TargetCalc : public UBTTaskNode
{
	GENERATED_BODY()


		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	bool CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp);


	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		float APMaxAttack = 2;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		float APMaxAttackWarlike = 3;
};
