// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraActor.h"
#include "ServerCamera.generated.h"

/**
 * 
 */
class APD_SplineActors;
UCLASS()
class PATD_SERVER_API AServerCamera : public ACameraActor
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
public:

	//AServerCamera();

	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	//Mueve la camara en funcion de la posicion de los players
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
		FVector GetPlayersAveragePosition();

	UFUNCTION(BlueprintCallable, Category = "CameraControl")
		void MoveTo(FVector targetPosition);
	//Mueve la camara en funcion de la posicion de los players
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
		void Camera_MoveInMovementPhase(TArray<FVector> targetList);

	//Calcula la posicion intermedia entre todos los players  - Para Move()
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
	FVector FindAvaragePosition(TArray<FVector> desiredPositions);

	//Calcula si esta en la posicion o no
	UFUNCTION(BlueprintCallable, Category = "CameraControl")
	bool IsInCurrentViewPort(FVector2D desiredPosition);

	//Sistema de movimiento
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector moveTargetPosition;

	//bool IsMoving();

	ECameraMoveState moveState= ECameraMoveState::Patrol;
	ECameraLookState lookState = ECameraLookState::Static;

	ECameraMoveState GetMoveState() { return moveState; };
	ECameraLookState GetLookState() { return lookState; };

	//Move
	void OnMoveEnd();
	float velocity=1000;
	FVector targetDirection;

	//Patrol
	/**Spline component*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APD_SplineActors* spline;
	float distance;
	float patrolVelocity = 100;
//	bool patrolRotate = true;
	void InitPatrol(FVector targetPosition);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector lookPosition;

	void LookAtPoint(FVector inLookPosition);
	void StopLookAt();

};
