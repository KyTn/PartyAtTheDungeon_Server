// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "ServerCamera.h"

#include "Components/SplineComponent.h"
#include "PD_ServerGameInstance.h"
#include "Actors/PD_SplineActors.h"
#include "Kismet/KismetMathLibrary.h"
#include "PD_PlayersManager.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "GM_Game/PD_GM_MapManager.h"

/*
AServerCamera::AServerCamera()
{
//	FName YourObjectName("Hiiii");
//	FObjectInitializer::CreateDefaultSubobject<USplineComponent>(this, FName(TEXT("Spline")));
	//USplineComponent* NewComp = FObjectInitializer::CreConstructObject<USplineComponent>(USplineComponent, this, YourObjectName);
	//YourSplineComponent=PCIP.CreateDefaultSubobject<USplineComponent>(this, TEXT("Speeds"));
}*/
/*
AServerCamera::AServerCamera(const FObjectInitializer& ObjectInitializer)
	//: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	//spline = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	//YourSplineComponent->AddPo
	//YourSplineComponent->AttachTo()
	//YourSplineComponent->AttachTo(GetRootComponent(), , EAttachLocation::KeepWorldPosition);
	//YourSplineComponent->AttachParent = RootComponent;
}*/

void AServerCamera::BeginPlay()
{
	Super::BeginPlay();

	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	if (SGI)
	{
		SGI->Camera_Register(this);

		TArray<FVector> targetsInitial = TArray<FVector>();
		for (int i = 0; i < SGI->playersManager->GetNumPlayers(); i++)
		{
			targetsInitial.Add(SGI->mapManager->LogicToWorldPosition(
			SGI->playersManager->GetCharacterByIndex(i)->GetCurrentLogicalPosition()));
		}

		Camera_MoveInMovementPhase(targetsInitial);
	}

	
}

// Called every frame
void AServerCamera::Tick(float DeltaTime)
{
	
	//UE_LOG(LogTemp, Log, TEXT("Move camera. Position:%s"), *GetActorLocation().ToString());
	Super::Tick(DeltaTime);
	if (moveState == ECameraMoveState::Moving) {
		timeCurrent += DeltaTime;

		if (timeCurrent< timeTotal)
		//if (!this->GetActorLocation().Equals(moveTargetPosition, 15.0)) //Compara con un offset de error, (Por pruebas se ha determinado que 15, pero pueden ser mas o menos)
		{//continua moviendose
			UE_LOG(LogTemp, Warning, TEXT("AServerCamera::Tick:: Objetivo:%s "), *moveTargetPosition.ToString());

			FVector incrementPosition = velocity*DeltaTime*targetDirection; //target direction es un vector
			//UE_LOG(LogTemp, Log, TEXT("Move camera. Position:%s"), *GetActorLocation().ToString());
			//UE_LOG(LogTemp, Log, TEXT("Move camera. Incremento:%s"), *incrementPosition.ToString());
			//UE_LOG(LogTemp, Log, TEXT("Move camera. Incremento valores:%s"), *targetDirection.ToString());
			FVector newLocation=GetActorLocation() + incrementPosition;

			SetActorLocation(newLocation);

			FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), lookPosition);
		//	PlayerRot.Yaw = 90;
			SetActorRotation(PlayerRot);

		}
		else
		{//ha llegado

			moveState= ECameraMoveState::EndMoving;
			OnMoveEnd();
		}

	}
	else if (moveState == ECameraMoveState::Patrol) {

		FRotator newRotator = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), lookPosition);

		//newRotator.Pitch = newRotator.Pitch - 50.0f;
		SetActorRotation(newRotator);

		distance += DeltaTime*patrolVelocity;
		//distance += 0.5;
		SetActorLocation(spline->GetSplineComponent()->GetWorldLocationAtDistanceAlongSpline(distance));
		/*
		if (patrolRotate) {
			SetActorRotation(spline->GetSplineComponent()->GetWorldRotationAtDistanceAlongSpline(distance));
		}
		*/
		if (distance > spline->GetSplineComponent()->GetDistanceAlongSplineAtSplinePoint(spline->GetSplineComponent()->GetNumberOfSplinePoints())) {
			distance = 0;
		}
		//if (distance>spline->GetSplineComponent()->Get)
	//	UE_LOG(LogTemp, Log, TEXT("Move camera Standard. Position:%s"), *GetActorLocation().ToString());
	}



	if (lookState == ECameraLookState::LookPoint) {

		//LookAtPoint(moveTargetPosition);
		//FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), lookPosition);
		//PlayerRot.Yaw = 0;

		//FRotator newrot = (GetActorLocation() - lookPosition).Rotation();
		//this->AddActorWorldRotation(newrot);
		//SetActorRotation(PlayerRot);
		
		
	}
	else if (lookState == ECameraLookState::LookActor) {
		UE_LOG(LogTemp, Warning, TEXT("ECameraMoveState::LookActor"));

	}

	
}


