// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_EnemyController.h"


bool APD_EnemyController::MoveTo(float x, float y)
{
	FVector newPosition = FVector(x, y, 0);

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


