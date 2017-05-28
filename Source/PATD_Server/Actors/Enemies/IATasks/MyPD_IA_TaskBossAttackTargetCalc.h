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
	
	
	bool CalculateFireball(UBehaviorTreeComponent& OwnerComp);
};
