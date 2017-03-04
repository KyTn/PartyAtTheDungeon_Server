// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PD_AIController.generated.h"

class PD_GM_MapManager;
/**
*
*/
UCLASS()
class PATD_SERVER_API APD_AIController : public AAIController
{
private:
	PD_GM_MapManager* mapMng;
	FStructTurnOrders* actions;
	PD_MG_LogicPosition currentPos;
	PD_MG_LogicPosition ActionPos;
	TArray<bool> aviablePos;
public:
	GENERATED_BODY()
	APD_AIController();
	//APD_AIController(const FString type);

	UPROPERTY(EditAnywhere, Category = "blackboard")
		UBlackboardData* blackboardData;

	UPROPERTY(EditAnywhere, Category = "behavior")
		UBehaviorTree* behaviorTree;


	void StartTurnZombie(PD_GM_MapManager* refMap, PD_MG_LogicPosition inCurrentPos);

	FStructTurnOrders* GetActions() { return actions; }

	void UpdateAP();

	bool EndBehaviour();

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void ArePlayersNear();

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void AddAttack();

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void AddMove();
};
