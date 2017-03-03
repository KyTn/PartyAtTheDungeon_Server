// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/PD_GenericController.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PD_EnemyController.generated.h"


class PD_GM_MapManager;
/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_EnemyController : public APD_GenericController
{
private:
	PD_GM_MapManager* mapMng;
	TArray<FStructOrderAction*> actions;
	PD_MG_LogicPosition currentPos;
	PD_MG_LogicPosition ActionPos;
public:
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
	
	void StartTurn(PD_GM_MapManager* refMap,PD_MG_LogicPosition inCurrentPos);

	TArray<FStructOrderAction*> GetActions() { return actions;}

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void ArePlayersNear();

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void AddAttack();

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void AddMove();
};
