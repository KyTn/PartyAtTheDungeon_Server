// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_CharacterController.h"
#include "Structs/PD_NetStructs.h"



bool APD_CharacterController::MoveTo(float x, float y)
{
	Super::MoveTo(x,y);
	UE_LOG(LogTemp, Warning, TEXT("Init GameMap"));

	FVector currentPosition = GetPawn()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Location is %s"), *currentPosition.ToString());


	FVector newPosition = FVector(x, y, 0);
	UE_LOG(LogTemp, Warning, TEXT("MyCharacter's new Location is %s"), *newPosition.ToString());

	//MoveToLocation(newPosition, -1.0f, true, false, false, true, 0, true);
	

	return true;
}

bool APD_CharacterController::ActionTo(FStructTargetToAction action)
{
	Super::ActionTo(action);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "AL ATAQUERRRRRR !!!!!");
	
	return true;

}

bool APD_CharacterController::Animate(uint8 typeAnimation)
{
	return true;
}

