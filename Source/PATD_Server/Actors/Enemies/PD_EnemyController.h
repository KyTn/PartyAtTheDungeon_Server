// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/PD_GenericController.h"
#include "PD_EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_EnemyController : public APD_GenericController
{

	GENERATED_BODY()
	APD_EnemyController();
	APD_EnemyController(const FString type);
	
	UPROPERTY(EditAnywhere, Category = "blackboard")
		UBlackboardData* blackboardData;

	UPROPERTY(EditAnywhere, Category = "behavior")
		UBehaviorTree* behaviorTree;


	virtual bool MoveTo(float x, float y) override;
	virtual bool ActionTo(float x, float y, uint8 id_action) override;
	virtual bool Animate(uint8 typeAnimation) override;
	
	void StartTurn();



	UFUNCTION(BlueprintCallable, Category = Behaviour)
		bool AreEnemiesNear();
};
