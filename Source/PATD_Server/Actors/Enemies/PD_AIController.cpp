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
#include "Structs/PD_ServerStructs.h"
#include "PD_PlayersManager.h"
#include "GM_Game/PD_GM_MapManager.h"

APD_AIController::APD_AIController(){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComp");
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComp");
	actions = FStructTurnOrders();
}

void APD_AIController::Possess(APawn* charac)
{
Super::Possess(charac);
APD_E_Character* ch = Cast<APD_E_Character>(charac);

	if (ch && ch->behaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*ch->behaviorTree->BlackboardAsset);
		BlackboardComp->SetValueAsInt("AP",0);
	
		/*BehaviorTreeComp->StartTree(*ch->behaviorTree);
		BehaviorTreeComp->RestartTree();
		BehaviorTreeComp->Start
		*/
	}
}

void APD_AIController::StartAITurnCalcultion(PD_GM_MapManager* refMap)
{
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::StartAITurnCalcultion"));
	
	mapMng = refMap;
	pathfinder = new PD_GM_Pathfinder(mapMng);

	//Preparar el struct de turno vacio
	actions.positionsToMove.Empty();
	actions.actions.Empty();
	BlackboardComp->SetValueAsInt("AP", 5);

	APD_E_Character* ch = Cast<APD_E_Character>(GetPawn());
	BehaviorTreeComp->StartTree(*ch->behaviorTree, EBTExecutionMode::SingleRun);
}


void APD_AIController::EndAITurnCalculation() {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;


	BehaviorTreeComp->StopTree();
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	SGI->getGameManager()->CallbackEndCreateEnemyOrders(logicCharacter->GetIDCharacter(), &actions);
	
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

/*
TArray<PD_GM_LogicCharacter*> APD_AIController::GetPlayersInRange() {
	TArray<PD_GM_LogicCharacter*> playersInRangeList;
	return playersInRangeList;
}*/

PD_GM_LogicCharacter* APD_AIController::GetClosestPlayer() {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	PD_PlayersManager* playerManager = SGI->getPlayerManager();
	PD_GM_LogicCharacter* closestPlayer=nullptr;
	float minDistance = 10000; //Un maxint
	for (int iPlayer = 0; iPlayer < playerManager->GetNumPlayers(); iPlayer++) {
		float iDistance = logicCharacter->GetCurrentLogicalPosition().EuclideanDistance(playerManager->GetCharacterByIndex(iPlayer)->GetCurrentLogicalPosition());
		if (iDistance < minDistance) {
			closestPlayer = playerManager->GetCharacterByIndex(iPlayer);
			minDistance = iDistance;
		}
	}
	
	return closestPlayer;
}
PD_GM_LogicCharacter* APD_AIController::GetLeastHPPlayer() {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	PD_PlayersManager* playerManager = SGI->getPlayerManager();
	PD_GM_LogicCharacter* leastHPPlayer = nullptr;
	float minHP= 10000; //Un maxint
	for (int iPlayer = 0; iPlayer < playerManager->GetNumPlayers(); iPlayer++) {
		float iHP = logicCharacter->GetTotalStats()->HPCurrent;
		if (iHP < minHP) {
			leastHPPlayer = playerManager->GetCharacterByIndex(iPlayer);
			minHP = iHP;
		}
	}

	return leastHPPlayer;
}
PD_MG_LogicPosition APD_AIController::GetClosestDoorPosition() {
	PD_MG_LogicPosition logicPosition;
	return logicPosition;
}
bool APD_AIController::CheckInRangeFromPositionToCharacter(PD_MG_LogicPosition positionFrom, PD_GM_LogicCharacter* character) {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	float iDistance = positionFrom.EuclideanDistance(character->GetCurrentLogicalPosition());
	if (iDistance > logicCharacter->weapon->RangeWeapon) {
		return false;
	}
	FHitResult hit;
	FVector iniPos = mapMng->LogicToWorldPosition(positionFrom);
	iniPos.Z = 50;
	FVector endPos = mapMng->LogicToWorldPosition(character->GetCurrentLogicalPosition());
	endPos.Z = 50;

	GetWorld()->LineTraceSingleByChannel(hit, iniPos, endPos, ECollisionChannel::ECC_Visibility);

	if (hit.GetActor() != character->GetCharacterBP()) {
		return false;
	}

	return true;

}

