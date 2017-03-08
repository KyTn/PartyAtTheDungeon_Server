// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PD_EnemyController.h"


bool APD_EnemyController::MoveTo(float x, float y)
{
	Super::MoveTo(x, y);
	UE_LOG(LogTemp, Warning, TEXT("Init GameMap"));

	FVector currentPosition = GetPawn()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Location is %s"), *currentPosition.ToString());


	FVector newPosition = FVector(x, y, 0);
	UE_LOG(LogTemp, Warning, TEXT("MyCharacter's new Location is %s"), *newPosition.ToString());

	MoveToLocation(newPosition, -1.0f, true, false, false, true, 0, true);
	return true;
}

bool APD_EnemyController::ActionTo(float x, float y, uint8 id_action)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "AL ATAQUERRRRRR !!!!!");

	return true;

}

bool APD_EnemyController::Animate(uint8 typeAnimation)
{
	return true;
}
