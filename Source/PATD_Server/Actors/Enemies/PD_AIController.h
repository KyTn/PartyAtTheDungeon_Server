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
class PD_GM_Pathfinder;
/**
*
*/
UCLASS()
class PATD_SERVER_API APD_AIController : public APD_GenericController
{
	GENERATED_BODY()
private:
		PD_GM_MapManager* mapMng;
		FStructTurnOrders actions;
		PD_GM_LogicCharacter* currentcharac;
		PD_GM_Pathfinder* pathfinder;
public:
		APD_AIController();

	UPROPERTY(transient)
		UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(transient)
		UBehaviorTreeComponent* BehaviorTreeComp;

	void Possess(APawn* charac) override;

	void StartAITurnCalcultion(PD_GM_MapManager* refMap, PD_GM_LogicCharacter* logicCharacter);
	void EndAITurnCalculation();


	PD_GM_MapManager* GetMapMng() { return mapMng; };
	FStructTurnOrders* GetTurnOrders() { return &actions; };
	PD_GM_LogicCharacter* GetLogicCharacter() { return currentcharac; };
	PD_GM_Pathfinder* GetPathFinder() { return pathfinder; };

	/*
	virtual bool MoveTo(float x, float y) override;
	virtual bool ActionTo(float x, float y, uint8 id_action) override;
	virtual bool Animate(uint8 typeAnimation) override;
	*/


};
