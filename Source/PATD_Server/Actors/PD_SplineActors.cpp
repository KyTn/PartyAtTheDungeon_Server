// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Components/SplineComponent.h"
#include "PD_SplineActors.h"


// Sets default values
APD_SplineActors::APD_SplineActors(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the spline component and set it as the root component.
	spline = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, FName(TEXT("Spline")));
	spline->SetMobility(EComponentMobility::Movable);
	SetRootComponent(spline);
}

// Called when the game starts or when spawned
void APD_SplineActors::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_SplineActors::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

/*
METODOS DEL ACTOR
*/

void APD_SplineActors::RemovePoints() //Remove todos los puntos del Spline
{
	UE_LOG(LogTemp, Warning, TEXT("APD_SplineActors::RemovePoints()"));
	spline->ClearSplinePoints(true); //Con true se hace UPDATE a los splinesPoints
}

void APD_SplineActors::SetPoints(TArray<FVector> pointsToAdd) //Configura los puntos del Spline
{
	for (int i = 0; i < pointsToAdd.Num(); i++)
	{
		spline->AddSplinePoint(pointsToAdd[i], ESplineCoordinateSpace::World, true); //Añadir punto en el espacio World y actaulizando los spline (el ultimo true)
	}
}

void APD_SplineActors::SetIsUsing(bool newIsUsing) //Configura si se esta usando o no
{
	isUsing = newIsUsing;
}
bool APD_SplineActors::GetIsUsing()
{
	return isUsing;
}
void APD_SplineActors::SetToActorLocation(FVector newPosition) //mueve el splineActor a la posicion del actor que lo va a usar
{
	this->SetActorLocation(newPosition);
}

USplineComponent* APD_SplineActors::GetSplineComponent()
{
	return spline;
}
