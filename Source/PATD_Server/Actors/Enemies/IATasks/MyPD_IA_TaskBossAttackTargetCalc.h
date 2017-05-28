// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Enemies/IATasks/PD_IA_TaskAttackTargetCalc.h"
#include "MyPD_IA_TaskBossAttackTargetCalc.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UMyPD_IA_TaskBossAttackTargetCalc : public UPD_IA_TaskAttackTargetCalc
{
	GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int probToUseMinions = 50;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int rangeControlMinions = 10;

	bool CalculateFireball(UBehaviorTreeComponent& OwnerComp);
	bool SetMinionsBehaviour(UBehaviorTreeComponent& OwnerComp);
};
