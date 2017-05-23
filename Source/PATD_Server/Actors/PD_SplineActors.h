// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PD_SplineActors.generated.h"

UCLASS()
class PATD_SERVER_API APD_SplineActors : public AActor
{
	GENERATED_BODY()
private:
	//Variables
	bool isUsing;


public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USplineComponent*  spline;

	// Sets default values for this actor's properties
	APD_SplineActors(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Funciones
	void RemovePoints(); //Remove todos los puntos del Spline
	void SetPoints(TArray<FVector> pointsToAdd); //Configura los puntos del Spline
	void SetIsUsing(bool newIsUsing); //Configura si se esta usando o no
	bool GetIsUsing();
	void SetToActorLocation(FVector newPosition); //mueve el splineActor a la posicion del actor que lo va a usar
	USplineComponent* GetSplineComponent();

};
