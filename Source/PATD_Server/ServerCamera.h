// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraActor.h"
#include "ServerCamera.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API AServerCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:

	AServerCamera();

	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	//Mueve la camara en funcion de la posicion de los players
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
		FVector Camera_MoveOnlyPlayers();

	//Mueve la camara en funcion de la posicion de los players
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
		void Camera_MoveInMovementPhase(TArray<FVector> targetPositions);

	//Calcula la posicion intermedia entre todos los players  - Para Move()
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
	FVector FindAvaragePosition(TArray<FVector> desiredPositions);

	//Calcula si esta en la posicion o no
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
	bool IsInCurrentViewPort(FVector2D desiredPosition);

	
};
