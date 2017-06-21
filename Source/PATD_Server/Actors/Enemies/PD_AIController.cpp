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
#include "PATD_Server/GM_Game/PD_GM_EnemyManager.h"
#include "Structs/PD_ServerStructs.h"
#include "PD_PlayersManager.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"
#include "Actors/Interactuables/PD_E_Door.h"

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
		canAction = false;
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
	actions.consumablesToConsume.Empty();
	actions.interactuablesToInteract.Empty();

	

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

PD_GM_LogicCharacter* APD_AIController::GetMostHPEnemy() {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	PD_GM_EnemyManager* enemyManager = SGI->getGameManager()->enemyManager;
	PD_GM_LogicCharacter* mostHPEnemy = nullptr;
	float maxHP = 0; 
	for (int iEnemy = 0; iEnemy < enemyManager->GetEnemies().Num(); iEnemy++) {
		float iHP = logicCharacter->GetTotalStats()->HPCurrent;
		if (iHP > maxHP) {
			mostHPEnemy = enemyManager->GetCharacterByIndex(iEnemy);
			maxHP = iHP;
		}
	}

	return mostHPEnemy;
}

PD_MG_LogicPosition APD_AIController::GetClosestDoorPosition() {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	TArray<PD_MG_LogicPosition> listPositions;
	PD_MM_Room* room = nullptr;
	room = mapMng->MapInfo->roomByLogPos[logicCharacter->GetCurrentLogicalPosition()];
	TArray<APD_E_Door*> listActorDoors;
	mapMng->MapInfo->doorActorByLogPos.GenerateValueArray(listActorDoors);
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::GetClosestDoorPosition: Puertas Totales:%d "), listActorDoors.Num());

	for (APD_E_Door* door : listActorDoors) {
		if (!(door->IsDoorOpen)) {
			if (door->doorInfo->room_ConnA == room) {
				listPositions.Add(door->doorInfo->connA);
			}
			else if (door->doorInfo->room_ConnB == room) {
				listPositions.Add(door->doorInfo->connB);

			}
		}
	}

	
	return GetClosestPosition(listPositions);
}

APD_E_Interactuable* APD_AIController::GetClosestDoor() {
	TArray<APD_E_Door*> listActorDoors;
	mapMng->MapInfo->doorActorByLogPos.GenerateValueArray(listActorDoors);
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::GetClosestDoor: Puertas Totales:%d "), listActorDoors.Num());
	TArray<PD_MG_LogicPosition> listPositions;

	for (APD_E_Door* door : listActorDoors) {
		listPositions.Add(door->ActualLogicPosition);
	}
	PD_MG_LogicPosition doorPos = GetClosestPosition(listPositions);
	if (mapMng->MapInfo->doorActorByLogPos.Contains(doorPos)) {
		return mapMng->MapInfo->doorActorByLogPos[doorPos];
	}

	return nullptr;
}


PD_MG_LogicPosition APD_AIController::GetActivationPosition(APD_E_Interactuable* interactuable) {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;

	APD_E_Door* door = Cast<APD_E_Door>(interactuable);

	PD_MM_Room* room = nullptr;
	room = mapMng->MapInfo->roomByLogPos[logicCharacter->GetCurrentLogicalPosition()];

	if (door) {
		if (door->doorInfo->room_ConnA == room) {
			return door->doorInfo->connA;
		}
		else if (door->doorInfo->room_ConnB == room) {
			return door->doorInfo->connB;

		}
	}
	return PD_MG_LogicPosition();
}


PD_MG_LogicPosition APD_AIController::GetClosestPosition(TArray<PD_MG_LogicPosition> listPosition) {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	float minDistance = 10000; //Un maxint
	PD_MG_LogicPosition selectedPosition;
	for (PD_MG_LogicPosition logicPosition:listPosition) {
		float iDistance = logicCharacter->GetCurrentLogicalPosition().EuclideanDistance(logicPosition);
		if (iDistance < minDistance) {
			selectedPosition = logicPosition;
			minDistance = iDistance;
		}
	}

	return selectedPosition;
}



