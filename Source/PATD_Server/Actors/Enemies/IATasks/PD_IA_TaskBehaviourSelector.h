// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "PD_IA_TaskBehaviourSelector.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_IA_TaskBehaviourSelector : public UBTTaskNode
{
	GENERATED_BODY()


	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float minHPDefense=50;
	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float minHPFlee=10;
	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float minHPBerserker=5;

	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float coward_minHPDefense=60;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		float coward_minHPFlee=15;
	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float coward_minHPBerserker=0;

	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float warlike_minHPDefense=40;
	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float warlike_minHPFlee=10;
	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float warlike_minHPBerserker=10;
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	

		//UPD_IA_TaskBehaviourSelector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer);
};
