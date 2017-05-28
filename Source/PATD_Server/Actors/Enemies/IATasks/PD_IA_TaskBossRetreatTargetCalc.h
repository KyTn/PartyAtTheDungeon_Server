// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "Actors/Enemies/IATasks/PD_IA_TaskFleeTargetCalc.h"
#include "PD_IA_TaskBossRetreatTargetCalc.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_IA_TaskBossRetreatTargetCalc : public UPD_IA_TaskFleeTargetCalc
{
	GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int probToUseMinions=50;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int rangeControlMinions = 10;

	bool SetMinionsBehaviour(UBehaviorTreeComponent& OwnerComp);
	bool SetHealConsumible(UBehaviorTreeComponent& OwnerComp);
};