bool APD_AIController::CheckInRangeFromPositionToCharacter(PD_MG_LogicPosition positionFrom, PD_GM_LogicCharacter* character,int range) {
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;
	//float iDistance = positionFrom.EuclideanDistance(character->GetCurrentLogicalPosition());
	 
	TArray<PD_MG_LogicPosition> listInRange=mapMng->GetAllTilesInRange(range, positionFrom);
	//TArray<PD_MG_LogicPosition> listInRange = mapMng->GetAllTilesInRange(2, positionFrom);

	
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::CheckInRangeFromPositionToCharacter:  Rango: %d"), range);
	/*
	if (iDistance > logicCharacter->weapon->RangeWeapon) {
		return false;
	}*/

	if (!listInRange.Contains(character->GetCurrentLogicalPosition())) {
		UE_LOG(LogTemp, Log, TEXT("APD_AIController::CheckInRangeFromPositionToCharacter:  NO ESTA EN RANGO"));

		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("APD_AIController::CheckInRangeFromPositionToCharacter:  esta en rango a falta de raycast"));

	FHitResult hit;
	FVector iniPos = mapMng->LogicToWorldPosition(positionFrom);
	iniPos.Z = 80;
	FVector endPos = mapMng->LogicToWorldPosition(character->GetCurrentLogicalPosition());
	endPos.Z = 80;


	FCollisionQueryParams paramsRay = FCollisionQueryParams();
	paramsRay.AddIgnoredActor(GetPawn());
	GetWorld()->LineTraceSingleByChannel(hit, iniPos, endPos, ECollisionChannel::ECC_Camera, paramsRay);

	//DrawDebugLine(GetWorld(),iniPos,hit.ImpactPoint,FColor(255, 0, 0),true, -1, 0,12.333);
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::CheckInRangeFromPositionToCharacter: Raycast Fallado"));

	if (hit.GetActor() != character->GetCharacterBP()) {
		
		UE_LOG(LogTemp, Log, TEXT("APD_AIController::CheckInRangeFromPositionToCharacter: Raycast Fallado"));

		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("APD_AIController::CheckInRangeFromPositionToCharacter: Raycast Acertado"));
	return true;

}
TArray<PD_MG_LogicPosition>  APD_AIController::GetValidPositionsAdyacentsTo(PD_MG_LogicPosition position) {
	TArray<PD_MG_LogicPosition> listAdyacents=mapMng->Get_LogicPosition_Diagonals_And_Adyacents_To(position);
	TArray<PD_MG_LogicPosition> listResult;
	for (PD_MG_LogicPosition logicPosition : listAdyacents) {
		//UE_LOG(LogTemp, Log, TEXT("Pathfinding successors %d,%d ==Wall:%d,Prop:%d,DoorClosed:%d"), logicPosition.GetX(), logicPosition.GetY(), mapMng->IsLogicPositionAWall(logicPosition), mapMng->IsLogicPositionAProp(logicPosition), (mapMng->IsLogicPositionADoor(logicPosition) && !mapMng->MapInfo->doorActorByLogPos[logicPosition]->IsDoorOpen));

		if (!(mapMng->IsLogicPositionAWall(logicPosition)
			|| mapMng->IsLogicPositionAProp(logicPosition)
			|| (mapMng->IsLogicPositionADoor(logicPosition) && !(mapMng->MapInfo->doorActorByLogPos[logicPosition]->IsDoorOpen))
			)) {
			//UE_LOG(LogTemp, Log, TEXT("APD_AIController::GetValidPositionsAdyacentsTo: %d,%d"), logicPosition.GetX(), logicPosition.GetY());
			listResult.Add(logicPosition);

		}
			
		
	}
	return listResult;
}

TArray<PD_GM_LogicCharacter*>  APD_AIController::GetEnemiesInRange(float range) {
	
	TArray<PD_GM_LogicCharacter*> listReturn;

	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)this->GetPawn())->logic_character;

	for (PD_GM_LogicCharacter* otherEnemy : SGI->gameManager->enemyManager->GetEnemies()) {
		float iDistance = logicCharacter->GetCurrentLogicalPosition().EuclideanDistance(otherEnemy->GetCurrentLogicalPosition());
		bool isSelf = logicCharacter == otherEnemy;
		if (iDistance<range && !isSelf) {
			listReturn.Add(otherEnemy);
		}
	}
	return listReturn;
}


void APD_AIController::SetGoalPosition(PD_MG_LogicPosition inGoalPosition) {
	goalPosition = inGoalPosition;
	usePosition = true;
	useCharacter = false;
}
void APD_AIController::SetGoalCharacter(PD_GM_LogicCharacter* inGoalCharacter) {
	goalCharacter = inGoalCharacter;
	useCharacter = true;
	usePosition = false;
}
void APD_AIController::SetGoalCharacterAndPosition(PD_GM_LogicCharacter* inGoalCharacter, PD_MG_LogicPosition inGoalPosition) {
	goalCharacter = inGoalCharacter;
	goalPosition = inGoalPosition;
	useCharacter = true;
	usePosition = true;
}
/*
APD_E_Interactuable* APD_AIController::GetAdyacentDoor(PD_MG_LogicPosition position) {
	TArray<PD_MG_LogicPosition> listAdyacents = mapMng->Get_LogicPosition_Diagonals_And_Adyacents_To(position);
	TArray<PD_MG_LogicPosition> listResult;
	for (PD_MG_LogicPosition logicPosition : listAdyacents) {
		//UE_LOG(LogTemp, Log, TEXT("Pathfinding successors %d,%d ==Wall:%d,Prop:%d,DoorClosed:%d"), logicPosition.GetX(), logicPosition.GetY(), mapMng->IsLogicPositionAWall(logicPosition), mapMng->IsLogicPositionAProp(logicPosition), (mapMng->IsLogicPositionADoor(logicPosition) && !mapMng->MapInfo->doorActorByLogPos[logicPosition]->IsDoorOpen));
		mapMng->getInteractuableAt(logicPosition);
	//	&& !(mapMng->MapInfo->doorActorByLogPos[logicPosition]->IsDoorOpen)
		if (mapMng->IsLogicPositionADoor(logicPosition)) {
			return nullptr;
		}

	}
	return nullptr;
}*/

bool APD_AIController::CanEndMoveAtPosition(PD_MG_LogicPosition position) {
	return !mapMng->IsLogicPositionADoor(position);
		//&& !mapMng->MapInfo->doorActorByLogPos[adyacentLogicPosition]->IsDoorOpen;

}