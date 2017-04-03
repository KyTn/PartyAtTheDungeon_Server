// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PATD_Server/Structs/PD_NetStructs.h"
#include "Actors/PD_GenericController.h"
//#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
//#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PD_AIController.generated.h"


class APD_E_Character;
class PD_GM_LogicCharacter;
class PD_GM_MapManager;
/**
*
*/
UCLASS()
class PATD_SERVER_API APD_AIController : public APD_GenericController
{
	GENERATED_BODY()
private:
		PD_GM_MapManager* mapMng;
		FStructTurnOrders* actions;
		PD_GM_LogicCharacter* currentcharac;
public:
		APD_AIController();

	UPROPERTY(transient)
		UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(transient)
		UBehaviorTreeComponent* BehaviorTreeComp;

	void Possess(APawn* charac) override;

	void StartTurn(PD_GM_MapManager* refMap, PD_GM_LogicCharacter* logicCharacter);

};
