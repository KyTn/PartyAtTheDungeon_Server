// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/PD_E_Character.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PD_ServerGameInstance.h"
#include "PD_AIController.h"
#include <math.h>
#include "GM_Game/PD_GM_GameManager.h"
#include "PATD_Server/GM_Game/Pathfinder/PD_GM_Pathfinder.h"

APD_AIController::APD_AIController(){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComp");
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComp");
}

void APD_AIController::Possess(APawn* charac)
{
Super::Possess(charac);
APD_E_Character* ch = Cast<APD_E_Character>(charac);

	if (ch && ch->behaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*ch->behaviorTree->BlackboardAsset);
		BlackboardComp->SetValueAsInt("AP",0);
		BehaviorTreeComp->StartTree(*ch->behaviorTree);
	}
}

void APD_AIController::StartAITurnCalcultion(PD_GM_MapManager* refMap, PD_GM_LogicCharacter* logicCharacter)
{
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::StartAITurnCalcultion"));
	mapMng = refMap;
	currentcharac = logicCharacter;
	pathfinder = new PD_GM_Pathfinder(mapMng);

	actions.positionsToMove.Empty();
	actions.actions.Empty();
	BlackboardComp->SetValueAsInt("AP", 5);
}


void APD_AIController::EndAITurnCalculation() {

	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	SGI->getGameManager()->CallbackEndCreateEnemyOrders(currentcharac->GetIDCharacter(), &actions);
}


/*

bool APD_AIController::MoveTo(float x, float y)
{
	Super::MoveTo(x, y);
	UE_LOG(LogTemp, Warning, TEXT("Init GameMap"));

	FVector currentPosition = GetPawn()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Enemy Location is %s"), *currentPosition.ToString());


	FVector newPosition = FVector(x, y, 0);
	UE_LOG(LogTemp, Warning, TEXT("Enemy new Location is %s"), *newPosition.ToString());

	MoveToLocation(newPosition, -1.0f, true, false, false, true, 0, true);
	return true;
}

bool APD_AIController::ActionTo(float x, float y, uint8 id_action)
{
	Super::ActionTo(x, y, id_action);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "AL ATAQUERRRRRR !!!!!");

	return true;

}

bool APD_AIController::Animate(uint8 typeAnimation)
{
	return true;
}*/