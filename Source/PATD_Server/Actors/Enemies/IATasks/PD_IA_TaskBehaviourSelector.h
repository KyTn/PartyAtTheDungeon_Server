// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Structs/PD_ServerEnums.h"
#include "PD_IA_TaskBehaviourSelector.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API UPD_IA_TaskBehaviourSelector : public UBTTaskNode
{
	GENERATED_BODY()


	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float minHPHalfLife=60;
	UPROPERTY( EditAnywhere, Category = "Selection Parameters")
		float minHPAlmostDead=25;

	/*
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
	*/
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int attackProb = 60;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int defenseProb = 60;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int swindlerProb = 60;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int fleeProb = 60;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		int berserkerProb = 60;


	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		TEnumAsByte<EIABehaviour> warlikeDefaultBehaviour ;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		TEnumAsByte<EIABehaviour> cowardDefaultBehaviour;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		TEnumAsByte<EIABehaviour> smartDefaultBehaviour ;
	UPROPERTY(EditAnywhere, Category = "Selection Parameters")
		TEnumAsByte<EIABehaviour> neutralDefaultBehaviour ;

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
		void SelectBehaviour(UBehaviorTreeComponent & OwnerComp);
		void SelectInSetBehaviour(UBehaviorTreeComponent & OwnerComp, TArray<EIABehaviour> validBehaviours);

		void SelectGoals(UBehaviorTreeComponent & OwnerComp);
		//UPD_IA_TaskBehaviourSelector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer);
};
