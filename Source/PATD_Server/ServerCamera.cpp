// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "ServerCamera.h"

#include "PD_ServerGameInstance.h"


AServerCamera::AServerCamera()
{

}


void AServerCamera::BeginPlay()
{
	Super::BeginPlay();

	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	if (SGI)
	{
		SGI->Camera_Register(this);
	}

	
}

// Called every frame
void AServerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//Mueve la camara en funcion de la posicion de los players
FVector AServerCamera::Camera_MoveOnlyPlayers()
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

void AServerCamera::Camera_MoveInMovementPhase(TArray<FVector> targetPositions)
{
	SetActorLocation(FindAvaragePosition(targetPositions));
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
		aux_posPlayer = aux_posPlayer + FVector(-400.0f, 0.0f, 0.0f); //se suma un vector Z para darle altura a la camara (pasar el 2000 a const cuando  se requiera)

		avaragePos += aux_posPlayer;
		UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Location is %s"), *aux_posPlayer.ToString());
		numTargets++;
	}

	if (numTargets > 0)
		avaragePos /= numTargets;

	avaragePos.Z = 600.0f; //Con esta parte a priori no es necesario sumar la Z, le ponemos la que tenga la camara

	UE_LOG(LogTemp, Warning, TEXT("CVamera's Location is %s"), *avaragePos.ToString());
	UE_LOG(LogTemp, Warning, TEXT("numTargets %d"), numTargets);

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