//Mueve la camara en funcion de la posicion de los players
FVector AServerCamera::GetPlayersAveragePosition()
{
	TArray<FVector> desiredPositions = TArray<FVector>();

	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	if (SGI)
	{
		for (int i = 0; i < SGI->playersManager->GetNumPlayers(); i++)
		{
			desiredPositions.Add(SGI->mapManager->LogicToWorldPosition(SGI->playersManager->GetDataPlayers()[i]->logic_Character->GetCurrentLogicalPosition()));
		}

		//SetActorLocation(FindAvaragePosition(desiredPositions));
		return FindAvaragePosition(desiredPositions);
	}
	
	return FVector(0, 0, 0);
}

void AServerCamera::Camera_MoveInMovementPhase(TArray<FVector> targetPointList)
{
//	moveTargetPosition = FindAvaragePosition(targetPointList);

	
	//SetActorLocation();
}


//Calcula la posicion intermedia entre todos los players  - Para Move()
FVector AServerCamera::FindAvaragePosition(TArray<FVector> desiredPositions)
{
	FVector avaragePos = FVector(0.0f);
	int numTargets = 0;

	for (int i = 0; i < desiredPositions.Num(); i++)
	{
		// mapManager->LogicToWorldPosition(playersManager->GetDataPlayers()[i]->logic_Character->GetCurrentLogicalPosition());
		FVector aux_posPlayer = desiredPositions[i];
		//aux_posPlayer = aux_posPlayer + FVector(-400.0f, 0.0f, 0.0f); //se suma un vector Z para darle altura a la camara (pasar el 2000 a const cuando  se requiera)

		avaragePos += aux_posPlayer;
		//UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Location is %s"), *aux_posPlayer.ToString());
		numTargets++;
	}

	if (numTargets > 0)
		avaragePos /= numTargets;

	avaragePos.Z = 800.0f; //Con esta parte a priori no es necesario sumar la Z, le ponemos la que tenga la camara

	//UE_LOG(LogTemp, Warning, TEXT("CVamera's Location is %s"), *avaragePos.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("numTargets %d"), numTargets);

	return avaragePos;
}

//Calcula el FOV de la camara - Para Zoom()
bool AServerCamera::IsInCurrentViewPort(FVector2D desiredPosition)
{

	FVector2D viewport = FVector2D(0, 0);
	GEngine->GameViewport->GetViewportSize(viewport);
	
	
	if ((desiredPosition.X > 0) && (desiredPosition.Y > 0) && (desiredPosition.X < viewport.X) && (desiredPosition.Y && viewport.Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("AServerCamera:::IsInCurrentViewPort - Si esta en Viewport"));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AServerCamera:::IsInCurrentViewPort - NO esta en Viewport"));
		return false;
	}
	

	UE_LOG(LogTemp, Warning, TEXT("AServerCamera desiredPosition is %s"), *desiredPosition.ToString());
	
	return false;

}

void AServerCamera::MoveToPositions(TArray<FVector> targetPositions) {
	
	FVector position = FindAvaragePosition(targetPositions);
	position.Z= GetZoomForPositionList(targetPositions);
	//UE_LOG(LogTemp, Warning, TEXT("ECameraMoveState::MoveToPositions: GetZoomForPositionList: zoomSelected %d"), position.Z);
	//UE_LOG(LogTemp, Warning, TEXT("ECameraMoveState::MoveToPositions: GetZoomForPositionList: %d"), GetZoomForPositionList(targetPositions));
	//position.Z = 1000;
	//UE_LOG(LogTemp, Warning, TEXT("ECameraMoveState::MoveToPositions: GetZoomForPositionList: zoomSelected %d"), position.Z);
	UE_LOG(LogTemp, Warning, TEXT("ECameraMoveState::MoveToPositions: GetZoomForPositionList: Position con Zoom %s"), *position.ToString());
	MoveTo(position);

}

void AServerCamera::MoveTo(FVector targetPosition) {
	UE_LOG(LogTemp, Log, TEXT("CameraMoveTo targetDirection.Target:%s"), *targetPosition.ToString());
	moveTargetPosition = targetPosition;
	targetDirection = (moveTargetPosition - this->GetActorLocation());
	UE_LOG(LogTemp, Log, TEXT("CameraMoveTo targetDirection.Location:%s"), *this->GetActorLocation().ToString());
	targetDirection.Normalize();

	timeTotal = FVector::Dist(moveTargetPosition, this->GetActorLocation()) / velocity;
	timeCurrent = 0;
	moveState = ECameraMoveState::Moving;
}

void AServerCamera::InitPatrol(FVector targetPosition) {
	FVector v=FVector(targetPosition.X, targetPosition.Y, 1000);
	lookPosition = FVector(targetPosition.X, targetPosition.Y, 0);
	spline->SetActorLocation(v);
	moveState = ECameraMoveState::Patrol;
	
}

void AServerCamera::OnMoveEnd() {
	moveState = ECameraMoveState::EndMoving;
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	SGI->gameManager->OnCameraEndMove();
}

void AServerCamera::LookAtPoint(FVector inLookPosition) {
	lookPosition = FVector(inLookPosition.X, inLookPosition.Y,0);
	lookState = ECameraLookState::LookPoint;
}
void AServerCamera::StopLookAt() {
	lookState = ECameraLookState::Static;
}

bool AServerCamera::SetCameraOnView()
{
	FOutputDeviceNull ar;

	const FString command = FString::Printf(TEXT("PutCameraPlayerOnView"));

	if (this->CallFunctionByNameWithArguments(*command, ar, NULL, true))
	{
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::SetCharacterCameraOnView -- EXITO EN LLAMAR A LA FUNCION"));
		return true;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("APD_E_Character::SetCharacterCameraOnView - EEROR EN LLAMATR A LA FUNCION"));
		return false;
	}
}


FVector AServerCamera::FindMaxDeviation(TArray<FVector> desiredPositions) {
	FVector averagePosition= FindAvaragePosition(desiredPositions);
	FVector maxDiff = FVector(0,0,0);
	for (FVector position : desiredPositions) {
		FVector diff= position - averagePosition;
		
		if (maxDiff.X < FMath::Abs(diff.X)) {
			maxDiff.X = FMath::Abs(diff.X);
		}
		if (maxDiff.Y < FMath::Abs(diff.Y)) {
			maxDiff.Y = FMath::Abs(diff.Y);
		}
	}

	return maxDiff;
}

float AServerCamera::GetZoomForPositionList(TArray<FVector> inPositionsToShowList) {

	UE_LOG(LogTemp, Warning, TEXT("AServerCamera::GetZoomForPositionList: %s"), *this->GetName());
	FVector deviation = FindMaxDeviation(inPositionsToShowList);



	float zoomFromX = deviation.X*3;
	float zoomFromY = deviation.Y*3;


	float zoomDesired = 1000;
	if (deviation.Equals(FVector(0, 0, 0))) {
		zoomDesired = 1000;
	}
	else {
		zoomDesired = FMath::Max((float)1000,FMath::Max(zoomFromX, zoomFromY));
	}
	FVector mostrar = FVector(zoomFromX, zoomFromY, zoomDesired);
	UE_LOG(LogTemp, Warning, TEXT("ECameraMoveState::GetZoomForPositionList: FVectorMostrar %s"), *mostrar.ToString());

	return zoomDesired;
	/*
	if (FMath::Abs(GetActorLocation().Z - zoomDesired) > 15) {


		float newZ = 1000;
		if (zoomDesired > GetActorLocation().Z) {
			newZ = velocityZoom*DeltaTime + GetActorLocation().Z;

		}
		else {
			newZ = velocityZoom*DeltaTime - GetActorLocation().Z;
		}
		FVector newLocation = GetActorLocation();
		newLocation.Z = newZ;
		SetActorLocation(newLocation);

	}*/
}

/*
void AServerCamera::SetDesiredPositionToShow(TArray<FVector> inPositionsToShowList) {
	positionsToShowList = positionsToShowList;
	
}*/
